#include "graphicsview.h"
#include "cell.h"

#include <boost/random.hpp>
#include <QDebug>
#include <QResizeEvent>
#include <QGraphicsEllipseItem>

boost::random::mt19937 rng;

GraphicsView::GraphicsView(QWidget *parent) :
    QGraphicsView(parent), particles(soa_vectors(4000)) {
    scene = new QGraphicsScene();
    this->setScene(scene);


#if 1
    int size = sqrt(particles.size());
    fp dx = fp(1.0)/size;
    cell = new Cell(1.0, 1.0, 4*dx);
    boost::random::uniform_01<> dist;

    for (int i = 0; i < particles.size(); ++i) {
        particles.x[i] = dist(rng);
        particles.y[i] = dist(rng);
    }
#else
    int size = sqrt(particles.size());
    fp dx = fp(1.0)/size;
    cell = new Cell(1.0, 1.0, 4*dx);

#pragma omp parallel for
    for (int x = 0; x < size; ++x) {
        for (int y = 0; y < size; ++y) {
            particles.x[x*size+y] = x*dx;
            particles.y[x*size+y] = y*dx;
        }
    }
#endif

    cell->add_particles(particles);
    cell->find_distances(dists, indices);
    render();
    fitInView(-0.15, -0.15, 1.15, 1.15, Qt::KeepAspectRatio);
    setRenderHint(QPainter::HighQualityAntialiasing);
    setRenderHint(QPainter::Antialiasing);
}

void GraphicsView::resizeEvent(QResizeEvent *e) {
    fitInView(-0.15, -0.15, 1.15, 1.15, Qt::KeepAspectRatio);
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    vector v;
    v.x = fp(mapToScene(event->pos()).x());
    v.y = fp(mapToScene(event->pos()).y());
    selected = cell->nearest(v);
    in_cell = cell->idx(v);
    qDebug() << "selected " << selected;
    render();
}

void GraphicsView::render() {
#if 0
    delete scene;
    scene = new QGraphicsScene();
    setScene(scene);
#else
    scene->clear();
#endif
    double w = 0.01;
    double w2 = w/2.0;
    for (int i = 0; i < particles.size(); ++i) {
        fp x = particles.x[i];
        fp y = particles.y[i];
        scene->addEllipse(x - w2, y - w2, w, w, QPen(Qt::black));
    }

#if 1
    for (int x = 0; x < cell->w; ++x) {
        scene->addLine(x*cell->dx, 0, x*cell->dx, cell->w*cell->dx);
    }

    for (int y = 0; y < cell->h; ++y) {
        scene->addLine(0, y*cell->dx, cell->h*cell->dx, y*cell->dx);
    }
#endif

    if (selected <= 0) return;

#if 0 //DEBUG candidates
    soa_candidates candidates;

    cell->find_candidates(in_cell % cell->w, in_cell / cell->w, candidates);

    for (int i = 0; i < candidates.size(); i++) {
        int idx = candidates.indices[i];
        //std::cout << idx << std::endl;

        fp x = particles.x[idx] - w2;
        fp y = particles.y[idx] - w2;
        scene->addEllipse(x, y, w, w, QPen(Qt::red), QBrush(Qt::red));
    }
#endif

#if 1
    fp x = particles.x[selected];
    fp y = particles.y[selected];
    QGraphicsEllipseItem *item = scene->addEllipse(x - w2, y - w2, w, w, QPen(Qt::green), QBrush(Qt::green));
    item->setZValue(1.0);
    fp dx = cell->dx;
    scene->addEllipse(x - dx/2.0, y - dx/2.0, dx, dx);
#endif

    std::vector<int> &sel_indices = indices[selected];

    qDebug() << " in cell" << in_cell % cell->w << ", " << in_cell / cell->w;

    for (int i = 0; i < sel_indices.size(); i++) {
        fp x = particles.x[sel_indices[i]] - w2;
        fp y = particles.y[sel_indices[i]] - w2;
        scene->addEllipse(x, y, w, w, QPen(Qt::red), QBrush(Qt::red));
    }

    //update();
}
