#ifndef HCI_ULTRALOADINGBAR_H
#define HCI_ULTRALOADINGBAR_H

#include <QObject>
#include <QWidget>
#include <QTimer>
#include "./UltraGUI_global.h"

namespace hci
{
    class ULTRAGUI_EXPORT UltraLoadingBar : public QWidget
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
            explicit UltraLoadingBar(QWidget* parent = nullptr);

            virtual ~UltraLoadingBar();

            //sets wether use rounded bar ends (===)
            inline void setRoundedBar(bool rounded = true)
            {
                m_rounded = rounded;
            }

            //sets wether draw the background or not
            inline void drawBackground(bool draw)
            {
                m_drawBackGround = draw;
            }

        public slots:
            //sets bar status (0~1000)
            void setProgressBar(int status);

            //reset the bar to 0 with no animation (instantly)
            void setProgressBarToZero();

        signals:
            //signal emitted when percentage (0~100) changes
            void onPercentageChange(uint8_t);

    };
}

#endif // HCI_STARTUPWIDGET_H
