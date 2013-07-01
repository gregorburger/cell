#include "cell.h"
#include <cmath>
#include <thrust/sort.h>
#include <thrust/fill.h>
#include <thrust/scan.h>
#include <thrust/iterator/retag.h>
#include <thrust/system/omp/memory.h>
#include <algorithm>
#include <iostream>
#include "parallel_radix_sort.h"

Cell::Cell(fp width, fp height, fp dx)
    : dx(dx), w(std::ceil(width/dx)), h(std::ceil(height/dx)),
      grid(w * h) {

    thrust::fill(&grid[0], &grid[w * h - 1], 0);
}

Cell::~Cell() {
}

int Cell::idx(const vector &v) const {
    assert(v.x >= 0 && v.x <= w*dx);
    assert(v.y >= 0 && v.y <= h*dx);
    int _idx = int(v.y/dx)*w + int(v.x/dx);
    assert(_idx < w * h);
    assert(_idx >= 0);
    return _idx;
}

int Cell::idx(std::size_t i) const {
    int _idx = int(particles->y[i]/dx)*w + int(particles->x[i]/dx);
    assert(_idx < w * h);
    assert(_idx >= 0);
    return _idx;
}

void Cell::add_particles(soa_vectors &particles) {
    if (cell_indices.size() != particles.size()) cell_indices.resize(particles.size());
    this->particles = &particles;

#pragma omp parallel for
    for (std::size_t i = 0; i < particles.size(); ++i) {
        //cell_indices[i] = idx(particles[i]);
        cell_indices[i] = idx(i);
    }

#if 1
    thrust::sort_by_key(thrust::retag<thrust::omp::tag>(cell_indices.begin()),
                        thrust::retag<thrust::omp::tag>(cell_indices.end()),
                        thrust::retag<thrust::omp::tag>(thrust::make_zip_iterator(thrust::make_tuple(particles.x.begin(), particles.y.begin()))));
#else
    int *ci_ptr = &cell_indices[0];
    vector *p_ptr = &particles[0];
    parallel_radix_sort::SortPairs(ci_ptr, p_ptr, particles.size());
#endif

    grid[cell_indices[0]] = 0;

#pragma omp parallel for
    for (int i = 1; i < cell_indices.size(); ++i) {
        if (cell_indices[i-1] != cell_indices[i]) {
            grid[cell_indices[i]] = i;
        }
    }
}

void Cell::cell(int _idx, int *start, int *stop) const {
    assert(_idx < w*h);
    *start = grid[_idx];
    *stop = _idx == (w*h-1)
            ? particles->size()
            : grid[_idx+1];
    assert(*stop >= 0 && *stop <= particles->size());
    assert(*start >= 0 && *start <= particles->size());
    //can this be always satified even on empty cells after filled cells?
    //assert(*start <= *stop);
}

inline
void Cell::cell(const vector &v, int *start, int *stop) const {
    int _idx = idx(v);
    cell(_idx, start, stop);
}

std::vector<vector> Cell::nb(const vector &v) const {
    std::vector<vector> nb;

    int start, stop;
    cell(v, &start, &stop);

    fp dist = dx/2.0*dx/2.0;

    for (int i = start; i < stop; ++i) {
        vector u(vector(particles->x[i], particles->y[i]));
        fp r_ij = (u-v).length_squared();
        if (r_ij <= dist)
            nb.push_back(u);
    }

    return nb;
}


void Cell::find_distances(std::vector<std::vector<fp> > &dists,
                          std::vector<std::vector<int> > &indices) const {
    //hopefully resize checks wheather the resize is silly or not
    dists.resize(particles->size());
    indices.resize(particles->size());

#pragma omp parallel for
    for (int ci = 0; ci < grid.size(); ++ci) {
        int start, stop;
        cell(ci, &start, &stop);
        if (stop <= start) {
            continue;
        }
        soa_candidates candidates;
        candidates.reserve(170);
        int x = ci % w;
        int y = ci / w;
        find_candidates(x, y, candidates);
        for (int i = start; i < stop; ++i) {
            filter_by_radius(i, candidates, dists[i], indices[i]);
        }
    }
}

void Cell::find_candidates(int x, int y, soa_candidates &candidates) const {
    int start, stop;

    for (int yo = -1; yo < 2; ++yo) {
        for (int xo = -1; xo < 2; ++xo) {
            if (!idx_ok(x+xo, y+yo)) continue;

            cell((y+yo)*w+x+xo, &start, &stop);

            for (int i = start; i < stop; i++) {
                candidates.indices.push_back(i);
                candidates.x.push_back(particles->x[i]);
                candidates.y.push_back(particles->y[i]);
            }
        }
    }
}


//#define WITHOUT_BRANCH

typedef void(*callback)(fp dist, int idx, std::vector<fp> &dists, std::vector<int> &indices);

void bad(fp dist, int idx, std::vector<fp> &dists, std::vector<int> &indices) {

}

void good(fp dist, int idx, std::vector<fp> &dists, std::vector<int> &indices) {
    dists.push_back(dist);
    indices.push_back(idx);
}

void Cell::filter_by_radius(int i, const soa_candidates &candidates,
                            std::vector<fp> &dists, std::vector<int> &indices) const {

    indices.reserve(30);
    dists.reserve(30);

    fp radius = dx/2.0*dx/2.0;

    for (int j = 0; j < candidates.size(); ++j) {
        //fp r_ij = (v_i - v_j).length_squared();
        fp r_ij = dist_squared(i, candidates.indices[j]);
        if (r_ij > radius) continue;
        dists.push_back(r_ij);
        indices.push_back(candidates.indices[j]);
    }
}

int Cell::nearest(const vector &v) const {
    int start, stop;
    cell(v, &start, &stop);

    int _nearest = start;
    fp dist = dist_squared(start, v);

    for (int i = start+1; i < stop; ++i) {
        fp ndist = dist_squared(i, v);
        if (ndist < dist) {
            dist = ndist;
            _nearest = i;
        }
    }
    return _nearest;
}

void Cell::flatten(const std::vector<std::vector<fp> > &dists, const std::vector<std::vector<int> > &indices,
                   fp **flatten_dists, int **flatten_indices,
                   int **offsets, int *size) const {
    int *sizes = new int[particles->size()];

#pragma omp parallel for
    for (int i = 0; i < indices.size(); ++i) {
        sizes[i] = indices[i].size();
    }

    *offsets = new int[particles->size()];
    thrust::exclusive_scan(&sizes[0], &sizes[particles->size()], &(*offsets)[0]);
    *size = (*offsets)[particles->size()-1];

    *flatten_dists = new fp[*size];
    *flatten_indices = new int[*size];

//#pragma omp parallel for
    for (int i = 0; i < particles->size(); ++i) {
        const std::vector<fp> &_d = dists.at(i);
        const std::vector<int> &_i = indices.at(i);
        int start = (*offsets)[i];
        for (int j = 0; j < _d.size(); ++j) {
            (*flatten_dists)[start + j] = _d[j];
            (*flatten_indices)[start + j] = _i[j];
        }
    }
}
