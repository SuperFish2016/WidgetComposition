#include "graphicswidgetsegment.h"
#include "graphicswidgetsequence.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
GraphicsWidgetSegment::GraphicsWidgetSegment(GraphicsWidgetComposition *parent, const QUuid &id)
{
    setFlags(QGraphicsItem::ItemUsesExtendedStyleOption | QGraphicsItem::ItemHasNoContents);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    createLayout();
}

void GraphicsWidgetSegment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF segmentRect(boundingRect());
    QPen pen;
    pen.setWidth(0); // cosmetic
    QBrush brush(Qt::SolidPattern);
    QColor color = Qt::red;

    QRectF exposedRect(option->exposedRect);
    if(exposedRect.left() - 1 >= boundingRect().left())
        exposedRect.adjust(-1, 0, 0, 0);
    if(exposedRect.right() + 1 <= boundingRect().right())
        exposedRect.adjust(0, 0, 1, 0);
    QRectF visibleRect(segmentRect.intersected(exposedRect));
    visibleRect.adjust(0, 0, -1. / painter->transform().m11(), -1. / painter->transform().m22());
    if(visibleRect.isEmpty() == false) {
        pen.setColor(color);
        brush.setColor(color);
        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawRect(visibleRect);
    }
}

void GraphicsWidgetSegment::createLayout()
{
    layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    layout_->setContentsMargins(0,0,0,0);
    layout_->setSpacing(0);
    this->setLayout(layout_);
}

QSizeF GraphicsWidgetSegment::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    return QSizeF(1300 , QGraphicsWidget::sizeHint(which, constraint).height());
}

GraphicsWidgetSequence* GraphicsWidgetSegment::sequence(int index) const
{
    GraphicsWidgetSequence *sequence = nullptr;
    if(index <= 0)
        sequence = dynamic_cast<GraphicsWidgetSequence*>(layout_->itemAt(0));
    else if(index >= layout_->count())
        sequence = dynamic_cast<GraphicsWidgetSequence*>(layout_->itemAt(layout_->count() - 1));
    else
        sequence = dynamic_cast<GraphicsWidgetSequence*>(layout_->itemAt(index));
    return sequence;
}

GraphicsWidgetSequence* GraphicsWidgetSegment::sequence(const QUuid &id, bool isTrackId) const
{
    for(int i = 0; i < layout_->count(); i++) {
        GraphicsWidgetSequence *sequence = dynamic_cast<GraphicsWidgetSequence*>(layout_->itemAt(i));
        if(isTrackId)
        {
            if(sequence && sequence->trackId() == id)
                return sequence;
        }
        else
        {
            if(sequence && sequence->id() == id)
                return sequence;
        }
    }
    return nullptr;
}

qint32 GraphicsWidgetSegment::sequenceIndex(GraphicsWidgetSequence *p_sequence) const
{
    for(int i = 0; i < sequenceCount(); i++)
    {
        if(p_sequence == sequence(i))
            return i;
    }
    return -1;
}

void GraphicsWidgetSegment::addSequence(GraphicsWidgetSequence *sequence, int sequenceIndex)
{
    if(sequenceIndex <= 0)
        layout_->insertItem(0, sequence);
    else if(sequenceIndex >= layout_->count())
        layout_->insertItem(layout_->count(), sequence);
    else
        layout_->insertItem(sequenceIndex, sequence);
}

void GraphicsWidgetSegment::deleteSequence(GraphicsWidgetSequence *sequence)
{
    for(int i = 0; i < layout_->count(); i++)
    {
        if(layout_->itemAt(i) == sequence)
        {
            layout_->removeAt(i);
            break;
        }
    }
}

bool GraphicsWidgetSegmentIndicator::extendGrid(QPointF &rPoint, eGridPosition which) const {

        if(which == Vertical) {
            QPointF ret(mapFromScene(rPoint));
            ret.setX(boundingRect().left());
            rPoint = mapToScene(ret);
            return true;
        }
        return false;
}

