#ifndef GRAPHICSWIDGETSEQUENCE_H
#define GRAPHICSWIDGETSEQUENCE_H
#include "graphicscommon.h"
#include <QUuid>
#include <QGraphicsLinearLayout>
class GraphicsWidgetSegment;
class AbstractGraphicsWidgetResources;
class GraphicsWidgetSequence : public GraphicsWidgetBase
{
    Q_OBJECT
public:
    GraphicsWidgetSequence(GraphicsWidgetSegment* parent, eSequenceType type, const QUuid& track_id, const QUuid& id = QUuid::createUuid());
    virtual ~GraphicsWidgetSequence(){}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    qint32 resourceCount() const{return layout_->count();}
    void addResource(AbstractGraphicsWidgetResources* resource, int index);
    void deleteResource(AbstractGraphicsWidgetResources* resource);
    AbstractGraphicsWidgetResources* resource(int index) const;
    AbstractGraphicsWidgetResources* resource(const QUuid &rId) const;
    QList<AbstractGraphicsWidgetResources*> resourceList() const;
    GraphicsWidgetSegment* getSegment() const;
    inline QUuid id() const{return id_;}
    inline QUuid trackId() const{return track_id_;}

    eSequenceType type(){return type_;}
    void setHeight(qint32 d){height_ = d; updateGeometry();}
protected:
    virtual bool extendGrid(QPointF& point, eGridPosition which);
    virtual qreal heightAdviceForHorizontalGrid() const{return layout_->contentsRect().size().height();}
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;
private:
    void createLayout();
private:
    Q_DISABLE_COPY(GraphicsWidgetSequence)

    QUuid id_;
    QUuid track_id_;
    qint32 height_;
    eSequenceType type_;
    QGraphicsLinearLayout *layout_;
//	GraphicsWidgetDummyResource *mpFillerResource;
};

#endif // GRAPHICSWIDGETSEQUENCE_H
