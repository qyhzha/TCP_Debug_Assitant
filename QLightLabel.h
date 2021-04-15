#ifndef QLIGHTLABEL_H
#define QLIGHTLABEL_H

#include <QWidget>
#include <QPainter>
#include <QBrush>

class QLightLabel : public QWidget
{
        Q_OBJECT

    protected:
        bool m_state;
        QColor m_color;

        void paintEvent(QPaintEvent *);

    public:
        QLightLabel(QWidget *parent = NULL);

        void setState(bool state);
        bool state() const;
};

#endif // QLIGHTLABEL_H
