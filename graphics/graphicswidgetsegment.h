#ifndef GRAPHICSWIDGETSEGMENT_H
#define GRAPHICSWIDGETSEGMENT_H
#include "graphicscommon.h"
#include <QUuid>
#include <QGraphicsLinearLayout>
class GraphicsWidgetComposition;
class GraphicsWidgetSequence;
class GraphicsWidgetTimeline;
class GraphicsWidgetSegment : public GraphicsWidgetBase
{
    Q_OBJECT
public:
    GraphicsWidgetSegment(GraphicsWidgetComposition* parent, const QUuid& id = QUuid::createUuid());
    virtual ~GraphicsWidgetSegment(){}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void addSequence(GraphicsWidgetSequence* sequence, int sequenceIndex);
    void deleteSequence(GraphicsWidgetSequence* sequence);

    GraphicsWidgetSequence* sequence(int index) const;
    GraphicsWidgetSequence* sequence(const QUuid &id, bool isTrackId = false) const;
    QList<GraphicsWidgetSequence*> sequenceList() const;

    void setDuration(qint32 d){}

    inline qint32 sequenceIndex(GraphicsWidgetSequence *sequence) const;
    inline qint32 sequenceCount() const{return layout_->count();}
protected:
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &rConstraint = QSizeF()) const;
private:
    void createLayout();
private:
    Q_DISABLE_COPY(GraphicsWidgetSegment)
    QGraphicsLinearLayout* layout_;
    Duration duration_ = 1200;
};

class GraphicsWidgetSegmentIndicator : public GraphicsWidgetBase
{
    Q_OBJECT
public:
    GraphicsWidgetSegmentIndicator(GraphicsWidgetTimeline* parent){}
    virtual ~GraphicsWidgetSegmentIndicator(){}
    bool extendGrid(QPointF &rPoint, eGridPosition which) const;
};

#endif // GRAPHICSWIDGETSEGMENT_H
