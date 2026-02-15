#include "ugbuttonarray.h"

#include <QPainter>

using namespace gui;

//=========================================================
void UGButtonArray::clear()
{
    for (auto&& el : m_buttons)
    {
        disconnect(el, SIGNAL(onClickEntry(const UltraEntry*)), this, SLOT(onPress(const UltraEntry*)));
        m_layout.removeWidget(el);
        delete el;
    }

    m_buttons.clear();
    m_selector = -1;
}
//=========================================================
void UGButtonArray::paintEvent(QPaintEvent* event)
{
    // QPainter painter(this);
    // painter.fillRect(rect(), QColor(255, 0, 0, 20)); //for debug
    QWidget::paintEvent(event);
}
//=========================================================
UGButtonArray::UGButtonArray(QWidget* parent)
    : QWidget{parent},
      m_layout(QBoxLayout::Direction::LeftToRight, this),
      m_selector(-1)
{
    setLayout(&m_layout);
    m_layout.setContentsMargins(0, 0, 0, 0);
    // m_layout.setSpacing(1);
}
//=========================================================
UGButtonArray::~UGButtonArray() { clear(); }
//=========================================================
void UGButtonArray::setArrayDir(ArrayDirection dir)
{
    if (dir == AD_Horizontal)
        m_layout.setDirection(QBoxLayout::Direction::LeftToRight);
    else
        m_layout.setDirection(QBoxLayout::Direction::TopToBottom);
}
//=========================================================
void UGButtonArray::addButton(const UltraEntry& entry, bool active)
{
    auto ncentry = entry;

    auto b = new UGButton(this);

    ncentry.index = m_buttons.size();
    b->setEntry(ncentry);
    if (active) m_selector = ncentry.index;

    b->drawBorder();
    b->setBorderRadius(5);

    m_buttons.push_back(b);
    m_layout.addWidget(b);

    connect(b, SIGNAL(onClickEntry(const UltraEntry*)), this, SLOT(onPress(const UltraEntry*)));
}
//=========================================================
void UGButtonArray::setVisible(int32_t id, bool visible)
{
    for (auto&& el : m_buttons)
        if (el->entry().id == id)
        {
            el->setVisible(visible);
            return;
        }
}
//=========================================================
const UltraEntry* UGButtonArray::current()
{
    if (m_selector == -1) return nullptr;
    return &m_buttons[m_selector]->entry();
}
//=========================================================
void UGButtonArray::onPress(const UltraEntry* p)
{
    emit onClick(p);

    for (int32_t i = 0; i < m_buttons.size(); i++)
        if (p->index == i)
        {
            if (i != m_selector)
            {
                emit onChange(&m_buttons[i]->entry());
                m_selector = i;
            }

            return;
        }
}
//=========================================================
