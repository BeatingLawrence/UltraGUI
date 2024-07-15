#include "mainwindow.h"

#include <ultragui.h>

#include <QRandomGenerator>

#include "ui_mainwindow.h"

using namespace gui;

//=========================================================
void MainWindow::setDarkTheme(bool dt)
{
    QPalette temp = palette();

    if (dt)
    {
        temp.setColor(QPalette::Button, QColor(50, 50, 50, 255));
        temp.setColor(QPalette::ButtonText, "white");
        temp.setColor(QPalette::Disabled, QPalette::ButtonText,
                      "gray");  // for the UltraSelector or UltraScroll
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
void MainWindow::testToggleEnabled() { ui->buttonLogger->push("Test Toggle Enabled"); }
void MainWindow::testToggleDisabled() { ui->buttonLogger->push("Test Toggle Disabled"); }
void MainWindow::testToggleForceDisableClicked() { ui->testToggle->setState(false); }
void MainWindow::testToggleForceEnableClicked() { ui->testToggle->setState(true); }
//=========================================================
void MainWindow::buttonTestToggleCBChange(int x) { ui->testPB->setToggleMode(x != 0); }
//=========================================================
void MainWindow::buttonTestTouchCBChange(int x) { ui->testPB->useHovering(x != 0); }
//=========================================================
void MainWindow::timerTick()
{
    static int i = 15;
    ui->picker->addEntry(gui::UltraGui::createUltraEntry(QString("Entry%1").arg(i + 1), i));
    i++;
}
//=========================================================
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setDarkTheme(false);
    ui->testPB->setBorderRadius(5);
    ui->testPB->drawBorder();
    ui->stackedWidget->setCurrentWidget(ui->page_1);
    ui->darkThemeToggle->setToggleMode(true);
    QWidget::setWindowTitle(gui::UltraGui::version().text.c_str());
    //
    connect(ui->darkThemeToggle, SIGNAL(onChange(bool)), this, SLOT(setDarkTheme(bool)));
    connect(ui->selector, SIGNAL(onClickForStacked(QWidget*)), ui->stackedWidget,
            SLOT(setCurrentWidget(QWidget*)));
    //
    UltraEntry entry = gui::UltraGui::createUltraEntry("Page 1", 0);
    entry.data       = ui->page_1;
    ui->selector->addEntry(entry);
    entry      = gui::UltraGui::createUltraEntry("Page 2", 1);
    entry.data = ui->page_2;
    ui->selector->addEntry(entry);
    //
    ui->testToggle->setToggleMode(true);
    //
    connect(ui->testPB, SIGNAL(onClick()), this, SLOT(testButtonClicked()));
    connect(ui->testPB, SIGNAL(onEnable()), this, SLOT(testButtonEnabled()));
    connect(ui->testPB, SIGNAL(onDisable()), this, SLOT(testButtonDisabled()));
    //
    connect(ui->testToggle, SIGNAL(onEnable()), this, SLOT(testToggleEnabled()));
    connect(ui->testToggle, SIGNAL(onDisable()), this, SLOT(testToggleDisabled()));
    connect(ui->testToggleEnable, SIGNAL(clicked()), this, SLOT(testToggleForceEnableClicked()));
    connect(ui->testToggleDisable, SIGNAL(clicked()), this, SLOT(testToggleForceDisableClicked()));
    //
    connect(ui->buttonTestToggleCB, SIGNAL(stateChanged(int)), this, SLOT(buttonTestToggleCBChange(int)));
    connect(ui->buttonTestTouchCB, SIGNAL(stateChanged(int)), this, SLOT(buttonTestTouchCBChange(int)));
    //
    ui->slider->setTouchScreenMode(false);
    connect(ui->slider, SIGNAL(onChange(uint32_t)), this, SLOT(_sliderChange(uint32_t)));
    //
    ui->loadingBar->setRoundedBar();
    //
    entry = gui::UltraGui::createUltraEntry("Entry1", 0);
    ui->scroll->addEntry(entry);
    entry = gui::UltraGui::createUltraEntry("Entry2", 1);
    ui->scroll->addEntry(entry);
    entry        = gui::UltraGui::createUltraEntry("Entry3", 2);
    entry.grayed = true;
    ui->scroll->addEntry(entry);
    ui->scroll->hideGrayedEntries();
    ui->scroll->setRounded(true);
    //
    //
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry1", 0));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry2", 1));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry3", 2));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry4", 3));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry5", 4));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry6", 5));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry7", 6));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry8", 7));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry9", 8));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry10", 9));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry11", 10));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry12", 11));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry13", 12));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry14", 13));
    ui->picker->addEntry(gui::UltraGui::createUltraEntry("Entry15", 14));

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerTick()));

    m_timer.setInterval(1000);
    m_timer.setSingleShot(false);
    m_timer.start();
}
//=========================================================
MainWindow::~MainWindow() { delete ui; }
//=========================================================
