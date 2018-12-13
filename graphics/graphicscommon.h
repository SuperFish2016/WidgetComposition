#ifndef GRAPHICSCOMMON_H
#define GRAPHICSCOMMON_H
#include <QGraphicsWidget>
#include "graphicsviewscalable.h"
#include "imf/imfcommon.h"
#include <QDebug>

#define CPL_COLOR_TIMELINE_TOP 125, 124, 127
#define CPL_COLOR_TIMELINE_BOTTOM 108, 109, 113
#define CPL_COLOR_RESOURCE_NOT_DROPPABLE 198, 43, 43

#define CPL_COLOR_TIMED_TEXT_RESOURCE 191, 159, 72
#define CPL_COLOR_ANC_RESOURCE 153, 72, 191
#define CPL_COLOR_DUMMY_RESOURCE 129, 129, 129
#define CPL_COLOR_MARKER_RESOURCE 255, 255, 255, 50
#define CPL_COLOR_DURATION_INDICATOR 255, 0, 93
#define CPL_COLOR_DEFAULT_SNAP_INDICATOR 255, 0, 93
#define CPL_COLOR_DEFAULT_MARKER 255, 111, 79
#define CPL_COLOR_BACKGROUND 33, 33, 33
#define CPL_BORDER_COLOR 83, 83, 85
#define CPL_FONT_COLOR 58, 58, 58

#define COLOR_TRACK_BG 52, 52, 52
#define COLOR_TRACK_BORDER 83, 83, 85
#define COLOR_FRAME_INDICATOR 0, 170, 255
#define COLOR_TIMECODE_LABEL 0, 170, 220

#define COLOR_SEQUENCE_BG 52, 52, 52
#define COLOR_SEQUENCE_BORDER 83, 83, 85

#define COLOR_VIDEO_RESOURCE 116, 102, 171
#define COLOR_AUDIO_RESOURCE 110, 162, 110

#define HEIGHT_TRACK 30
#define HEIGHT_TIMELINE_PAINTER 30
#define HEIGHT_TIMELINE_WIDGET  30

enum eSequenceType {
    AncillaryDataSequence = (1u << 0),
    CommentarySequence = (1u << 1),
    HearingImpairedCaptionsSequence = (1u << 2),
    KaraokeSequence = (1u << 3),
    MainAudioSequence = (1u << 4),
    MainImageSequence = (1u << 5),
    SubtitlesSequence = (1u << 6),
    VisuallyImpairedTextSequence = (1u << 7),
    MarkerSequence = (1u << 8),
    DolbyMetadataSequence = (1u << 9),
    Unknown = (1u << 10)
};


enum eGraphicsItemType {

    GraphicsWidgetBaseType = QGraphicsItem::UserType + 1,
    GraphicsObjectBaseType,
    GraphicsObjectVerticalIndicatorType,
    GraphicsWidgetCompositionType,
    GraphicsWidgetFileResourceType,
    GraphicsWidgetDummyResourceType,
    GraphicsWidgetVideoResourceType,
    GraphicsWidgetAudioResourceType,
    GraphicsWidgetTimedTextResourceType,
    GraphicsWidgetAncillaryDataResourceType,
    GraphicsWidgetMarkerResourceType,
    GraphicsWidgetMarkerType,
    GraphicsWidgetTimelineType,
    TimelinePainterType
};


enum eGridPosition {
    VideoHorizontal = (1u << 0),
    AudioHorizontal = (1u << 1),
    TimedTextHorizontal = (1u << 2),
    DataHorizontal = (1u << 3),
    MarkerHorizontal = (1u << 4),
    Vertical = (1u << 5),
};



class AbstractGridExtension
{
public:
    AbstractGridExtension(){}
    virtual ~AbstractGridExtension(){}
public:
    /*! \brief
    You have to implement this method by aligning the rPoint to the grid and return true. Otherwise return false
    "which" specifies the grid. rPoint is given in scene coordinates.
    */
    virtual bool extendGrid(QPointF &rPoint, eGridPosition which) const = 0;
    virtual qreal HeightAdviceForHorizontalGrid() const { return -1; }
    virtual QColor ColorAdviceForGrid() const { return QColor(CPL_COLOR_DEFAULT_SNAP_INDICATOR); }
private:
    Q_DISABLE_COPY(AbstractGridExtension)
};

