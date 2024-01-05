#ifndef GUI_ULTRALOGS_H
#define GUI_ULTRALOGS_H

#include <QTimer>
#include <QWidget>

#include "UltraGUI_global.h"

namespace gui
{
    class ULTRAGUI_EXPORT UltraLogs : public QWidget
    {
        Q_OBJECT

       private:
        QList<QPair<QString, QColor>> m_entries;

        QList<QPair<QString, QColor>> m_queue;

        unsigned int m_animation;  // 0~20

        QTimer m_timer;

        bool m_border;

        bool m_aura;

        uint32_t m_numberOfLines;

        static unsigned int Indentation;

        uint8_t _alpha(unsigned int index, unsigned int lastIndex);

       private slots:
        void _tick();

       protected:
        virtual void mouseDoubleClickEvent(QMouseEvent* event) override;

        virtual void paintEvent(QPaintEvent* event) override;

       public:
        explicit UltraLogs(QWidget* parent = nullptr);

        ~UltraLogs();

        inline void useBorder(bool useBorder = true) { m_border = useBorder; }

       public slots:
        void push(const QString& message, const QColor& textColor = QColor());

        void clear();

       signals:
        void onDoubleClick();
    };
}  // namespace gui
#endif  // GUI_LOGSWIDGET_H
