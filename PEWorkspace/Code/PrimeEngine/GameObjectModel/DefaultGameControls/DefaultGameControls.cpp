// GmaeControls.cpp



// Sibling/Children Includes

#include "DefaultGameControls.h"

#include "PrimeEngine/Scene/TextSceneNode.h"
#include "PrimeEngine/Scene/DebugRenderer.h"

// definitions of keyboard and controller events. s.a. Button pressed, etc

#include "../../Events/StandardControllerEvents.h"

#include "../../Events/StandardKeyboardEvents.h"

#if APIABSTRACTION_IOS

	#include "../../Events/StandardIOSEvents.h"

#endif



// definitions of standard game events. the events that any game could potentially use

#include "../../Events/StandardGameEvents.h"

#include "../../Lua/LuaEnvironment.h"
// #include "CharacterControl/Tank/ClientTank.h"
#include "CharacterControl/ClientGameObjectManagerAddon.h"
#include <cstring>


// Arkane Control Values

#define Analog_To_Digital_Trigger_Distance 0.5f

static float Debug_Fly_Speed = 4.0f; //Units per second

#define Debug_Rotate_Speed 2.0f //Radians per second


namespace PE {

namespace Components {



using namespace PE::Events;



PE_IMPLEMENT_CLASS1(DefaultGameControls, Component);



void DefaultGameControls::addDefaultComponents()

{

	Component::addDefaultComponents();



	PE_REGISTER_EVENT_HANDLER(Event_UPDATE, DefaultGameControls::do_UPDATE);

}



void DefaultGameControls::do_UPDATE(Events::Event *pEvt)
{

	// Process input events (controller buttons, triggers...)

	Handle iqh = Events::EventQueueManager::Instance()->getEventQueueHandle("input");

	// Process input event -> game event conversion

	while (!iqh.getObject<Events::EventQueue>()->empty())
	{
		Events::Event *pInputEvt = iqh.getObject<Events::EventQueue>()->getFront();

		m_frameTime = ((Event_UPDATE*)(pEvt))->m_frameTime;

		// Have DefaultGameControls translate the input event to GameEvents

		handleKeyboardDebugInputEvents(pInputEvt);

		handleControllerDebugInputEvents(pInputEvt);

        handleIOSDebugInputEvents(pInputEvt);

		handleMouseDebugInputEvents(pInputEvt);
		

		iqh.getObject<Events::EventQueue>()->destroyFront();

	}



	// Events are destoryed by destroyFront() but this is called every frame just in case

	iqh.getObject<Events::EventQueue>()->destroy();

}

    

void DefaultGameControls::handleIOSDebugInputEvents(Event *pEvt)

{

	#if APIABSTRACTION_IOS

		m_pQueueManager = Events::EventQueueManager::Instance();

		if (Event_IOS_TOUCH_MOVED::GetClassId() == pEvt->getClassId())

		{

			Event_IOS_TOUCH_MOVED *pRealEvent = (Event_IOS_TOUCH_MOVED *)(pEvt);

            

			if(pRealEvent->touchesCount > 1)

			{

				Handle h("EVENT", sizeof(Event_FLY_CAMERA));

                Event_FLY_CAMERA *flyCameraEvt = new(h) Event_FLY_CAMERA ;

                

                Vector3 relativeMovement(0.0f,0.0f,1.0f * 30.0 * pRealEvent->m_normalized_dy);

                flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;

                m_pQueueManager->add(h, QT_GENERAL);

                

			}

			else

			{

				Handle h("EVENT", sizeof(Event_ROTATE_CAMERA));

				Event_ROTATE_CAMERA *rotateCameraEvt = new(h) Event_ROTATE_CAMERA ;

                

				Vector3 relativeRotate(-pRealEvent->m_normalized_dx * 10, pRealEvent->m_normalized_dy * 10, 0.0f);

				rotateCameraEvt->m_relativeRotate = relativeRotate * Debug_Rotate_Speed * m_frameTime;

				m_pQueueManager->add(h, QT_GENERAL);

            

			}

		}

	#endif

}

// mouse vs actual px position difference: 5~7px (height)

void DefaultGameControls::handleMouseDebugInputEvents(Event *pEvt)
{
	m_pQueueManager = Events::EventQueueManager::Instance();
	// dx9_keybarodMouse
	if (Event_MOUSE_LEFT_CLICK::GetClassId() == pEvt->getClassId()){
		// print them in integer... otherwise NAN / 0 values.
		// y pos: 3~5px difference. ui pos is 3px smaller than actual mouse pos: maybe it's a good offset.
		// you can assume each letter is roughly 15px wide (20px is too wide)
		// TODO: textSceneNode should include button name, instead of accessing via index
		
		if(CharacterControl::Components::ClientGameObjectManagerAddon::tankSN != nullptr) {
			Matrix4x4& base = CharacterControl::Components::ClientGameObjectManagerAddon::tankSN->m_base;
			Vector3 pos = base.getPos();

			// button 0: TANK_DOWN, 1: TANK_UP, 2: TANK_RIGHT, 3: TANK_LEFT
			for (int i = 0; i < PE::Components::DebugRenderer::m_textSceneNodes.size(); i++)
			{
				float xPos = PE::Components::DebugRenderer::m_textSceneNodes[i]->g_pos2D.m_x;
				float yPos = PE::Components::DebugRenderer::m_textSceneNodes[i]->g_pos2D.m_y;
				float index2_char_length = PE::Components::DebugRenderer::m_textSceneNodes[i]->m_strLen;
				if (strcmp(PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, "TANK1_DOWN") == 0)
				{
					if(m_pContext->g_cursorPos.x >= xPos && m_pContext->g_cursorPos.x <= (xPos+15*index2_char_length))
					{
						if(m_pContext->g_cursorPos.y >= yPos && m_pContext->g_cursorPos.y <= (yPos+30))
						{
							// working
							PEINFO("////// MOUSE CLICK: %d, %d", m_pContext->g_cursorPos.x, m_pContext->g_cursorPos.y);
							PEINFO("Tank Button m_uiName: %s, xPos: %f, yPos: %f", PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, xPos, yPos);
							base.setPos(Vector3(pos.m_x, pos.m_y - 0.5f, pos.m_z));
						}
					}
				}
				else if (strcmp(PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, "TANK1_UP") == 0)
				{
					if(m_pContext->g_cursorPos.x >= xPos && m_pContext->g_cursorPos.x <= (xPos+15*index2_char_length))
					{
						if(m_pContext->g_cursorPos.y >= yPos && m_pContext->g_cursorPos.y <= (yPos+30))
						{
							// working
							PEINFO("////// MOUSE CLICK: %d, %d", m_pContext->g_cursorPos.x, m_pContext->g_cursorPos.y);
							PEINFO("Tank Button m_uiName: %s, xPos: %f, yPos: %f", PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, xPos, yPos);
							base.setPos(Vector3(pos.m_x, pos.m_y + 0.5f, pos.m_z));
						}
					}
				}
				else if (strcmp(PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, "TANK1_LEFT") == 0)
				{
					if(m_pContext->g_cursorPos.x >= xPos && m_pContext->g_cursorPos.x <= (xPos+15*index2_char_length))
					{
						if(m_pContext->g_cursorPos.y >= yPos && m_pContext->g_cursorPos.y <= (yPos+30))
						{
							// working
							PEINFO("////// MOUSE CLICK: %d, %d", m_pContext->g_cursorPos.x, m_pContext->g_cursorPos.y);
							PEINFO("Tank Button m_uiName: %s, xPos: %f, yPos: %f", PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, xPos, yPos);
							base.setPos(Vector3(pos.m_x - 0.5f, pos.m_y, pos.m_z));
						}
					}
				}
				else if (strcmp(PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, "TANK1_RIGHT") == 0)
				{
					if(m_pContext->g_cursorPos.x >= xPos && m_pContext->g_cursorPos.x <= (xPos+15*index2_char_length))
					{
						if(m_pContext->g_cursorPos.y >= yPos && m_pContext->g_cursorPos.y <= (yPos+30))
						{
							// working
							PEINFO("////// MOUSE CLICK: %d, %d", m_pContext->g_cursorPos.x, m_pContext->g_cursorPos.y);
							PEINFO("Tank Button m_uiName: %s, xPos: %f, yPos: %f", PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, xPos, yPos);
							base.setPos(Vector3(pos.m_x + 0.5f, pos.m_y, pos.m_z));
						}
					}
				}
			}
		}

		// TEXT COLOR BUTTONS
		for (int i = 0; i < PE::Components::DebugRenderer::m_textSceneNodes.size(); i++)
		{
			float xPos = PE::Components::DebugRenderer::m_textSceneNodes[i]->g_pos2D.m_x;
			float yPos = PE::Components::DebugRenderer::m_textSceneNodes[i]->g_pos2D.m_y;
			float index2_char_length = PE::Components::DebugRenderer::m_textSceneNodes[i]->m_strLen;
			if (strcmp(PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, "TEXT1_RED") == 0)
			{
				if(m_pContext->g_cursorPos.x >= xPos && m_pContext->g_cursorPos.x <= (xPos+15*index2_char_length))
				{
					if(m_pContext->g_cursorPos.y >= yPos && m_pContext->g_cursorPos.y <= (yPos+30))
					{
						// working
						PEINFO("////// MOUSE CLICK: %d, %d", m_pContext->g_cursorPos.x, m_pContext->g_cursorPos.y);
						PEINFO("Button m_uiName: %s, xPos: %f, yPos: %f", PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, xPos, yPos);
						m_pContext->text_rgb_1 = Vector3(1.0f, 0.0f, 0.0f);
					}
				}
			}
			if (strcmp(PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, "TEXT1_GREEN") == 0)
			{
				if(m_pContext->g_cursorPos.x >= xPos && m_pContext->g_cursorPos.x <= (xPos+15*index2_char_length))
				{
					if(m_pContext->g_cursorPos.y >= yPos && m_pContext->g_cursorPos.y <= (yPos+30))
					{
						// working
						PEINFO("////// MOUSE CLICK: %d, %d", m_pContext->g_cursorPos.x, m_pContext->g_cursorPos.y);
						PEINFO("Button m_uiName: %s, xPos: %f, yPos: %f", PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, xPos, yPos);
						m_pContext->text_rgb_1 = Vector3(0.0f, 1.0f, 0.0f);
					}
				}
			}
			if (strcmp(PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, "TEXT1_BLUE") == 0)
			{
				if(m_pContext->g_cursorPos.x >= xPos && m_pContext->g_cursorPos.x <= (xPos+15*index2_char_length))
				{
					if(m_pContext->g_cursorPos.y >= yPos && m_pContext->g_cursorPos.y <= (yPos+30))
					{
						// working
						PEINFO("////// MOUSE CLICK: %d, %d", m_pContext->g_cursorPos.x, m_pContext->g_cursorPos.y);
						PEINFO("Button m_uiName: %s, xPos: %f, yPos: %f", PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, xPos, yPos);
						m_pContext->text_rgb_1 = Vector3(0.0f, 0.0f, 1.0f);
					}
				}
			}
			if (strcmp(PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, "TOGGLE_DEBUG_INFO") == 0)
			{
				if(m_pContext->g_cursorPos.x >= xPos && m_pContext->g_cursorPos.x <= (xPos+15*index2_char_length))
				{
					if(m_pContext->g_cursorPos.y >= yPos && m_pContext->g_cursorPos.y <= (yPos+30))
					{
						// working
						PEINFO("////// MOUSE CLICK: %d, %d", m_pContext->g_cursorPos.x, m_pContext->g_cursorPos.y);
						PEINFO("Button m_uiName: %s, xPos: %f, yPos: %f", PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName, xPos, yPos);
						if(m_pContext->toggleDebugInfo)
							m_pContext->toggleDebugInfo = false;
						else
							m_pContext->toggleDebugInfo = true;
					}
				}
			}
		}
		// TODO: TANK1, TANK2, TANK3 buttons vertically aligned. when clicked, expand and show up, down, left, right on the right
		// TODO:  WHEN BUTTON IS CLICKED, BLINK THE TEXT BG COLOR TO WHITE OR STH 

		/*
		if(m_pContext->g_cursorPos.x >= PE::Components::DebugRenderer::m_textSceneNodes[2]->g_pos2D.m_x) 
		{
			if(m_pContext->g_cursorPos.y <= PE::Components::DebugRenderer::m_textSceneNodes[2]->g_pos2D.m_y) 
			{
				if(m_pContext->g_cursorPos.y <= PE::Components::DebugRenderer::m_textSceneNodes[2]->g_pos2D.m_y + 30) 
				{
					// text height is around 25px
					PEINFO("DefaultGameControls MOUSE LEFT CLICKED RiGHT");
					// working
					// m_pContext->imgui_wasd = 1;
				}
			}
		}
		if(CharacterControl::Components::ClientGameObjectManagerAddon::tankSN != nullptr) {
			Matrix4x4& base = CharacterControl::Components::ClientGameObjectManagerAddon::tankSN->m_base;
			Vector3 pos = base.getPos();
			PEINFO("////DefaultGameControls::: TANK SCENE NODE: %f", pos.m_x);
			// move with buttons
			if(m_pContext->g_cursorPos.x >= PE::Components::DebugRenderer::m_textSceneNodes[0]->g_pos2D.m_x) 
			{
				if(m_pContext->g_cursorPos.x <= PE::Components::DebugRenderer::m_textSceneNodes[0]->g_pos2D.m_x + ) 
				{
					if(m_pContext->g_cursorPos.y >= PE::Components::DebugRenderer::m_textSceneNodes[0]->g_pos2D.m_y) 
					{
						if(m_pContext->g_cursorPos.y <= PE::Components::DebugRenderer::m_textSceneNodes[0]->g_pos2D.m_y + 30) 
						{
							// text height is around 25px
							// working
							PEINFO("DefaultGameControls MOUSE LEFT CLICKED DOWN");
							base.setPos(Vector3(pos.m_x, pos.m_y - 0.1f, pos.m_z));
						}
					}
				}
			}
		}
		*/
		// check if m_textSceneNodes is not null or empty
		if(PE::Components::DebugRenderer::m_textSceneNodes.size() > 0)
		{
			// PEINFO("ui 0 m_str: %s", PE::Components::DebugRenderer::m_textSceneNodes[0]->m_str);
			// PEINFO("ui 0 index pos %f, %f:", PE::Components::DebugRenderer::m_textSceneNodes[0]->g_pos2D.m_x, PE::Components::DebugRenderer::m_textSceneNodes[0]->g_pos2D.m_y);
			// PEINFO("ui 1 m_str: %s", PE::Components::DebugRenderer::m_textSceneNodes[1]->m_str);
			// PEINFO("ui 1 index pos %f, %f:", PE::Components::DebugRenderer::m_textSceneNodes[1]->g_pos2D.m_x, PE::Components::DebugRenderer::m_textSceneNodes[1]->g_pos2D.m_y);

			// GT Frame
			// PEINFO("ui 2 m_str: %s", PE::Components::DebugRenderer::m_textSceneNodes[2]->m_str);
			// PEINFO("ui 2 index pos %f, %f:", PE::Components::DebugRenderer::m_textSceneNodes[2]->g_pos2D.m_x, PE::Components::DebugRenderer::m_textSceneNodes[2]->g_pos2D.m_y);

			// SERVER, CLIENT
			// PEINFO("ui 3 m_str: %s", PE::Components::DebugRenderer::m_textSceneNodes[3]->m_str);
			// PEINFO("ui 3 index pos %f, %f:", PE::Components::DebugRenderer::m_textSceneNodes[3]->g_pos2D.m_x, PE::Components::DebugRenderer::m_textSceneNodes[3]->g_pos2D.m_y);

			// PEINFO("ui 4 m_str: %s", PE::Components::DebugRenderer::m_textSceneNodes[4]->m_str);
			// PEINFO("ui 4 index pos %f, %f:", PE::Components::DebugRenderer::m_textSceneNodes[4]->g_pos2D.m_x, PE::Components::DebugRenderer::m_textSceneNodes[4]->g_pos2D.m_y);
			/*
			// Lua Command
			PEINFO("ui 5 m_str: %s", PE::Components::DebugRenderer::m_textSceneNodes[5]->m_str);
			PEINFO("ui 5 index pos %f, %f:", PE::Components::DebugRenderer::m_textSceneNodes[5]->g_pos2D.m_x, PE::Components::DebugRenderer::m_textSceneNodes[5]->g_pos2D.m_y);
			
			// FPS
			PEINFO("ui 6 m_str: %s", PE::Components::DebugRenderer::m_textSceneNodes[6]->m_str);
			PEINFO("ui 6 index pos %f, %f:", PE::Components::DebugRenderer::m_textSceneNodes[6]->g_pos2D.m_x, PE::Components::DebugRenderer::m_textSceneNodes[6]->g_pos2D.m_y);

			// check if the mouse click is inside the text box
			// if(m_pContext->g_cursorPos.x >= m_pContext->m_textSceneNodes[0].g_pos2D.x && m_pContext->g_cursorPos.x <= m_pContext->m_textSceneNodes[0].g_pos2D.x + 100)
			// {
			// 	if(m_pContext->g_cursorPos.y >= m_pContext->m_textSceneNodes[0].g_pos2D.y && m_pContext->g_cursorPos.y <= m_pContext->m_textSceneNodes[0].g_pos2D.y + 100)
			// 	{
			// 		// if it is, then change the color of the text
			// 		m_pContext->m_textSceneNodes[0].m_rgb = Vector3(1.0f, 0.0f, 0.0f);
			// 	}
			// }
			*/
		}

	}
}

void DefaultGameControls::handleKeyboardDebugInputEvents(Event *pEvt)
{
	m_pQueueManager = Events::EventQueueManager::Instance();

	if (Event_KEY_A_HELD::GetClassId() == pEvt->getClassId())
	{

		Handle h("EVENT", sizeof(Event_FLY_CAMERA));

		Event_FLY_CAMERA *flyCameraEvt = new(h) Event_FLY_CAMERA ;

		Vector3 relativeMovement(-1.0f,0.0f,0.0f);

		flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;

		m_pQueueManager->add(h, QT_GENERAL);
	}

	else if (Event_KEY_S_HELD::GetClassId() == pEvt->getClassId())
	{
		Handle h("EVENT", sizeof(Event_FLY_CAMERA));
		Event_FLY_CAMERA *flyCameraEvt = new(h) Event_FLY_CAMERA ;

		Vector3 relativeMovement(0.0f,0.0f,-1.0f);

		flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;

		m_pQueueManager->add(h, QT_GENERAL);

	}

	else if (Event_KEY_D_HELD::GetClassId() == pEvt->getClassId())

	{

		Handle h("EVENT", sizeof(Event_FLY_CAMERA));

		Event_FLY_CAMERA *flyCameraEvt = new(h) Event_FLY_CAMERA ;

		Vector3 relativeMovement(1.0f,0.0f,0.0f);
		flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;
		m_pQueueManager->add(h, QT_GENERAL);
	}

	else if (Event_KEY_W_HELD::GetClassId() == pEvt->getClassId())
	{
		Handle h("EVENT", sizeof(Event_FLY_CAMERA));
		Event_FLY_CAMERA *flyCameraEvt = new(h) Event_FLY_CAMERA ;

		Vector3 relativeMovement(0.0f,0.0f,1.0f);

		flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;

		m_pQueueManager->add(h, QT_GENERAL);
	}
	else if (Event_KEY_LEFT_HELD::GetClassId() == pEvt->getClassId())
	{

		Handle h("EVENT", sizeof(Event_ROTATE_CAMERA));

		Event_ROTATE_CAMERA *rotateCameraEvt = new(h) Event_ROTATE_CAMERA ;
		Vector3 relativeRotate(-1.0f,0.0f,0.0f);
		rotateCameraEvt->m_relativeRotate = relativeRotate * Debug_Rotate_Speed * m_frameTime;
		m_pQueueManager->add(h, QT_GENERAL);
	}
	else if (Event_KEY_DOWN_HELD::GetClassId() == pEvt->getClassId())
	{
		Handle h("EVENT", sizeof(Event_ROTATE_CAMERA));
		Event_ROTATE_CAMERA *rotateCameraEvt = new(h) Event_ROTATE_CAMERA ;
		Vector3 relativeRotate(0.0f,-1.0f,0.0f);

		rotateCameraEvt->m_relativeRotate = relativeRotate * Debug_Rotate_Speed * m_frameTime;

		m_pQueueManager->add(h, QT_GENERAL);
	}
	else if (Event_KEY_RIGHT_HELD::GetClassId() == pEvt->getClassId())
	{
		Handle h("EVENT", sizeof(Event_ROTATE_CAMERA));

		Event_ROTATE_CAMERA *rotateCameraEvt = new(h) Event_ROTATE_CAMERA ;
		Vector3 relativeRotate(1.0f,0.0f,0.0f);

		rotateCameraEvt->m_relativeRotate = relativeRotate * Debug_Rotate_Speed * m_frameTime;

		m_pQueueManager->add(h, QT_GENERAL);

	}

	else if (Event_KEY_UP_HELD::GetClassId() == pEvt->getClassId())
	{

		Handle h("EVENT", sizeof(Event_ROTATE_CAMERA));

		Event_ROTATE_CAMERA *rotateCameraEvt = new(h) Event_ROTATE_CAMERA ;

		

		Vector3 relativeRotate(0.0f,1.0f,0.0f);

		rotateCameraEvt->m_relativeRotate = relativeRotate * Debug_Rotate_Speed * m_frameTime;

		m_pQueueManager->add(h, QT_GENERAL);

	}

	else

	{

		Component::handleEvent(pEvt);

	}

}



void DefaultGameControls::handleControllerDebugInputEvents(Event *pEvt)

{

	if (Event_ANALOG_L_THUMB_MOVE::GetClassId() == pEvt->getClassId())

	{

		Event_ANALOG_L_THUMB_MOVE *pRealEvent = (Event_ANALOG_L_THUMB_MOVE*)(pEvt);

		

		Handle h("EVENT", sizeof(Event_FLY_CAMERA));

		Event_FLY_CAMERA *flyCameraEvt = new(h) Event_FLY_CAMERA ;

		

		Vector3 relativeMovement(pRealEvent->m_absPosition.getX(), 0.0f, pRealEvent->m_absPosition.getY());//Flip Y and Z axis

		flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;

		m_pQueueManager->add(h, QT_GENERAL);

	}

	else if (Event_ANALOG_R_THUMB_MOVE::GetClassId() == pEvt->getClassId())

	{

		Event_ANALOG_R_THUMB_MOVE *pRealEvent = (Event_ANALOG_R_THUMB_MOVE *)(pEvt);

		

		Handle h("EVENT", sizeof(Event_ROTATE_CAMERA));

		Event_ROTATE_CAMERA *rotateCameraEvt = new(h) Event_ROTATE_CAMERA ;

		

		Vector3 relativeRotate(pRealEvent->m_absPosition.getX(), pRealEvent->m_absPosition.getY(), 0.0f);

		rotateCameraEvt->m_relativeRotate = relativeRotate * Debug_Rotate_Speed * m_frameTime;

		m_pQueueManager->add(h, QT_GENERAL);

	}

	else if (Event_PAD_N_DOWN::GetClassId() == pEvt->getClassId())

	{

	}

	else if (Event_PAD_N_HELD::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_PAD_N_UP::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_PAD_S_DOWN::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_PAD_S_HELD::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_PAD_S_UP::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_PAD_W_DOWN::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_PAD_W_HELD::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_PAD_W_UP::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_PAD_E_DOWN::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_PAD_E_HELD::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_PAD_E_UP::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_BUTTON_A_HELD::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_BUTTON_Y_DOWN::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_BUTTON_A_UP::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_BUTTON_B_UP::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_BUTTON_X_UP::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_BUTTON_Y_UP::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_ANALOG_L_TRIGGER_MOVE::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_ANALOG_R_TRIGGER_MOVE::GetClassId() == pEvt->getClassId())

	{

		

	}

	else if (Event_BUTTON_L_SHOULDER_DOWN::GetClassId() == pEvt->getClassId())

	{

		

	}

	else

	{

		Component::handleEvent(pEvt);

	}

}



}; // namespace Components

}; // namespace PE

