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
    std::vector<vector> particles = std::vector<vector>(1000);
    std::vector<vector> nb;
};

#endif // GRAPHICSVIEW_H
