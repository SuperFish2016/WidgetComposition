#ifndef GRAPHICSVIEWSCALABLE_H
#define GRAPHICSVIEWSCALABLE_H
#include <QGraphicsView>

enum eScaleStatus
{
    ScaleMax,
    ScaleMin,
    ScaleNormal
};
class GraphicsViewScalable : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsViewScalable(QWidget* parent = nullptr);
    virtual ~GraphicsViewScalable(){}

public slots:
    void zoomIn();
    void zoomOut();
    eScaleStatus scaleView(qreal scaleFactor);
private:
    Q_DISABLE_COPY(GraphicsViewScalable)

    eScaleStatus status_;
};

class AbstractViewTransformNotifier
{
    friend eScaleStatus GraphicsViewScalable::scaleView(qreal scaleFactor);
public:
    QTransform GetViewTransform() const {
        return (GetObservableView() ? GetObservableView()->transform() : QTransform());
    }
protected:
    virtual void ViewTransformEvent(const QTransform &rViewTransform) {}
    virtual QGraphicsView* GetObservableView() const = 0;
};

#endif // GRAPHICSVIEWSCALABLE_H
