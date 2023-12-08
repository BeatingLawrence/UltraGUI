#include "ultrabarsgraph.h"
#include <QPainter>

using namespace hci;

//=========================================================
void UltraBarsGraph::paintEvent(QPaintEvent* event)
{
    (void)event;
    QPainter painter(this);
}
//=========================================================
UltraBarsGraph::UltraBarsGraph(QWidget* parent) : QWidget(parent)
{
}
//=========================================================
void UltraBarsGraph::setTopValue(int32_t)
{
}
//=========================================================
void UltraBarsGraph::setBottomValue(int32_t)
{
}
//=========================================================
void UltraBarsGraph::setValueNumber(size_t)
{
}
//=========================================================
void UltraBarsGraph::pushValue(int32_t)
{
}
//=========================================================
