#include "mainwindow.h"

#include <ultragui.h>

#include <QRandomGenerator>

#include "ui_mainwindow.h"

using namespace gui;

/*  Used QPalette roles:
 *
 *  -Button
 *  -ButtonText
 *  -WindowText
 *  -Base: used for background of elements (except buttons)
 *  -Text: used for text when base is used
 *  -AlternateBase: as alternative to Base in certain cases (the knob and background of a toggle)
 *  -Accent: used as accent color for all the components
 */

//=========================================================
void MainWindow::setDarkTheme(bool dt)
{
    QPalette temp = palette();

    if (dt)
    {
        // UltraGUI:
        temp.setColor(QPalette::Button, QColor(0x1e1e1e));
        temp.setColor(QPalette::ButtonText, QColor(0xffffff));
        temp.setColor(QPalette::Disabled, QPalette::ButtonText, "gray");
        temp.setColor(QPalette::WindowText, QColor(0xffffff));
        temp.setColor(QPalette::Base, QColor(0x3e3e3e));
        temp.setColor(QPalette::Text, QColor(0xffffff));
        temp.setColor(QPalette::AlternateBase, QColor(0x5e5e5e));

        // Other:
        temp.setColor(QPalette::Window, QColor(0x212121));
    }
    else
    {
        // UltraGUI:
        temp.setColor(QPalette::Button, QColor(0xb3b3b3));
        temp.setColor(QPalette::ButtonText, QColor(0x000000u));
        temp.setColor(QPalette::Disabled, QPalette::ButtonText, "gray");
        temp.setColor(QPalette::WindowText, QColor(0x000000u));
        temp.setColor(QPalette::Base, QColor(0xa3a3a3));
        temp.setColor(QPalette::Text, QColor(0x000000u));
        temp.setColor(QPalette::AlternateBase, QColor(0x424242));  // darker than base

        // Other:
        temp.setColor(QPalette::Window, QColor(0xd1d1d1));
    }

    setPalette(temp);
}
//=========================================================
void MainWindow::_sliderChange(float value) { ui->sliderLabel->setText(QString("%1").arg(value)); }
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
void MainWindow::timerTick2()
{
    static int angle = 0;
    ui->plotter->add(qSin(qDegreesToRadians((float)angle)));

    angle += 10;
    if (angle >= 360) angle = 0;
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

    entry      = gui::UltraGui::createUltraEntry("Page 3", 2);
    entry.data = ui->page_3;
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
    connect(ui->slider, SIGNAL(onChange(float)), this, SLOT(_sliderChange(float)));
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

    ui->plotter->setHorizontalScale(5.0f, 0.01f, 0.5f);
    ui->plotter->setVerticalScale(1.0f, 0.5f);
    ui->plotter->setNotchDuration(1.0f);
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
    connect(&m_timer2, SIGNAL(timeout()), this, SLOT(timerTick2()));

    m_timer.setInterval(1000);
    m_timer.setSingleShot(false);
    m_timer.start();

    m_timer2.setInterval(100);
    m_timer2.setSingleShot(false);
    m_timer2.start();
}
//=========================================================
MainWindow::~MainWindow() { delete ui; }
//=========================================================
