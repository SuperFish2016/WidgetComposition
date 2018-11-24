#include "graphicswidgetcomposition.h"
#include <QStyleOptionGraphicsItem>
GraphicsWidgetComposition::GraphicsWidgetComposition(QGraphicsItem* parent)
    :GraphicsWidgetBase(parent)
{
    createLayout();
}

void GraphicsWidgetComposition::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF segment_rect(boundingRect());
    QPen pen;
    pen.setWidth(0);
    QBrush brush(Qt::SolidPattern);
    QRectF exposed_rect(option->exposedRect);
    if(exposed_rect.left() - 1 >= boundingRect().left())
        exposed_rect.adjust(-1, 0, 0, 0);
    if(exposed_rect.right() + 1 <= boundingRect().right())
        exposed_rect.adjust(0, 0, 1, 0);
    QRectF visible_rect(segment_rect.intersected(exposed_rect));
    visible_rect.adjust(0, 0, -1. / painter->transform().m11(), -1. / painter->transform().m22());
    if(visible_rect.isEmpty() == false) {
        QColor color(Qt::red);
        color.setAlpha(50);
        brush.setColor(color);
        pen.setColor(color);
        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawRect(visible_rect);
    }
}

void GraphicsWidgetComposition::createLayout()
{
    layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    layout_->setContentsMargins(0,0,0,0);
    layout_->setSpacing(0);

    segment_ = new GraphicsWidgetSegment(this);
    layout_->insertItem(0, segment_);
    this->setLayout(layout_);
}

void GraphicsWidgetComposition::add_segment(GraphicsWidgetSegment *segment, int index)
{
    if(index <= 0)
        layout_->insertItem(0, segment);
    else if(index >= layout_->count())
        layout_->insertItem(layout_->count(), segment);
    else
        layout_->insertItem(index, segment);
}

void GraphicsWidgetComposition::delete_segment(GraphicsWidgetSegment *segment)
{
    for(int i = 0; i < layout_->count(); i++) {
        if(layout_->itemAt(i) == segment) {
            layout_->removeAt(i);
            break;
        }
    }
}

QSizeF GraphicsWidgetComposition::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    return QGraphicsWidget::sizeHint(which, constraint);
}

void GraphicsWidgetComposition::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QGraphicsScene* ptr_scene = scene();
    if(ptr_scene)
    {
        QRectF scene_rect = boundingRect();
        ptr_scene->setSceneRect(scene_rect);
    }
}


