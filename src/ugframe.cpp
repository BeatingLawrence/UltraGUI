#include "ugframe.h"

#include <QPainter>

using namespace gui;

//===============================================
void UGFrame::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QColor c;

    if (m_timer.isActive())
        c = palette().accent().color();
    else
        c = palette().base().color();

    c.setAlpha(m_alpha);
    painter.setPen(QPen(c, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect().adjusted(1, 1, -2, -2), 3, 3);
}
//===============================================
void UGFrame::timerTick()
{
    m_alpha -= 20;

    if (m_alpha <= 0)
    {
        if (m_blinksRemaining > 0)
        {
            m_blinksRemaining--;

            if (m_blinksRemaining == 0)
            {
                m_alpha = 255;
                stop();
                return;
            }
        }
        m_alpha = 255;
    }

    repaint();
}
//===============================================
UGFrame::UGFrame(QWidget* parent)
    : QFrame(parent),
      m_timer(),
      m_alpha(255),
      m_blinksRemaining(0)
{
    m_timer.setSingleShot(false);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerTick()));
}
//===============================================
UGFrame::~UGFrame() {}
//===============================================
void UGFrame::blink(bool repeat)
{
    m_blinksRemaining = repeat ? -1 : 1;
    m_alpha           = 255;
    if (!m_timer.isActive()) m_timer.start(40);
}
//===============================================
void UGFrame::blink(uint8_t blinks)
{
    if (blinks == 0)
    {
        stop();
        return;
    }

    m_blinksRemaining = static_cast<int>(blinks);
    m_alpha           = 255;
    if (!m_timer.isActive()) m_timer.start(40);
}
//===============================================
void UGFrame::stop()
{
    m_timer.stop();
    m_blinksRemaining = 0;
    m_alpha           = 255;
    repaint();
}
//===============================================
