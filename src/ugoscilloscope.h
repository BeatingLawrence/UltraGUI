#ifndef GUI_OSCILLOSCOPE_H
#define GUI_OSCILLOSCOPE_H

// QPalette roles used by UGOscilloscope:
// - Base: background (lightened when selected).
// - Text: grid and labels.
// - Accent: signal trace.
	
#include <QWidget>
#include <QVector>
#include <cstdint>

#include "ultragui_global.h"

namespace gui
{
    class FloatCircBuf
    {
        QVector<float> m_buf;
        qsizetype m_cursor, m_size;

       public:
        FloatCircBuf();

        void alloc(qsizetype size);

        void add(float val);

        qsizetype size() { return m_size; };

        void clear();

        float& operator[](qsizetype index);
    };

    class ULTRAGUI_EXPORT UGOscilloscope : public QWidget
    {
        Q_OBJECT

       private:
        FloatCircBuf m_data;
        QVector<float> m_frame;
        uint32_t m_zeroIndex;
        uint32_t m_samplesInWindow;

        float m_sample, m_hdivisionVal, m_vdivisionVal;

        uint16_t m_hdivisions, m_vdivisions;

        QString m_name;

        bool m_selected, m_externalPaint;

        void _computeBufSize();

        virtual void paintEvent(QPaintEvent* event) override;

        void _drawFloat(QPainter& painter, float number, const QPointF& pos, bool alignRight = false);

       public slots:
        // clear the buffer
        void clear();

        // append a value to the buffer, displaying it on the graph
        void add(float value);

        // make the background color of the widget a bit more bright
        void setSelected(bool selected);

       public:
        explicit UGOscilloscope(QWidget* parent = nullptr);

        virtual ~UGOscilloscope();

        void addBlock(const float values[], int size);

        // setup the vertical measuring system
        void setVerticalScale(float divsize, uint16_t divno);

        // setup the horizontal measuring system
        void setHorizontalScale(float divsize, uint16_t divno, float samplesize);

        // replace current graph with a full frame and set the sample index that maps to time = 0
        void setFrame(const QVector<float>& frame, uint32_t zeroIndex);

        // set only the sample index that maps to time = 0 (for frame scrolling)
        void setZeroIndex(uint32_t zeroIndex);

        // set vertical full-scale max value (top label = +max, bottom label = -max)
        void setMaxValue(float maxValue);

        // set total visible time window (left = -t/2, center = 0, right = +t/2)
        void setTimeWindow(float t);

        // set visible time window, number of shown samples, and center sample index (time = 0)
        void setSampleWindow(float t, uint32_t samplesInWindow, uint32_t centerIndex);

        // set a name for the graph
        void setName(const QString& name);

        QString name();

        // render to an external device/painter. Useful for export to a PDF
        void renderTo(QPaintDevice* device);
        void renderTo(QPainter* painter);
    };
}  // namespace gui

#endif  // GUI_OSCILLOSCOPE_H
