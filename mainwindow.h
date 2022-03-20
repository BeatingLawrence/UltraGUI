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

        void simpleButtonClicked();
        void verticalUB1Clicked();
        void verticalUB2Clicked();
        void verticalUB3Clicked();
        void horizontalUB1Clicked();
        void horizontalUB2Clicked();
        void horizontalUB3Clicked();

    public:
        MainWindow(QWidget* parent = nullptr);

        ~MainWindow();

};
#endif // MAINWINDOW_H
