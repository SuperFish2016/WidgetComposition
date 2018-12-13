#ifndef IMFPACKAGE_H
#define IMFPACKAGE_H
#include <QObject>
#include <QColor>
#include <QFileInfo>
class Asset : public QObject
{
    Q_OBJECT
public:
    enum eAssetType
    {
        AssetVideo = 0,
        AssetAudio,
        AssetSubtitles
    };
    bool HasAffinity() const{return true;}
public:
    QFileInfo source_file_;
    QString   annotation_;
    QString   hash_;

    qint64    entry_point_ = 0;
    qint64    repeat_count_ = 1;
    qint64    source_duration_ = 0;
    qint64    intrinsic_duration_ = 0;
};

class AssetMxfTrack : public Asset
{
public:
    AssetMxfTrack(){}
    ~AssetMxfTrack(){}


    QColor GetColor() const{return QColor(Qt::red);}
    bool GetIsOutsidePackage() {return false;}
    QString GetOriginalFileName(){return "Name";}
};
#endif // IMFPACKAGE_H
