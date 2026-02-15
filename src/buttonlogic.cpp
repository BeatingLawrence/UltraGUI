#include "buttonlogic.h"

#include <QMouseEvent>

using namespace gui;

//=========================================================
void ButtonLogic::_fadeIn()
{
    m_hovering = true;
    fadeIn();
}
//=========================================================
void ButtonLogic::_fadeOut()
{
    m_hovering = false;
    fadeOut();
}
//=========================================================
void ButtonLogic::_resetStateMachine()
{
    m_state  = Inactive;
    m_active = false;
    _fadeOut();

    if (!m_conf.automode && !m_conf.toggle) m_conf.toggle = true;

    update();
}
//=========================================================
void ButtonLogic::_notifyChange(bool state)
{
    if (state)
        emit onEnable();
    else
        emit onDisable();

    emit onChange(state);
}
//=========================================================
void ButtonLogic::_notifyAspectChange(bool state)
{
    m_active = state;
    stateChange(state);
}
//=========================================================
void ButtonLogic::_onEnter()
{
    switch (m_state)
    {
        case Inactive:
            if (m_conf.hovering)
            {
                m_state = Hovering;
                _fadeIn();
            }
            break;
        case Hovering:
            break;
        case Pressed:
            break;
        case Active:
            break;
        case ActivePressed:
            break;
    }
}
//=========================================================
void ButtonLogic::_onLeave()
{
    switch (m_state)
    {
        case Inactive:
            break;
        case Hovering:
            m_state = Inactive;
            _fadeOut();
            break;
        case Pressed:
            m_state = Inactive;
            _fadeOut();
            if (m_conf.hovering) _notifyAspectChange(false);
            break;
        case Active:
            break;
        case ActivePressed:
            _fadeOut();
            if (m_conf.toggle)
            {
                m_state = Active;
                _notifyAspectChange(true);
            }
            else
            {
                m_state = Inactive;
                _notifyAll(false);
            }
            break;
    }
}
//=========================================================
void ButtonLogic::_onPress()
{
    switch (m_state)
    {
        case Inactive:
            if (m_conf.toggle)
            {
                m_state = Pressed;
                _fadeIn();
            }
            else
            {
                m_state = ActivePressed;
                _notifyAll(true);
            }
            break;
        case Hovering:
            if (m_conf.toggle)
            {
                m_state = Pressed;
                if (m_conf.automode) _notifyAspectChange(true);
            }
            else
            {
                m_state = ActivePressed;
                _notifyAll(true);
            }
            break;
        case Pressed:
            break;
        case Active:
            m_state = ActivePressed;
            if (!m_conf.hovering && m_conf.automode) _fadeIn();
            break;
        case ActivePressed:
            break;
    }
}
//=========================================================
void ButtonLogic::_onRelease()
{
    switch (m_state)
    {
        case Inactive:
            break;
        case Hovering:
            break;
        case Pressed:
            emit onClick();
            click();
            if (m_conf.automode)
            {
                m_state = Active;

                if (m_conf.hovering)
                    _notifyChange(true);
                else
                {
                    _fadeOut();
                    _notifyAll(true);
                }
            }
            else if (!m_conf.hovering)
            {
                _notifyChange(true);
                m_state = Inactive;
                _fadeOut();
            }
            else
            {
                _notifyChange(true);
                m_state = Hovering;
            }
            break;
        case Active:
            break;
        case ActivePressed:
            emit onClick();
            click();
            if (m_conf.automode)
            {
                if (m_conf.hovering)
                {
                    m_state = Hovering;
                }
                else
                {
                    m_state = Inactive;
                    _fadeOut();
                }
                _notifyAspectChange(false);
            }
            else
                m_state = Active;

            _notifyChange(false);

            break;
    }
}
//=========================================================
void ButtonLogic::_printCurrentState()
{
    switch (m_state)
    {
        case Inactive:
            qDebug("inactive");
            break;
        case Hovering:
            qDebug("hovering");
            break;
        case Pressed:
            qDebug("pressed");
            break;
        case Active:
            qDebug("active");
            break;
        case ActivePressed:
            qDebug("active pressed");
            break;
    }
}
//=========================================================
void ButtonLogic::stateChange(bool newState) {}
//=========================================================
void ButtonLogic::fadeIn() {}
//=========================================================
void ButtonLogic::fadeOut() {}
//=========================================================
void ButtonLogic::click() {}
//=========================================================
void ButtonLogic::configureHovering(bool enable)
{
    m_conf.hovering = enable;
    setMouseTracking(enable);
    // force state machine reset
    _resetStateMachine();
}
//=========================================================
void ButtonLogic::configureToggle(bool enable)
{
    m_conf.toggle = enable;
    // force state machine reset
    _resetStateMachine();
}
//=========================================================
void ButtonLogic::configureAutoMode(bool enable)
{
    m_conf.automode = enable;
    _resetStateMachine();
}
//=========================================================
void ButtonLogic::transition(ButtonMachineEvent event)
{
    //_printCurrentState();
    switch (event)
    {
        case Press:
            _onPress();
            break;
        case Release:
            _onRelease();
            break;
        case Enter:
            _onEnter();
            break;
        case Leave:
            _onLeave();
            break;
    }

    //_printCurrentState();

    // qDebug("-----");

    update();
}
//=========================================================
void ButtonLogic::mousePressEvent(QMouseEvent* event)
{
    transition(Press);
    QPushButton::mousePressEvent(event);
}
//=========================================================
void ButtonLogic::mouseReleaseEvent(QMouseEvent* event)
{
    transition(Release);
    QPushButton::mouseReleaseEvent(event);
}
//=========================================================
void ButtonLogic::leaveEvent(QEvent* event)
{
    transition(Leave);
    QPushButton::leaveEvent(event);
}
//=========================================================
void ButtonLogic::enterEvent(QEnterEvent* event)
{
    transition(Enter);
    QPushButton::enterEvent(event);
}
//=========================================================
void ButtonLogic::mouseMoveEvent(QMouseEvent* event)
{
    QPushButton::mouseMoveEvent(event);

    bool hit = false;

    if (underMouse()) hit = hitButton(event->position().toPoint());

    if (!hit && (m_state == ActivePressed || m_state == Pressed)) transition(Leave);
}
//=========================================================
void ButtonLogic::hideEvent(QHideEvent* event)
{
    transition(Leave);
    QPushButton::hideEvent(event);
}
//=========================================================
bool ButtonLogic::hitButton(const QPoint& pos) const { return rect().contains(pos); }
//=========================================================
ButtonLogic::ButtonLogic(QWidget* parent)
    : QPushButton(parent),
      m_state(Inactive),
      m_conf(),
      m_active(false),
      m_hovering(false)
{
    setMouseTracking(m_conf.hovering);
}
//=========================================================
bool ButtonLogic::isActive() const { return m_active; }
//=========================================================
bool ButtonLogic::isHovering() const { return m_hovering; }
//=========================================================
void ButtonLogic::forceState(bool state)
{
    if (!m_conf.toggle) return;
    if (state == m_active) return;

    if (state)
        m_state = Active;
    else
        m_state = m_hovering && m_conf.hovering ? Hovering : Inactive;

    _notifyAspectChange(state);

    update();
}
//=========================================================
