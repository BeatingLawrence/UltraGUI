#ifndef HCI_ULTRASLIDER_H
#define HCI_ULTRASLIDER_H

#include <QWidget>
#include <QTimer>
#include "./UltraGUI_global.h"

/*  This class constructs and draws a slider object
 *
 *  The slider is made up of a background and a spot which is the selector.
 *
 *  The user can move the selector dragging it with
 *  the mouse or pressing the wheel button to bring it at the zero position.
 *  Moving the mouse wheel while cursor is hovering on the slider will make
 *  it increment or decrement depending on the rotation of the wheel.
 *  Clicking the slider in any point will make the cursor reaching that point.
 *
 *  Setting the touch screen mode to true will prevent the cursor from animating.
 *
 *  QPalette colors used by this component:
 *
 *      -QPalette::Base
 *          Used for the cursor
 *
 *      -QPalette::Light
 *          Used for filling the active area of the cursor
 *
 *      -QPalette::Dark
 *          Used for filling the inactive area of the cursor
 *
 *      -QPalette::Window
 *          Used for the background
 *
 */

namespace hci
{
    class ULTRAGUI_EXPORT UltraSlider : public QWidget
    {
            Q_OBJECT
        private:
            int32_t m_slider;
            int32_t m_zero;
            int32_t m_max;
            uint32_t m_pixelPos;
            uint32_t m_pixelZeroPos;
            uint32_t m_cursorDimension;

            bool m_hovering;
            bool m_touchScreenMode;

            QTimer m_timer;

            uint32_t _sliderToPixel(uint16_t slider);

            void _updateCursorDimensionTick();

        private slots:
            void _tick();

        protected:
            virtual void paintEvent(QPaintEvent* event) override;

            virtual void mouseMoveEvent(QMouseEvent* event) override;

            virtual void mousePressEvent(QMouseEvent* event) override;

            virtual void wheelEvent(QWheelEvent* event) override;

            virtual void leaveEvent(QEvent* event) override;

        public:
            explicit UltraSlider(QWidget* parent = nullptr);

            //sets the zero position of the slider
            inline void setZero(uint32_t zero)
            {
                m_zero = zero;
                update();
            }

            //sets the current slider position
            inline void setSlider(uint32_t slider)
            {
                m_slider = slider;
                repaint();
            }

            //sets the maximum value
            inline void setMax(uint32_t max)
            {
                m_max = max;
                update();
            }

            //zeroes the cursor
            inline void zero()
            {
                m_slider = m_zero;

                if(!m_timer.isActive())
                {
                    m_timer.start();
                }
            }

            //sets the touchscreen mode status
            inline void setTouchScreenMode(bool state)
            {
                m_touchScreenMode = state;
                _updateCursorDimensionTick();
            }

        signals:
            void onChange(uint32_t value);

    };
}

#endif // HCI_ULTRASLIDER_H
