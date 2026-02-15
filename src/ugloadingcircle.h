#ifndef GUI_UGLOADINGCIRCLE_H
#define GUI_UGLOADINGCIRCLE_H

// QPalette roles used by UGLoadingCircle:
// - WindowText: rotating stroke/arc color.

#include <QPixmap>
#include <QTimer>
#include <QWidget>

#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UGLoadingCircle : public QWidget
    {
        Q_OBJECT

       private:
        QTimer m_timer;

        bool m_loading;
        uint8_t m_progress;

        int16_t m_animation;  // 0~359

        uint8_t m_toSum;

        bool m_ccw;

       private slots:
        void _tick();

       protected:
        virtual void paintEvent(QPaintEvent* event) override;

       public:
        explicit UGLoadingCircle(QWidget* parent = nullptr);

        // sets the loading state of the widget
        void setLoading(bool state = true);

        // sets the visible arc length in percentage (0 = point, 100 = full circle)
        void setProgress(uint8_t progress);

        // compatibility no-op
        void setPixmap(const QPixmap& pixmap);

        // sets the direction
        inline void setCCW(bool ccw = true) { m_ccw = ccw; }

        // sets the rotation speed (deg/s). Speed is not 100% accurate
        void setSpeed(uint32_t speed = 360);

        // compatibility no-op
        inline void setIconColorAdapting(bool state = true)
        {
            (void)state;
        }

       signals:
    };
}  // namespace gui

#endif  // GUI_ULTRALOADINGCIRCLE_H
