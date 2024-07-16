#include "ugplotter.h"

#include <QPainter>
#include <QPainterPath>
#include <QPdfWriter>
#include <QPixmap>
#include <cmath>

#include "ultragui.h"

#define LEFT_MARGIN 30
#define RIGHT_MARGIN 30
#define TOP_MARGIN 10
#define BUFFER_SIZE 10000

using namespace gui;

//===============================================
void UGPlotter::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter;
    //
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
    int span         = right.x() - origin.x();                        // pixel count for the x axis
    int range        = (h / 2) - TOP_MARGIN;                          // pixel count for half of y axis
    float xStep      = (m_sample / m_horizontalRange) * (float)span;  // number of pixels for one sample
    float xNotchStep = (m_notch / m_horizontalRange) * (float)span;   // number of pixels for one notch
    float yStep      = ((float)range) / m_verticalScale;              // number of pixels for value unit
    //
    // draw
    painter.drawLine(top, bottom - QPoint(0, 1));
    painter.drawLine(origin, right);
    painter.translate(origin);

    // vertical scale notches
    painter.drawLine(QPoint(-3, range), QPoint(3, range));
    painter.drawLine(QPoint(-3, -range), QPoint(3, -range));
    painter.drawLine(QPoint(-3, range / 2), QPoint(3, range / 2));
    painter.drawLine(QPoint(-3, -range / 2), QPoint(3, -range / 2));
    painter.drawLine(QPoint(-2, range / 4), QPoint(2, range / 4));
    painter.drawLine(QPoint(-2, -range / 4), QPoint(2, -range / 4));
    painter.drawLine(QPoint(-2, (range / 4) * 3), QPoint(2, (range / 4) * 3));
    painter.drawLine(QPoint(-2, -(range / 4) * 3), QPoint(2, -(range / 4) * 3));

    // lines
    painter.save();
    painter.setPen(QPen(UltraGui::faded(palette().text().color(), 0.2f), 1));
    painter.drawLine(QPoint(3, range), QPoint(span, range));
    painter.drawLine(QPoint(3, -range), QPoint(span, -range));
    painter.drawLine(QPoint(3, range / 2), QPoint(span, range / 2));
    painter.drawLine(QPoint(3, -range / 2), QPoint(span, -range / 2));
    painter.drawLine(QPoint(2, range / 4), QPoint(span, range / 4));
    painter.drawLine(QPoint(2, -range / 4), QPoint(span, -range / 4));
    painter.drawLine(QPoint(2, (range / 4) * 3), QPoint(span, (range / 4) * 3));
    painter.drawLine(QPoint(2, -(range / 4) * 3), QPoint(span, -(range / 4) * 3));
    painter.restore();
    //
    // time scale notches
    {
        int biased                   = m_sampleCount - m_cursor;
        float numberOfSamplePerNotch = m_notch / m_sample;
        float translated             = fmod((float)biased, numberOfSamplePerNotch);
        float n                      = (float)span - (translated * xStep);
        float f = ((float)biased - fmod((float)biased, numberOfSamplePerNotch)) * m_sample;

        painter.setPen(QPen(palette().text(), 1));

        for (int i = 0; i < 100; i++)  // max 100 notches
        {
            painter.drawLine(QPointF(n, -2), QPointF(n, 2));

            if (f > -0.001f) _drawFloat(painter, f, 10, n, false);

            f -= m_notch;
            n -= xNotchStep;

            if (n <= 0.0f) break;
        }
    }

    // lines
    painter.setPen(QPen(UltraGui::faded(palette().text().color(), 0.2f), 1));
    painter.drawLine(QPoint(3, range), QPoint(span, range));
    painter.drawLine(QPoint(3, -range), QPoint(span, -range));
    painter.drawLine(QPoint(3, range / 2), QPoint(span, range / 2));
    painter.drawLine(QPoint(3, -range / 2), QPoint(span, -range / 2));
    painter.drawLine(QPoint(2, range / 4), QPoint(span, range / 4));
    painter.drawLine(QPoint(2, -range / 4), QPoint(span, -range / 4));
    painter.drawLine(QPoint(2, (range / 4) * 3), QPoint(span, (range / 4) * 3));
    painter.drawLine(QPoint(2, -(range / 4) * 3), QPoint(span, -(range / 4) * 3));

    // text
    auto f = painter.font();
    f.setPixelSize(14);
    f.setBold(true);
    painter.setFont(f);
    painter.drawText(QRect(QPoint(0, -range), QPoint(span, -(range / 4) * 3)), m_name,
                     QTextOption(Qt::AlignCenter));
    f.setPixelSize(8);
    painter.setFont(f);
    painter.setPen(QPen(palette().text(), 1));
    _drawFloat(painter, m_verticalScale, range + 3, -30);
    _drawFloat(painter, -m_verticalScale, -range + 3, -30);
    _drawFloat(painter, m_verticalScale / 2, range / 2 + 3, -30);
    _drawFloat(painter, -m_verticalScale / 2, -range / 2 + 3, -30);
    _drawFloat(painter, m_verticalScale / 4, range / 4 + 3, -30);
    _drawFloat(painter, -m_verticalScale / 4, -range / 4 + 3, -30);
    _drawFloat(painter, (m_verticalScale / 4) * 3, (range / 4) * 3 + 3, -30);
    _drawFloat(painter, -(m_verticalScale / 4) * 3, -(range / 4) * 3 + 3, -30);

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
            {
                dataPath.moveTo(0, yStep * m_data.at(i) * -1.0f);
            }
            else
            {
                p += QPointF(xStep, 0);
            }

            p.setY(yStep * m_data.at(i) * -1.0f);
            dataPath.lineTo(p);

            if (p.x() > span)
            {
                break;
            }
        }
    }
    else
    {
        p = QPointF(span, yStep * m_data.at(m_cursor) * -1.0f);
        dataPath.moveTo(p);

        for (uint32_t i = m_cursor + 1; i < (uint32_t)m_data.size(); i++)
        {
            p -= QPointF(xStep, 0);
            p.setY(yStep * m_data.at(i) * -1.0f);
            dataPath.lineTo(p);

            if (p.x() <= 1)
            {
                break;
            }
        }
    }

    painter.drawPath(dataPath);
    painter.end();
}
//===============================================
void UGPlotter::_drawFloat(QPainter& painter, float number, int height, int width, bool seconds)
{
    painter.save();
    QRect r(width + 2, -height, 50, 10);
    auto f = painter.font();
    f.setPixelSize(8);
    painter.setFont(f);
    auto str = QString(seconds ? "%1s" : "%1").arg(number, 0, 'f', 2);

    while (str.size() > 3)
    {
        if (str[str.size() - 2] == '.') break;

        if (str[str.size() - 1] != '0') break;

        str.remove(str.size() - 1, 1);
    }

    painter.drawText(r, str, QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
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
    if (((int)m_cursor - (int)m_cursorStep) > 0)
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
      m_verticalScale(0.0f),
      m_verticalScaleZoomStep(0.0f),
      m_horizontalRange(0.0f),
      m_horizontalZoomStep(0.0f),
      m_notch(0.0f),
      m_sample(0.0f),
      m_cursor(0),
      m_cursorStep(0),
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
