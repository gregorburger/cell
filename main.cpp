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

    std::vector<vector> particles;
    const int size = 4000;
    fp dx = fp(1.0)/size;

    Cell c(1.0, 1.0, 3*dx);

    particles.reserve(size*size);

    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            particles.push_back(vector{x*dx, y*dx});
        }
    }

    double before = omp_get_wtime();
    std::cout << "building cell structure" << std::endl;
    c.addParticles(particles);
    std::cout << "done" << std::endl;

    double after_building = omp_get_wtime();

    std::vector<std::vector<fp> > *dists = new std::vector<std::vector<fp> >();
    std::vector<std::vector<int> > *indices = new std::vector<std::vector<int> >();

    std::cout << "finding neighbors" << std::endl;
    c.find_distances(*dists, *indices);
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
    for (int i = 0; i < indices->size(); ++i) {
        int nb = indices->at(i).size();
        sum += nb;
        max = std::max(max, nb);
        min = std::min(min, nb);
    }
    std::cout << "average nb size " << sum/indices->size() << std::endl;
    std::cout << "max nb size " << max << std::endl;
    std::cout << "min nb size " << min << std::endl;
#endif
}

#endif

