#include "graphicsscene.h"
#include "graphicswidgettimeline.h"
#include "graphicswidgetcomposition.h"
#include <QGraphicsSceneMouseEvent>
GraphicsSceneBase::GraphicsSceneBase(const EditRate &CplEditRate, QObject* parent):
    QGraphicsScene(parent),snap_width_(26),cpl_edit_rate_(EditRate::EditRate24)
{
    connect(this, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(slot_scene_rect_changed(QRectF)));
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
