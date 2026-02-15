#include "ugplotter.h"

#include <QPainter>
#include <QPainterPath>
#include <QPdfWriter>
#include <QPixmap>
#include <cmath>

#include "ultragui.h"

#define LEFT_MARGIN 30
#define RIGHT_MARGIN 10
#define TOP_MARGIN 10
#define BUFFER_SIZE 10000

using namespace gui;

//===============================================
void UGPlotter::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter;

    // prepare
    painter.begin(this);
    int w = painter.device()->width();
    int h = painter.device()->height();
    painter.fillRect(QRect(0, 0, w, h), (m_selected && !m_externalPaint) ? palette().base().color().lighter()
                                                                         : palette().base());
    QPoint top(LEFT_MARGIN, TOP_MARGIN);
    QPoint bottom(LEFT_MARGIN, h - TOP_MARGIN);
    QPoint origin(LEFT_MARGIN, h / 2);
    QPoint right(w - RIGHT_MARGIN, h / 2);

    float span       = w - RIGHT_MARGIN - LEFT_MARGIN;         // pixel count for the x axis
    float range      = ((float)h / 2.0f) - (float)TOP_MARGIN;  // pixel count for half of y axis
    float xSamplePix = (m_sample / m_horizontalRange) * span;  // number of pixels for one sample
    float xNotchPix  = (m_notch / m_horizontalRange) * span;   // number of pixels for one notch
    float yStep      = range / m_verticalScale;                // number of pixels for value unit

    // draw
    painter.drawLine(top, bottom - QPoint(0, 1));
    painter.drawLine(origin, right);
    painter.translate(origin);

    // vertical scale notches
    painter.drawLine(QPointF(-3.0f, range), QPointF(3.0f, range));
    painter.drawLine(QPointF(-3.0f, -range), QPointF(3.0f, -range));
    painter.drawLine(QPointF(-3.0f, range / 2.0f), QPointF(3.0f, range / 2.0f));
    painter.drawLine(QPointF(-3.0f, -range / 2.0f), QPointF(3.0f, -range / 2.0f));
    painter.drawLine(QPointF(-2.0f, range / 4.0f), QPointF(2.0f, range / 4.0f));
    painter.drawLine(QPointF(-2.0f, -range / 4.0f), QPointF(2.0f, -range / 4.0f));
    painter.drawLine(QPointF(-2.0f, (range / 4.0f) * 3.0f), QPointF(2.0f, (range / 4.0f) * 3.0f));
    painter.drawLine(QPointF(-2.0f, -(range / 4.0f) * 3.0f), QPointF(2.0f, -(range / 4.0f) * 3.0f));

    // lines
    painter.save();
    painter.setPen(QPen(UltraGui::faded(palette().text().color(), 0.2f), 1));
    painter.drawLine(QPointF(3.0f, range), QPointF(span, range));
    painter.drawLine(QPointF(3.0f, -range), QPointF(span, -range));
    painter.drawLine(QPointF(3.0f, range / 2.0f), QPointF(span, range / 2.0f));
    painter.drawLine(QPointF(3.0f, -range / 2.0f), QPointF(span, -range / 2.0f));
    painter.drawLine(QPointF(2.0f, range / 4.0f), QPointF(span, range / 4.0f));
    painter.drawLine(QPointF(2.0f, -range / 4.0f), QPointF(span, -range / 4.0f));
    painter.drawLine(QPointF(2.0f, (range / 4.0f) * 3.0f), QPointF(span, (range / 4.0f) * 3.0f));
    painter.drawLine(QPointF(2.0f, -(range / 4.0f) * 3.0f), QPointF(span, -(range / 4.0f) * 3.0f));
    painter.restore();

    // time scale notches
    {
        uint32_t biased       = m_sampleCount - m_cursor;
        float samplesPerNotch = m_notch / m_sample;
        float translated      = fmod((float)biased, samplesPerNotch);
        float n               = span - (translated * xSamplePix);
        float f               = ((float)biased - translated) * m_sample;

        painter.setPen(QPen(palette().text(), 1));

        while (true)
        {
            painter.drawLine(QPointF(n, -2), QPointF(n, 2));

            if (f > -0.001f) _drawFloat(painter, f, {n, 10.0f});

            f -= m_notch;
            n -= xNotchPix;

            if (n <= 0.0f) break;
        }
    }

    // text
    auto f = painter.font();
    f.setPixelSize(14);
    f.setBold(true);
    painter.setFont(f);
    painter.drawText(QRectF(QPointF(0, -range), QPointF(span, -(range / 4.0f) * 3.0f)), m_name,
                     QTextOption(Qt::AlignCenter));

    f.setPixelSize(8);
    painter.setFont(f);

    painter.setPen(QPen(palette().text(), 1));
    _drawFloat(painter, -m_verticalScale, {-4.0f, range}, true);
    _drawFloat(painter, m_verticalScale, {-4.0f, -range}, true);
    _drawFloat(painter, -m_verticalScale / 2.0f, {-4.0f, range / 2.0f}, true);
    _drawFloat(painter, m_verticalScale / 2.0f, {-4.0f, -range / 2.0f}, true);
    _drawFloat(painter, -m_verticalScale / 4.0f, {-4.0f, range / 4.0f}, true);
    _drawFloat(painter, m_verticalScale / 4.0f, {-4.0f, -range / 4.0f}, true);
    _drawFloat(painter, -(m_verticalScale / 4.0f) * 3.0f, {-4.0f, (range / 4.0f) * 3.0f}, true);
    _drawFloat(painter, (m_verticalScale / 4.0f) * 3.0f, {-4.0f, -(range / 4.0f) * 3.0f}, true);

    // data
    if (m_data.size() < 2)
    {
        painter.end();
        return;
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(palette().accent(), 1));
    QPointF p;
    QPainterPath dataPath;

    if (m_plotFromLeft)
    {
        for (uint32_t i = m_cursor; i < (uint32_t)m_data.size(); i++)
        {
            if (i == m_cursor)
                dataPath.moveTo(0, yStep * m_data.at(i) * -1.0f);
            else
                p += QPointF(xSamplePix, 0);

            p.setY(yStep * m_data.at(i) * -1.0f);
            dataPath.lineTo(p);

            if (p.x() > span) break;
        }
    }
    else
    {
        p = QPointF(span, yStep * m_data.at(m_cursor) * -1.0f);
        dataPath.moveTo(p);

        for (uint32_t i = m_cursor + 1; i < (uint32_t)m_data.size(); i++)
        {
            p -= QPointF(xSamplePix, 0);
            p.setY(yStep * m_data.at(i) * -1.0f);
            dataPath.lineTo(p);

            if (p.x() <= 1) break;
        }
    }

    painter.drawPath(dataPath);
    painter.end();
}
//===============================================
void UGPlotter::_drawFloat(QPainter& painter, float number, const QPointF& pos, bool alignRight)
{
    painter.save();
    QRectF r(0, 0, 50.0f, 10.0f);

    r.moveCenter(pos);

    if (alignRight)
        r.moveRight(pos.x());
    else
        r.moveLeft(pos.x());

    auto f = painter.font();
    f.setPixelSize(8);
    painter.setFont(f);
    auto str = QString("%1").arg(number, 0, 'f', 2);

    // while (str.size() > 3)
    // {
    //     if (str[str.size() - 2] == '.') break;
    //     if (str[str.size() - 1] != '0') break;
    //     str.remove(str.size() - 1, 1);
    // }

    painter.drawText(r, str, QTextOption((alignRight ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter));
    painter.restore();
}
//===============================================
void UGPlotter::zoomIn_X()
{
    if ((m_horizontalRange - m_horizontalZoomStep) > 0) m_horizontalRange -= m_horizontalZoomStep;
    update();
}
//===============================================
void UGPlotter::zoomOut_X()
{
    m_horizontalRange += m_horizontalZoomStep;
    update();
}
//===============================================
void UGPlotter::zoomIn_Y()
{
    if ((m_verticalScale - m_verticalScaleZoomStep) > 0) m_verticalScale -= m_verticalScaleZoomStep;
    update();
}
//===============================================
void UGPlotter::zoomOut_Y()
{
    m_verticalScale += m_verticalScaleZoomStep;
    update();
}
//===============================================
void UGPlotter::scroll_L()
{
    if ((m_cursor + m_cursorStep) < (uint32_t)m_data.size()) m_cursor += m_cursorStep;
    update();
}
//===============================================
void UGPlotter::scroll_R()
{
    if ((m_cursor - m_cursorStep) > 0)
        m_cursor -= m_cursorStep;
    else
        m_cursor = 0;

    update();
}
//===============================================
void UGPlotter::resetCursor()
{
    m_cursor = 0;
    update();
}
//===============================================
UGPlotter::UGPlotter(QWidget* parent)
    : QWidget(parent),
      m_verticalScale(1.0f),
      m_verticalScaleZoomStep(0.5f),
      m_horizontalRange(10.0f),
      m_horizontalZoomStep(1.0f),
      m_notch(1.0f),
      m_sample(0.2f),
      m_cursor(0),
      m_cursorStep(10),
      m_sampleCount(0),
      m_plotFromLeft(false),
      m_name(),
      m_selected(false),
      m_paused(false),
      m_externalPaint(false)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
}
//===============================================
void UGPlotter::setVerticalScale(float max, float step)
{
    m_verticalScaleZoomStep = step;
    m_verticalScale         = max;
    update();
}
//===============================================
void UGPlotter::setHorizontalScale(float range, float sampleDuration, float step)
{
    m_horizontalRange    = range;
    m_sample             = sampleDuration;
    m_horizontalZoomStep = step;
    update();
}
//===============================================
void UGPlotter::setNotchDuration(float duration)
{
    m_notch = duration;
    update();
}
//===============================================
void UGPlotter::setHorizontalZoomStep(float step) { m_horizontalZoomStep = step; }
//===============================================
void UGPlotter::clear()
{
    m_data.clear();
    m_sampleCount = 0;
    m_cursor      = 0;
    update();
}
//===============================================
void UGPlotter::add(float value)
{
    if (m_paused) return;

    m_data.push_front(value);
    m_sampleCount++;

    if (m_data.size() == BUFFER_SIZE) m_data.pop_back();

    update();
}
//===============================================
void UGPlotter::setName(const QString& name)
{
    m_name = name;
    update();
}
//===============================================
QString UGPlotter::name() { return m_name; }
//===============================================
void UGPlotter::setSelected(bool selected)
{
    m_selected = selected;
    update();
}
//===============================================
void UGPlotter::renderTo(QPaintDevice* device)
{
    m_externalPaint = true;
    render(device, QPoint(), QRegion(), RenderFlags());
    m_externalPaint = false;
}
//===============================================
void UGPlotter::renderTo(QPainter* painter)
{
    m_externalPaint = true;
    render(painter, QPoint(), QRegion(), RenderFlags());
    m_externalPaint = false;
}
//===============================================
void UGPlotter::setPaused(bool paused)
{
    m_paused = paused;
    if (!paused) m_cursor = 0;
    update();
}
//===============================================
void UGPlotter::setScrollStep(float step) { m_cursorStep = ceil(step / m_sample); }
//===============================================
void UGPlotter::setScrollStep(uint32_t step) { m_cursorStep = step; }
//===============================================
gui::UGPlotter::~UGPlotter() {}
//===============================================
