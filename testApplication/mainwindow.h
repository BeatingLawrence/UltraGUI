#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

   private slots:
    void setDarkTheme(bool dt);

    void _sliderChange(uint32_t value);

    void testButtonClicked();
    void testButtonEnabled();
    void testButtonDisabled();

    void buttonTestToggleCBChange(int);
    void buttonTestTouchCBChange(int);

   public:
    MainWindow(QWidget* parent = nullptr);

    ~MainWindow();
};
#endif  // MAINWINDOW_H
