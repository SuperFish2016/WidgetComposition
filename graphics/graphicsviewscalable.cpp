#include "graphicsviewscalable.h"
#include <QGraphicsItem>
GraphicsViewScalable::GraphicsViewScalable(QWidget* parent)
    :QGraphicsView(parent)
{
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    // Work around(skip the problem): Disable funny behavior of exposedrect.
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    //setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

void GraphicsViewScalable::zoomIn()
{
    scaleView(1 / qreal(1.1));
}

void GraphicsViewScalable::zoomOut()
{
    scaleView(qreal(1.1));
}

void GraphicsViewScalable::scaleView(qreal scaleFactor) {

    qreal factor = transform().scale(scaleFactor, 1).mapRect(QRectF(0, 0, 1, 1)).width();
    if(factor < 0.007 || factor > 50) return; /// control timeline scale level.
    scale(scaleFactor, 1);
    QList<QGraphicsItem*> items_list(items());
    for(int i = 0; i < items_list.size(); i++) {
        AbstractViewTransformNotifier* pItem = dynamic_cast<AbstractViewTransformNotifier*>(items_list.at(i));
        if(pItem) {
            pItem->ViewTransformEvent(transform());
        }
    }
}
