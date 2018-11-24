#include "widgettrackdetails.h"
#include <QLabel>
#include <QAction>
#include <QButtonGroup>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QGraphicsSceneContextMenuEvent>
#include "widgetcomposition.h"
AbstractWidgetTrackDetails::AbstractWidgetTrackDetails(QWidget *parent)
    :QFrame(parent)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    setObjectName("WidgetTrackDetailBase");
    setStyleSheet(QString(
        "QFrame#%1 {"
        "background-color: %2;"
        "border-width: 1px;"
        "border-top-width: 0px;"
        "border-left-width: 0px;"
        "border-color: %3;"
        "border-style: solid;"
        "border-radius: 0px;""}").arg(objectName()).arg(QColor(COLOR_TRACK_BG).name()).
                  arg(QColor(COLOR_TRACK_BORDER).name()));
}

void AbstractWidgetTrackDetails::resizeEvent(QResizeEvent *e)
{
    emit sig_height_about_to_change(e->size().height());
}

WidgetTrackDetailsTimeline::WidgetTrackDetailsTimeline(QWidget *parent)
    :AbstractWidgetTrackDetails(parent)
{
   // createToolBar();
    createLayout();
}

void WidgetTrackDetailsTimeline::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    QAction* action_frames = menu.addAction("Frames");
    QAction* action_timecode = menu.addAction("Timecode");
    action_frames->setCheckable(true);
    action_timecode->setCheckable(true);
    connect(&menu, &QMenu::aboutToShow, this, [=](){if(is_show_frames_) action_frames->setChecked(true);
                                                    else action_timecode->setChecked(true);});
    QAction* action = menu.exec(event->globalPos());
    bool show_frames = false;
    if(action == action_frames)
    {
        show_frames = true;
    }
    else if(action == action_timecode)
    {
        show_frames = false;
    }
    this->slot_show_frames(show_frames);
    emit sig_frame_timecode_toggled(show_frames);
}

void WidgetTrackDetailsTimeline::createLayout()
{
    label_timecode_ = new QLabel(this);
    label_timecode_->setText(Timecode().GetAsString());
    QFont font;
    font.setFamily("Aharoni");
    font.setPointSize(font.pointSize() * 2.5);

    label_timecode_->setStyleSheet(QString("color: %1;").arg(QColor(COLOR_TIMECODE_LABEL).name()));
    label_timecode_->setFont(font);
    label_timecode_->setAlignment(Qt::AlignCenter);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(label_timecode_);
    this->setLayout(layout);
}

void WidgetTrackDetailsTimeline::createToolBar()
{
    tool_bar_ = new QToolBar(tr("Composition Toolbar"), this);
   // tool_bar_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QToolButton *p_button_add_track = new QToolButton(nullptr);
    p_button_add_track->setIcon(QIcon(":/add.png"));
    p_button_add_track->setText(tr(""));
    p_button_add_track->setCheckable(true);
   // p_button_add_track->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
 //   p_button_add_track->setPopupMode(QToolButton::InstantPopup);
    QMenu *p_add_track_menu = new QMenu(tr("Add Track"), this);
    p_add_track_menu->addSeparator();
    p_button_add_track->setMenu(p_add_track_menu);
    tool_bar_->addWidget(p_button_add_track);
    QAction *p_action = tool_bar_->addAction(QIcon(":/cutter.png"), tr("Edit"));
    p_action->setShortcut(Qt::Key_E);
    p_action->setCheckable(true);

    //p_action->setAutoRepeat(false);
    //connect(p_add_track_menu, SIGNAL(aboutToShow()), this, SLOT(rAddTrackMenuAboutToShow()));
    //connect(p_add_track_menu, SIGNAL(triggered(QAction*)), this, SLOT(rAddTrackMenuActionTriggered(QAction*)));
}

void WidgetTrackDetailsTimeline::slot_set_timecode(const Timecode &tc)
{
    if(is_show_frames_)
        label_timecode_->setText(tc.GetFramesAsString());
    else
        label_timecode_->setText(tc.GetAsString());
    last_timecode_ = tc;
}

void WidgetTrackDetailsTimeline::slot_show_frames(bool checked)
{
    is_show_frames_ = checked;
    this->slot_set_timecode(last_timecode_);
}

