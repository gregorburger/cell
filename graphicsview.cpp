#include "graphicsview.h"
#include "cell.h"

#include <boost/random.hpp>
#include <QDebug>
#include <QResizeEvent>

boost::random::mt19937 rng;

GraphicsView::GraphicsView(QWidget *parent) :
    QGraphicsView(parent) {
    scene = new QGraphicsScene();
    this->setScene(scene);
    cell = new Cell(1.0, 1.0, 0.1);

    boost::random::uniform_01<> dist;

    for (int i = 0; i < particles.size(); ++i) {
        particles[i].x = dist(rng);
        particles[i].y = dist(rng);
    }

    particles.push_back(vector{0, 0});

    //particles[2001].x=100;

    cell->addParticles(particles);
    render();
    fitInView(-0.1, -0.1, 1.1, 1.1, Qt::KeepAspectRatio);
}

void GraphicsView::resizeEvent(QResizeEvent *e) {
    fitInView(-0.1, -0.1, 1.1, 1.1, Qt::KeepAspectRatio);
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    vector v;
    v.x = fp(mapToScene(event->pos()).x());
    v.y = fp(mapToScene(event->pos()).y());
    nb = cell->nb(v);
    render();
}

void GraphicsView::render() {
#if 1
    delete scene;
    scene = new QGraphicsScene();
    setScene(scene);
#else
    scene->clear();
#endif
    double w = 0.01;
    double w2 = w/2.0;
    foreach(const vector &v, particles) {
        scene->addEllipse(v.x - w2, v.y - w2, w, w, QPen(Qt::black));
    }

    foreach(const vector &v, nb) {
        scene->addEllipse(v.x  - w2, v.y - w2, w, w, QPen(Qt::red));
    }

    for (int x = 0; x < cell->w; ++x) {
        scene->addLine(x*cell->dx, 0, x*cell->dx, cell->w*cell->dx);
    }

    for (int y = 0; y < cell->h; ++y) {
        scene->addLine(0, y*cell->dx, cell->h*cell->dx, y*cell->dx);
    }
    //update();
}
