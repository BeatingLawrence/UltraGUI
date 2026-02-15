#include "uglogger.h"

#include <QPaintEvent>
#include <QPainter>

using namespace gui;

#define INDENTATION 10

//================================================================
uint8_t UGLogger::_alpha(unsigned int index, unsigned int lastIndex)
{
    if (index > lastIndex) return 0;

    uint8_t toReturn = (uint8_t)(((float)index / (float)lastIndex) * 255.0f);
    return (255 - toReturn);
}
//================================================================
void UGLogger::_tick()
{
    m_animation++;
    repaint();

    if (m_animation != 20) return;  // animation end

    m_animation = 0;

    if (m_queue.empty())
        m_timer.stop();

    else
    {
        m_entries.push_front(qMakePair(m_queue.back().first, m_queue.back().second));
        m_queue.pop_back();

        if ((uint32_t)m_entries.size() > (m_numberOfLines + 1)) m_entries.pop_back();
    }
}
//================================================================
void UGLogger::mouseDoubleClickEvent(QMouseEvent* event)
{
    QWidget::mouseDoubleClickEvent(event);
    emit onDoubleClick();
}
//================================================================
void UGLogger::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::Antialiasing, false);
    QColor standardColor = palette().windowText().color();
    painter.setPen(QPen(QBrush(standardColor), 1));
    QFont widgetFont = font();
    widgetFont.setPointSize((height() / m_numberOfLines) - 2);
    painter.setFont(widgetFont);

    //=====================================================BORDER

    if (m_border)
    {
        painter.drawLine(1, 1, 1, height());
        painter.drawLine(1, 1, width() - 1, 1);
        painter.drawLine(width() - 1, 1, width() - 1, height());
    }

    //=====================================================MATH

    unsigned int step = height() / m_numberOfLines;
    unsigned int yPos = step;
    unsigned int diff = step - (unsigned int)(((float)m_animation / 20.0f) * (float)step);

    //=====================================================DRAW

    for (auto& el : m_entries)
    {
        QColor color;

        if (el.second.isValid())
            color = el.second;
        else
            color = standardColor;

        unsigned int y;

        if (m_timer.isActive())
            y = yPos - diff;
        else
            y = yPos;

        color.setAlpha(_alpha(y, height() + step));
        QPen pen(color);
        painter.setPen(pen);
        painter.drawText(INDENTATION, y, el.first);
        yPos += step;
    }
}
//================================================================
UGLogger::UGLogger(QWidget* parent)
    : QWidget(parent),
      m_animation(0),
      m_timer(this),
      m_border(true),
      m_aura(true),
      m_numberOfLines(5)
{
    m_timer.setInterval(10);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//================================================================
UGLogger::~UGLogger() { m_timer.stop(); }
//================================================================
void UGLogger::push(const QString& message, const QColor& textColor)
{
    if (message.isEmpty()) return;

    if (m_timer.isActive())
        m_queue.push_front(qMakePair(message, textColor));
    else
    {
        m_entries.push_front(qMakePair(message, textColor));

        if (((uint32_t)m_entries.size()) > (m_numberOfLines + 1)) m_entries.pop_back();

        m_timer.start();
    }
}
//================================================================
void UGLogger::clear()
{
    m_entries.clear();
    m_queue.clear();
    update();
}
//================================================================
