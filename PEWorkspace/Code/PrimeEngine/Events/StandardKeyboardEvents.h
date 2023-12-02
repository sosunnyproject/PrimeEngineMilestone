#ifndef __PYENGINE_2_0_STANDARD_KEYBOARD_EVENTS_H__
#define __PYENGINE_2_0_STANDARD_KEYBOARD_EVENTS_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes

// Sibling/Children includes
#include "StandardEvents.h"

namespace PE {
namespace Events {

// Add Mouse Click Events
struct Event_MOUSE_OVER : public Event {
	PE_DECLARE_CLASS(Event_MOUSE_OVER);
	virtual ~Event_MOUSE_OVER() {}
};
struct Event_MOUSE_LEFT_CLICK : public Event {
	PE_DECLARE_CLASS(Event_MOUSE_LEFT_CLICK);
	virtual ~Event_MOUSE_LEFT_CLICK(){}
};

struct Event_MOUSE_RIGHT_CLICK : public Event {
	PE_DECLARE_CLASS(Event_MOUSE_RIGHT_CLICK);
	virtual ~Event_MOUSE_RIGHT_CLICK(){}
};

struct Event_KEY_A_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_A_HELD);
	virtual ~Event_KEY_A_HELD(){}

	float m_moveCamByButton;
};

struct Event_KEY_S_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_S_HELD);
	virtual ~Event_KEY_S_HELD(){}

	float m_moveCamByButton;
};

struct Event_KEY_D_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_D_HELD);
	virtual ~Event_KEY_D_HELD(){}

	float m_moveCamByButton;
};

struct Event_KEY_W_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_W_HELD);
	virtual ~Event_KEY_W_HELD(){}

	float m_moveCamByButton;
};

struct Event_KEY_PERIOD_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_PERIOD_HELD);
	virtual ~Event_KEY_PERIOD_HELD(){}
};

struct Event_KEY_COMMA_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_COMMA_HELD);
	virtual ~Event_KEY_COMMA_HELD(){}
};

struct Event_KEY_K_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_K_HELD);
	virtual ~Event_KEY_K_HELD(){}
};

struct Event_KEY_L_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_L_HELD);
	virtual ~Event_KEY_L_HELD(){}
};

struct Event_KEY_LEFT_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_LEFT_HELD);
	virtual ~Event_KEY_LEFT_HELD(){}
};

struct Event_KEY_DOWN_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_DOWN_HELD);
	virtual ~Event_KEY_DOWN_HELD(){}
};

struct Event_KEY_RIGHT_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_RIGHT_HELD);
	virtual ~Event_KEY_RIGHT_HELD(){}
};

struct Event_KEY_UP_HELD : public Event {
	PE_DECLARE_CLASS(Event_KEY_UP_HELD);
	virtual ~Event_KEY_UP_HELD(){}
};

}; // namespace Events
}; // namespace PE

#endif
