#ifndef DISPATCHER_H
#define DISPATCHER_H
#include <QtQuick>


class DMEModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int channel READ getChannel)
    Q_PROPERTY(QString suffixStr READ getSuffixStr)
public:
    QString getSuffixStr() {
        return QString("Y");
    }

    int getChannel() {
        return 25;
    }
};

class AppModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DMEModel* dmeModel READ getDmeModel NOTIFY dmeModelChanged)
private:
    DMEModel _dmeModel;
public:
    DMEModel * getDmeModel() {
        return &_dmeModel;
    }

    Q_INVOKABLE void refresh() {
        emit dmeModelChanged();
    }

signals:
    void dmeModelChanged();
};


#endif // DISPATCHER_H
