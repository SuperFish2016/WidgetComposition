#ifndef GRAPHICSWIDGETRESOURCES_H
#define GRAPHICSWIDGETRESOURCES_H
#include "graphicscommon.h"
#include "imf/imfpackage.h"
#include <QUuid>
class GraphicsWidgetSequence;
class AbstractGraphicsWidgetResources;

enum eTrimHandlePos
{
    Left = 0,
    Right
};

class TrimHandle : public QGraphicsItem, public AbstractViewTransformNotifier
{
public:
    TrimHandle(AbstractGraphicsWidgetResources* parent, eTrimHandlePos pos);
    ~TrimHandle(){}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const{return rect_;}

    eTrimHandlePos direction() const{return pos_;}
    void setDirection(eTrimHandlePos p);
    void setHeight(qreal h);
    void setWidth(qreal w);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void ViewTransformEvent(const QTransform &rViewTransform);
    virtual QGraphicsView* GetObservableView() const;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    Q_DISABLE_COPY(TrimHandle)

    eTrimHandlePos pos_;
    qint32 mouse_x_offset_;
    bool   is_mouse_pressed_;
    QRectF rect_;
};

class DurationIndicator : public QGraphicsItem
{
public:
    DurationIndicator(AbstractGraphicsWidgetResources* parent);
    virtual ~DurationIndicator(){}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const{return rect_;}
    void setRect(const QRectF& rect){prepareGeometryChange(); rect_ = rect;}
private:
    Q_DISABLE_COPY(DurationIndicator)
    QRectF rect_;
};

class AbstractGraphicsWidgetResources : public GraphicsWidgetBase
{
    Q_OBJECT
public:
    AbstractGraphicsWidgetResources(GraphicsWidgetSequence* parent, const QColor& color);
    virtual ~AbstractGraphicsWidgetResources(){}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QUuid id() const{return id_;}

    QColor color() const{return color_;}
    void   setColor(const QColor& color){color_ = color; update();}

    QString  annotation() const{return asset_->annotation_;}
    qint32   repeatCount() const{return 1;}
    Duration entryPoint() const{return 0;}
    Duration sourceDuration() const{ return asset_->source_duration_; }
    Duration intrinsicDuration() const{return 1000;}
    EditRate editRate() const{return EditRate::EditRate24;}

    void setAnnotation(const QString& annotation){asset_->annotation_ = (annotation);}
    void setEntryPoint(const Duration& d);
    void setSourceDuration(const Duration& d);

    //! Transforms a local Timecode to an absolute Cpl timecode.
    Timecode MapToCplTimeline(const Timecode &rLocalTimecode) const;
    //! Transforms a local Duration to Cpl Duration (replaces Resource Edit Rate with Cpl Edit Rate).
    Duration MapToCplTimeline(const Duration &rLocalDuration) const;
    virtual void trimHandleInUse(eTrimHandlePos pos, bool active);
    virtual void TrimHandleMoved(eTrimHandlePos pos) {}

    void trimResource(qint32 pos, qint32 lastPos, eTrimHandlePos epos);

    void maximizeZValue();
    void restoreZValue();
    GraphicsWidgetSequence* getSequence() const;
    // QSharedPointer<AssetMxfTrack> asset_mxf_;
signals:
    void sourceDurationChanged(const Duration& old_duration, const Duration& new_duration);
    void entryPointChanged(const Duration& old_entry_point, const Duration& new_entry_point);
protected:

    virtual void resizeEvent(QGraphicsSceneResizeEvent* e);
    virtual void hideEvent(QHideEvent* e);
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* e);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* e);
    virtual bool extendGrid(QPointF &point, eGridPosition which) const;
    virtual double samplesPerFrame(const EditRate& compositionEditRate) const{return 1;}
    virtual void CPLEditRateChanged(){updateGeometry();}

    AssetMxfTrack* asset_;
    QSharedPointer<AssetMxfTrack> mAssset;
private:
    Q_DISABLE_COPY(AbstractGraphicsWidgetResources)
    QUuid id_;
    QColor color_;
    Duration old_entry_point_;
    Duration old_source_duration_;
    TrimHandle* left_trim_handle_;
    TrimHandle* right_trim_handle_;
    DurationIndicator* duration_indicator_;
    GraphicsObjectVerticalIndicator* vertical_indicator_;
};

class GraphicsWidgetFileResource : public AbstractGraphicsWidgetResources
{
    Q_OBJECT
public:
    GraphicsWidgetFileResource(GraphicsWidgetSequence *parent, const QColor& color);
    virtual ~GraphicsWidgetFileResource(){}
    virtual int type() const{return GraphicsWidgetFileResourceType;}
private:
    Q_DISABLE_COPY(GraphicsWidgetFileResource)
};

class GraphicsWidgetDummyResource : public GraphicsWidgetFileResource
{
public:
    GraphicsWidgetDummyResource();
    virtual ~GraphicsWidgetDummyResource(){}
private:
    Q_DISABLE_COPY(GraphicsWidgetDummyResource)
};

class GraphicsWidgetVideoResource : public GraphicsWidgetFileResource
{
    Q_OBJECT
public:
    GraphicsWidgetVideoResource(GraphicsWidgetSequence *parent);
    virtual ~GraphicsWidgetVideoResource(){}
    virtual int type() const{return GraphicsWidgetVideoResourceType;}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
    virtual void trimHandleInUse(eTrimHandlePos pos, bool active);
    virtual void cplEditRateChanged(){}
    virtual double samplesPerFrame(const EditRate& compositionEditRate)const{return 1;}
private slots:
    void slot_entry_point_changed();
    void slot_source_duration_changed();
private:
    Q_DISABLE_COPY(GraphicsWidgetVideoResource)
    QImage left_proxy_image_;
    QImage right_proxy_image_;
    bool   is_trim_active_;
    bool   is_proxy_visible_;
};

class GraphicsWidgetAudioResource : public GraphicsWidgetFileResource
{
public:
    GraphicsWidgetAudioResource(GraphicsWidgetSequence* parent);
    virtual ~GraphicsWidgetAudioResource(){}
    virtual double samplesPerFrame(const EditRate& compositionEditRate)const{return 1;}
private:
    Q_DISABLE_COPY(GraphicsWidgetAudioResource)
};

class GraphicsWidgetTimedTextResource : public GraphicsWidgetFileResource
{
public:
    GraphicsWidgetTimedTextResource(GraphicsWidgetSequence* parent);
    virtual ~GraphicsWidgetTimedTextResource(){}
    virtual double samplesPerFrame(const EditRate& compositionEditRate)const{return 1;}
private:
    Q_DISABLE_COPY(GraphicsWidgetTimedTextResource)
};




#endif // GRAPHICSWIDGETRESOURCES_H
