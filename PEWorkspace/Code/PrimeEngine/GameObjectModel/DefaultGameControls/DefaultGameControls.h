#ifndef __PYENGINE_DEFAULT_GAME_CONTROLS_H__
#define __PYENGINE_DEFAULT_GAME_CONTROLS_H__

// API Abstraction

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "../../Events/EventQueueManager.h"
#include "../../Events/Component.h"
#include "../../Events/StandardKeyboardEvents.h"
#include "../../Scene/CameraManager.h"
#include "../../Logging/Log.h"

// definitions of standard game events. the events that any game could potentially use
#include "../../Events/StandardGameEvents.h"

// Sibling/Children includes

namespace PE {
namespace Components {

struct DefaultGameControls : public Component
{
	PE_DECLARE_CLASS(DefaultGameControls);
public:
	
	DefaultGameControls(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : Component(context, arena, hMyself)
	{
		spotlightClickedOnce = false;
		dirlightClickedOnce = false;
		pointlightClickedOnce = false;
	}

	virtual ~DefaultGameControls(){}
	// Component ------------------------------------------------------------

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_UPDATE);
	virtual void do_UPDATE(Events::Event *pEvt);

	virtual void addDefaultComponents() ;

	//Methods----------------
	void handleIOSDebugInputEvents(Events::Event *pEvt);
	void handleKeyboardDebugInputEvents(Events::Event *pEvt);
	void handleControllerDebugInputEvents(Events::Event *pEvt);
	void handleMouseDebugInputEvents(Events::Event *pEvt);
	bool IsCursorWithinBounds(PE::GameContext* m_pContext, float xPos, float yPos, float index2_char_length);
	void OnClick_TextColorButtons(PE::GameContext* m_pContext, float xPos, float yPos, float index2_char_length, Vector3 color);
	void OnClick_CameraButtons(PE::GameContext* m_pContext, const char* uiKey, float xPos, float yPos, float index2_char_length);
	void OnClick_TankButtons(PE::GameContext* m_pContext, const char* uiKey, float xPos, float yPos, float index2_char_length, Vector3* pos, Matrix4x4& base);
	void OnClick_SoldierButtons(PE::GameContext* m_pContext, const char* uiKey, float xPos, float yPos, float index2_char_length, Vector3* pos, Matrix4x4& base);

	Events::EventQueueManager *m_pQueueManager;
	
	PrimitiveTypes::Float32 m_frameTime;
	bool spotlightClickedOnce;
	bool dirlightClickedOnce;
	bool pointlightClickedOnce;
};
}; // namespace Components
}; // namespace PE

#endif//File guard