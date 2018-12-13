#ifndef GRAPHICSWIDGETTIMELINE_H
#define GRAPHICSWIDGETTIMELINE_H
#include <QGraphicsLinearLayout>
#include <QButtonGroup>
#include "graphicscommon.h"
class GraphicsWidgetTimeline : public GraphicsWidgetBase
{
    Q_OBJECT
private:
    class TimelinePainter : public GraphicsWidgetBase
    {
    public:
        TimelinePainter(GraphicsWidgetTimeline* parent);
        virtual ~TimelinePainter(){}
        virtual int type() const{return TimelinePainterType;}
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    protected:
        virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;
    };
public:
    GraphicsWidgetTimeline(QGraphicsItem* parent = nullptr);
    virtual ~GraphicsWidgetTimeline(){}
    virtual int type() const{return GraphicsWidgetTimelineType;}
    bool is_paint_with_frames() const{return is_show_frames_;}
protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent* e);
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;
private:
    void creatLayout();
public slots:
    void setHeight(qint32 height);
    void slot_show_frames_toggled(bool checked){is_show_frames_ = checked; timeline_painter_->update();}
private:
    Q_DISABLE_COPY(GraphicsWidgetTimeline)
    bool   is_show_frames_ = false;
    qint32 height_;
    QGraphicsLinearLayout* timeline_layout_;
    TimelinePainter*       timeline_painter_;
    QButtonGroup*          button_group_;
};

#endif // GRAPHICSWIDGETTIMELINE_H
