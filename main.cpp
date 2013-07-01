//#define GUI
#ifdef GUI
#include "graphicsview.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphicsView w;
    w.show();

    return a.exec();
}
#else
#include "cell.h"
#include <boost/random.hpp>
#include <signal.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    int size;
    if (argc > 1) {
        size = atoi(argv[1]);
    } else {
        size = 2200;
    }
    fp dx = fp(1.0)/size;

    Cell c(1.0, 1.0, 3*dx);

    soa_vectors particles(size*size);

#pragma omp parallel for
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            particles.x[x*size+y] = x*dx;
            particles.y[x*size+y] = y*dx;
        }
    }

    double before = omp_get_wtime();
    std::cout << "building cell structure" << std::endl;
    c.add_particles(particles);
    std::cout << "done" << std::endl;

    double after_building = omp_get_wtime();

    std::vector<std::vector<fp> > dists;
    std::vector<std::vector<int> > indices;

    std::cout << "finding neighbors" << std::endl;
    c.find_distances(dists, indices);
    std::cout << "done" << std::endl;

    double after_finding = omp_get_wtime();
#if 0
    fp *flatten_dists;
    int *flatten_indices, *offsets;
    int flatten_size;

    c.flatten(*dists, *indices, &flatten_dists, &flatten_indices, &offsets, &flatten_size);
#endif
    double after = omp_get_wtime();

    std::cout << "building   done in " << after_building - before << std::endl;
    std::cout << "finding    done in " << after_finding - after_building << std::endl;
#if 0
    std::cout << "flattening done in " << after -  after_finding << std::endl;
#endif
    std::cout << "complete   done in " << after - before << std::endl;


#if 1
    int sum = 0;
    int max = 0;
    int min = 300;
#pragma omp parallel for reduction(+: sum) reduction(max: max) reduction(min: min)
    for (int i = 0; i < dists.size(); ++i) {
        int nb = dists[i].size();
        sum += nb;
        max = std::max(max, nb);
        min = std::min(min, nb);
    }
    std::cout << "average nb size " << sum/dists.size() << std::endl;
    std::cout << "max nb size " << max << std::endl;
    std::cout << "min nb size " << min << std::endl;
#endif
}

#endif

