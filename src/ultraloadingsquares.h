#ifndef GUI_ULTRALOADINGSQUARES_H
#define GUI_ULTRALOADINGSQUARES_H

#include <QTimer>
#include <QWidget>

#include "UltraGUI_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UltraLoadingSquares : public QWidget
    {
        Q_OBJECT

       public:
        enum AnimationSpeed : uint8_t
        {
            AS_Slow,
            AS_Normal,
            AS_Fast,
            AS_Superfast
        };

       private:
        bool m_loading;

        unsigned int m_animation;

        QTimer m_timer;

        AnimationSpeed m_speed;

        QSize m_tinyRectSize;

        unsigned int m_bottomLeftCorner;

        unsigned int m_bottomRightCorner;

        unsigned int m_topRightCorner;

        unsigned int m_topLeftCorner;  // end of animation

        void _increment(unsigned int& toIncrement, unsigned int end);

       private slots:
        void _onTimerTick();

       protected:
        virtual void paintEvent(QPaintEvent* event) override;

       public:
        UltraLoadingSquares(QWidget* parent = nullptr);

        void setLoading(bool state = true);

        inline void setSpeed(AnimationSpeed speed) { m_speed = speed; }

        void setSquaresSize(uint8_t size);
    };
}  // namespace gui

#endif  // GUI_LOADINGWIDGET_H
