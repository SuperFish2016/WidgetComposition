#ifndef GRAPHICSVIEWSCALABLE_H
#define GRAPHICSVIEWSCALABLE_H
#include <QGraphicsView>

class GraphicsViewScalable : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsViewScalable(QWidget* parent = nullptr);
    virtual ~GraphicsViewScalable(){}

public slots:
    void zoomIn();
    void zoomOut();
    void scaleView(qreal scaleFactor);
private:
    Q_DISABLE_COPY(GraphicsViewScalable)
};

class AbstractViewTransformNotifier
{
public:
    AbstractViewTransformNotifier(){}
    virtual ~AbstractViewTransformNotifier(){}
    friend void GraphicsViewScalable::scaleView(qreal scaleFactor);
public:
    QTransform GetViewTransform() const {
        return (GetObservableView() ? GetObservableView()->transform() : QTransform());
    }
protected:
    virtual void ViewTransformEvent(const QTransform &rViewTransform) {}
    virtual QGraphicsView* GetObservableView() const = 0;
};

#endif // GRAPHICSVIEWSCALABLE_H
