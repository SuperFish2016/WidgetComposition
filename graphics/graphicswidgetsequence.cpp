#include "graphicswidgetsequence.h"
#include "graphicswidgetsegment.h"
#include "graphicswidgetresources.h"
#include <QGraphicsLinearLayout>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
GraphicsWidgetSequence::GraphicsWidgetSequence(GraphicsWidgetSegment* parent, eSequenceType type, const QUuid& track_id, const QUuid& id)
    :GraphicsWidgetBase(parent),type_(type),id_(id), track_id_(track_id)
{
    height_ = HEIGHT_TRACK;
    setFlags(QGraphicsItem::ItemUsesExtendedStyleOption | QGraphicsItem::ItemStacksBehindParent);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    createLayout();
}

void GraphicsWidgetSequence::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *pWidget /*= nullptr*/)
{
    QRectF segment_rect(boundingRect());
    QPen pen;
    pen.setWidth(0); // cosmetic
    QBrush brush(Qt::SolidPattern);
    brush.setColor(QColor(COLOR_SEQUENCE_BG));

    QRectF exposed_rect(option->exposedRect);
    if(exposed_rect.left() - 1 >= boundingRect().left())
        exposed_rect.adjust(-1, 0, 0, 0);
    if(exposed_rect.right() + 1 <= boundingRect().right())
        exposed_rect.adjust(0, 0, 1, 0);
    QRectF visible_rect(segment_rect.intersected(exposed_rect));
    visible_rect.adjust(0, 0, -1. / painter->transform().m11(), -1. / painter->transform().m22());
    if(!visible_rect.isEmpty()) {
        pen.setColor(QColor(COLOR_SEQUENCE_BG));
        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawRect(visible_rect);
        pen.setColor(QColor(COLOR_SEQUENCE_BORDER));
        painter->setPen(pen);
        if(exposed_rect.bottom() >= segment_rect.bottom())
            painter->drawLine(visible_rect.bottomLeft(), visible_rect.bottomRight());
    }
}

void GraphicsWidgetSequence::createLayout()
{
    layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    layout_->setContentsMargins(0,0,0,0); /// this control resource border.
    layout_->setSpacing(0);
    this->setLayout(layout_);
}

bool GraphicsWidgetSequence::extendGrid(QPointF &point, eGridPosition which)
{
    QPointF ret(mapFromScene(point));
    switch(type_)
    {
    case MainImageSequence:
        if(which == VideoHorizontal)
            ret.setY(boundingRect().center().y());
        else
            return false;
        break;
    case MainAudioSequence:
        if(which == AudioHorizontal)
            ret.setY(boundingRect().center().y());
        else
            return false;
        break;
    }
    point = mapToScene(ret);
    return true;
}

QSizeF GraphicsWidgetSequence::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
        QSizeF size;
        if(constraint.isValid() == false) {
            switch(which) {
                case Qt::MinimumSize:
                    size = QSizeF(0, height_);
                    break;
                case Qt::PreferredSize:
                    size = QSizeF(0, height_);
                    break;
                case Qt::MaximumSize:
                    size = QSizeF(std::numeric_limits<qint32>::max(), std::numeric_limits<qint32>::max());
                    break;
                case Qt::MinimumDescent:
                    size = QSizeF(-1, -1);
                    break;
                case Qt::NSizeHints:
                    size = QSizeF(-1, -1);
                    break;
                default:
                    size = QSizeF(-1, -1);
                    break;
            }
        }
        else {
            qWarning() << "sizeHint() is constraint.";
            size = constraint;
        }
        return size;
}

void GraphicsWidgetSequence::addResource(AbstractGraphicsWidgetResources *resource, int index) {

    if(index <= 0) {
        layout_->insertItem(0, resource);
    }
    else if(index >= resourceCount()) {
        layout_->insertItem(resourceCount(), resource);
    }
    else {
        layout_->insertItem(index, resource);
    }
}

void GraphicsWidgetSequence::deleteResource(AbstractGraphicsWidgetResources *resource) {

    for(int i = 0; i < resourceCount(); i++) {
        if(layout_->itemAt(i) == resource) {
            layout_->removeAt(i);
            break;
        }
    }
}

AbstractGraphicsWidgetResources* GraphicsWidgetSequence::resource(int Index) const {

    AbstractGraphicsWidgetResources *p_resource = nullptr;
    if(Index <= 0) {
        p_resource = dynamic_cast<AbstractGraphicsWidgetResources*>(layout_->itemAt(0));
    }
    else if(Index >= resourceCount()) {
        p_resource = dynamic_cast<AbstractGraphicsWidgetResources*>(layout_->itemAt(resourceCount() - 1));
    }
    else {
        p_resource = dynamic_cast<AbstractGraphicsWidgetResources*>(layout_->itemAt(Index));
    }
    return p_resource;
}

AbstractGraphicsWidgetResources* GraphicsWidgetSequence::resource(const QUuid &rId) const {

    for(int i = 0; i < resourceCount(); i++) {
        AbstractGraphicsWidgetResources *p_resource = dynamic_cast<AbstractGraphicsWidgetResources*>(layout_->itemAt(i));
        if(p_resource && p_resource->id() == rId) {
            return p_resource;
        }
    }
    return nullptr;
}

QList<AbstractGraphicsWidgetResources*> GraphicsWidgetSequence::resourceList() const {

    QList<AbstractGraphicsWidgetResources*> resources;
    for(int i = 0; i < resourceCount(); i++) {
        resources.push_back(resource(i));
    }
    return resources;
}

