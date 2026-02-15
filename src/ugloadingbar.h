#ifndef GUI_UGLOADINGBAR_H
#define GUI_UGLOADINGBAR_H

// QPalette roles used by UGLoadingBar:
// - Base: bar background.
// - Accent: progress fill.

#include <QObject>
#include <QTimer>
#include <QWidget>

#include "ultragui_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UGLoadingBar : public QWidget
    {
        Q_OBJECT

       private:
        static const int RefreshPeriodMs;

        QTimer m_timer;

        uint32_t m_setPoint;

        uint32_t m_progress;

        uint32_t m_pixels;

        bool m_rounded;

        bool m_drawBackGround;

       private slots:
        void _onTimerTick();

       protected:
        virtual void paintEvent(QPaintEvent* event) override;

       public:
        explicit UGLoadingBar(QWidget* parent = nullptr);

        virtual ~UGLoadingBar();

        // sets wether use rounded bar ends (===)
        inline void setRoundedBar(bool rounded = true) { m_rounded = rounded; }

        // sets wether draw the background or not
        inline void drawBackground(bool draw) { m_drawBackGround = draw; }

       public slots:
        // sets bar status (0~1000)
        void setProgressBar(int status);

        // reset the bar to 0 with no animation (instantly)
        void setProgressBarToZero();

       signals:
        // signal emitted when percentage (0~100) changes
        void onPercentageChange(uint8_t);
    };
}  // namespace gui

#endif  // GUI_UGLOADINGBAR_H
