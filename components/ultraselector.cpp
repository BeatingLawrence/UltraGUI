#include "ultraselector.h"
#include <QMouseEvent>
#include <QPainter>
#include <math.h>

using namespace hci;

//=========================================================
QRectF UltraSelector::_animationToRect()
{
    uint32_t selectorWidth = 4;
    uint32_t selectorHeight = m_sectionHeight;
    uint32_t maxValue = m_entries.size() * 100;
    uint32_t y = (uint32_t)(((float)m_animation / (float)maxValue) * (float)(height()));
    QRect toReturn(0, y, selectorWidth, selectorHeight);
    return toReturn;
}
//=========================================================
uint32_t UltraSelector::_selectorToAnimation()
{
    return (m_selector * 100);
}
//=========================================================
void UltraSelector::_computeAnimationBoundaries()
{
    m_maxValue = m_entries.size() * 100;

    if(m_animation > m_maxValue)
    {
        m_animation = m_maxValue;
    }
}
//=========================================================
void UltraSelector::_tick()
{
    uint32_t targetAnimation = _selectorToAnimation();
    float toSum = (float)labs((int)targetAnimation - (int)m_animation) / 5.0f;

    if(targetAnimation > m_animation)
    {
        m_animation += ceil(toSum);
    }
    else if(targetAnimation < m_animation)
    {
        m_animation -= ceil(toSum);
    }
    else if(m_timer.isActive())
    {
        m_timer.stop();
    }

    repaint();
}
//=========================================================
void UltraSelector::paintEvent(QPaintEvent* event)
{
    (void)event;

    if(m_entries.empty())
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //=================================MATH
    m_sectionHeight = uint32_t((float)height() / (float)m_entries.size());
    QRect hoveringRect(0, 0, width(), m_sectionHeight);
    //=================================DRAW
    painter.setFont(font());

    for(int i = 0; i < m_entries.size(); i++)
    {
        QPen painterPen(QBrush(), 1);

        if(!m_currentMouseCursor.isNull() && hoveringRect.contains(m_currentMouseCursor, true))
        {
            QColor color = palette().color(QPalette::ButtonText);
            color.setAlpha(30);
            painter.setBrush(QBrush(color));
            painter.setPen(painterPen);
            painter.drawRoundedRect(hoveringRect, 5, 5);
            painter.setBrush(QBrush());
        }

        painterPen.setColor(palette().color(m_entries[i].grayed ? QPalette::Disabled : QPalette::Normal, QPalette::ButtonText));
        painter.setPen(painterPen);
        painter.drawText(hoveringRect.adjusted(10, 0, 0, 0), Qt::AlignVCenter | Qt::AlignLeft, m_entries[i].entryText);
        hoveringRect.translate(0, m_sectionHeight);
    }

    painter.setPen(QPen(QBrush(), 1));
    painter.setBrush(QBrush(palette().color(QPalette::Light)));
    painter.drawRoundedRect(_animationToRect(), 2, 2);
    painter.end();
}
//=========================================================
void UltraSelector::mouseMoveEvent(QMouseEvent* event)
{
    if(event->type() == QEvent::Type::MouseMove)
    {
        QPoint point = event->pos();

        if(point.x() > 0 && point.y() > 0)
        {
            m_currentMouseCursor = event->pos();
            repaint();
        }
    }

    QWidget::mouseMoveEvent(event);
}
//=========================================================
void UltraSelector::mousePressEvent(QMouseEvent* event)
{
    m_clicked = true;
    QWidget::mousePressEvent(event);
}
//=========================================================
void UltraSelector::mouseReleaseEvent(QMouseEvent* event)
{
    if(m_clicked)
    {
        m_clicked = false;
        QRect trackingArea(0, 0, width(), m_sectionHeight);

        for(auto& el : m_entries)
        {
            QPoint point = event->pos();

            if(trackingArea.contains(point, true))
            {
                if((m_selector != (uint32_t)el.entryId) && !el.grayed)
                {
                    m_selector = el.entryId;
                    m_timer.start();
                    emit onClick(el.entryId, el.data);

                    if(el.data != nullptr)
                    {
                        emit onClickForStacked((QWidget*)el.data);
                    }
                }

                break;
            }

            trackingArea.translate(0, (int)m_sectionHeight);
        }
    }

    QWidget::mouseReleaseEvent(event);
}
//=========================================================
void UltraSelector::leaveEvent(QEvent* event)
{
    m_currentMouseCursor = QPoint();
    m_clicked = false;
    repaint();
    QWidget::leaveEvent(event);
}
//=========================================================
UltraSelector::UltraSelector(QWidget* parent) :
    QWidget(parent),
    m_currentMouseCursor(),
    m_selector(0),
    m_animation(0),
    m_timer(this),
    m_clicked(false),
    m_sectionHeight(0),
    m_maxValue(0)
{
    m_timer.setInterval(10);
    setMouseTracking(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(_tick()));
}
//=========================================================
UltraSelector::~UltraSelector()
{
    m_timer.stop();
}
//=========================================================
void UltraSelector::addEntry(const UltraTypes::UltraEntry& entry)
{
    m_entries.push_back(entry);
    _computeAnimationBoundaries();
    update();
}
//=========================================================
bool UltraSelector::setEntries(const QVector<UltraTypes::UltraEntry>& entries)
{
    if(entries.size() == 0)
    {
        return false;
    }

    for(auto& el : entries)
    {
        if(el.entryId == UltraTypes::UltraEntryInvalidID)
        {
            return false;
        }
    }

    clearEntries();
    m_entries = entries;
    m_selector = 0;
    _computeAnimationBoundaries();
    update();
    return true;
}
//=========================================================
void UltraSelector::clearEntries()
{
    m_entries.clear();
    m_selector = 0;
    _computeAnimationBoundaries();
    update();
}
//=========================================================
bool UltraSelector::setGrayed(int32_t entryID, bool grayed)
{
    if(entryID == UltraTypes::UltraEntryInvalidID)
    {
        return false;
    }

    for(auto& el : m_entries)
    {
        if(el.entryId == entryID)
        {
            el.grayed = grayed;
            update();
            return true;
        }
    }

    return false;
}
//=========================================================
void UltraSelector::setAllGrayed(bool grayed)
{
    for(auto& el : m_entries)
    {
        el.grayed = grayed;
    }

    update();
}
//=========================================================
