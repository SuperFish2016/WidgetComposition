#include "graphicscommon.h"
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QToolTip>
#include <QMenu>
#include "graphicsscene.h"
#include "../utils/commands.h"
GraphicsWidgetBase::GraphicsWidgetBase(QGraphicsItem *parent)
    :QGraphicsWidget(parent)
{

}
GraphicsObjectVerticalIndicator::GraphicsItemLine::GraphicsItemLine(qreal width, qreal height, GraphicsObjectVerticalIndicator *parent)
    :QGraphicsItem(parent), line_size_(width, height)
{
    setColor(QColor(Qt::gray));
    setFlags(QGraphicsItem::ItemUsesExtendedStyleOption | QGraphicsItem::ItemStacksBehindParent);
}

void GraphicsObjectVerticalIndicator::GraphicsItemLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr*/) {

    QRectF indicator_rect(boundingRect());
    QPen pen;
    pen.setWidth(0); // cosmetic
    QBrush brush(Qt::SolidPattern);
    brush.setColor(color_);
    pen.setColor(color_);
    QRectF exposed_rect(option->exposedRect);
    if(exposed_rect.left() - 1 >= boundingRect().left())
        exposed_rect.adjust(-1, 0, 0, 0);
    if(exposed_rect.right() + 1 <= boundingRect().right())
        exposed_rect.adjust(0, 0, 1, 0);
    QRectF visible_rect(indicator_rect.intersected(exposed_rect));
    if(!visible_rect.isEmpty()) {
        painter->setPen(pen);
        painter->setBrush(brush);
//        visible_rect.adjust(-1, 0, -2, -1);
//        if(visible_rect.width() >= 1)
//            painter->drawRect(visible_rect);
//        else
        painter->drawLine(visible_rect.topLeft(), visible_rect.bottomLeft());
    }
}

GraphicsObjectVerticalIndicator::GraphicsObjectVerticalIndicator(qreal width, qreal height, const QColor& color, QGraphicsItem *parent)
    :GraphicsObjectBase(parent), color_(color),head_size_(12,17)
{
    line_ = new GraphicsItemLine(1, height, this); // 设置当前帧指示器的线部分
    line_->setColor(color);
    is_extend_grid_ = false;
    hideHead();
    setTransform(QTransform::fromScale(1 / GetViewTransform().m11(), 1));
    setFlags(QGraphicsItem::ItemUsesExtendedStyleOption | QGraphicsItem::ItemSendsGeometryChanges);
}

QRectF GraphicsObjectVerticalIndicator::boundingRect() const
{
    QRectF rect(QPointF(0, 0), QSizeF(head_size_));
    rect.moveLeft((int)(-rect.width() / 2));
    return rect;
}

void GraphicsObjectVerticalIndicator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF head_rect(boundingRect());
    QPen pen;
    pen.setWidth(0); // cosmetic
    QBrush brush(Qt::SolidPattern);

    QColor color(color_);
    QColor light_color(color_.lighter());
    QColor dark_color(color_.darker());

    QRectF exposed_rect(option->exposedRect);
    if(exposed_rect.left() - 1 >= boundingRect().left())
        exposed_rect.adjust(-1, 0, 0, 0);
    if(exposed_rect.right() + 1 <= boundingRect().right())
        exposed_rect.adjust(0, 0, 1, 0);
    QRectF visible_rect(head_rect.intersected(exposed_rect));
    head_rect.adjust(0, 0, -1, -1);

    const QPointF points[5] = {
        QPointF(head_rect.center().x(), head_rect.bottom()),
        QPointF(head_rect.left(), head_rect.bottom() - (head_rect.center().x() - head_rect.left())),
        head_rect.topLeft(),
        head_rect.topRight(),
        QPointF(head_rect.right(), head_rect.bottom() - (head_rect.center().x() - head_rect.left())),
    };

    QFont font;
    font.setPixelSize(boundingRect().width());
    QFontMetricsF font_metrics(font);
    QString text = font_metrics.elidedText(text_, Qt::ElideRight, points[1].y() - points[2].y());

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->save();
    if(!visible_rect.isEmpty()) {
        if(acceptHoverEvents() == true && option->state & QStyle::State_MouseOver) {
            if(option->state & QStyle::State_Selected) {
                pen.setColor(dark_color);
                brush.setColor(dark_color);
            }
            else {
                pen.setColor(light_color);
                brush.setColor(light_color);
            }
            if(1) {
                painter->setFont(font);
                painter->setPen(pen);
                painter->setBrush(brush);
                painter->drawPolygon(points, 5);
                if(!text_.isEmpty()) {
                    pen.setColor(QColor(CPL_FONT_COLOR));
                    QTransform transf(painter->transform());
                    transf.rotate(-90).translate(-points[4].y() - points[3].y(), font_metrics.height() / 2 - 1.5);
                    painter->setTransform(transf);
                    painter->setPen(pen);
                    painter->drawText(0, 0, text);
                }
            }
        }
        else {
            if(option->state & QStyle::State_Selected) {
                pen.setColor(dark_color);
                brush.setColor(dark_color);
            }
            else {
                pen.setColor(color);
                brush.setColor(color);
            }
            if(1) {
                painter->setFont(font);
                painter->setPen(pen);
                painter->setBrush(brush);
                painter->drawPolygon(points, 5);
                if(text_.isEmpty() == false) {
                    pen.setColor(QColor(CPL_FONT_COLOR));
                    QTransform transf(painter->transform());
                    transf.rotate(-90).translate(-points[4].y() - points[3].y(), font_metrics.height() / 2 - 1.5);
                    painter->setTransform(transf);
                    painter->setPen(pen);
                    painter->drawText(0, 0, text);
                }
            }
        }
    }
    painter->restore();
}

