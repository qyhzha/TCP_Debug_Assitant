#ifndef QLIGHTLABEL_H
#define QLIGHTLABEL_H

#include <QWidget>
#include <QPainter>
#include <QBrush>

class QLightLabel : public QWidget
{
        Q_OBJECT

    public:
        enum State {Normal, Working};

    protected:
        State m_state;
        QColor m_color;

        void paintEvent(QPaintEvent *);

    public:
        QLightLabel(QWidget *parent = NULL);
        void setState(const State &state);
};

#endif // QLIGHTLABEL_H
