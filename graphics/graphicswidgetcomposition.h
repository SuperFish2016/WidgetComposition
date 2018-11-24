#ifndef GRAPHICSWIDGETCOMPOSITION_H
#define GRAPHICSWIDGETCOMPOSITION_H
#include "graphicscommon.h"
#include "graphicswidgetsegment.h"
#include <qgraphicslinearlayout.h>

// Compositon widget 用来存放 segment 组件

class GraphicsWidgetComposition : public GraphicsWidgetBase
{
    Q_OBJECT
public:
    GraphicsWidgetComposition(QGraphicsItem* parent = nullptr);
    virtual ~GraphicsWidgetComposition(){}

    virtual int type() const{return GraphicsWidgetCompositionType;}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void add_segment(GraphicsWidgetSegment* segment, int index);
    void delete_segment(GraphicsWidgetSegment* segment);

    inline qint32 segmentCount() const{return layout_->count();}
    inline GraphicsWidgetSegment* segment() const{return segment_;}
protected:
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF& constraint = QSizeF()) const;
    virtual void resizeEvent(QGraphicsSceneResizeEvent *event);
private:
    void createLayout();
private:
    Q_DISABLE_COPY(GraphicsWidgetComposition)

    QGraphicsLinearLayout* layout_;
    GraphicsWidgetSegment* segment_;
};

#endif // GRAPHICSWIDGETCOMPOSITION_H
