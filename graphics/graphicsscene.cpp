#include "graphicsscene.h"
#include "graphicswidgettimeline.h"
#include "graphicswidgetcomposition.h"
#include <QGraphicsSceneMouseEvent>
GraphicsSceneBase::GraphicsSceneBase(const EditRate &CplEditRate, QObject* parent):
    QGraphicsScene(parent),snap_width_(26),cpl_edit_rate_(EditRate::EditRate24)
{
    connect(this, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(slot_scene_rect_changed(QRectF)));
}

GraphicsSceneBase::GridInfo GraphicsSceneBase::SnapToGrid(const QPointF &rPoint, GridPosition which, const QRectF &rSearchRect /*= QRectF()*/, QList<AbstractGridExtension*> ignoreItems /*= QList<AbstractGridExtension*>()*/) const {

    QGraphicsView *p_view(NULL);
    QRectF search_rect(rSearchRect);
    if(views().isEmpty() == false) p_view = views().first();
    QRectF snap_rect(0, 0, snap_width_, sceneRect().height());
    if(p_view) snap_rect.setWidth(snap_rect.width() / p_view->transform().m11());
    snap_rect.moveCenter(QPointF(rPoint.x(), snap_rect.center().y()));
    if(search_rect.isEmpty() == true) search_rect = snap_rect;
    QList<AbstractGridExtension*> grid_items(AddPermanentSnapItems());
    QList<QGraphicsItem*> graphics_items = items(search_rect.intersected(snap_rect), Qt::IntersectsItemBoundingRect);
    for(int i = 0; i < graphics_items.size(); i++) grid_items.push_back(dynamic_cast<AbstractGridExtension*>(graphics_items.at(i)));
    GridInfo ret;
    ret.SnapPos = rPoint;
    ret.IsHoizontalSnap = false;
    ret.IsVerticalSnap = false;
    ret.HorizontalOrigin = NULL;
    ret.VerticalOrigin = NULL;
    ret.HeightAdvice = -1;
    ret.ColorAdvice = QColor();
    QList<QPair<qreal, AbstractGridExtension*> > vertical_sigularities;
    QList<QPair<qreal, AbstractGridExtension*> > horizontal_sigularities;
    for(int i = 0; i < grid_items.size(); i++) {
        AbstractGridExtension *p_base_graphics_widget = grid_items.at(i);
        if(p_base_graphics_widget) {
            if(ignoreItems.contains(p_base_graphics_widget) == true) continue;
            bool use = false;
            if(which & Vertical) {
                QPointF proposed_point(rPoint);
                use = p_base_graphics_widget->extendGrid(proposed_point, Vertical);
                if(use && snap_rect.contains(proposed_point)) vertical_sigularities.push_back(QPair<qreal, AbstractGridExtension*>(proposed_point.x(), p_base_graphics_widget));
            }
            if(which & VideoHorizontal) {
                QPointF proposed_point(rPoint);
                use = p_base_graphics_widget->extendGrid(proposed_point, VideoHorizontal);
                if(use && snap_rect.contains(proposed_point)) horizontal_sigularities.push_back(QPair<qreal, AbstractGridExtension*>(proposed_point.y(), p_base_graphics_widget));
            }
            if(which & AudioHorizontal) {
                QPointF proposed_point(rPoint);
                use = p_base_graphics_widget->extendGrid(proposed_point, AudioHorizontal);
                if(use && snap_rect.contains(proposed_point)) horizontal_sigularities.push_back(QPair<qreal, AbstractGridExtension*>(proposed_point.y(), p_base_graphics_widget));
            }
            if(which & TimedTextHorizontal) {
                QPointF proposed_point(rPoint);
                use = p_base_graphics_widget->extendGrid(proposed_point, TimedTextHorizontal);
                if(use && snap_rect.contains(proposed_point)) horizontal_sigularities.push_back(QPair<qreal, AbstractGridExtension*>(proposed_point.y(), p_base_graphics_widget));
            }
            if(which & DataHorizontal) {
                QPointF proposed_point(rPoint);
                use = p_base_graphics_widget->extendGrid(proposed_point, DataHorizontal);
                if(use && snap_rect.contains(proposed_point)) horizontal_sigularities.push_back(QPair<qreal, AbstractGridExtension*>(proposed_point.y(), p_base_graphics_widget));
            }
        }
    }
    AbstractGridExtension *p_final_snap_item_vertical = NULL;
    qreal min_distance_vertical = std::numeric_limits<qreal>::max();
    for(int i = 0; i < vertical_sigularities.size(); i++) {
        qreal distance = std::abs((int)(rPoint.x() - vertical_sigularities.at(i).first));
        if(distance < min_distance_vertical) {
            min_distance_vertical = distance;
            ret.SnapPos.setX(vertical_sigularities.at(i).first);
            ret.IsVerticalSnap = true;
            p_final_snap_item_vertical = vertical_sigularities.at(i).second;
        }
        else if(distance == min_distance_vertical) {
            if(QGraphicsItem *p_item = dynamic_cast<QGraphicsItem*>(vertical_sigularities.at(i).second)) {
                QPointF probe = p_item->mapFromScene(rPoint.x(), 0);
                if(p_item->contains(QPointF(probe.x(), p_item->boundingRect().center().y()))) {
                    min_distance_vertical = distance;
                    ret.SnapPos.setX(vertical_sigularities.at(i).first);
                    ret.IsVerticalSnap = true;
                    p_final_snap_item_vertical = vertical_sigularities.at(i).second;
                }
            }
        }
    }
    AbstractGridExtension *p_final_snap_item_horizontal = NULL;
    qreal min_distance_horizontal = std::numeric_limits<qreal>::max();
    for(int i = 0; i < horizontal_sigularities.size(); i++) {
        qreal distance = std::abs((int)(rPoint.y() - horizontal_sigularities.at(i).first));
        if(distance < min_distance_horizontal) {
            min_distance_horizontal = distance;
            ret.SnapPos.setY(horizontal_sigularities.at(i).first);
            ret.IsHoizontalSnap = true;
            p_final_snap_item_horizontal = horizontal_sigularities.at(i).second;
        }
        else if(distance == min_distance_horizontal) {
            if(QGraphicsItem *p_item = dynamic_cast<QGraphicsItem*>(horizontal_sigularities.at(i).second)) {
                QPointF probe = p_item->mapFromScene(0, rPoint.y());
                if(p_item->contains(QPointF(p_item->boundingRect().center().x(), probe.y()))) {
                    min_distance_horizontal = distance;
                    ret.SnapPos.setY(horizontal_sigularities.at(i).first);
                    ret.IsHoizontalSnap = true;
                    p_final_snap_item_horizontal = horizontal_sigularities.at(i).second;
                }
            }
        }
    }
    if(p_final_snap_item_vertical) {
        ret.ColorAdvice = p_final_snap_item_vertical->ColorAdviceForGrid();
        ret.VerticalOrigin = p_final_snap_item_vertical;
    }
    if(p_final_snap_item_horizontal) {
        ret.HeightAdvice = p_final_snap_item_horizontal->HeightAdviceForHorizontalGrid();
        ret.HorizontalOrigin = p_final_snap_item_horizontal;
    }
    return ret;
}

