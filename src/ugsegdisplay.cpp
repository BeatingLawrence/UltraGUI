#include "ugsegdisplay.h"

#include <QPainter>
#include <QPainterPath>

#define PAINT_SEG(x) _paintSeg(painter, x, geom, vpath, hpath);

using namespace gui;

//=========================================================
void UGSegDisplay::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto str      = _print();
    uint8_t dec   = _countDec(str);
    uint8_t integ = _countInt(str);

    if (!dec && !integ) integ = m_int + m_dec / 2;

    bool minus = _isNegative();

    int totaldigitsnum = (integ * 2) + dec;

    float minuswidth  = !minus && m_autominus ? 0.0f : (float)width() * 0.08f;
    float pixperdigit = ((float)width() - minuswidth) / (float)totaldigitsnum;

    QRectF r(QPointF(minuswidth + 2.0f, 0), QSizeF(pixperdigit * 2, height()));

    painter.setBrush(palette().base());
    painter.setPen(QPen(Qt::PenStyle::NoPen));

    painter.drawRoundedRect(rect(), 8, 8);
    painter.setBrush(palette().accent());

    if (m_border)
        painter.setPen(QPen(palette().text(), 1));
    else
        painter.setPen(QPen(Qt::PenStyle::NoPen));

    if (minus) _paintMinus(painter, minuswidth);

    int i           = 0;
    float textStart = 0.0f;
    float step      = pixperdigit * 2;

    for (i = 0; i < str.size(); i++)
    {
        if (str[i] == '.')
        {
            painter.drawEllipse(QPointF(r.x() - 8.0f, height() - 6), 4, 4);

            // resize the rect for smaller decimal digits
            r.setWidth(pixperdigit);
            r.setHeight((float)height() / 2.0f);
            r.moveBottom((float)height());
            textStart = r.x();
            step      = pixperdigit;
            continue;
        }

        _paintDigit(painter, r, (uint8_t)(str[i].unicode()));
        r.translate(step, 0);
    }

    if (!dec) return;

    r.translate(-step, 0);  // revert last translation
    QRectF textrect(QPointF(textStart, 0), QSize((pixperdigit * dec) - 5.0f, (float)height() / 2.0f));
    painter.drawText(textrect, Qt::AlignVCenter | Qt::AlignRight, m_unit);
}
//=========================================================
bool UGSegDisplay::_isNegative()
{
    if (isnan(m_value)) return false;
    return m_value < 0;
}
//=========================================================
uint8_t UGSegDisplay::_countInt(const QString& str)
{
    if (!str.contains('.')) return 0;
    return str.indexOf('.');
}
//=========================================================
uint8_t UGSegDisplay::_countDec(const QString& str)
{
    if (!str.contains('.')) return 0;
    return str.size() - str.indexOf('.') - 1;
}
//=========================================================
void UGSegDisplay::_paintMinus(QPainter& painter, float w)
{
    float thick = (float)height() * 0.04f;

    QPainterPath path(QPointF(-(w / 2.0f), 0));
    path.lineTo(-(w / 2.0f) + (thick / 2.0f), -(thick / 2.0f));
    path.lineTo((w / 2.0f) - (thick / 2.0f), -(thick / 2.0f));
    path.lineTo(w / 2.0f, 0);
    path.lineTo((w / 2.0f) - (thick / 2.0f), thick / 2.0f);
    path.lineTo(-(w / 2.0f) + (thick / 2.0f), thick / 2.0f);
    path.closeSubpath();

    painter.save();
    painter.translate((w / 2.0f) + 1.0f, (float)height() / 2.0f);
    painter.drawPath(path);
    painter.restore();
}
//=========================================================
float UGSegDisplay::_normalize(float value)
{
    if (std::isnan(value) || std::isinf(value)) return value;  // do not attempt to normalize

    float p = qPow((float)10.0f, (float)m_dec);
    value   = value * p;
    int x   = value;
    return (float)x / p;
}
//=========================================================
QString UGSegDisplay::_print()
{
    if (std::isnan(m_value)) return QString(m_int, '-') + QString(1, '.') + QString(m_dec, '-');
    if (std::isinf(m_value)) return QString(m_int, '-') + QString(1, '.') + QString(m_dec, '-');

    QString format("%0#");
    format.append(QString("%1").arg(m_value < 0 ? (m_int + m_dec + 2) : (m_int + m_dec + 1)));
    format.append('.');
    format.append(QString("%1").arg(m_dec));
    format.append('f');

    auto str = QString::asprintf(format.toStdString().c_str(), m_value);

    str.remove(0, str.size() - (m_int + m_dec + 1));  // limit the length

    return str;
}
//=========================================================
//        1
//      -----
//    |       |
//  6 |       | 2
//    |   7   |
//      -----
//    |       |
//  5 |       | 3
//    |       |
//      -----
//        4
//=========================================================
void UGSegDisplay::_paintDigit(QPainter& painter, const QRectF& zone, uint8_t digit)
{
    float w     = zone.width() / 2.0f;            // width of the horizontal segment
    float h     = (zone.height() / 7.0f) * 3.0f;  // height of the vertical segment
    float thick = zone.height() * 0.12f;          // thickness of the segment

    // adjustments
    h += 1.0f;
    thick -= 1.0f;

    QPainterPath vpath(QPointF(0, -(h / 2.0f)));
    vpath.lineTo(thick / 2.0f, -(h / 2.0f) + (thick / 2.0f));
    vpath.lineTo(thick / 2.0f, (h / 2.0f) - (thick / 2.0f));
    vpath.lineTo(0, h / 2.0f);
    vpath.lineTo(-(thick / 2.0f), (h / 2.0f) - (thick / 2.0f));
    vpath.lineTo(-(thick / 2.0f), -(h / 2.0f) + (thick / 2.0f));
    vpath.closeSubpath();

    QPainterPath hpath(QPointF(-(w / 2.0f), 0));
    hpath.lineTo(-(w / 2.0f) + (thick / 2.0f), -(thick / 2.0f));
    hpath.lineTo((w / 2.0f) - (thick / 2.0f), -(thick / 2.0f));
    hpath.lineTo(w / 2.0f, 0);
    hpath.lineTo((w / 2.0f) - (thick / 2.0f), thick / 2.0f);
    hpath.lineTo(-(w / 2.0f) + (thick / 2.0f), thick / 2.0f);
    hpath.closeSubpath();

    float step = thick / 2.0f;

    Geometry geom = {};
    geom.centerX  = step + (w / 2.0f) + 1.0f;
    geom.centerX1 = step + 1.0f;
    geom.centerX2 = step + w + 1.0f;

    geom.centerY  = (zone.height() / 2.0f);
    geom.centerY1 = step + 1.0f;
    geom.centerY2 = step + (h / 2.0f) + 1.0f;
    geom.centerY3 = zone.height() - step - (h / 2.0f) - 1.0f;
    geom.centerY4 = zone.height() - step - 1.0f;

    painter.save();

    painter.translate(zone.topLeft());

    switch (digit)
    {
        case 0:
        case '0':
            PAINT_SEG(1);
            PAINT_SEG(2);
            PAINT_SEG(3);
            PAINT_SEG(4);
            PAINT_SEG(5);
            PAINT_SEG(6);
            break;

        case 1:
        case '1':
            PAINT_SEG(2);
            PAINT_SEG(3);
            break;

        case 2:
        case '2':
            PAINT_SEG(1);
            PAINT_SEG(2);
            PAINT_SEG(4);
            PAINT_SEG(5);
            PAINT_SEG(7);
            break;

        case 3:
        case '3':
            PAINT_SEG(1);
            PAINT_SEG(2);
            PAINT_SEG(3);
            PAINT_SEG(4);
            PAINT_SEG(7);
            break;

        case 4:
        case '4':
            PAINT_SEG(2);
            PAINT_SEG(3);
            PAINT_SEG(6);
            PAINT_SEG(7);
            break;

        case 5:
        case '5':
            PAINT_SEG(1);
            PAINT_SEG(3);
            PAINT_SEG(4);
            PAINT_SEG(6);
            PAINT_SEG(7);
            break;

        case 6:
        case '6':
            PAINT_SEG(1);
            PAINT_SEG(3);
            PAINT_SEG(4);
            PAINT_SEG(5);
            PAINT_SEG(6);
            PAINT_SEG(7);
            break;

        case 7:
        case '7':
            PAINT_SEG(1);
            PAINT_SEG(2);
            PAINT_SEG(3);
            break;

        case 8:
        case '8':
            PAINT_SEG(1);
            PAINT_SEG(2);
            PAINT_SEG(3);
            PAINT_SEG(4);
            PAINT_SEG(5);
            PAINT_SEG(6);
            PAINT_SEG(7);
            break;

        case 9:
        case '9':
            PAINT_SEG(1);
            PAINT_SEG(2);
            PAINT_SEG(3);
            PAINT_SEG(4);
            PAINT_SEG(6);
            PAINT_SEG(7);
            break;

        case '-':
            PAINT_SEG(7);
            break;
    }

    painter.restore();
}
//=========================================================
void UGSegDisplay::_paintSeg(QPainter& painter, uint8_t segment, const Geometry& geom, const QPainterPath& vs,
                             const QPainterPath& hs)
{
    painter.save();

    switch (segment)
    {
        case 1:
            painter.translate(geom.centerX, geom.centerY1);
            painter.drawPath(hs);
            break;

        case 4:
            painter.translate(geom.centerX, geom.centerY4);
            painter.drawPath(hs);
            break;

        case 7:
            painter.translate(geom.centerX, geom.centerY);
            painter.drawPath(hs);
            break;

        case 2:
            painter.translate(geom.centerX2, geom.centerY2);
            painter.drawPath(vs);
            break;

        case 3:
            painter.translate(geom.centerX2, geom.centerY3);
            painter.drawPath(vs);
            break;

        case 5:
            painter.translate(geom.centerX1, geom.centerY3);
            painter.drawPath(vs);
            break;

        case 6:
            painter.translate(geom.centerX1, geom.centerY2);
            painter.drawPath(vs);
            break;
    }

    painter.restore();
}
//=========================================================
UGSegDisplay::UGSegDisplay(QWidget* parent)
    : QWidget(parent),
      m_value(0.0f),
      m_int(2),
      m_dec(2),
      m_unit(),
      m_border(true),
      m_autominus(true)
{
}
//=========================================================
UGSegDisplay::~UGSegDisplay() {}
//=========================================================
void UGSegDisplay::setValue(float value)
{
    m_value = _normalize(value);
    update();
}
//=========================================================
void UGSegDisplay::setDigitsNum(uint8_t integer, uint8_t decimal)
{
    if (!integer && !decimal) integer = 1;  // prevent 0,0

    m_int = integer;
    m_dec = decimal;
    update();
}
//=========================================================
void UGSegDisplay::setUnit(const QString& unit)
{
    m_unit = unit;
    update();
}
//=========================================================
void UGSegDisplay::drawBorder(bool state)
{
    m_border = state;
    update();
}
//=========================================================
void UGSegDisplay::setAutoMinus(bool state)
{
    m_autominus = state;
    update();
}
//=========================================================
