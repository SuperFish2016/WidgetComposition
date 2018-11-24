#ifndef WIDGETTRACKDETAILS_H
#define WIDGETTRACKDETAILS_H
#include "imf/imfcommon.h"
#include "graphics/graphicscommon.h"
#include <QFrame>
#include <QUuid>
#include <QLabel>
#include <QToolBar>
class QLabel;
class QAction;
class QButtonGroup;
class AbstractWidgetTrackDetails : public QFrame
{
    Q_OBJECT
public:
    AbstractWidgetTrackDetails(QWidget* parent = nullptr);
    virtual ~AbstractWidgetTrackDetails(){}
    virtual QUuid track_id() const = 0;
    virtual void  set_track_id(const QUuid& id) = 0;
    virtual eSequenceType type() const = 0;
signals:
    void sig_height_about_to_change(qint32 height);
    void sig_delete_clicked(const QUuid& id);
protected:
    virtual void resizeEvent(QResizeEvent* e);
};

class WidgetTrackDetailsTimeline : public AbstractWidgetTrackDetails
{
    Q_OBJECT
private:
    enum eButtons
    {
        ButtonSettings,
        ButtonLock,
        ButtonTimeCode,
        ButtonFrames
    };
public:
    WidgetTrackDetailsTimeline(QWidget* parent = nullptr);
    virtual ~WidgetTrackDetailsTimeline(){}
    virtual QUuid track_id() const{ return QUuid();}
    virtual void  set_track_id(const QUuid &id){}
    virtual eSequenceType type() const{return Unknown;}
protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
private:
    void createLayout();
    void createToolBar();
signals:
    void sig_frame_timecode_toggled(bool is_show_frame);
public slots:
    void slot_set_timecode(const Timecode& tc);
private slots:
    void slot_buttons_toggled(int, bool);
    void slot_show_frames(bool checked);
private:
    Q_DISABLE_COPY(WidgetTrackDetailsTimeline)

    bool            is_show_frames_ = false;
    Timecode        last_timecode_;
    QLabel*         label_timecode_;
    QToolBar*       tool_bar_;
};

class WidgetTrackDetails : public AbstractWidgetTrackDetails
{
    Q_OBJECT
public:
    enum eActionType
    {
        ActionInsert,
        ActionDelete,
        ActionEdit
    };
public:
    WidgetTrackDetails(const QUuid& track_id, qint32 track_order, eSequenceType type, QWidget* parent = nullptr);
    virtual ~WidgetTrackDetails(){}
    virtual QSize sizeHint() const;
    virtual QUuid track_id() const{return track_id_;}
    virtual void  set_track_id(const QUuid &id){track_id_ = id;}
    virtual eSequenceType type() const{return type_;}
private:
    void createLayout();
signals:
    void sig_track_action_triggered(eActionType type);
private slots:
    void slot_delete_track();
    void slot_update_actions();
    void slot_action_triggered(QAction*); // handle actions trigger in current widget.
private:
    Q_DISABLE_COPY(WidgetTrackDetails)

    QAction* actionDelete_;
    QAction* actionAdd_;
    QAction* actionEdit_;
    QUuid    track_id_;
    qint32   track_order_;
    qint32   index_in_splitter_;
    eSequenceType type_;
};

#endif // WIDGETTRACKDETAILS_H
