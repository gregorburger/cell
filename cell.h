#ifndef CELL_H
#define CELL_H

#include <vector>
#include <cassert>
#include <cmath>

typedef float fp;

struct vector {
    vector() {}
    vector(fp x, fp y) : x(x), y(y) {}
    fp x, y;

    inline
    vector operator-(const vector &other) const {
        return vector(x-other.x, y-other.y);
    }

    inline
    fp length_squared() const {
        return x*x+y*y;
    }
};

inline
fp dist_squared(const vector &u, const vector &v) {
    fp x = u.x - v.x;
    fp y = u.y - v.y;
    return x*x+y*y;
}

struct soa_vectors {
    soa_vectors(std::size_t size) : x(size), y(size) {
    }

    soa_vectors() : x(0), y(0) {
    }

    inline
    std::size_t size() const {
        return x.size();
    }

    std::vector<fp> x;
    std::vector<fp> y;
};

struct soa_candidates {
    std::vector<fp> x, y;
    soa_vectors vectors;
    std::vector<int> indices;

    inline
    void reserve(std::size_t size) {
        indices.reserve(size);
        x.reserve(size);
        y.reserve(size);
    }

    inline
    std::size_t size() const {
        return x.size();
    }
};

struct Cell {
    typedef std::vector<std::pair<vector, int> > candidates_type;

    Cell(fp width, fp height, fp dx);
    virtual ~Cell();

    void add_particles(soa_vectors &particles);

    void find_distances(std::vector<std::vector<fp> > &dists,
                        std::vector<std::vector<int> > &indices) const;

    std::vector<vector> nb(const vector &v) const;

    void flatten(const std::vector<std::vector<fp> > &dists,
                 const std::vector<std::vector<int> > &indices,
                 fp **flatten_dists,
                 int **flatten_indices,
                 int **offsets,
                 int *size) const;

    void cell(int _idx, int *start, int *stop) const;
    void cell(const vector &v, int *start, int *stop) const;


    //void find_candidates(int x, int y, candidates_type &candidates) const;
    void find_candidates(int x, int y, soa_candidates &candidates) const;

    void filter_by_radius(int i, const soa_candidates &candidates,
                          std::vector<fp> &dists, std::vector<int> &indices) const;

    int nearest(const vector &v) const;

    inline
    bool idx_ok(int x, int y) const {
        if (x < 0 || x >= w) return false;
        if (y < 0 || y >= h) return false;
        return true;
    }

    inline
    fp dist_squared(std::size_t i, std::size_t j) const {
        fp x = particles->x[i] - particles->x[j];
        fp y = particles->y[i] - particles->y[j];
        return x*x+y*y;
    }

    inline
    fp dist_squared(std::size_t i, vector v) const {
        fp x = particles->x[i] - v.x;
        fp y = particles->y[i] - v.y;
        return std::sqrt(x*x+y*y);
    }

    fp dx;
    int w, h;
    std::vector<int> grid;

    int idx(const vector &v) const;
    int idx(std::size_t i) const;
    soa_vectors *particles;
    std::vector<int> cell_indices;
};

#endif // CELL_H