class GraphicsWidgetBase : public QGraphicsWidget, public AbstractGridExtension
{
public:
    GraphicsWidgetBase(QGraphicsItem *parent = nullptr);
    virtual ~GraphicsWidgetBase(){}
    virtual int type() const{return GraphicsWidgetBaseType;}
    EditRate GetCplEditRate() const{return EditRate::EditRate24;}
    virtual bool extendGrid(QPointF &rPoint, eGridPosition which) const { return false; }

private:
    Q_DISABLE_COPY(GraphicsWidgetBase)
};

class GraphicsObjectBase : public QGraphicsObject, public AbstractGridExtension
{
public:
    GraphicsObjectBase(QGraphicsItem* parent = nullptr):QGraphicsObject(parent){}
    virtual ~GraphicsObjectBase(){}
    virtual int type() const{return GraphicsObjectBaseType;}
private:
    Q_DISABLE_COPY(GraphicsObjectBase)
};

class GraphicsObjectVerticalIndicator : public GraphicsObjectBase, public AbstractViewTransformNotifier
{
    Q_OBJECT
private:
    class GraphicsItemLine : public QGraphicsItem
    {
    public:
        GraphicsItemLine(qreal width, qreal height, GraphicsObjectVerticalIndicator* parent);
        virtual ~GraphicsItemLine(){}
        virtual int type() const{return GraphicsObjectVerticalIndicatorType;}
        virtual QRectF boundingRect() const{return QRectF(QPointF(0, 0), QSizeF(line_size_));}
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

        void setColor(const QColor& c){color_ = c; update();}
        void setSize(const QSize& s){prepareGeometryChange(); line_size_ = s;}
    private:
        Q_DISABLE_COPY(GraphicsItemLine)
        QSize  line_size_;
        QColor color_;
    };
public:
    GraphicsObjectVerticalIndicator(qreal width, qreal height, const QColor& color, QGraphicsItem* parent);
    virtual ~GraphicsObjectVerticalIndicator(){}
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setHeadSize(const QSize& size){prepareGeometryChange(); head_size_ = size;}
    void setColor(const QColor& c){color_ = c;}
    void setHeight(int height) const{line_->setSize(QSize(line_->boundingRect().width(), height));}
    void setLineSize(const QSize& size){line_->setSize(size);}
    void showHead(){setFlag(QGraphicsItem::ItemHasNoContents, false);}
    void hideHead(){setFlag(QGraphicsItem::ItemHasNoContents, true);}
    void showLine(){line_->setFlag(QGraphicsItem::ItemHasNoContents, false);}
    void hideLine(){line_->setFlag(QGraphicsItem::ItemHasNoContents, true);}
    void setGridExtention(bool v){is_extend_grid_ = v;}
    QSize headSize() const{return head_size_;}
protected:
    virtual bool extendGrid(QPointF &rPoint, eGridPosition which) const;
    virtual QColor ColorAdviceForGrid() const { return color_; }
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    virtual void ViewTransformEvent(const QTransform& trans);
    virtual QGraphicsView* GetObservableView() const{if(scene() && scene()->views().empty() == false)
            return scene()->views().first(); return nullptr;}
signals:
    void sig_xPos_changed(qreal xPos);
public slots:
    void slot_set_xPos(qreal xPos){
        setX(xPos);
    }
private:
    QColor color_;
    GraphicsItemLine* line_;
    QSize   head_size_;
    bool    is_extend_grid_;
    QString text_;
private:
    Q_DISABLE_COPY(GraphicsObjectVerticalIndicator)
};

class GraphicsWidgetHollowProxyWidget : public QGraphicsWidget, public AbstractViewTransformNotifier
{
    Q_OBJECT
public:
    GraphicsWidgetHollowProxyWidget();
    virtual ~GraphicsWidgetHollowProxyWidget(){}
};

class GraphicsWidgetMarkerIndicator : public GraphicsObjectVerticalIndicator
{
public:
    GraphicsWidgetMarkerIndicator(qreal width, qreal height);
    virtual ~GraphicsWidgetMarkerIndicator(){}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
private:
    Q_DISABLE_COPY(GraphicsWidgetMarkerIndicator)
    bool is_mouse_pressed_;
};



class GraphicsHelper
{
public:
    static qreal  GetDefaultFontHeight(){}
    static QColor GetSegmentColor(int index, bool transparent = false);
};


#endif // GRAPHICSCOMMON_H

