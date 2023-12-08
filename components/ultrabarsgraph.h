#ifndef HCI_ULTRABARSGRAPH_H
#define HCI_ULTRABARSGRAPH_H

#include <QWidget>
#include "./UltraGUI_global.h"

namespace hci
{
    class ULTRAGUI_EXPORT UltraBarsGraph : public QWidget
    {
            Q_OBJECT
        private:
            QList<int32_t> m_graph;

            virtual void paintEvent(QPaintEvent* event) override;

        public:
            explicit UltraBarsGraph(QWidget* parent = nullptr);

            void setTopValue(int32_t);

            void setBottomValue(int32_t);

            void setValueNumber(size_t);

        public slots:
            void pushValue(int32_t);

    };
}

#endif // HCI_ULTRABARSGRAPH_H
