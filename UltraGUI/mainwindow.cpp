#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "./components/ultrabutton.h"
#include "./components/ultragui.h"
#include <QRandomGenerator>

using namespace hci;

//=========================================================
void MainWindow::setDarkTheme(bool dt)
{
    QPalette temp = palette();

    if(dt)
    {
        temp.setColor(QPalette::Button, QColor(50, 50, 50, 255));
        temp.setColor(QPalette::ButtonText, "white");
        temp.setColor(QPalette::Disabled, QPalette::ButtonText, "gray");    //for the UltraSelector or UltraScroll
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
void MainWindow::simpleButtonClicked()
{
    ui->buttonLogger->push("Simple Button Clicked");
}
void MainWindow::verticalUB1Clicked()
{
    ui->buttonLogger->push("Vertical 1 Button Clicked");
}
void MainWindow::verticalUB2Clicked()
{
    ui->buttonLogger->push("Vertical 2 Button Clicked");
}
void MainWindow::verticalUB3Clicked()
{
    ui->buttonLogger->push("Vertical 3 Button Clicked");
}
void MainWindow::horizontalUB1Clicked()
{
    ui->buttonLogger->push("Horizontal 1 Button Clicked");
}
void MainWindow::horizontalUB2Clicked()
{
    ui->buttonLogger->push("Horizontal 2 Button Clicked");
}
void MainWindow::horizontalUB3Clicked()
{
    ui->buttonLogger->push("Horizontal 3 Button Clicked");
}
//=========================================================
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
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
    entry.data = ui->page_1;
    ui->selector->addEntry(entry);
    entry = UltraTypes::createUltraEntry("Page 2", 1);
    entry.data = ui->page_2;
    ui->selector->addEntry(entry);
    //
    ui->verticalStackUB_1->drawBorder();
    ui->verticalStackUB_2->drawBorder();
    ui->verticalStackUB_3->drawBorder();
    ui->verticalStackUB_1->useInLineContainment(UltraButton::BO_Vertical, UltraButton::BP_First);
    ui->verticalStackUB_2->useInLineContainment(UltraButton::BO_Vertical, UltraButton::BP_Mid);
    ui->verticalStackUB_3->useInLineContainment(UltraButton::BO_Vertical, UltraButton::BP_Last);
    ui->verticalStackUB_1->setEdgeRadius(10);
    ui->verticalStackUB_2->setEdgeRadius(10);
    ui->verticalStackUB_3->setEdgeRadius(10);
    ui->horizontalStackUB_1->drawBorder();
    ui->horizontalStackUB_2->drawBorder();
    ui->horizontalStackUB_3->drawBorder();
    ui->horizontalStackUB_1->useInLineContainment(UltraButton::BO_Horizontal, UltraButton::BP_First);
    ui->horizontalStackUB_2->useInLineContainment(UltraButton::BO_Horizontal, UltraButton::BP_Mid);
    ui->horizontalStackUB_3->useInLineContainment(UltraButton::BO_Horizontal, UltraButton::BP_Last);
    connect(ui->simplePushButton, SIGNAL(clicked()), this, SLOT(simpleButtonClicked()));
    connect(ui->verticalStackUB_1, SIGNAL(clicked()), this, SLOT(verticalUB1Clicked()));
    connect(ui->verticalStackUB_2, SIGNAL(clicked()), this, SLOT(verticalUB2Clicked()));
    connect(ui->verticalStackUB_3, SIGNAL(clicked()), this, SLOT(verticalUB3Clicked()));
    connect(ui->horizontalStackUB_1, SIGNAL(clicked()), this, SLOT(horizontalUB1Clicked()));
    connect(ui->horizontalStackUB_2, SIGNAL(clicked()), this, SLOT(horizontalUB2Clicked()));
    connect(ui->horizontalStackUB_3, SIGNAL(clicked()), this, SLOT(horizontalUB3Clicked()));
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
    entry = UltraTypes::createUltraEntry("Entry3", 2);
    entry.grayed = true;
    ui->scroll->addEntry(entry);
    ui->scroll->hideGrayedEntries();
    ui->scroll->setRounded(true);
}
//=========================================================
MainWindow::~MainWindow()
{
    delete ui;
}
//=========================================================
