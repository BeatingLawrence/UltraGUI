#ifndef GUI_UGSLIDER_H
#define GUI_UGSLIDER_H

// QPalette roles used by UGSlider:
// - Base: main track.
// - Accent: highlighted range.
// - WindowText: indicator line.
// - AlternateBase: slider knob.

#include <QTimer>
#include <QWidget>

#include "ultragui_global.h"

/*  This class draws a slider object
 *
 *  The slider is made up of a background and a spot which is the selector.
 *
 *  The user can move the selector dragging it with
 *  the mouse or pressing the wheel button to bring it at the zero position.
 *  Moving the mouse wheel while cursor is hovering on the slider will make
 *  it increment or decrement depending on the rotation of the wheel.
 *  Clicking the slider in any point will make the cursor reach that point.
 *
 */

namespace gui
{
    class ULTRAGUI_EXPORT UGSlider : public QWidget
    {
        Q_OBJECT
       private:
        float m_slider, m_zero, m_max, m_min;

        float _sliderToPixel(float slider);
        float _pixelToSlider(float pixels);
        float _cap(float val);
        float _all_margins();
        float _one_margin();

        void _incSlider(float x);
        void _decSlider(float x);

       protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void mouseMoveEvent(QMouseEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void wheelEvent(QWheelEvent* event) override;

       public:
        explicit UGSlider(QWidget* parent = nullptr);

        virtual ~UGSlider();

        inline void setRange(int32_t min, int32_t max, int32_t zero = 0)
        {
            m_min    = min;
            m_max    = max;
            m_zero   = zero;
            m_slider = zero;
            update();
        }

        // zeroes the cursor
        inline void zero()
        {
            m_slider = m_zero;
            emit onChange(m_slider);
        }

       public slots:
        void set(float value);

       signals:
        void onChange(float value);
    };
}  // namespace gui

#endif  // GUI_UGSLIDER_H
