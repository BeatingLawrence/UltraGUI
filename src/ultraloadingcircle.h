#ifndef GUI_ULTRALOADINGCIRCLE_H
#define GUI_ULTRALOADINGCIRCLE_H

#include <QPixmap>
#include <QTimer>
#include <QWidget>

#include "UltraGUI_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UltraLoadingCircle : public QWidget
    {
        Q_OBJECT

       private:
        QPixmap m_pixmap;

        QTimer m_timer;

        bool m_loading;

        int16_t m_animation;  // 0~359

        uint8_t m_toSum;

        bool m_ccw;

        bool m_adaptColors;

        void _adaptColors();

       private slots:
        void _tick();

       protected:
        virtual void paintEvent(QPaintEvent* event) override;

        virtual void changeEvent(QEvent* event) override;

       public:
        explicit UltraLoadingCircle(QWidget* parent = nullptr);

        // sets the loading state of the widget
        void setLoading(bool state = true);

        // sets the rotating pixmap, must be a square pixmap
        void setPixmap(const QPixmap& pixmap);

        // sets the direction
        inline void setCCW(bool ccw = true) { m_ccw = ccw; }

        // sets the rotation speed (deg/s). Speed is not 100% accurate
        void setSpeed(uint32_t speed = 360);

        // use adaptive colors for the icon. Disable before setting a colored icon
        inline void setIconColorAdapting(bool state = true) { m_adaptColors = state; }

       signals:
    };
}  // namespace gui

#endif  // GUI_ULTRALOADINGCIRCLE_H
