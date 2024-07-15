#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

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

    QTimer m_timer;

   private slots:
    void setDarkTheme(bool dt);

    void _sliderChange(uint32_t value);

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

    void timerTick();

   public:
    MainWindow(QWidget* parent = nullptr);

    ~MainWindow();
};
#endif  // MAINWINDOW_H