GraphicsSceneBase::GridInfo GraphicsSceneBase::SnapToGrid(const QPointF &rPoint, GridPosition which, const QRectF &rSearchRect /*= QRectF()*/, AbstractGridExtension *pIgnoreItem /*= NULL*/) const {

    QList<AbstractGridExtension*> ignore_list;
    ignore_list << pIgnoreItem;
    return SnapToGrid(rPoint, which, rSearchRect, ignore_list);
}

GraphicsSceneComposition::GraphicsSceneComposition()
{
    composition_graphics_ = new GraphicsWidgetComposition();
    addItem(composition_graphics_);
    current_frame_indicator_ = new GraphicsObjectVerticalIndicator(2, sceneRect().height(), QColor(COLOR_FRAME_INDICATOR), nullptr);
    addItem(current_frame_indicator_);
    current_frame_indicator_->hideHead();
    current_frame_indicator_->setGridExtention(true);
    current_frame_indicator_->setZValue(2);
    connect(composition_graphics_, &GraphicsWidgetComposition::geometryChanged, this, &GraphicsSceneComposition::slot_composition_geometry_changed);
}

QList<AbstractGridExtension*> GraphicsSceneComposition::AddPermanentSnapItems() const {

    QList<AbstractGridExtension*> ret;
    ret << current_frame_indicator_;
    return ret;
}


void GraphicsSceneComposition::slot_composition_geometry_changed()
{
    if(current_frame_indicator_)
        current_frame_indicator_->setHeight(composition_graphics_->boundingRect().height());
}

GraphicsSceneTimeline::GraphicsSceneTimeline(const EditRate &ed, QObject *parent)
    :GraphicsSceneBase(ed, parent)
{
    widget_timeline_ = new GraphicsWidgetTimeline();
    addItem(widget_timeline_);
    snap_indicator_ = new GraphicsObjectVerticalIndicator(1, sceneRect().height(), QColor(CPL_COLOR_DEFAULT_SNAP_INDICATOR), nullptr);
    addItem(snap_indicator_);
    snap_indicator_->setZValue(2);
    snap_indicator_->hide();

    // old use indicator.png
    current_frame_indicator_ = new GraphicsObjectVerticalIndicator(1, 20, QColor(COLOR_FRAME_INDICATOR), nullptr);
    current_frame_indicator_->showHead();
    current_frame_indicator_->hideLine();
    current_frame_indicator_->setPos(0, 29);
    current_frame_indicator_->setZValue(2);
    addItem(current_frame_indicator_);
    connect(widget_timeline_, &GraphicsWidgetTimeline::geometryChanged, this, &GraphicsSceneTimeline::slot_timeline_geometry_changed);
    connect(current_frame_indicator_, &GraphicsObjectVerticalIndicator::sig_xPos_changed, this, &GraphicsSceneTimeline::slot_x_pos_changed);

    // add default marker
    for(int i = 0; i < 5; i++)
    {
        GraphicsWidgetMarkerIndicator* marker = new GraphicsWidgetMarkerIndicator(1, 40);
        addItem(marker);
        marker->setPos(50 * i, 0);
        marker->setZValue(1);
    }
}

void GraphicsSceneTimeline::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    GraphicsSceneBase::mousePressEvent(e);
    if(e->button() == Qt::LeftButton)
    {
        if(mouseGrabberItem() == nullptr)
        {
            current_frame_indicator_->setX((qint64)(e->scenePos().x() + 0.5));
        }
    }
}

void GraphicsSceneTimeline::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    GraphicsSceneBase::mouseMoveEvent(e);
    if(e->button() != Qt::LeftButton) return;
    {

    }
}

void GraphicsSceneTimeline::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{

}

void GraphicsSceneTimeline::slot_timeline_geometry_changed()
{
    if(current_frame_indicator_)
        current_frame_indicator_->setPos(current_frame_indicator_->pos().x(),  widget_timeline_->boundingRect().bottom() -
                                         current_frame_indicator_->boundingRect().height());
}
