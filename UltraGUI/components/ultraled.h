#ifndef HCI_ULTRALED_H
#define HCI_ULTRALED_H

#include <QWidget>
#include "./UltraGUI_global.h"

namespace hci
{
    class ULTRAGUI_EXPORT UltraLed : public QWidget
    {
            Q_OBJECT

        private:
            bool m_active;

            uint8_t m_ledBorderThickness;

        protected:

            virtual void paintEvent(QPaintEvent* event);

        public:
            explicit UltraLed(QWidget* parent = nullptr);

            void ledBorderThickness(uint8_t ledBorderThickness);

        public slots:
            void activate(bool active = true);

            void deactivate();
    };
}


#endif
