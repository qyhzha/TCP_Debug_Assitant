#include "QLightLabel.h"

QLightLabel::QLightLabel(QWidget *parent) : QWidget(parent), m_state(Normal)
{
    update();
}

void QLightLabel::setState(const QLightLabel::State &state)
{
    m_state = state;

    update();
}

void QLightLabel::paintEvent(QPaintEvent *)
{
    int side = qMin(width(), height());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 100.0, side / 100.0);

    switch (m_state)
    {
        case Normal:
            m_color = Qt::gray;
            break;
        case Working:
            m_color = QColor(32, 255, 32);
            break;
        default:
            m_color = Qt::gray;
    }

    QRadialGradient radial(0, 0, 50, 0,
                           0);    //设置圆的原点和焦点在中心,半径50
    radial.setSpread(QGradient::PadSpread);
    radial.setColorAt(0, m_color);
    radial.setColorAt(0.9, m_color);
    radial.setColorAt(1, m_color);

    painter.setPen(Qt::transparent);
    painter.setBrush(radial);
    painter.drawEllipse(-50, -50, 100, 100);
}

