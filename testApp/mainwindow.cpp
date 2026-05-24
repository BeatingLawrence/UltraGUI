#include "mainwindow.h"

#include <ultragui.h>

#include <QColorDialog>
#include <QApplication>
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
    const QColor accentColor = temp.accent().color();

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

    temp.setColor(QPalette::Accent, accentColor);
    QApplication::setPalette(temp);
    setPalette(temp);
    refreshAccentColorButton();
}
//=========================================================
void MainWindow::pickAccentColor()
{
    const QColor color = QColorDialog::getColor(palette().accent().color(), this, "Select Accent Color");
    if (!color.isValid()) return;
    applyAccentColor(color);
}
//=========================================================
void MainWindow::applyAccentColor(const QColor& color)
{
    if (!color.isValid()) return;

    QPalette temp = palette();
    temp.setColor(QPalette::Accent, color);
    QApplication::setPalette(temp);
    setPalette(temp);
    refreshAccentColorButton();
}
//=========================================================
void MainWindow::refreshAccentColorButton()
{
    const QColor accent = palette().accent().color();
    const QColor border = palette().buttonText().color();

    ui->accentColorButton->setStyleSheet(
        QString("QPushButton { background-color: %1; border: 1px solid %2; border-radius: 4px; }")
            .arg(accent.name(QColor::HexRgb))
            .arg(border.name(QColor::HexRgb)));
}
//=========================================================
void MainWindow::_sliderChange(float value)
{
    ui->sliderLabel->setText(QString::number(value, 'f', 2));
    ui->segdisplay->setValue(value);
}
//=========================================================
void MainWindow::testButtonClicked() { ui->buttonLogger->push("Test Button Clicked"); }
void MainWindow::testButtonEnabled() { ui->buttonLogger->push("Test Button Enabled"); }
void MainWindow::testButtonDisabled() { ui->buttonLogger->push("Test Button Disabled"); }
void MainWindow::testToggleEnabled() { ui->buttonLogger->push("Test Toggle Enabled"); }
void MainWindow::testToggleDisabled() { ui->buttonLogger->push("Test Toggle Disabled"); }
//=========================================================
void MainWindow::testToggleForceDisableClicked()
{
    ui->testToggle->setState(false);
    if (!ui->buttonTestAutomodeCB->isChecked()) ui->testPB->setState(false);
}
//=========================================================
void MainWindow::testToggleForceEnableClicked()
{
    ui->testToggle->setState(true);
    if (!ui->buttonTestAutomodeCB->isChecked()) ui->testPB->setState(true);
}
//=========================================================
void MainWindow::buttonTestToggleCBChange(int x)
{
    ui->testPB->setToggleMode(x != 0);

    if (!x && !ui->buttonTestAutomodeCB->isChecked()) ui->buttonTestToggleCB->setCheckState(Qt::Checked);
}
//=========================================================
void MainWindow::buttonTestTouchCBChange(int x) { ui->testPB->useHovering(x != 0); }
//=========================================================
void MainWindow::buttonTestAutomodeCBChange(int x)
{
    ui->testPB->setAutoMode(x != 0);
    if (!x)
    {
        ui->buttonTestToggleCB->setCheckState(Qt::Checked);
    }
}
//=========================================================
void MainWindow::buttonArrayClick(const gui::UltraEntry* e)
{
    ui->buttonLogger->push(QString("Button array click %1").arg(e->id));
}
//=========================================================
void MainWindow::buttonArrayChange(const gui::UltraEntry* e)
{
    ui->buttonLogger->push(QString("Button array change %1").arg(e->id));
}
//=========================================================
void MainWindow::timerTick()
{
    static int i = 15;
    ui->picker->addEntry({QString("Entry%1").arg(i + 1), i});
    i++;
}
//=========================================================
void MainWindow::timerTick2()
{
    static int angle = 0;

    auto x = qSin(qDegreesToRadians((float)angle));
    ui->plotter->add(x);

    if (!m_oscFrame.isEmpty())
    {
        const uint32_t maxIndex = static_cast<uint32_t>(m_oscFrame.size() - 1);
        if (m_oscDirection > 0)
        {
            if (m_oscZeroIndex >= maxIndex)
                m_oscDirection = -1;
            else
                m_oscZeroIndex++;
        }
        else
        {
            if (m_oscZeroIndex == 0)
                m_oscDirection = 1;
            else
                m_oscZeroIndex--;
        }

        ui->oscilloscope->setZeroIndex(m_oscZeroIndex);
    }

    m_loadingBarProgress += 5;
    if (m_loadingBarProgress > 1000)
    {
        m_loadingBarProgress = 0;
        ui->loadingBar->setProgressBarToZero();
    }
    else
    {
        ui->loadingBar->setProgressBar(m_loadingBarProgress);
        if (m_loadingBarProgress == 1000) ui->loadingFrame->blink(static_cast<uint8_t>(10));
    }
    const uint8_t circleProgress = static_cast<uint8_t>(m_loadingBarProgress / 10);
    ui->loadingCircle->setProgress(circleProgress);
    ui->loadingCircle2->setProgress(circleProgress);
    ui->loadingCircle3->setProgress(circleProgress);
    ui->loadingCircle4->setProgress(circleProgress);

    angle += 10;
    if (angle >= 360) angle = 0;
}
//=========================================================
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_loadingBarProgress(0),
      m_oscFrame(),
      m_oscZeroIndex(0),
      m_oscDirection(1)
{
    ui->setupUi(this);
    setDarkTheme(false);
    ui->frame->setTitle("UGFrame", Qt::AlignRight);
    ui->testPB->setBorderRadius(5);
    ui->testPB->drawBorder();
    ui->stackedWidget->setCurrentWidget(ui->page_1);
    ui->darkThemeToggle->setToggleMode(true);
    QWidget::setWindowTitle(gui::UltraGui::version().text.c_str());
    //
    connect(ui->darkThemeToggle, SIGNAL(onChange(bool)), this, SLOT(setDarkTheme(bool)));
    connect(ui->accentColorButton, SIGNAL(clicked()), this, SLOT(pickAccentColor()));
    connect(ui->selector, SIGNAL(onClickForStacked(QWidget*)), ui->stackedWidget,
            SLOT(setCurrentWidget(QWidget*)));
    //
    UltraEntry entry = {"Page 1"};
    entry.data       = ui->page_1;
    ui->selector->addEntry(entry, true);

    entry      = {"Page 2"};
    entry.data = ui->page_2;
    ui->selector->addEntry(entry);

    entry      = {"Page 3"};
    entry.data = ui->page_3;
    ui->selector->addEntry(entry);

    entry      = {"Page 4"};
    entry.data = ui->page_4;
    ui->selector->addEntry(entry);
    entry      = {"Page 5"};
    entry.data = ui->page_5;
    ui->selector->addEntry(entry);

    //
    ui->testToggle->setToggleMode(true);
    //
    ui->segdisplay->setUnit("Volt");
    //
    ui->verticalButtonArray->setArrayDir(gui::AD_Vertical);
    ui->verticalButtonArray->addButton({"Button 1", 2});
    ui->verticalButtonArray->addButton({"Button 2", 3});
    ui->verticalButtonArray->addButton({"Button 3", 4});
    //
    ui->horizontalButtonArray->setArrayDir(gui::AD_Horizontal);
    ui->horizontalButtonArray->addButton({"Button 1", 5});
    ui->horizontalButtonArray->addButton({"Button 2", 6});
    ui->horizontalButtonArray->addButton({"Button 3", 7});

    ui->verticalButtonArray_2->setArrayDir(gui::AD_Vertical);
    ui->verticalButtonArray_2->addButton({"Button 1"});
    ui->verticalButtonArray_2->addButton({"Button 2"});
    ui->verticalButtonArray_2->addButton({"Button 3"});
    //
    connect(ui->testPB, SIGNAL(onClick()), this, SLOT(testButtonClicked()));
    connect(ui->testPB, SIGNAL(onEnable()), this, SLOT(testButtonEnabled()));
    connect(ui->testPB, SIGNAL(onDisable()), this, SLOT(testButtonDisabled()));
    connect(ui->testPB, SIGNAL(onChange(bool)), ui->statusLed, SLOT(activate(bool)));
    //
    connect(ui->testToggle, SIGNAL(onEnable()), this, SLOT(testToggleEnabled()));
    connect(ui->testToggle, SIGNAL(onDisable()), this, SLOT(testToggleDisabled()));
    connect(ui->testToggleEnable, SIGNAL(clicked()), this, SLOT(testToggleForceEnableClicked()));
    connect(ui->testToggleDisable, SIGNAL(clicked()), this, SLOT(testToggleForceDisableClicked()));
    //
    connect(ui->buttonTestToggleCB, SIGNAL(stateChanged(int)), this, SLOT(buttonTestToggleCBChange(int)));
    connect(ui->buttonTestTouchCB, SIGNAL(stateChanged(int)), this, SLOT(buttonTestTouchCBChange(int)));
    connect(ui->buttonTestAutomodeCB, SIGNAL(stateChanged(int)), this, SLOT(buttonTestAutomodeCBChange(int)));
    //
    connect(ui->slider, SIGNAL(onChange(float)), this, SLOT(_sliderChange(float)));
    //
    connect(ui->horizontalButtonArray, SIGNAL(onClick(const gui::UltraEntry*)), this,
            SLOT(buttonArrayClick(const gui::UltraEntry*)));

    connect(ui->verticalButtonArray, SIGNAL(onClick(const gui::UltraEntry*)), this,
            SLOT(buttonArrayClick(const gui::UltraEntry*)));

    connect(ui->verticalButtonArray, SIGNAL(onChange(const gui::UltraEntry*)), this,
            SLOT(buttonArrayChange(const gui::UltraEntry*)));
    //
    ui->loadingBar->setRoundedBar();
    ui->loadingBar->setProgressBarToZero();
    ui->statusLed->activate(false);

    ui->loadingCircle->setProgress(0);
    ui->loadingCircle->setSpeed(240);
    ui->loadingCircle->setLoading(true);
    ui->loadingCircle2->setProgress(0);
    ui->loadingCircle2->setSpeed(240);
    ui->loadingCircle2->setLoading(true);
    ui->loadingCircle3->setProgress(0);
    ui->loadingCircle3->setSpeed(240);
    ui->loadingCircle3->setLoading(true);
    ui->loadingCircle4->setProgress(0);
    ui->loadingCircle4->setSpeed(240);
    ui->loadingCircle4->setLoading(true);

    ui->loadingDots->setLoading(true);
    ui->loadingDots2->setLoading(true);
    ui->loadingDots3->setLoading(true);
    ui->loadingDots4->setLoading(true);

    ui->loadingSquares->setLoading(true);
    ui->loadingSquares2->setLoading(true);
    ui->loadingSquares3->setLoading(true);
    ui->loadingSquares4->setLoading(true);
    //
    entry = {"Entry 1"};
    ui->scroll->addEntry(entry);
    entry = {"Entry 2"};
    ui->scroll->addEntry(entry);
    entry        = {"Entry 3"};
    entry.grayed = true;
    ui->scroll->addEntry(entry);
    ui->scroll->hideGrayedEntries();
    ui->scroll->setRounded(true);
    //

    ui->plotter->setHorizontalScale(5.0f, 0.01f, 0.5f);
    ui->plotter->setVerticalScale(1.0f, 0.5f);
    ui->plotter->setNotchDuration(1.0f);
    ui->oscilloscope->setMaxValue(1.0f);

    constexpr int periodSamples = 120;
    constexpr int periods       = 12;
    QVector<float> onePeriod;
    onePeriod.reserve(periodSamples);

    for (int i = 0; i < periodSamples; i++)
    {
        const float phaseDeg = (360.0f * static_cast<float>(i)) / static_cast<float>(periodSamples);
        onePeriod.push_back(qSin(qDegreesToRadians(phaseDeg)));
    }

    m_oscFrame.reserve(periodSamples * periods);
    for (int p = 0; p < periods; p++)
        for (float sample : onePeriod)
            m_oscFrame.push_back(sample);

    m_oscZeroIndex = static_cast<uint32_t>(m_oscFrame.size() / 2);
    ui->oscilloscope->setFrame(m_oscFrame, m_oscZeroIndex);
    ui->oscilloscope->setSampleWindow(10.0f, 241, m_oscZeroIndex);
    //
    //
    ui->picker->addEntry({"Entry 1"});
    ui->picker->addEntry({"Entry 2"});
    ui->picker->addEntry({"Entry 3"});
    ui->picker->addEntry({"Entry 4"});

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
