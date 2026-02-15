#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColor>
#include <QTimer>
#include <QVector>

#include "types.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

   private:
    Ui::MainWindow* ui;

    QTimer m_timer, m_timer2;
    int m_loadingBarProgress;
    QVector<float> m_oscFrame;
    uint32_t m_oscZeroIndex;
    int m_oscDirection;

   private slots:
    void setDarkTheme(bool dt);
    void pickAccentColor();

    void _sliderChange(float value);

    void testButtonClicked();
    void testButtonEnabled();
    void testButtonDisabled();
    //
    void testToggleEnabled();
    void testToggleDisabled();
    void testToggleForceDisableClicked();
    void testToggleForceEnableClicked();

    void buttonTestToggleCBChange(int);
    void buttonTestTouchCBChange(int);
    void buttonTestAutomodeCBChange(int);

    void buttonArrayClick(const gui::UltraEntry*);
    void buttonArrayChange(const gui::UltraEntry*);

    void timerTick();
    void timerTick2();

   private:
    void applyAccentColor(const QColor& color);
    void refreshAccentColorButton();

   public:
    MainWindow(QWidget* parent = nullptr);

    ~MainWindow();
};
#endif  // MAINWINDOW_H
