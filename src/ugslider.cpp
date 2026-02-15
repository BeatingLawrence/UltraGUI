#include "ugslider.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>

using namespace gui;

//=========================================================
float UGSlider::_sliderToPixel(float slider)
{
    float span      = qAbs((float)((float)m_min - (float)m_max));
    float mintozero = qAbs((float)m_min - (float)m_zero);
    float sliderpos = mintozero + slider;
    float x         = sliderpos / span;
    x *= ((float)width() - _all_margins());
    x += _one_margin();

    return x;
}
//=========================================================
float UGSlider::_pixelToSlider(float pixels)
{
    pixels -= _one_margin();
    float maxpix = (float)width() - _all_margins();
    float x      = pixels / maxpix;

    float span = qAbs((float)((float)m_min - (float)m_max));
    x *= span;
    x += (float)m_min;
    x -= (float)m_zero;

    return x;
}
//=========================================================
float UGSlider::_cap(float val)
{
    float limit = (float)width() - _one_margin();
    if (val > limit) return limit;
    if (val < _one_margin()) return _one_margin();
    return val;
}
//=========================================================
float UGSlider::_all_margins() { return (float)height(); }
//=========================================================
float UGSlider::_one_margin() { return (float)height() / 2.0f; }
//=========================================================
void UGSlider::_incSlider(float x)
{
    if (m_slider + x > m_max)
        m_slider = m_max;
    else
        m_slider += x;
}
//=========================================================
void UGSlider::_decSlider(float x)
{
    if (m_slider - x < m_min)
        m_slider = m_min;
    else
        m_slider -= x;
}
//=========================================================
void UGSlider::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    float pixelZeroPos = _sliderToPixel(m_zero);
    float sliderPixel  = _sliderToPixel(m_slider);

    // background
    painter.fillRect(QRect(QPoint((int32_t)_sliderToPixel(m_min), (float)height() / 4.0f),
                           QPoint((int32_t)_sliderToPixel(m_max), (float)height() - (float)height() / 4.0f)),
                     palette().base());

    // slider
    painter.fillRect(QRect(QPoint(pixelZeroPos, (float)height() / 4.0f),
                           QPoint(sliderPixel, (float)height() - (float)height() / 4.0f)),
                     palette().accent());

    painter.setPen(QPen(palette().windowText(), 2));

    // zero-line
    painter.drawLine(QPoint(pixelZeroPos, 0), QPoint(pixelZeroPos, height()));

    // cursor
    painter.setPen(QPen(Qt::PenStyle::NoPen));
    painter.setBrush(palette().alternateBase());
    QRect cursor(QPoint(0, 0), QSize(height(), height()));
    cursor.moveCenter(QPoint(sliderPixel, height() / 2));
    painter.drawEllipse(cursor);
}
//=========================================================
void UGSlider::mouseMoveEvent(QMouseEvent* event)
{
    if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
    {
        float pos = event->position().x();
        m_slider  = _pixelToSlider(_cap(pos));
        emit onChange(m_slider);
        repaint();
    }

    QWidget::mouseMoveEvent(event);
}
//=========================================================
void UGSlider::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        m_slider = m_zero;
        emit onChange(m_slider);
    }
    else if (event->button() == Qt::LeftButton)
    {
        float pos = event->position().x();
        m_slider  = _pixelToSlider(_cap(pos));
        emit onChange(m_slider);
    }

    repaint();
}
//=========================================================
void UGSlider::wheelEvent(QWheelEvent* event)
{
    auto m    = event->angleDelta();
    float var = 0.01f * float(qAbs(m.y()));

    if (m.y() > 0)
        _decSlider(var);
    else
        _incSlider(var);

    emit onChange(m_slider);
    repaint();

    QWidget::wheelEvent(event);
}
//=========================================================
UGSlider::UGSlider(QWidget* parent)
    : QWidget(parent),
      m_slider(0.0f),
      m_zero(0.0f),
      m_max(100.0f),
      m_min(-20.0f)
{
}
//=========================================================
UGSlider::~UGSlider() {}
//=========================================================
void UGSlider::set(float value)
{
    if (value < m_min)
        m_slider = m_min;
    else if (value > m_max)
        m_slider = m_max;
    else
        m_slider = value;

    update();
}
//=========================================================