void WidgetTrackDetailsTimeline::slot_buttons_toggled(int id, bool checked)
{
//    if(id == ButtonTimeCode)
//    {
//        QAbstractButton* button = button_gp_->button(ButtonFrames);
//        {
//            if(button)
//                button->setChecked(!checked);
//        }
//    }
//    if(id == ButtonFrames)
//    {
//        QAbstractButton* button = button_gp_->button(ButtonTimeCode);
//        {
//            if(button)
//                button->setChecked(!checked);
//        }
//    }
}

WidgetTrackDetails::WidgetTrackDetails(const QUuid& track_id, qint32 track_order, eSequenceType type, QWidget* parent)
    :AbstractWidgetTrackDetails(parent), type_(type), track_id_(track_id), track_order_(track_order + 1)
{
    createLayout();
}

void WidgetTrackDetails::createLayout()
{
    QToolButton* tool_button = new QToolButton(this);
    tool_button->setSizePolicy(tool_button->sizePolicy().horizontalPolicy(), QSizePolicy::Expanding);
    QString tool_button_style = QString("QToolButton{border:1px; color: white; background-color:%1;}\
                                        QToolButton:menu-button{border:1px; color: white; background-color:%2;}").arg(QColor(COLOR_TRACK_BG).name())
                                                                                                                 .arg(QColor(COLOR_TRACK_BG).name());
    tool_button->setStyleSheet(tool_button_style);
    QLabel* label_title = new QLabel(this);
    switch (type_) {
    case MainImageSequence:
        label_title->setText("Video " + QString::number(track_order_));
        tool_button->setIcon(QIcon(":/film.png"));
        break;
    case MainAudioSequence:
        label_title->setText("Audio " + QString::number(track_order_));
        tool_button->setIcon(QIcon(":/sound.png"));
        break;
    case SubtitlesSequence:
        label_title->setText("Subtitles " + QString::number(track_order_));
        tool_button->setIcon(QIcon(":/text_bw.png"));
        break;
    case DolbyMetadataSequence:
        label_title->setText("Metadata");
        tool_button->setIcon(QIcon(":/metadata.png"));
        break;
    default:
        break;
    }
    QFont font;
    font.setFamily("Verdana");
    label_title->setFont(font);
    label_title->setStyleSheet("color:white");
    tool_button->setAutoRaise(true);
    tool_button->setPopupMode(QToolButton::MenuButtonPopup);
    tool_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QMenu* menu = new QMenu(this);
    actionAdd_      = new QAction("Add Track", this);
    actionDelete_   = new QAction("Delete Track", this);
    actionEdit_     = new QAction("Edit Track", this);
    actionAdd_->setData(QString("ADD,")  + QString::number(type_));
    actionDelete_->setData(QString("DEL"));
    actionEdit_->setData(QString("EDIT"));
    menu->addAction(actionAdd_);
    menu->addAction(actionDelete_);
    menu->addSeparator();
    menu->addAction(actionEdit_);
    tool_button->setMenu(menu);
    connect(menu, &QMenu::triggered, this, &WidgetTrackDetails::slot_action_triggered);
    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(label_title);
    layout->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(tool_button);
    layout->setContentsMargins(10,0,0,0);
    this->setLayout(layout);
    connect(menu, &QMenu::aboutToShow, this, &WidgetTrackDetails::slot_update_actions);
    connect(actionDelete_, &QAction::triggered, this, &WidgetTrackDetails::slot_delete_track);
}

void WidgetTrackDetails::slot_delete_track()
{

}

void WidgetTrackDetails::slot_update_actions()
{

}

void WidgetTrackDetails::slot_action_triggered(QAction *action)
{
    if(action == actionAdd_)
    {
        emit sig_track_action_triggered(ActionInsert);
    }
    else if(action == actionDelete_)
    {
        emit sig_track_action_triggered(ActionDelete);
    }
    else if(action == actionEdit_)
    {
        emit sig_track_action_triggered(ActionEdit);
    }
}

QSize WidgetTrackDetails::sizeHint() const
{
    QSize size = QFrame::sizeHint();
    size.setHeight(HEIGHT_TRACK);
    return size;
}
