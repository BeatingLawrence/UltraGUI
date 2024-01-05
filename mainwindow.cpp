#include "mainwindow.h"

#include <QRandomGenerator>

#include "./components/ultragui.h"
#include "ui_mainwindow.h"

using namespace hci;

//=========================================================
void MainWindow::setDarkTheme(bool dt)
{
    QPalette temp = palette();

    if (dt)
    {
        temp.setColor(QPalette::Button, QColor(50, 50, 50, 255));
        temp.setColor(QPalette::ButtonText, "white");
        temp.setColor(QPalette::Disabled, QPalette::ButtonText, "gray");  // for the UltraSelector or UltraScroll
        temp.setColor(QPalette::Window, "black");
        temp.setColor(QPalette::WindowText, "white");
        temp.setColor(QPalette::Light, QColor(29, 255, 13));
        temp.setColor(QPalette::Midlight, QColor(9, 63, 5));
        temp.setColor(QPalette::Dark, QColor(66, 66, 66));
    }
    else
    {
        temp.setColor(QPalette::Button, QColor("white").darker(110));
        temp.setColor(QPalette::ButtonText, "black");
        temp.setColor(QPalette::Disabled, QPalette::ButtonText, "gray");
        temp.setColor(QPalette::Window, QColor(0xdedede));
        temp.setColor(QPalette::WindowText, "black");
        temp.setColor(QPalette::Light, QColor(13, 85, 255));
        temp.setColor(QPalette::Midlight, QColor(170, 196, 255));
        temp.setColor(QPalette::Dark, QColor(190, 190, 190));
    }

    setPalette(temp);
}
//=========================================================
void MainWindow::_sliderChange(uint32_t value)
{
    ui->sliderLabel->setText(QString("%1").arg(value));
    ui->loadingBar->setProgressBar(value * 10);
}
//=========================================================
void MainWindow::testButtonClicked() { ui->buttonLogger->push("Test Button Clicked"); }
void MainWindow::testButtonEnabled() { ui->buttonLogger->push("Test Button Enabled"); }
void MainWindow::testButtonDisabled() { ui->buttonLogger->push("Test Button Disabled"); }
//=========================================================
void MainWindow::buttonTestToggleCBChange(int x) { ui->testPB->setToggleMode(x != 0); }
//=========================================================
void MainWindow::buttonTestTouchCBChange(int x) { ui->testPB->setTouchScreenMode(x != 0); }
//=========================================================
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setDarkTheme(false);
    ui->stackedWidget->setCurrentWidget(ui->page_1);
    QWidget::setWindowTitle(hci::UltraGui::version().completeText);
    //
    connect(ui->darkThemeToggle, SIGNAL(onChange(bool)), this, SLOT(setDarkTheme(bool)));
    connect(ui->selector, SIGNAL(onClickForStacked(QWidget*)), ui->stackedWidget, SLOT(setCurrentWidget(QWidget*)));
    //
    UltraTypes::UltraEntry entry = UltraTypes::createUltraEntry("Page 1", 0);
    entry.data                   = ui->page_1;
    ui->selector->addEntry(entry);
    entry      = UltraTypes::createUltraEntry("Page 2", 1);
    entry.data = ui->page_2;
    ui->selector->addEntry(entry);
    //

    connect(ui->testPB, SIGNAL(onClick()), this, SLOT(testButtonClicked()));
    connect(ui->testPB, SIGNAL(onEnable()), this, SLOT(testButtonEnabled()));
    connect(ui->testPB, SIGNAL(onDisable()), this, SLOT(testButtonDisabled()));

    connect(ui->buttonTestToggleCB, SIGNAL(stateChanged(int)), this, SLOT(buttonTestToggleCBChange(int)));
    connect(ui->buttonTestTouchCB, SIGNAL(stateChanged(int)), this, SLOT(buttonTestTouchCBChange(int)));

    //
    ui->slider->setTouchScreenMode(false);
    connect(ui->slider, SIGNAL(onChange(uint32_t)), this, SLOT(_sliderChange(uint32_t)));
    //
    ui->loadingBar->setRoundedBar();
    //
    entry = UltraTypes::createUltraEntry("Entry1", 0);
    ui->scroll->addEntry(entry);
    entry = UltraTypes::createUltraEntry("Entry2", 1);
    ui->scroll->addEntry(entry);
    entry        = UltraTypes::createUltraEntry("Entry3", 2);
    entry.grayed = true;
    ui->scroll->addEntry(entry);
    ui->scroll->hideGrayedEntries();
    ui->scroll->setRounded(true);
}
//=========================================================
MainWindow::~MainWindow() { delete ui; }
//=========================================================
