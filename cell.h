#ifndef CELL_H
#define CELL_H

#include <vector>
#include <cassert>

typedef float fp;

struct vector {
    fp x, y;

    inline
    vector operator-(const vector &other) const {
        return vector{x-other.x, y-other.y};
    }

    inline
    fp length_squared() const {
        return x*x+y*y;
    }
};

template <typename T>
struct matrix {
    matrix(int width, int height) : w(width), h(height) {
        data = new int[width*height];
    }

    virtual ~matrix() {
        delete[] data;
    }

    T &operator()(int x, int y) {
        assert(x < w);
        assert(y < h);
        return data[y*w + x];
    }

    int w, h;
    T *data;
};

struct Cell {
    typedef std::vector<std::pair<vector, int> > candidates_type;

    Cell(fp width, fp height, fp dx);
    virtual ~Cell();

    void add_particles(std::vector<vector> &particles);

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


    void find_candidates(int x, int y, candidates_type &candidates) const;
    void filter_by_radius(int i, const candidates_type &candidates,
                          std::vector<fp> &dists, std::vector<int> &indices) const;

    int nearest(const vector &v) const;

    inline
    bool idx_ok(int x, int y) const {
        if (x < 0 || x >= w) return false;
        if (y < 0 || y >= h) return false;
        return true;
    }

    fp dx;
    int w, h;
    int *grid;

    int idx(const vector &v) const;
    std::vector<vector> *particles;
    std::vector<int> cell_indices;
};

#endif // CELL_H
