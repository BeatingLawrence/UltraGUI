#include "buttonbehavior.h"

#include <QMouseEvent>

using namespace gui;

//=========================================================
void ButtonBehavior::_fadeIn()
{
    m_hovering = true;
    fadeIn();
}
//=========================================================
void ButtonBehavior::_fadeOut()
{
    m_hovering = false;
    fadeOut();
}
//=========================================================
void ButtonBehavior::stateChange(bool newState) {}
//=========================================================
void ButtonBehavior::fadeIn() {}
//=========================================================
void ButtonBehavior::fadeOut() {}
//=========================================================
void ButtonBehavior::click() {}
//=========================================================
void ButtonBehavior::configureHovering(bool enable)
{
    m_conf.hovering = enable;
    setMouseTracking(enable);
    // force state machine reset
    m_state  = Inactive;
    m_active = false;
    update();
}
//=========================================================
void ButtonBehavior::configureToggle(bool enable)
{
    m_conf.toggle = enable;
    // force state machine reset
    m_state       = Inactive;
    m_active      = false;
    update();
}
//=========================================================
void ButtonBehavior::transition(ButtonMachineEvent event)
{
    ButtonMachineState newState = m_state;

    switch (m_state)
    {
        case Inactive:
            if (event == Enter && m_conf.hovering)
            {
                newState = Hovering;
                _fadeIn();
            }
            else if (event == Press && !m_conf.hovering)
            {
                if (m_conf.toggle)
                {
                    newState = Pressed;
                    _fadeIn();
                }
                else
                    newState = ActivePressed;
            }
            else if (event == Release || event == Leave)
            {
                _fadeOut();
            }
            break;

        case Hovering:
            if (event == Leave)
            {
                newState = Inactive;
                _fadeOut();
            }
            else if (event == Press)
            {
                if (m_conf.toggle)
                    newState = Pressed;
                else
                    newState = ActivePressed;
            }
            break;

        case Pressed:
            if (event == Release)
            {
                click();
                emit onClick();
                newState = Active;
                if (!m_conf.hovering && m_conf.toggle) _fadeOut();
            }
            else if (event == Leave)
                newState = Inactive;
            break;

        case Active:
            if (event == Press)
            {
                newState = ActivePressed;
                if (!m_conf.hovering && m_conf.toggle) _fadeIn();
            }
            break;

        case ActivePressed:
            if (event == Leave)
            {
                if (m_conf.toggle)
                    newState = Active;
                else
                    newState = Inactive;
            }
            else if (event == Release)
            {
                click();
                emit onClick();
                if (m_conf.hovering)
                    newState = Hovering;
                else
                {
                    newState = Inactive;
                    if (m_conf.toggle) _fadeOut();
                }
            }
            break;
    }

    // qDebug("old state: %s, new state: %s, event: %s", debugState(m_state), debugState(newState), debugEvent(event));

    if (m_state != newState)
    {
        notify(m_state, newState);
        m_state = newState;
    }

    update();
}
//=========================================================
void ButtonBehavior::notify(ButtonMachineState oldState, ButtonMachineState newState)
{
    if ((newState == Active && oldState == Pressed) || (newState == ActivePressed && oldState != Active))
    {
        // notify activation
        emit onEnable();
        emit onChange(true);
        m_active = true;
        stateChange(true);
    }
    if ((newState == Inactive && oldState != Hovering && oldState != Pressed) || (newState == Hovering && oldState == ActivePressed))
    {
        // notify deactivation
        emit onDisable();
        emit onChange(false);
        m_active = false;
        stateChange(false);
    }
}
//=========================================================
void ButtonBehavior::mousePressEvent(QMouseEvent *event)
{
    transition(Press);
    QPushButton::mousePressEvent(event);
}
//=========================================================
void ButtonBehavior::mouseReleaseEvent(QMouseEvent *event)
{
    transition(Release);
    QPushButton::mouseReleaseEvent(event);
}
//=========================================================
void ButtonBehavior::leaveEvent(QEvent *event)
{
    transition(Leave);
    QPushButton::leaveEvent(event);
}
//=========================================================
void ButtonBehavior::enterEvent(QEnterEvent *event)
{
    transition(Enter);
    QPushButton::enterEvent(event);
}
//=========================================================
void ButtonBehavior::mouseMoveEvent(QMouseEvent *event)
{
    QPushButton::mouseMoveEvent(event);

    bool hit = false;

    if (underMouse()) hit = hitButton(event->position().toPoint());

    if (!hit && (m_state == ActivePressed || m_state == Pressed)) transition(Leave);
}
//=========================================================
void ButtonBehavior::hideEvent(QHideEvent *event)
{
    transition(Leave);
    QPushButton::hideEvent(event);
}
//=========================================================
bool ButtonBehavior::hitButton(const QPoint &pos) const { return rect().contains(pos); }
//=========================================================
ButtonBehavior::ButtonBehavior(QWidget *parent)
    : QPushButton(parent),
      m_state(Inactive),
      m_conf(),
      m_active(false)
{
    setMouseTracking(m_conf.hovering);
}
//=========================================================
bool ButtonBehavior::isActive() const { return m_active; }
//=========================================================
bool ButtonBehavior::isHovering() const { return m_hovering; }
//=========================================================
void ButtonBehavior::forceState(bool state)
{
    if (!m_conf.toggle) return;
    if (state == m_active) return;

    if (state)
    {
        m_state = Active;
        // notify activation
        emit onEnable();
        emit onChange(true);
    }
    else
    {
        m_state = m_hovering && m_conf.hovering ? Hovering : Inactive;
        // notify deactivation
        emit onDisable();
        emit onChange(false);
    }

    m_active = state;
    stateChange(state);

    update();
}
//=========================================================
