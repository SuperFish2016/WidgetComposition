#ifndef GRAPHICSSCENEBASE_H
#define GRAPHICSSCENEBASE_H
#include <QGraphicsScene>
#include "graphicscommon.h"
#include "imf/imfcommon.h"
class GraphicsWidgetComposition;
class GraphicsWidgetTimeline;
class GraphicsObjectVerticalIndicator;
class AbstractGraphicsWidgetResource;
class QUndoCommand;
typedef unsigned int GridPosition;
typedef unsigned int SequenceTypes;

// Scene的基类
class GraphicsSceneBase : public QGraphicsScene
{
    Q_OBJECT
public:
    struct GridInfo
    {
      bool IsHorizontalSnap;
      bool IsVerticalSnap;
      QPointF   SnapPos;
      qreal     HeightAdvice;
      QColor    ColorAdvice;
    };
    GraphicsSceneBase(const EditRate& CplEditRate = EditRate::EditRate24, QObject* parent = nullptr);
    virtual ~GraphicsSceneBase(){}

    EditRate GetEditRate() const{return cpl_edit_rate_;}
    void SetCplEditRate(const EditRate& edit_rate){cpl_edit_rate_ = edit_rate;}
    void SetSnapWidth(int width){snap_width_ = width;}

    GridInfo SnapToGrid(const QPointF& p, GridPosition which, const QRectF& searchRect = QRectF(),
                        QList<AbstractGridExtension*> ignoreItems = QList<AbstractGridExtension*>()) const;
    GridInfo SnapToGrid(const QPointF& p, GridPosition which, const QRectF& searchRect = QRectF(),
                        AbstractGridExtension* ignoreItems = nullptr) const;

signals:
    void sig_clear_selection();
private slots:
    void slot_scene_rect_changed(const QRectF& rect){GraphicsSceneResizeEvent(rect);}
protected:
    virtual void GraphicsSceneResizeEvent(const QRectF& r){}

private:
    Q_DISABLE_COPY(GraphicsSceneBase)
    EditRate cpl_edit_rate_;
    qint32   snap_width_;
};

// Compostion 的场景，主要存放了一个Composition Widget

class GraphicsSceneComposition : public GraphicsSceneBase
{
    Q_OBJECT
public:
    GraphicsSceneComposition();
    virtual ~GraphicsSceneComposition(){}

    GraphicsWidgetComposition* composition() const { return composition_graphics_; }
    GraphicsObjectVerticalIndicator* frameIndicator() const{return current_frame_indicator_;}
private slots:
    void slot_composition_geometry_changed();
private:
    Q_DISABLE_COPY(GraphicsSceneComposition)
    GraphicsWidgetComposition*          composition_graphics_;
    GraphicsObjectVerticalIndicator*    current_frame_indicator_;
    GraphicsObjectVerticalIndicator*    snap_indicator_;
    AbstractGraphicsWidgetResource*     ghost_resource_;
};

// Timeline 刻度尺的 scene， 该控件下面就是composition 的scene
class GraphicsSceneTimeline : public GraphicsSceneBase
{
    Q_OBJECT
public:
    GraphicsSceneTimeline(const EditRate& ed, QObject* parent);
    virtual ~GraphicsSceneTimeline(){}
    GraphicsWidgetTimeline* timeline() const{return widget_timeline_;}
    GraphicsObjectVerticalIndicator* frameIndicator() const{return current_frame_indicator_;}

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* e);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* e);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);
signals:
    void sig_current_frame_changed(const Timecode& newFrame);
    void sig_push_command(QUndoCommand *command); //eg:delete marker.
private slots:
    void slot_x_pos_changed(qreal xPos)
    {
        emit sig_current_frame_changed(Timecode(GetEditRate(), xPos));
    }
    void slot_timeline_geometry_changed();
private:
    Q_DISABLE_COPY(GraphicsSceneTimeline)

    GraphicsWidgetTimeline* widget_timeline_;
    GraphicsObjectVerticalIndicator* current_frame_indicator_;
    GraphicsObjectVerticalIndicator* snap_indicator_;

    bool is_drag_active_;
};



#endif // GRAPHICSSCENEBASE_H
