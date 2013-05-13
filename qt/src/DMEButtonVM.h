#ifndef DMEBUTTONMODEL_H
#define DMEBUTTONMODEL_H
#include <QtQuick>
#include <QtQuick/QQuickView>


class DMEButtonVM : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString strValue READ getStrValue NOTIFY strValueChanged)

public:
    Q_INVOKABLE
    void onEnterPressed() {
    }

    QString getStrValue() {
        return QString("25Y");
    }

signals:
    void strValueChanged();
};

#endif // DMEBUTTONMODEL_H
