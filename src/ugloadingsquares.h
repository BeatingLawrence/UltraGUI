#ifndef GUI_UGLOADINGSQUARES_H
#define GUI_UGLOADINGSQUARES_H

// QPalette roles used by UGLoadingSquares:
// - WindowText: fill color for animated squares.

#include <QTimer>
#include <QWidget>

#include "ultragui_global.h"

class QResizeEvent;

namespace gui
{
    class ULTRAGUI_EXPORT UGLoadingSquares : public QWidget
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
        void _updateGeometry();

       private slots:
        void _onTimerTick();

       protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void resizeEvent(QResizeEvent* event) override;

       public:
        UGLoadingSquares(QWidget* parent = nullptr);

        void setLoading(bool state = true);

        inline void setSpeed(AnimationSpeed speed) { m_speed = speed; }
    };
}  // namespace gui

#endif  // GUI_UGLOADINGSQUARES_H
