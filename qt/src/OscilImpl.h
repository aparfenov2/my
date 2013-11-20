#ifndef OSCILIMPL_H
#define OSCILIMPL_H

#include <QtQuick>

class OscilImpl : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor bkColor READ getBkColor WRITE setBkColor)
    Q_PROPERTY(QColor gridColor READ getGridColor WRITE setGridColor)
    Q_PROPERTY(QColor cursorColor READ getCursorColor WRITE setCursorColor)

    public:
        OscilImpl(QQuickItem *parent = 0): QQuickPaintedItem(parent){
            bkColor =  QColor("#292929");
            gridColor =  QColor(255,255,255,128);
            cursorColor =  QColor("#FF0000");

        }

        void paint(QPainter *painter)
        {

            QBrush brush(bkColor);
            QPen pen(gridColor);

            painter->setBrush(brush);
            painter->setPen(Qt::NoPen);
            //painter->setRenderHint(QPainter::Antialiasing);

            int width = boundingRect().width();
            int height = boundingRect().height();

            painter->drawRect(0, 0, width , height );
            painter->setBrush(Qt::NoBrush);
            painter->setPen(pen);

            int cw = 26;
            int ch = 20;

            for (int i=1; i*cw < width; i++) {
                painter->drawLine(i*cw,0,i*cw,height);
            }
            for (int i=1; i*ch < height; i++) {
                painter->drawLine(0,i*ch,width,i*ch);
            }


        }

        QColor getBkColor() const
        {
            return bkColor;
        }
        void setBkColor(QColor color)
        {
            bkColor = color;
        }

        QColor getGridColor() const
        {
            return gridColor;
        }
        void setGridColor(QColor color)
        {
            gridColor = color;
        }

        QColor getCursorColor() const
        {
            return cursorColor;
        }
        void setCursorColor(QColor color)
        {
            cursorColor = color;
        }

    private:
        QColor bkColor;
        QColor gridColor;
        QColor cursorColor;

    signals:
//        void bkColorChanged();
public slots:

};

#endif // OSCILIMPL_H