// This make sure the indicator is not changed when do scale timeline view
void GraphicsObjectVerticalIndicator::ViewTransformEvent(const QTransform &trans)
{
    setTransform(QTransform::fromScale(1 / GetViewTransform().m11(), 1));
}

QVariant GraphicsObjectVerticalIndicator::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionChange) {
        QPointF new_pos = value.toPointF();
        if(new_pos.x() != pos().x())
            emit sig_xPos_changed(new_pos.x());
    }
    return GraphicsObjectBase::itemChange(change, value);
}

GraphicsWidgetMarkerIndicator::GraphicsWidgetMarkerIndicator(qreal width, qreal height)
    :GraphicsObjectVerticalIndicator(width, height, QColor(CPL_COLOR_DEFAULT_MARKER), nullptr)
{
    showHead();
    hideLine();
    setAcceptHoverEvents(true);
    setGridExtention(true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

QRectF GraphicsWidgetMarkerIndicator::boundingRect() const
{
    QRectF rect(QPointF(0, 0), QSizeF(8, 100));
    rect.moveLeft((int)(-rect.width() / 2));
    return rect;
}

void GraphicsWidgetMarkerIndicator::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF head_rect(boundingRect());
    QPen pen;
    pen.setWidth(0); // cosmetic
    QBrush brush(Qt::SolidPattern);

    QColor color = QColor(Qt::gray);
    QColor light_color(color.lighter());
    QColor dark_color(color.darker());

    QColor brush_color(QColor("#8b4513"));
    QColor brush_light_color(brush_color.lighter());
    QColor brush_lighter_color(brush_color.lighter(180));

    QRectF exposed_rect(option->exposedRect);
    if(exposed_rect.left() - 1 >= boundingRect().left())
        exposed_rect.adjust(-1, 0, 0, 0);
    if(exposed_rect.right() + 1 <= boundingRect().right())
        exposed_rect.adjust(0, 0, 1, 0);
    QRectF visible_rect(head_rect.intersected(exposed_rect));
    head_rect.adjust(0, 0, -1, -1);

    const QPointF points[5] = {
        QPointF(0, 12),
        QPointF(-4,8),// head_rect.bottom() - (head_rect.center().x() - head_rect.left())),
        QPointF(-4, 0),
        QPointF(4, 0),
        QPointF(4,8)//head_rect.right(), head_rect.bottom() - (head_rect.center().x() - head_rect.left())),
    };

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->save();
    if(!visible_rect.isEmpty()) {
        if(acceptHoverEvents() == true && option->state & QStyle::State_MouseOver) {
            if(option->state & QStyle::State_Selected) {
                pen.setColor(light_color);
                brush.setColor(brush_lighter_color);
            }
            else {
                pen.setColor(light_color);
                brush.setColor(brush_light_color);
            }
            if(1) {
                painter->setPen(pen);
                painter->setBrush(brush);
                painter->drawPolygon(points, 5);
                pen.setStyle(Qt::DashLine);
                pen.setColor(brush_light_color);
                painter->setPen(pen);
                painter->drawLine(QPointF(head_rect.center().x() + 0.5 ,0), QPointF(head_rect.center().x() + 0.5, head_rect.bottom()));
            }
        }
        else {
            if(option->state & QStyle::State_Selected) {
                pen.setColor(light_color);
                brush.setColor(brush_lighter_color);
            }
            else {
                pen.setColor(color);
                brush.setColor(brush_light_color);
            }
            if(1) {
                painter->setPen(pen);
                painter->setBrush(brush);
                painter->drawPolygon(points, 5);
            }
            pen.setStyle(Qt::DashLine);
            pen.setWidth(0);
            pen.setColor(brush_light_color);
            painter->setPen(pen);
            painter->drawLine(QPointF(head_rect.center().x() + 0.5 ,0), QPointF(head_rect.center().x() +0.5 , head_rect.bottom()));
        }
    }
    painter->restore();
}

void GraphicsWidgetMarkerIndicator::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setZValue(1);
    GraphicsObjectVerticalIndicator::mousePressEvent(event);
    if(event->button() == Qt::LeftButton)
    {
        is_mouse_pressed_ = true;
    }
}

void GraphicsWidgetMarkerIndicator::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    GraphicsObjectVerticalIndicator::mouseMoveEvent(event);
    this->setX(qint64(event->scenePos().x() + .5));
}

void GraphicsWidgetMarkerIndicator::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    GraphicsObjectVerticalIndicator::mousePressEvent(event);
    if(event->button() == Qt::LeftButton)
    {
        is_mouse_pressed_ = false;
    }
    setZValue(0);
}

void GraphicsWidgetMarkerIndicator::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    update();
    QToolTip::showText(event->screenPos(), "Marker");
}

void GraphicsWidgetMarkerIndicator::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    update();
    QToolTip::showText(event->screenPos(), "");
}

void GraphicsWidgetMarkerIndicator::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction* action_edit = menu.addAction("Edit");
    QAction* action_delete = menu.addAction("Delete");
    QAction* action_select =  menu.exec(event->screenPos());
    if(action_delete == action_select)
    {
        GraphicsSceneTimeline* ptr_scene = qobject_cast<GraphicsSceneTimeline*>(scene());
        if(ptr_scene)
        {
            ptr_scene->sig_push_command(new DeleteMarkerCommand(this, nullptr));
        }
    }
    else if(action_edit == action_select)
    {

    }
}
