#ifndef HCI_ULTRALOADINGDOTS_H
#define HCI_ULTRALOADINGDOTS_H

#include <QWidget>
#include <QTimer>
#include "./UltraGUI_global.h"

namespace hci
{
    class ULTRAGUI_EXPORT UltraLoadingDots : public QWidget
    {
            Q_OBJECT

        private:
            QTimer m_timer;

            QRectF m_dot;

            QVector<QPair<QPointF, bool>> m_points; //point, hidden

            bool m_loading;

            //converts the input to a series of points
            void _computeGeometry(uint8_t n, uint16_t r);

        private slots:
            void _tick();

        protected:
            virtual void paintEvent(QPaintEvent* event) override;

        public:
            explicit UltraLoadingDots(QWidget* parent = nullptr);

            enum BlinkingSpeed
            {
                BS_Slow,
                BS_Normal,
                BS_Fast,
                BS_Superfast
            };

            //sets the size of the dots (diameter), the number of them, and the
            //distance between the center of every dot and the center of the widget
            void setDots(uint8_t size, uint8_t numberOfDots, uint16_t radiusFromCenter);

            //sets the loading state of the widget
            void setLoading(bool state = true);

            //sets the blinking speed
            void setSpeed(BlinkingSpeed speed);

        signals:

    };
}

#endif // HCI_ULTRALOADINGDOTS_H
