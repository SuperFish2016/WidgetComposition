#ifndef WIDGETCOMPOSITION_H
#define WIDGETCOMPOSITION_H
#include <QFrame>
#include <QSplitter>
#include "utils/commands.h"
#include "graphics/graphicswidgettimeline.h"
#include "graphics/graphicsscene.h"
#include "graphics/graphicswidgetcomposition.h"
class ImprovedSplitter;
class QToolBar;
class WidgetTrackDetailsTimeline;
class WidgetComposition : public QFrame
{
    Q_OBJECT
public:
    WidgetComposition();
    virtual~ WidgetComposition(){}

    void add_track_details(AbstractWidgetTrackDetails* track_details, int index);
    void delete_track_details(AbstractWidgetTrackDetails* track_details);

    qint32 track_details_count() const;
    AbstractWidgetTrackDetails* track_details(int trackIndex) const;

    void buildUI();
protected:
    bool eventFilter(QObject *watched, QEvent *event);

public slots:
    void slot_add_segment_request();
    void slot_delete_segment_request();
    void slot_add_track_request(eSequenceType type);
    void slot_track_menu_triggered(int actionType);

    void slot_delete_track_request(int track_index);
    void slot_delete_track_request(const QUuid& id);
    void slot_splitter_size_changed(int ,int);

    void slot_push_command(QUndoCommand *command){undo_stack_->push(command);}
private:
    qint32 lastTrackIndex(eSequenceType type);
    qint32 lastTrackLocalIndex(eSequenceType type); // set video 0, video 1...
    void createLayout();
    void createStyle();
private:
    Q_DISABLE_COPY(WidgetComposition)

    WidgetTrackDetailsTimeline* timeline_details_;
    /// timeline with measure
    GraphicsViewScalable*   timeline_view_;
    GraphicsSceneTimeline*  timeline_scene_;
    GraphicsWidgetTimeline* timeline_;

    // place segment -> sequence -> resource(without timeline)
    GraphicsViewScalable*     composition_view_; // VIEW
    GraphicsSceneComposition* composition_scene_; // SCENE
    GraphicsWidgetComposition* composition_graphics_;// ITEM
    GraphicsWidgetTimeline*    timeline_graphics_;//ITEM

    // place track details widget
    QScrollArea* track_details_area;
    QSplitter* outer_splitter_;
    QSplitter* inner_l_splitter_; // timeline + tracks track details.
    QSplitter* inner_r_splitter_; // timeline + tracks view
    ImprovedSplitter *track_splitter_; // split track details and timeline track
    QUndoStack* undo_stack_;
};

/// this caused layout issue
/// // QSplitter can control the child widgets size by dragging the boundary.
/// It can include serval widget.
class ImprovedSplitter : public QSplitter
{
    Q_OBJECT
public:
    ImprovedSplitter(QWidget* parent = nullptr);
    ImprovedSplitter(Qt::Orientation orientation, QWidget *parent = nullptr);
    virtual ~ImprovedSplitter(){}
    void addWidget(QWidget *widget);
    void insertWidget(int index, QWidget* widget);
    qint32 count() const{return QSplitter::count() - 1;}
protected:
    virtual void childEvent(QChildEvent *event);
private slots:
    void slot_splitter_moved(int pos, int index);
private:
    Q_DISABLE_COPY(ImprovedSplitter)

    QWidget*    dummy_widget_;
    QList<int>	old_splitter_size_;

};
#endif // WIDGETCOMPOSITION_H
