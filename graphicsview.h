#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

#include "cell.h"

class GraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = 0);

    void resizeEvent(QResizeEvent *);

    void mousePressEvent(QMouseEvent *event);

signals:
    
public slots:
private:
    void render();
    QGraphicsScene *scene;
    Cell *cell;
    soa_vectors particles;
    int selected = -1;
    int in_cell = -1;
    std::vector<std::vector<fp> > dists;
    std::vector<std::vector<int> > indices;
};

#endif // GRAPHICSVIEW_H
