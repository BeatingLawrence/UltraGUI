#include "ultrabutton.h"

#include <QPainterPathStroker>
#include <QFontMetrics>
#include <QEvent>
#include <QPainter>
#include <math.h>

#include <QResizeEvent>

using namespace hci;

//=========================================================
UltraButton::UltraButton(QWidget* parent) :
    QPushButton(parent),
    m_ledState(false),
    m_buttonState(false),
    m_toggleMode(false),
    m_synchronous(true),
    m_hovering(false),
    m_leftJustify(false),
    m_iconLeftJustify(false),
    m_shrinkToFit(false),
    m_useHoveringAnimation(true),
    m_blinking(false),
    m_adaptIconsColor(true),
    m_drawBoundaryLine(false),
    m_geometryRequired(true),
    m_touchScreenMode(false),
    m_activeText(""),
    m_edgeRadius(0),
    m_leftPadding(5),
    m_iconLeftPadding(1),
    m_shrinkPadding(0),
    m_hoveringAnimation(0),
    m_animationToSum(0),
    m_position(UltraButton::BP_Mid),
    m_orientation(UltraButton::BO_Disabled),
    m_data(nullptr),
    m_defaultIcon(),
    m_activeIcon(),
    m_timer(this)
{
    _configureTiming(UltraButton::AS_Normal);
    setMouseTracking(!m_touchScreenMode);
    connect(this, SIGNAL(pressed()), this, SLOT(_mousePressed()));
    connect(this, SIGNAL(released()), this, SLOT(_mouseReleased()));
    connect(this, SIGNAL(clicked()), this, SLOT(_mouseClicked()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//=========================================================
UltraButton::~UltraButton()
{
    m_timer.stop();
}
//=========================================================
void UltraButton::paintEvent(QPaintEvent* event)
{
    (void) event;

    if(m_geometryRequired)
    {
        _computeGeometry();
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setFont(font());

    if(m_shrinkToFit && !text().isEmpty())
    {
        int widthOfText = painter.fontMetrics().horizontalAdvance(text());
        setFixedWidth(widthOfText + m_shrinkPadding);
    }

    //=====================================================COLOR
    QColor selectedColor;

    if(m_ledState)
    {
        selectedColor = palette().color(QPalette::Light);
    }
    else
    {
        selectedColor = _computeTransient(palette().color(QPalette::Button), palette().color(QPalette::Midlight), m_hoveringAnimation);
    }

    //=====================================================DRAW
    painter.setPen(QPen(QBrush(palette().color(QPalette::ButtonText)), 1, Qt::SolidLine, Qt::RoundCap, Qt::MiterJoin));
    painter.fillPath(m_completeFrame, QBrush(selectedColor));

    if(m_drawBoundaryLine)
    {
        _draw(painter, m_pathsToDraw);
    }

    //=====================================================ICON & TEXT

    if(m_defaultIcon.isNull())
    {
        if(m_ledState)
        {
            if(m_activeText.isEmpty() && !text().isEmpty())
            {
                _writeString(painter, text(), palette().color(QPalette::ButtonText));
            }
            else
            {
                _writeString(painter, m_activeText, palette().color(QPalette::ButtonText));
            }
        }
        else
        {
            if(text().isEmpty() && !m_activeText.isEmpty())
            {
                _writeString(painter, m_activeText, palette().color(QPalette::ButtonText));
            }
            else
            {
                _writeString(painter, text(), palette().color(QPalette::ButtonText));
            }
        }
    }
    else
    {
        int xPos = m_iconLeftJustify ? m_iconLeftPadding : (width() / 2) - (m_defaultIcon.width() / 2);
        int yPos = (height() / 2) - (m_defaultIcon.height() / 2);
        QRect target(xPos, yPos, m_defaultIcon.width(), m_defaultIcon.height());

        if(m_ledState)
        {
            painter.drawPixmap(target, m_activeIcon.isNull() ? m_defaultIcon : m_activeIcon, m_defaultIcon.rect());
        }
        else
        {
            painter.drawPixmap(target, m_defaultIcon, m_defaultIcon.rect());
        }
    }

    additionalPainting(painter);
}
//=========================================================
QColor UltraButton::_computeTransient(const QColor& first, const QColor& second, uint8_t selector)
{
    float mR = second.redF() - first.redF();
    float mG = second.greenF() - first.greenF();
    float mB = second.blueF() - first.blueF();
    float mA = second.alphaF() - first.alphaF();
    uint8_t R = ((float)selector * mR) + first.red();
    uint8_t G = ((float)selector * mG) + first.green();
    uint8_t B = ((float)selector * mB) + first.blue();
    uint8_t A = ((float)selector * mA) + first.alpha();
    return QColor(R, G, B, A);
}
//=========================================================
void UltraButton::enterEvent(QEvent* event)
{
    (void)event;

    if(!m_touchScreenMode)
    {
        m_hovering = true;
        m_blinking = false;

        if(m_useHoveringAnimation)
        {
            m_timer.start();
        }
        else
        {
            m_hoveringAnimation = 255;
        }
    }
}
//=========================================================
void UltraButton::leaveEvent(QEvent* event)
{
    (void)event;

    if(!m_touchScreenMode)
    {
        m_hovering = false;
        m_blinking = false;

        if(m_useHoveringAnimation)
        {
            m_timer.start();
        }
        else
        {
            m_hoveringAnimation = 0;
        }
    }
    else
    {
        m_hoveringAnimation = 0;
        update();
    }
}
//=========================================================
void UltraButton::additionalPainting(QPainter& painter)
{
    (void) painter;
    //noop
}
//=========================================================
void UltraButton::hideEvent(QHideEvent* event)
{
    QPushButton::hideEvent(event);
    m_hovering = false;
    m_blinking = false;
    m_hoveringAnimation = 0;
    m_timer.stop();
}
//=========================================================
void UltraButton::changeEvent(QEvent* event)
{
    if(event->type() == QEvent::ApplicationPaletteChange || event->type() == QEvent::PaletteChange)
    {
        if(!m_defaultIcon.isNull() && m_adaptIconsColor)
        {
            _adaptIconsColor();
        }
    }

    QPushButton::changeEvent(event);
}
//=========================================================
void UltraButton::resizeEvent(QResizeEvent* event)
{
    m_geometryRequired = true;
    update();
    QWidget::resizeEvent(event);
}
//=========================================================
void UltraButton::_mousePressed()
{
    if(!m_toggleMode)
    {
        m_buttonState = true;
        emit onEnable(m_data);
        emit onChange(true);

        if(m_synchronous)
        {
            activate(m_buttonState);
        }
    }
    else if(m_touchScreenMode)
    {
        m_hoveringAnimation = 255;
        update();
    }
}
//=========================================================
void UltraButton::_mouseReleased()
{
    if(!m_toggleMode)
    {
        m_buttonState = false;
        emit onDisable(m_data);
        emit onChange(false);

        if(m_synchronous)
        {
            activate(m_buttonState);
        }
    }
    else if(m_touchScreenMode)
    {
        m_hoveringAnimation = 0;
        update();
    }
}
//=========================================================
void UltraButton::_mouseClicked()
{
    if(m_toggleMode)
    {
        bool prev = m_buttonState;
        m_buttonState = !m_buttonState;

        if(prev && !m_buttonState)
        {
            emit onDisable(m_data);
            emit onChange(false);
        }
        else
        {
            emit onEnable(m_data);
            emit onChange(true);
        }

        if(m_synchronous)
        {
            activate(m_buttonState);
        }
    }
    else
    {
        emit onClick(m_data);
    }
}
//=========================================================
void UltraButton::_tick()
{
    if(m_timer.isActive())
    {
        if(m_hovering || m_blinking)
        {
            _sum(m_hoveringAnimation, m_animationToSum, 255);
        }
        else
        {
            _sum(m_hoveringAnimation, m_animationToSum, 255, true);
        }

        if(m_blinking && m_hoveringAnimation == 255)
        {
            m_blinking = false;
        }
        else if(m_hoveringAnimation == 0 || m_hoveringAnimation == 255)
        {
            m_timer.stop();
        }

        repaint();
    }
}
//=========================================================
void UltraButton::_writeString(QPainter& painter, const QString& string, const QColor& color)
{
    QPen pen = painter.pen();
    pen.setColor(color);
    painter.setPen(pen);

    if(m_leftJustify)
    {
        painter.drawText(rect().adjusted(m_leftPadding, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, string);
    }
    else
    {
        painter.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, string);
    }
}
//=========================================================
void UltraButton::_configureTiming(AnimationSpeed speed)
{
    switch(speed)
    {
        case UltraButton::AS_Slow:
            m_timer.setInterval(10);
            m_animationToSum = 5;
            break;

        case UltraButton::AS_Normal:
            m_timer.setInterval(5);
            m_animationToSum = 5;
            break;

        case UltraButton::AS_Fast:
            m_timer.setInterval(5);
            m_animationToSum = 10;
            break;

        case UltraButton::AS_Superfast:
            m_timer.setInterval(5);
            m_animationToSum = 12;
            break;
    }
}
//=========================================================
void UltraButton::_sum(uint8_t& value, uint8_t toSum, uint8_t max, bool subtract)
{
    if(subtract)
    {
        if(toSum > value)
        {
            value = 0;
        }
        else
        {
            value -= toSum;
        }
    }
    else
    {
        if((uint16_t)((uint16_t)value + (uint16_t)toSum) > (uint16_t)max)
        {
            value = max;
        }
        else
        {
            value += toSum;
        }
    }
}
//=========================================================
void UltraButton::_adaptIconsColor()
{
    QColor tempColor = palette().color(QPalette::ButtonText);
    QImage defaultIcon = m_defaultIcon.toImage();

    for(int y = 0; y < defaultIcon.height(); y++)
    {
        for(int x = 0; x < defaultIcon.width(); x++)
        {
            tempColor.setAlpha(defaultIcon.pixelColor(x, y).alpha());
            defaultIcon.setPixelColor(x, y, tempColor);
        }
    }

    m_defaultIcon = QPixmap::fromImage(defaultIcon);
    //======================

    if(m_activeIcon.isNull())
    {
        return;
    }

    QImage offIcon = m_activeIcon.toImage();

    for(int y = 0; y < offIcon.height(); y++)
    {
        for(int x = 0; x < offIcon.width(); x++)
        {
            tempColor.setAlpha(offIcon.pixelColor(x, y).alpha());
            offIcon.setPixelColor(x, y, tempColor);
        }
    }

    m_activeIcon = QPixmap::fromImage(offIcon);
}
//=========================================================
void UltraButton::_draw(QPainter& painter, const QVector<QPainterPath> paths)
{
    painter.setBrush(QBrush()); //do not fill

    for(auto& el : paths)
    {
        painter.setRenderHint(QPainter::Antialiasing, _isCurve(el));
        painter.drawPath(el);
        painter.setRenderHint(QPainter::Antialiasing, false);
    }
}
//=========================================================
void UltraButton::_computeGeometry()
{
    QPainterPath top;
    QPainterPath shortTop;
    QPainterPath topTinyLeft;
    QPainterPath topTinyRight;
    QPainterPath bottom;
    QPainterPath shortBottom;
    QPainterPath bottomTinyLeft;
    QPainterPath bottomTinyRight;
    QPainterPath right;
    QPainterPath shortRight;
    QPainterPath rightTinyTop;
    QPainterPath rightTinyBottom;
    QPainterPath left;
    QPainterPath shortLeft;
    QPainterPath leftTinyTop;
    QPainterPath leftTinyBottom;
    QPainterPath topLeftCorner;
    QPainterPath topRightCorner;
    QPainterPath bottomRightCorner;
    QPainterPath bottomLeftCorner;
    m_completeFrame.clear();
    m_pathsToDraw.clear();
    //
    top.moveTo(QPoint(0, 0));
    top.lineTo(QPoint(width() - 1, 0));
    bottom.moveTo(QPoint(width() - 1, height() - 1));
    bottom.lineTo(QPoint(0, height() - 1));
    right.moveTo(QPoint(width() - 1, 0));
    right.lineTo(QPoint(width() - 1, height() - 1));
    left.moveTo(QPoint(0, height() - 1));
    left.lineTo(QPoint(0, 0));
    //
    shortTop.moveTo(QPoint(m_edgeRadius, 0));
    shortTop.lineTo(QPoint(width() - 1 - m_edgeRadius, 0));
    shortRight.moveTo(width() - 1, m_edgeRadius);
    shortRight.lineTo(QPoint(width() - 1, height() - m_edgeRadius));
    shortBottom.moveTo(QPoint(width() - m_edgeRadius, height() - 1));
    shortBottom.lineTo(QPoint(m_edgeRadius, height() - 1));
    shortLeft.moveTo(QPoint(0, height() - 1 - m_edgeRadius));
    shortLeft.lineTo(QPoint(0, m_edgeRadius));
    //
    topRightCorner.moveTo(QPoint(width() - m_edgeRadius, 0));
    topRightCorner.arcTo(QRectF(width() - (m_edgeRadius * 2), 0, m_edgeRadius * 2, m_edgeRadius * 2), 90, -90);
    bottomRightCorner.moveTo(QPoint(width(), height() - m_edgeRadius));
    bottomRightCorner.arcTo(QRectF(width() - (m_edgeRadius * 2), height() - (m_edgeRadius * 2), m_edgeRadius * 2, m_edgeRadius * 2), 0, -90);
    bottomLeftCorner.moveTo(QPoint(m_edgeRadius, height()));
    bottomLeftCorner.arcTo(QRectF(0, height() - (m_edgeRadius * 2), m_edgeRadius * 2, m_edgeRadius * 2), -90, -90);
    topLeftCorner.moveTo(QPoint(0, m_edgeRadius));
    topLeftCorner.arcTo(QRectF(0, 0, m_edgeRadius * 2, m_edgeRadius * 2), -180, -90);
    //
    topTinyLeft.moveTo(QPoint(0, 0));
    topTinyLeft.lineTo(QPoint(m_edgeRadius, 0));
    topTinyRight.moveTo(QPoint(width() - m_edgeRadius, 0));
    topTinyRight.lineTo(QPoint(width(), 0));
    rightTinyTop.moveTo(QPoint(width() - 1, 0));
    rightTinyTop.lineTo(QPoint(width() - 1, m_edgeRadius));
    rightTinyBottom.moveTo(QPoint(width() - 1, height() - m_edgeRadius));
    rightTinyBottom.lineTo(QPoint(width() - 1, height()));
    bottomTinyRight.moveTo(QPoint(width() - 1, height() - 1));
    bottomTinyRight.lineTo(QPoint(width() - m_edgeRadius, height() - 1));
    bottomTinyLeft.moveTo(QPoint(m_edgeRadius, height() - 1));
    bottomTinyLeft.lineTo(QPoint(0, height() - 1));
    leftTinyBottom.moveTo(QPoint(0, height() - 1));
    leftTinyBottom.lineTo(QPoint(0, height() - m_edgeRadius));
    leftTinyTop.moveTo(QPoint(0, m_edgeRadius));
    leftTinyTop.lineTo(QPoint(0, 0));

    //
    if((m_edgeRadius == 0) || (m_position == UltraButton::BP_Mid && m_orientation != UltraButton::BO_Disabled))
    {
        //Choosing which parts are going to be used for square button
        if(m_orientation == UltraButton::BO_Disabled)
        {
            m_pathsToDraw.push_back(top);
            m_pathsToDraw.push_back(right);
            m_pathsToDraw.push_back(bottom);
            m_pathsToDraw.push_back(left);
            m_completeFrame.addRect(rect());
        }
        else if(m_orientation == UltraButton::BO_Horizontal)
        {
            switch(m_position)
            {
                case UltraButton::BP_First:
                {
                    m_pathsToDraw.push_back(top);
                    m_pathsToDraw.push_back(bottom);
                    m_pathsToDraw.push_back(left);
                    m_completeFrame.addRect(rect());
                }
                break;

                case UltraButton::BP_Mid:
                {
                    m_pathsToDraw.push_back(top);
                    m_pathsToDraw.push_back(bottom);
                    m_completeFrame.addRect(rect());
                }
                break;

                case UltraButton::BP_Last:
                {
                    m_pathsToDraw.push_back(top);
                    m_pathsToDraw.push_back(bottom);
                    m_pathsToDraw.push_back(right);
                    m_completeFrame.addRect(rect());
                }
                break;
            }
        }
        else if(m_orientation == UltraButton::BO_Vertical)
        {
            switch(m_position)
            {
                case UltraButton::BP_First:
                {
                    m_pathsToDraw.push_back(top);
                    m_pathsToDraw.push_back(right);
                    m_pathsToDraw.push_back(left);
                    m_completeFrame.addRect(rect());
                }
                break;

                case UltraButton::BP_Mid:
                {
                    m_pathsToDraw.push_back(right);
                    m_pathsToDraw.push_back(left);
                    m_completeFrame.addRect(rect());
                }
                break;

                case UltraButton::BP_Last:
                {
                    m_pathsToDraw.push_back(right);
                    m_pathsToDraw.push_back(bottom);
                    m_pathsToDraw.push_back(left);
                    m_completeFrame.addRect(rect());
                }
                break;
            }
        }
    }
    else
    {
        //Choosing which parts are going to be used for rounded button
        if(m_orientation == UltraButton::BO_Disabled)
        {
            m_pathsToDraw.push_back(topLeftCorner);
            m_pathsToDraw.push_back(shortTop);
            m_pathsToDraw.push_back(topRightCorner);
            m_pathsToDraw.push_back(shortRight);
            m_pathsToDraw.push_back(bottomRightCorner);
            m_pathsToDraw.push_back(shortBottom);
            m_pathsToDraw.push_back(bottomLeftCorner);
            m_pathsToDraw.push_back(shortLeft);
            m_completeFrame = topLeftCorner;
            m_completeFrame.connectPath(shortTop);
            m_completeFrame.connectPath(topRightCorner);
            m_completeFrame.connectPath(shortRight.translated(1, 0));
            m_completeFrame.connectPath(bottomRightCorner);
            m_completeFrame.connectPath(shortBottom.translated(0, 1));
            m_completeFrame.connectPath(bottomLeftCorner);
            m_completeFrame.connectPath(shortLeft);
        }
        else if(m_orientation == UltraButton::BO_Horizontal)
        {
            switch(m_position)
            {
                case UltraButton::BP_First:
                {
                    m_pathsToDraw.push_back(topLeftCorner);
                    m_pathsToDraw.push_back(shortTop);
                    m_pathsToDraw.push_back(topTinyRight);
                    m_pathsToDraw.push_back(bottomTinyRight);
                    m_pathsToDraw.push_back(shortBottom);
                    m_pathsToDraw.push_back(bottomLeftCorner);
                    m_pathsToDraw.push_back(shortLeft);
                    m_completeFrame = topLeftCorner;
                    m_completeFrame.connectPath(shortTop);
                    m_completeFrame.connectPath(topTinyRight);
                    m_completeFrame.connectPath(right.translated(1, 0));
                    m_completeFrame.connectPath(bottomTinyRight.translated(0, 1));
                    m_completeFrame.connectPath(shortBottom.translated(0, 1));
                    m_completeFrame.connectPath(bottomLeftCorner);
                    m_completeFrame.connectPath(shortLeft);
                }
                break;

                case UltraButton::BP_Last:
                {
                    m_pathsToDraw.push_back(topTinyLeft);
                    m_pathsToDraw.push_back(shortTop);
                    m_pathsToDraw.push_back(topRightCorner);
                    m_pathsToDraw.push_back(shortRight);
                    m_pathsToDraw.push_back(bottomRightCorner);
                    m_pathsToDraw.push_back(shortBottom);
                    m_pathsToDraw.push_back(bottomTinyLeft);
                    m_completeFrame = topTinyLeft;
                    m_completeFrame.connectPath(shortTop);
                    m_completeFrame.connectPath(topRightCorner);
                    m_completeFrame.connectPath(shortRight.translated(1, 0));
                    m_completeFrame.connectPath(bottomRightCorner);
                    m_completeFrame.connectPath(shortBottom.translated(0, 1));
                    m_completeFrame.connectPath(bottomTinyLeft.translated(0, 1));
                    m_completeFrame.connectPath(left.translated(-1, 0));
                }
                break;

                default:
                    break;  //BP::Mid drawed by square path above
            }
        }
        else if(m_orientation == UltraButton::BO_Vertical)
        {
            switch(m_position)
            {
                case UltraButton::BP_First:
                {
                    m_pathsToDraw.push_back(topLeftCorner);
                    m_pathsToDraw.push_back(shortTop);
                    m_pathsToDraw.push_back(topRightCorner);
                    m_pathsToDraw.push_back(shortRight);
                    m_pathsToDraw.push_back(rightTinyBottom);
                    m_pathsToDraw.push_back(leftTinyBottom);
                    m_pathsToDraw.push_back(shortLeft);
                    m_completeFrame = topLeftCorner;
                    m_completeFrame.connectPath(shortTop);
                    m_completeFrame.connectPath(topRightCorner);
                    m_completeFrame.connectPath(shortRight.translated(1, 0));
                    m_completeFrame.connectPath(rightTinyBottom.translated(1, 0));
                    m_completeFrame.connectPath(bottom.translated(0, 1));
                    m_completeFrame.connectPath(leftTinyBottom);
                    m_completeFrame.connectPath(shortLeft);
                }
                break;

                case UltraButton::BP_Last:
                {
                    m_pathsToDraw.push_back(rightTinyTop);
                    m_pathsToDraw.push_back(shortRight);
                    m_pathsToDraw.push_back(bottomRightCorner);
                    m_pathsToDraw.push_back(shortBottom);
                    m_pathsToDraw.push_back(bottomLeftCorner);
                    m_pathsToDraw.push_back(shortLeft);
                    m_pathsToDraw.push_back(leftTinyTop);
                    m_completeFrame = rightTinyTop.translated(1, 0);
                    m_completeFrame.connectPath(shortRight.translated(1, 0));
                    m_completeFrame.connectPath(bottomRightCorner);
                    m_completeFrame.connectPath(shortBottom.translated(0, 1));
                    m_completeFrame.connectPath(bottomLeftCorner);
                    m_completeFrame.connectPath(shortLeft);
                    m_completeFrame.connectPath(leftTinyTop);
                    m_completeFrame.connectPath(top.translated(0, -1));
                }
                break;

                default:
                    break;  //BP::Mid drawed by square path above
            }
        }
    }

    //
    m_geometryRequired = false;
}
//=========================================================
bool UltraButton::_isCurve(const QPainterPath& path)
{
    for(int i = 0; i < path.elementCount(); i++)
    {
        if(path.elementAt(i).isCurveTo())
        {
            return true;
        }
    }

    return false;
}
//=========================================================
void UltraButton::setIcons(const QPixmap& defaultIcon, const QPixmap& activeIcon, bool colorAdapting)
{
    if(defaultIcon.isNull())
    {
        return;
    }

    if(defaultIcon.width() > width() && defaultIcon.height() > height())
    {
        return;
    }

    m_defaultIcon = defaultIcon.copy();

    if(!activeIcon.isNull())
    {
        if(defaultIcon.size() == activeIcon.size())
        {
            m_activeIcon = activeIcon.copy();
        }
    }

    m_adaptIconsColor = colorAdapting;

    if(colorAdapting)
    {
        _adaptIconsColor();
    }

    update();
}
//=========================================================
void UltraButton::blink()
{
    if(!m_hovering)
    {
        m_blinking = true;
        m_timer.start();
    }
}
//=========================================================
void UltraButton::activate(bool active)
{
    m_ledState = active;
    update();
}
//=========================================================
void UltraButton::deactivate()
{
    activate(false);
}
//=========================================================
void UltraButton::animateHovering(bool animate, const AnimationSpeed& speed)
{
    m_useHoveringAnimation = animate;
    _configureTiming(speed);
}
//=========================================================
