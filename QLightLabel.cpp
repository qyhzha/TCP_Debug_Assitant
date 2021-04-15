#include "QLightLabel.h"

QLightLabel::QLightLabel(QWidget *parent) : QWidget(parent), m_state(false)
{
    update();
}

void QLightLabel::setState(bool state)
{
    m_state = state;

    update();
}

bool QLightLabel::state() const
{
    return m_state;
}

void QLightLabel::paintEvent(QPaintEvent *)
{
    int side = qMin(width(), height());

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 100.0, side / 100.0);

    if (m_state)
    {
        m_color = QColor(32, 255, 32);
    }
    else
    {
        m_color = Qt::gray;
    }

    QRadialGradient radial(0, 0, 50, 0, 0);
    radial.setSpread(QGradient::PadSpread);
    radial.setColorAt(0, m_color);
    radial.setColorAt(0.9, m_color);
    radial.setColorAt(1, m_color);

    painter.setPen(Qt::transparent);
    painter.setBrush(radial);
    painter.drawEllipse(-50, -50, 100, 100);
}

