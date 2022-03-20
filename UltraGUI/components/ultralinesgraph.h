#ifndef HCI_ULTRALINESGRAPH_H
#define HCI_ULTRALINESGRAPH_H

#include <QWidget>
#include "./UltraGUI_global.h"

namespace hci
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
} // namespace hci

#endif // HCI_ULTRALINESGRAPH_H
