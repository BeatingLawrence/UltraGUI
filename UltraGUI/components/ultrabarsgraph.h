#ifndef HCI_ULTRABARSGRAPH_H
#define HCI_ULTRABARSGRAPH_H

#include <QWidget>
#include "./UltraGUI_global.h"

namespace hci
{

    class ULTRAGUI_EXPORT UltraBarsGraph : public QWidget
    {
            Q_OBJECT
        public:
            explicit UltraBarsGraph(QWidget* parent = nullptr);

        signals:

    };

} // namespace hci

#endif // HCI_ULTRABARSGRAPH_H
