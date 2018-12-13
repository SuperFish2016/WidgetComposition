#include "graphicswidgettimeline.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include "imf/imfcommon.h"

GraphicsWidgetTimeline::GraphicsWidgetTimeline(QGraphicsItem *parent)
    :GraphicsWidgetBase(parent), height_(HEIGHT_TIMELINE_WIDGET)
{
    setFlags(QGraphicsItem::ItemHasNoContents);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    creatLayout();
}

// C.T  resizeEvent is QGraphicsSceneResizeEvent
void GraphicsWidgetTimeline::resizeEvent(QGraphicsSceneResizeEvent *e)
{
    QGraphicsScene* ptr_scene = scene();
    if(ptr_scene)
    {
        QRectF scene_rect = boundingRect();
        ptr_scene->setSceneRect(scene_rect);
    }
}

QSizeF GraphicsWidgetTimeline::sizeHint(Qt::SizeHint which, const QSizeF &constraint /*= QSizeF()*/) const {

    QSizeF size;
    if(constraint.isValid() == false) {
        switch(which) {
            case Qt::MinimumSize:
                size = QSizeF(std::numeric_limits<qint32>::max(), height_);
                break;
            case Qt::PreferredSize:
                size = QSizeF(std::numeric_limits<qint32>::max(), height_);
                break;
            case Qt::MaximumSize:
                size = QSizeF(std::numeric_limits<qint32>::max(), height_);
                break;
            case Qt::MinimumDescent:
                size = QSizeF(-1, -1);
                break;
            case Qt::NSizeHints:
                size = QSizeF(-1, -1);
                break;
            default:
                size = QSizeF(-1, -1);
                break;
        }
    }
    else {
        qWarning() << "sizeHint() is constraint.";
        size = constraint;
    }
    return size;
}

void GraphicsWidgetTimeline::creatLayout()
{
    timeline_painter_ = new TimelinePainter(this);
    timeline_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    timeline_layout_->setContentsMargins(0,0,0,0); // resize event make it fill the scene.
    timeline_layout_->setSpacing(0);
    timeline_layout_->addItem(timeline_painter_);
    this->setLayout(timeline_layout_);
}

void GraphicsWidgetTimeline::setHeight(qint32 height)
{
    height_ = height;
    updateGeometry();
}

