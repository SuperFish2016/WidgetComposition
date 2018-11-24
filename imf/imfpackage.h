#ifndef IMFPACKAGE_H
#define IMFPACKAGE_H
#include <QObject>
#include <QColor>
class Asset : public QObject
{
    Q_OBJECT
public:
    enum eAssetType
    {
        mxf = 0,

    };
    bool HasAffinity() const{return true;}
};

class AssetMxfTrack : public Asset
{
public:
    AssetMxfTrack();
    QColor GetColor() const{return QColor(Qt::red);}
    bool GetIsOutsidePackage() {return false;}
    QString GetOriginalFileName(){return "Name";}
};
#endif // IMFPACKAGE_H
