// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#if APIABSTRACTION_D3D9 | APIABSTRACTION_D3D11 | (APIABSTRACTION_OGL)

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/Events/StandardKeyboardEvents.h"
#include "PrimeEngine/Render/D3D11Renderer.h"
#include "PrimeEngine/Render/D3D9Renderer.h"
#include "../../../Lua/LuaEnvironment.h"
// Sibling/Children includes
#include "DX9_KeyboardMouse.h"
#include "PrimeEngine/Application/WinApplication.h"

namespace PE {
using namespace Events;
namespace Components {

PE_IMPLEMENT_CLASS1(DX9_KeyboardMouse, Component);

void DX9_KeyboardMouse::addDefaultComponents()
{
	Component::addDefaultComponents();
	PE_REGISTER_EVENT_HANDLER(Events::Event_UPDATE, DX9_KeyboardMouse::do_UPDATE);
}

void DX9_KeyboardMouse::do_UPDATE(Events::Event *pEvt)
{
	
	m_pQueueManager = Events::EventQueueManager::Instance();

	generateButtonEvents();
}

void DX9_KeyboardMouse::generateButtonEvents()
{
#if PE_PLAT_IS_WIN32
	WinApplication *pWinApp = static_cast<WinApplication*>(m_pContext->getApplication());
	if(GetFocus() == pWinApp->getWindowHandle())
#endif
	{
		// Check Mouse EVents
		/*
			BOOL GetCursorPos( LPPOINT lpPoint);
		*/
		if(GetCursorPos(&cursorPos)) {
			if(ScreenToClient(pWinApp->getWindowHandle(), &cursorPos)) {
				// MouseOver: correct position: inside the PE window only.
				// PEINFO("MouseOver Position: %d, %d", cursorPos.x, cursorPos.y);
				// Handle h("EVENT", sizeof(Event_MOUSE_OVER));
				// new (h) Event_MOUSE_OVER();
				// m_pQueueManager->add(h, Events::QT_INPUT);

				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
					PEINFO("VK_LBUTTON Position: %d, %d", cursorPos.x, cursorPos.y);
					Handle h("EVENT", sizeof(Event_MOUSE_LEFT_CLICK));
					new (h) Event_MOUSE_LEFT_CLICK();
					m_pQueueManager->add(h, Events::QT_INPUT);
					m_pContext->g_cursorPos.x = cursorPos.x;
					m_pContext->g_cursorPos.y = cursorPos.y;
				}

				if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
					//PEINFO("VK_RBUTTON Position: %d, %d", cursorPos.x, cursorPos.y);
					Handle h("EVENT", sizeof(Event_MOUSE_RIGHT_CLICK));
					new (h) Event_MOUSE_RIGHT_CLICK();
					m_pQueueManager->add(h, Events::QT_INPUT);
					m_pContext->g_cursorPos.x = cursorPos.x;
					m_pContext->g_cursorPos.y = cursorPos.y;
				}
			}
		}

		//Check for Button Down events

		//Check for Button Up events
		
		//Check for Button Held events
		// Add imgui_wasd buttons interaction variable
		if((GetAsyncKeyState('A') & 0x8000) || m_pContext->imgui_wasd == 2)
		{
			Handle h("EVENT", sizeof(Event_KEY_A_HELD));
			Event_KEY_A_HELD *event = new (h) Event_KEY_A_HELD;
			event->m_move = 1.0f;
			m_pQueueManager->add(h, Events::QT_INPUT);
			m_pContext->m_button = 2; 
		}
		if((GetAsyncKeyState('S') & 0x8000) || m_pContext->imgui_wasd == 3)
		{
			Handle h("EVENT", sizeof(Event_KEY_S_HELD));
			Event_KEY_S_HELD *event = new (h) Event_KEY_S_HELD;
			event->m_move = 1.0f; 
			m_pQueueManager->add(h, Events::QT_INPUT);
			m_pContext->m_button = 3; 
		}
		if((GetAsyncKeyState('D') & 0x8000) || m_pContext->imgui_wasd == 4)
		{
			Handle h("EVENT", sizeof(Event_KEY_D_HELD));
			Event_KEY_D_HELD *event = new (h) Event_KEY_D_HELD;
			event->m_move = 1.0f; 
			m_pQueueManager->add(h, Events::QT_INPUT);
			m_pContext->m_button = 4; 
		}
		if((GetAsyncKeyState('W') & 0x8000) || m_pContext->imgui_wasd == 1)
		{
			Handle h("EVENT", sizeof(Event_KEY_W_HELD));
			Event_KEY_W_HELD *event = new (h) Event_KEY_W_HELD;
			event->m_move = 1.0f; 
			m_pQueueManager->add(h, Events::QT_INPUT);
			m_pContext->m_button = 1; 
		}
		if(GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			Handle h("EVENT", sizeof(Event_KEY_LEFT_HELD));
			new (h) Event_KEY_LEFT_HELD;
			m_pQueueManager->add(h, Events::QT_INPUT);
			PEINFO("ARROW LEFT key is pressed");
		}
		if(GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			Handle h("EVENT", sizeof(Event_KEY_DOWN_HELD));
			new (h) Event_KEY_DOWN_HELD;
			m_pQueueManager->add(h, Events::QT_INPUT);
			PEINFO("ARROW DOWN key is pressed");
		}
		if(GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			Handle h("EVENT", sizeof(Event_KEY_RIGHT_HELD));
			new (h) Event_KEY_RIGHT_HELD;
			m_pQueueManager->add(h, Events::QT_INPUT);
			PEINFO("ARROW RIGHT key is pressed");
		}
		if(GetAsyncKeyState(VK_UP) & 0x8000)
		{
			Handle h("EVENT", sizeof(Event_KEY_UP_HELD));
			new (h) Event_KEY_UP_HELD;
			m_pQueueManager->add(h, Events::QT_INPUT);
			PEINFO("ARROW UP key is pressed");
		}
		if(GetAsyncKeyState(',') & 0x8000)
		{
			Handle h("EVENT", sizeof(Event_KEY_COMMA_HELD));
			new (h) Event_KEY_COMMA_HELD;
			m_pQueueManager->add(h, Events::QT_INPUT);
		}
		if(GetAsyncKeyState('.') & 0x8000)
		{
			Handle h("EVENT", sizeof(Event_KEY_PERIOD_HELD));
			new (h) Event_KEY_PERIOD_HELD;
			m_pQueueManager->add(h, Events::QT_INPUT);
		}
		if(GetAsyncKeyState('K') & 0x8000)
		{
			Handle h("EVENT", sizeof(Event_KEY_K_HELD));
			new (h) Event_KEY_K_HELD;
			m_pQueueManager->add(h, Events::QT_INPUT);
		}
		if(GetAsyncKeyState('L') & 0x8000)
		{
			Handle h("EVENT", sizeof(Event_KEY_L_HELD));
			new (h) Event_KEY_L_HELD;
			m_pQueueManager->add(h, Events::QT_INPUT);
		}
	}
}

}; // namespace Components
}; // namespace PE

#endif // API Abstraction
