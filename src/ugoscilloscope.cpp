#include "ugoscilloscope.h"

#include <QPainter>
#include <QPainterPath>
#include <QPdfWriter>
#include <algorithm>
#include <cmath>
#include <stdexcept>

#define LEFT_MARGIN 10
#define RIGHT_MARGIN 10
#define TOP_MARGIN 10

using namespace gui;

//===============================================
void UGOscilloscope::_computeBufSize()
{
    float totalTime = (float)m_hdivisions * m_hdivisionVal;
    qsizetype size  = (float)(totalTime / m_sample);

    m_data.alloc(size);
}
//===============================================
void UGOscilloscope::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter;

    // prepare
    painter.begin(this);
    int w = painter.device()->width();
    int h = painter.device()->height();
    painter.fillRect(QRect(0, 0, w, h), (m_selected && !m_externalPaint) ? palette().base().color().lighter()
                                                                         : palette().base());

    float span    = w - RIGHT_MARGIN - LEFT_MARGIN;         // pixel count for the x axis
    float range   = ((float)h / 2.0f) - (float)TOP_MARGIN;  // pixel count for half of y axis
    float hdivpix = (span / 2.0f) / (float)m_hdivisions;
    float vdivpix = range / (float)m_vdivisions;

    // number of x pixels for one sample
    float xSamplePix = (m_sample / (m_hdivisions * m_hdivisionVal)) * span;

    // number of y pixels for value unit
    float ySamplePix = range / (m_vdivisions * m_vdivisionVal);

    QPointF center(rect().center());
    QPointF p(LEFT_MARGIN, center.y());

    QColor linesColor(palette().text().color());
    linesColor.setAlpha(80);
    painter.setPen(QPen(linesColor, 1));

    // draw horizontal lines
    for (int i = 0; i <= m_vdivisions; i++)
    {
        painter.drawLine(p, p + QPointF(span, 0));
        p += QPointF(0, vdivpix);
    }

    p = QPointF(LEFT_MARGIN, center.y()) + QPointF(0, -vdivpix);

    for (int i = 0; i < m_vdivisions; i++)
    {
        painter.drawLine(p, p + QPointF(span, 0));
        p += QPointF(0, -vdivpix);
    }

    // draw vertical lines

    p = QPointF(center.x(), TOP_MARGIN);

    for (int i = 0; i <= m_hdivisions; i++)
    {
        painter.drawLine(p, p + QPointF(0, range * 2));
        p += QPointF(hdivpix, 0);
    }

    p = QPointF(center.x(), TOP_MARGIN) + QPointF(-hdivpix, 0);

    for (int i = 0; i < m_hdivisions; i++)
    {
        painter.drawLine(p, p + QPointF(0, range * 2));
        p += QPointF(-hdivpix, 0);
    }

    // labels (same font box size style as UGPlotter)
    {
        auto f = painter.font();
        f.setPixelSize(8);
        painter.setFont(f);
        painter.setPen(QPen(palette().text(), 1));

        const float maxValue   = m_vdivisionVal * m_vdivisions;
        const float timeWindow = m_hdivisionVal * m_hdivisions;

        painter.save();
        painter.translate(LEFT_MARGIN, center.y());

        auto drawLabel = [&painter](float value, float x, float y, Qt::Alignment horizontalAlign)
        {
            QRectF r(0.0f, 0.0f, 50.0f, 10.0f);

            if (horizontalAlign == Qt::AlignLeft)
                r.moveLeft(x);
            else if (horizontalAlign == Qt::AlignRight)
                r.moveRight(x);
            else
                r.moveCenter(QPointF(x, y));

            if (horizontalAlign != Qt::AlignHCenter) r.moveCenter(QPointF(r.center().x(), y));

            painter.drawText(r, QString("%1").arg(value, 0, 'f', 2),
                             QTextOption(horizontalAlign | Qt::AlignVCenter));
        };

        drawLabel(maxValue, 0.0f, -range, Qt::AlignLeft);
        drawLabel(-maxValue, 0.0f, range, Qt::AlignLeft);

        drawLabel(-(timeWindow / 2.0f), 0.0f, 10.0f, Qt::AlignLeft);
        drawLabel(0.0f, span / 2.0f, 10.0f, Qt::AlignHCenter);
        drawLabel(timeWindow / 2.0f, span, 10.0f, Qt::AlignRight);

        painter.restore();
    }

    if (!m_frame.isEmpty())
    {
        uint32_t samplesToShow = m_samplesInWindow;
        if (samplesToShow < 3)
            samplesToShow = 3;

        if ((samplesToShow % 2) == 0) samplesToShow++;

        const qreal stepX        = span / static_cast<qreal>(samplesToShow - 1);
        const int64_t halfWindow = static_cast<int64_t>((samplesToShow - 1) / 2);
        bool hasCurrentSegment   = false;
        QPainterPath framePath;

        for (uint32_t i = 0; i < samplesToShow; ++i)
        {
            const int64_t offset = static_cast<int64_t>(i) - halfWindow;
            const qreal x        = static_cast<qreal>(i) * stepX;

            const int64_t idx64 = static_cast<int64_t>(m_zeroIndex) + static_cast<int64_t>(offset);
            if (idx64 < 0 || idx64 >= m_frame.size())
            {
                hasCurrentSegment = false;
                continue;
            }

            const QPointF point(x, -(m_frame.at(static_cast<int>(idx64)) * ySamplePix));
            if (!hasCurrentSegment)
            {
                framePath.moveTo(point);
                hasCurrentSegment = true;
            }
            else
            {
                framePath.lineTo(point);
            }
        }

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(QPen(palette().accent(), 1));
        painter.translate(LEFT_MARGIN, center.y());
        painter.drawPath(framePath);
        return;
    }

    if (m_data.size() < 2) return;

    // graph
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(palette().accent(), 1));

    QPainterPath dataPath;

    p.setX(0);
    p.setY(-(m_data[0] * ySamplePix));
    dataPath.moveTo(p);

    for (qsizetype i = 1; i < m_data.size(); i++)
    {
        if (p.x() + xSamplePix > span)
        {
            dataPath.moveTo(0, p.y());
            p.setX(xSamplePix);
        }
        else
        {
            p += QPointF(xSamplePix, 0);
        }

        p.setY(-(m_data[i] * ySamplePix));
        dataPath.lineTo(p);
    }

    painter.translate(LEFT_MARGIN, center.y());
    painter.drawPath(dataPath);
}
//===============================================
void UGOscilloscope::_drawFloat(QPainter& painter, float number, const QPointF& pos, bool alignRight)
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
UGOscilloscope::UGOscilloscope(QWidget* parent)
    : QWidget(parent),
      m_sample(0.2f),
      m_hdivisionVal(10.0f),
      m_vdivisionVal(0.25f),
      m_hdivisions(3),
      m_vdivisions(4),
      m_frame(),
      m_zeroIndex(0),
      m_samplesInWindow(151),
      m_name("Test"),
      m_selected(false),
      m_externalPaint(false)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    _computeBufSize();
}
//===============================================
void UGOscilloscope::setVerticalScale(float divsize, uint16_t divno)
{
    m_vdivisionVal = divsize;
    m_vdivisions   = divno;
    update();
}
//===============================================
void UGOscilloscope::setHorizontalScale(float divsize, uint16_t divno, float samplesize)
{
    m_hdivisionVal = divsize;
    m_hdivisions   = divno;
    m_sample       = samplesize;
    const float totalTime = m_hdivisionVal * m_hdivisions;
    if (m_sample > 0.0f)
    {
        const uint32_t computed = static_cast<uint32_t>(std::round(totalTime / m_sample)) + 1;
        m_samplesInWindow       = std::max<uint32_t>(3, computed | 1U);
    }
    _computeBufSize();
    update();
}
//===============================================
void UGOscilloscope::setFrame(const QVector<float>& frame, uint32_t zeroIndex)
{
    m_frame = frame;

    if (m_frame.isEmpty())
    {
        m_zeroIndex = 0;
        update();
        return;
    }

    const uint32_t last = static_cast<uint32_t>(m_frame.size() - 1);
    m_zeroIndex         = std::min(zeroIndex, last);
    update();
}
//===============================================
void UGOscilloscope::setZeroIndex(uint32_t zeroIndex)
{
    if (m_frame.isEmpty()) return;

    const uint32_t last = static_cast<uint32_t>(m_frame.size() - 1);
    m_zeroIndex         = std::min(zeroIndex, last);
    update();
}
//===============================================
void UGOscilloscope::setMaxValue(float maxValue)
{
    if (maxValue <= 0.0f) return;

    const uint16_t safeDivisions = std::max<uint16_t>(1, m_vdivisions);
    m_vdivisionVal               = maxValue / static_cast<float>(safeDivisions);
    update();
}
//===============================================
void UGOscilloscope::setTimeWindow(float t)
{
    if (t <= 0.0f) return;

    const uint16_t safeDivisions = std::max<uint16_t>(1, m_hdivisions);
    m_hdivisionVal               = t / static_cast<float>(safeDivisions);
    if (m_samplesInWindow < 3) m_samplesInWindow = 3;
    m_sample = t / static_cast<float>(m_samplesInWindow - 1);
    _computeBufSize();
    update();
}
//===============================================
void UGOscilloscope::setSampleWindow(float t, uint32_t samplesInWindow, uint32_t centerIndex)
{
    if (t <= 0.0f) return;

    if (samplesInWindow < 3) samplesInWindow = 3;
    if ((samplesInWindow % 2) == 0) samplesInWindow++;

    const uint16_t safeDivisions = std::max<uint16_t>(1, m_hdivisions);
    m_hdivisionVal               = t / static_cast<float>(safeDivisions);
    m_samplesInWindow            = samplesInWindow;
    m_sample                     = t / static_cast<float>(m_samplesInWindow - 1);

    if (!m_frame.isEmpty())
    {
        const uint32_t last = static_cast<uint32_t>(m_frame.size() - 1);
        m_zeroIndex         = std::min(centerIndex, last);
    }
    else
    {
        m_zeroIndex = centerIndex;
    }

    _computeBufSize();
    update();
}
//===============================================
void UGOscilloscope::clear()
{
    m_data.clear();
    m_frame.clear();
    m_zeroIndex = 0;
    update();
}
//===============================================
void UGOscilloscope::add(float value)
{
    if (!m_frame.isEmpty())
    {
        m_frame.clear();
        m_zeroIndex = 0;
    }

    m_data.add(value);
    update();
}
//===============================================
void UGOscilloscope::setName(const QString& name)
{
    m_name = name;
    update();
}
//===============================================
QString UGOscilloscope::name() { return m_name; }
//===============================================
void UGOscilloscope::setSelected(bool selected)
{
    m_selected = selected;
    update();
}
//===============================================
void UGOscilloscope::renderTo(QPaintDevice* device)
{
    m_externalPaint = true;
    render(device, QPoint(), QRegion(), RenderFlags());
    m_externalPaint = false;
}
//===============================================
void UGOscilloscope::renderTo(QPainter* painter)
{
    m_externalPaint = true;
    render(painter, QPoint(), QRegion(), RenderFlags());
    m_externalPaint = false;
}
//===============================================
gui::UGOscilloscope::~UGOscilloscope() {}
//===============================================
void UGOscilloscope::addBlock(const float values[], int size)
{
    if (!m_frame.isEmpty())
    {
        m_frame.clear();
        m_zeroIndex = 0;
    }

    for (int i = 0; i < size; i++) m_data.add(values[i]);
    update();
}
//===============================================
FloatCircBuf::FloatCircBuf()
    : m_buf(),
      m_cursor(0),
      m_size(0)
{
}
//===============================================
void FloatCircBuf::alloc(qsizetype size)
{
    m_buf.clear();
    m_buf.assign(size, 0.0f);
}
//===============================================
void FloatCircBuf::add(float val)
{
    if (m_buf.empty()) throw std::invalid_argument("uninitialized buffer");

    m_buf[m_cursor] = val;

    if (m_size < m_buf.size()) m_size++;

    m_cursor++;

    if (m_cursor >= m_buf.size()) m_cursor = 0;
}
//===============================================
void FloatCircBuf::clear()
{
    m_cursor = 0;
    m_size   = 0;
}
//===============================================
float& FloatCircBuf::operator[](qsizetype index)
{
    if (!m_size) throw std::invalid_argument("size is zero");
    return m_buf[index % m_size];
}
//===============================================
