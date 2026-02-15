#ifndef GUI_UGPLOTTER_H
#define GUI_UGPLOTTER_H

// QPalette roles used by UGPlotter:
// - Base: background (lightened when selected).
// - Text: axes, ticks, and labels.
// - Accent: plotted data curve.
	
#include <QWidget>

namespace gui
{
    class UGPlotter : public QWidget
    {
        Q_OBJECT

       private:
        QList<float> m_data;

        float m_verticalScale;
        float m_verticalScaleZoomStep;
        float m_horizontalRange;
        float m_horizontalZoomStep;
        float m_notch;
        float m_sample;

        uint32_t m_cursor;
        uint32_t m_cursorStep;
        uint32_t m_sampleCount;

        bool m_plotFromLeft;

        QString m_name;

        bool m_selected, m_paused, m_externalPaint;

        virtual void paintEvent(QPaintEvent* event) override;

        void _drawFloat(QPainter& painter, float number, const QPointF& pos, bool alignRight = false);

       public slots:
        // zoom in on X axis
        void zoomIn_X();

        // zoom out on X axis
        void zoomOut_X();

        // zoom in on Y axis
        void zoomIn_Y();

        // zoom out on Y axis
        void zoomOut_Y();

        // scroll to the left using scrollstep
        void scroll_L();

        // scroll to the right using the scrollstep
        void scroll_R();

        // re-align the horizontal scroll to the last sample
        void resetCursor();

        // stop scrolling when new samples are inserted
        void setPaused(bool paused = true);

        // clear the buffer
        void clear();

        // append a value to the buffer, displaying it on the graph
        void add(float value);

        // make the background color of the widget a bit more bright
        void setSelected(bool selected);

       public:
        explicit UGPlotter(QWidget* parent = nullptr);

        virtual ~UGPlotter();

        // max is the maximum amplitude value to be plotted,
        // step is the value used for zoom features
        void setVerticalScale(float max, float step);

        // range is the quantity represented by the graph length,
        // sampleduration is the size of one sample,
        // step is the value used for zoom features
        void setHorizontalScale(float range, float sampleDuration, float step);

        // duration is the size of one horizontal division
        void setNotchDuration(float duration);

        // this is the same param as the third of setHorizontalScale()
        void setHorizontalZoomStep(float step);

        // set a name for the graph
        void setName(const QString& name);

        QString name();

        // render to an external device/painter. Useful for export to a PDF
        void renderTo(QPaintDevice* device);
        void renderTo(QPainter* painter);

        // set the horizontal scroll step value
        void setScrollStep(float step);

        // set the horizontal scroll step expressed in number of samples
        void setScrollStep(uint32_t step);
    };
}  // namespace gui

#endif  // SIMHCI_HCI_PLOTTER_H
