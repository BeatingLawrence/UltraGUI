#ifndef GUI_ULTRALINESGRAPH_H
#define GUI_ULTRALINESGRAPH_H

#include <QWidget>

#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UltraLinesGraph : public QWidget
    {
        Q_OBJECT
       private:
        QList<int32_t> m_graph;

        uint32_t m_distanceBetweenPoints;

        bool m_positiveOnly;

       protected:
        virtual void paintEvent(QPaintEvent* event) override;

       public:
        explicit UltraLinesGraph(QWidget* parent = nullptr);

        virtual ~UltraLinesGraph();

       signals:
    };
}  // namespace gui

#endif  // GUI_ULTRALINESGRAPH_H