GraphicsWidgetTimeline::TimelinePainter::TimelinePainter(GraphicsWidgetTimeline *parent):
    GraphicsWidgetBase(parent)
{
    setFlags(QGraphicsItem::ItemUsesExtendedStyleOption);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void GraphicsWidgetTimeline::TimelinePainter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    ///  0.03 < lod < 9.5 (scale to maximum), default is 1(no scale).
    qreal sq_lod = lod * lod;
    const qint32 rounded_editrate = 24; //GetCplEditRate().GetRoundendQuotient();
    const QRectF exposed_rect = option->exposedRect;/// Only set ItemUseExtendedStyleOption
    const QRectF track_rect(boundingRect()); // Never changed( no matter scale), exposed_rect is part of this.
    const qreal   track_height = boundingRect().height();
    const quint32 exposed_left = (quint32)exposed_rect.left();
    const quint32 exposed_right = (quint32)exposed_rect.right();
    const quint32 pixel_distance_frames = 1;
    const quint32 pixel_distance_seconds = rounded_editrate;//24
    const quint32 pixel_distance_10_seconds = pixel_distance_seconds * 10;
    const quint32 pixel_distance_30_seconds = pixel_distance_seconds * 30;
    const quint32 pixel_distance_minutes = pixel_distance_seconds * 60;
    const quint32 pixel_distance_15_minutes = pixel_distance_minutes * 15;
    const quint32 pixel_distance_30_minutes = pixel_distance_minutes * 30;
    const quint32 pixel_distance_hours = pixel_distance_minutes * 60;
    // 在不同缩放级别下，不同帧数 对应的像素距离，从上面知道 sq_lod 最大为9.* ，因此 shown_pixel_distance_frames 最大为90
    const quint32 shown_pixel_distance_frames = pixel_distance_frames * sq_lod;
    const quint32 shown_pixel_distance_seconds = rounded_editrate * sq_lod;
    const quint32 shown_pixel_distance_10_seconds = pixel_distance_10_seconds * sq_lod;
    const quint32 shown_pixel_distance_30_seconds = pixel_distance_30_seconds * sq_lod;
    const quint32 shown_pixel_distance_minutes = pixel_distance_minutes * sq_lod;
    const quint32 shown_pixel_distance_15_minutes = pixel_distance_15_minutes * sq_lod;
    const quint32 shown_pixel_distance_30_minutes = pixel_distance_30_minutes * sq_lod;
    const quint32 shown_pixel_distance_hours = pixel_distance_hours * sq_lod;

    GraphicsWidgetTimeline* parent = qobject_cast<GraphicsWidgetTimeline*>(parentWidget());
    bool is_paint_frames = false;
    if(parent && parent->is_paint_with_frames())
    {
        is_paint_frames = true;
    }
    QPen pen;
    pen.setCosmetic(true); // no scaling
    pen.setColor(QColor(CPL_BORDER_COLOR));
    pen.setWidth(0);
    QFont font("Arial");
    font.setPixelSize(11);
    painter->setFont(font);
    painter->setPen(pen);
    painter->fillRect(track_rect.intersected(exposed_rect), QColor(COLOR_SEQUENCE_BG));

    pen.setColor(QColor(Qt::white)); // set Text color
    painter->setPen(pen);

    QString timecode_str;
    const qint32 timecode_str_width = 60;
    const qint32 timecode_str_height = 12;
    qint32  timecode_y_pos = 0.4 * track_height;
    qint32  line_y_pos     = 0.8 * track_height;
    qint32  line_y_pos_with_text = line_y_pos - 3;
    if(shown_pixel_distance_frames >= 4) {
        for(quint32 i = exposed_left; i <= exposed_right; i++) {
            painter->drawLine(i, line_y_pos, i, track_height); // 画出刻度线
            if(shown_pixel_distance_frames > 11) {
                if(i % 5 != 0) continue;
                painter->drawLine(i, line_y_pos_with_text, i, line_y_pos); // 突出刻度线
                QTransform transf = painter->transform(); //保存当前的转换矩阵
                painter->setTransform(QTransform(transf).scale(1 / transf.m11(), 1)); //不进行缩放坐标系
                if(is_paint_frames)
                    timecode_str = QString::number(i);
                else
                    timecode_str = Timecode(GetCplEditRate(), i).GetAsString("%1:%2:%3:%4");
                painter->drawText(i *  transf.m11() - timecode_str_width / 2, timecode_y_pos, timecode_str_width, timecode_str_height, Qt::AlignCenter, timecode_str);
                painter->setTransform(transf);
            }
        }
    }
    if(shown_pixel_distance_seconds >= 4) {
        for(quint32 i = exposed_left - (exposed_left % rounded_editrate); i <= exposed_right; i += pixel_distance_seconds) {
            painter->drawLine(i, line_y_pos, i, track_height); // 画出刻度线
            if(shown_pixel_distance_seconds >= 65 && shown_pixel_distance_frames <= 11) {
                 painter->drawLine(i, line_y_pos_with_text, i, line_y_pos); // 突出刻度线
                QTransform transf = painter->transform(); // TODO: Is there a better way?
                painter->setTransform(QTransform(transf).scale(1 / transf.m11(), 1));
                if(is_paint_frames)
                    timecode_str = QString::number(i);
                else
                    timecode_str = Timecode(GetCplEditRate(), i).GetAsString("%1:%2:%3:%4");
                painter->drawText(i * transf.m11() - timecode_str_width / 2, timecode_y_pos, timecode_str_width, timecode_str_height, Qt::AlignCenter, timecode_str);
                painter->setTransform(transf);
            }
        }
    }
    if(shown_pixel_distance_10_seconds >= 4) {
        for(quint32 i = exposed_left - (exposed_left % pixel_distance_10_seconds); i <= exposed_right; i += pixel_distance_10_seconds) {
            painter->drawLine(i, line_y_pos, i, track_height); // 画出刻度线
            if(shown_pixel_distance_10_seconds >= 65 && shown_pixel_distance_seconds < 65 && shown_pixel_distance_seconds > 6) {
                 painter->drawLine(i, line_y_pos_with_text, i, line_y_pos); // 突出刻度线
                QTransform transf = painter->transform(); // TODO: Is there a better way?
                painter->setTransform(QTransform(transf).scale(1 / transf.m11(), 1));

                if(is_paint_frames)
                    timecode_str = Timecode(GetCplEditRate(), i).GetFramesAsString();
                else
                    timecode_str = Timecode(GetCplEditRate(), i).GetAsString("%1:%2:%3:%4");
                painter->drawText(i * transf.m11() - timecode_str_width / 2, timecode_y_pos, timecode_str_width, timecode_str_height, Qt::AlignCenter, timecode_str);
                painter->setTransform(transf);
            }
        }
    }
    if(shown_pixel_distance_30_seconds >= 6) {
        for(quint32 i = exposed_left - (exposed_left % pixel_distance_30_seconds); i <= exposed_right; i += pixel_distance_30_seconds) {
            painter->drawLine(i,line_y_pos, i, track_height); // 30 seconds
            if(shown_pixel_distance_30_seconds >= 65 && shown_pixel_distance_10_seconds < 65) {
                 painter->drawLine(i, line_y_pos_with_text, i, line_y_pos); // 突出刻度线
                QTransform transf = painter->transform(); // TODO: Is there a better way?
                painter->setTransform(QTransform(transf).scale(1 / transf.m11(), 1));
                if(is_paint_frames)
                    timecode_str = Timecode(GetCplEditRate(), i).GetFramesAsString();
                else
                    timecode_str = Timecode(GetCplEditRate(), i).GetAsString("%1:%2:%3:%4");
                painter->drawText(i * transf.m11() - timecode_str_width / 2, timecode_y_pos, timecode_str_width, timecode_str_height, Qt::AlignCenter, timecode_str);
                painter->setTransform(transf);
            }
        }
    }
    if(shown_pixel_distance_minutes >= 6) {
        for(quint32 i = exposed_left - (exposed_left % pixel_distance_minutes); i <= exposed_right; i += pixel_distance_minutes) {
            painter->drawLine(i, line_y_pos, i, track_height); // minutes
            if(shown_pixel_distance_minutes >= 55 && shown_pixel_distance_30_seconds < 65) {
                 painter->drawLine(i, line_y_pos_with_text, i, line_y_pos); // 突出刻度线
                QTransform transf = painter->transform(); // TODO: Is there a better way?
                painter->setTransform(QTransform(transf).scale(1 / transf.m11(), 1));
                if(is_paint_frames)
                    timecode_str = Timecode(GetCplEditRate(), i).GetFramesAsString();
                else
                    timecode_str = Timecode(GetCplEditRate(), i).GetAsString("%1:%2:%3:%4");
                painter->drawText(i * transf.m11() - timecode_str_width / 2, timecode_y_pos, timecode_str_width, timecode_str_height, Qt::AlignCenter, timecode_str);
                painter->setTransform(transf);
            }
        }
    }

    if(shown_pixel_distance_15_minutes >= 7) {
        for(quint32 i = exposed_left - (exposed_left % pixel_distance_15_minutes); i <= exposed_right; i += pixel_distance_15_minutes) {
            painter->drawLine(i, line_y_pos, i, track_height); // 30 minutes
          //  painter->drawLine(i, 0, i, 0.14 * track_height); // 30 minutes
            if(shown_pixel_distance_15_minutes >= 55 && shown_pixel_distance_minutes < 55) {
                 painter->drawLine(i, line_y_pos_with_text, i, line_y_pos); // 突出刻度线
                QTransform transf = painter->transform(); // TODO: Is there a better way?
                painter->setTransform(QTransform(transf).scale(1 / transf.m11(), 1));
                if(is_paint_frames)
                    timecode_str = Timecode(GetCplEditRate(), i).GetFramesAsString();
                else
                    timecode_str = Timecode(GetCplEditRate(), i).GetAsString("%1:%2:%3:%4");
                painter->drawText(i * transf.m11() - timecode_str_width / 2, timecode_y_pos, timecode_str_width, timecode_str_height, Qt::AlignCenter, timecode_str);
                painter->setTransform(transf);
            }
        }
    }

    if(shown_pixel_distance_30_minutes >= 7) {
        for(quint32 i = exposed_left - (exposed_left % pixel_distance_30_minutes); i <= exposed_right; i += pixel_distance_30_minutes) {
            painter->drawLine(i, line_y_pos, i, track_height); // 30 minutes
            if(shown_pixel_distance_30_minutes >= 55 && shown_pixel_distance_15_minutes < 65) {
                 painter->drawLine(i, line_y_pos_with_text, i, line_y_pos); // 突出刻度线
                QTransform transf = painter->transform(); // TODO: Is there a better way?
                painter->setTransform(QTransform(transf).scale(1 / transf.m11(), 1));
                if(is_paint_frames)
                    timecode_str = Timecode(GetCplEditRate(), i).GetFramesAsString();
                else
                    timecode_str = Timecode(GetCplEditRate(), i).GetAsString("%1:%2:%3:%4");
                painter->drawText(i * transf.m11() - timecode_str_width / 2, timecode_y_pos, timecode_str_width, timecode_str_height, Qt::AlignCenter, timecode_str);
                painter->setTransform(transf);
            }
        }
    }
    if(shown_pixel_distance_hours >= 7 && shown_pixel_distance_frames < 4) {
        for(quint32 i = exposed_left - (exposed_left % pixel_distance_hours); i <= exposed_right; i += pixel_distance_hours) {
            painter->drawLine(i, line_y_pos, i, track_height); // 30 minutes
        }
    }
}

QSizeF GraphicsWidgetTimeline::TimelinePainter::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    QSizeF size;
    if(constraint.isValid() == false) {
        switch(which) {
            case Qt::MinimumSize:
                size = QSizeF(0, HEIGHT_TIMELINE_PAINTER);
                break;
            case Qt::PreferredSize:
                size = QSizeF(0, HEIGHT_TIMELINE_PAINTER);
                break;
            case Qt::MaximumSize:
                size = QSizeF(std::numeric_limits<qint32>::max(), std::numeric_limits<qint32>::max());
                break;
            case Qt::MinimumDescent:
                size = QSizeF(-1, -1);
                break;
            case Qt::NSizeHints:
                size = QSizeF(-1, -1);
                break;
            default:
                size = QSizeF(-1, -1);
                break;
        }
    }
    else {
        qWarning() << "sizeHint() is constraint.";
        size = constraint;
    }
    return size;
}
