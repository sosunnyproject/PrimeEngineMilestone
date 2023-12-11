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
#include "CharacterControl/Characters/SoldierNPCMovementSM.h"
#include "PrimeEngine/GameObjectModel/GameObjectManager.h"

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

bool DefaultGameControls::IsCursorWithinBounds(PE::GameContext* m_pContext, float xPos, float yPos, float index2_char_length)
{
    return m_pContext->g_cursorPos.x >= xPos && m_pContext->g_cursorPos.x <= (xPos+15*index2_char_length) && m_pContext->g_cursorPos.y >= yPos && m_pContext->g_cursorPos.y <= (yPos+30);
}

// mouse vs actual px position difference: 5~7px (height)
void DefaultGameControls::handleMouseDebugInputEvents(Event *pEvt)
{
	m_pQueueManager = Events::EventQueueManager::Instance();
	// dx9_keybarodMouse
	if (Event_MOUSE_LEFT_CLICK::GetClassId() == pEvt->getClassId())
	{
		// print them in integer... otherwise NAN / 0 values.
		// y pos: 3~5px difference. ui pos is 3px smaller than actual mouse pos: maybe it's a good offset.
		// you can assume each letter is roughly 15px wide (20px is too wide)
		for (int i = 0; i < PE::Components::DebugRenderer::m_textSceneNodes.size(); i++)
		{
			float xPos = PE::Components::DebugRenderer::m_textSceneNodes[i]->g_pos2D.m_x;
			float yPos = PE::Components::DebugRenderer::m_textSceneNodes[i]->g_pos2D.m_y;
			float index2_char_length = PE::Components::DebugRenderer::m_textSceneNodes[i]->m_strLen;
			const char* uiName = PE::Components::DebugRenderer::m_textSceneNodes[i]->m_uiName;

			// if there is a TANK in the scene
			if(CharacterControl::Components::ClientGameObjectManagerAddon::tankSNarray[0] != nullptr) 
			{
				// Button Toggle Tank
				if (strcmp(uiName, "TANK1_BUTTONS") == 0)
				{
					if(IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
					{
						if (m_pContext->btnTank1_toggle) {
							m_pContext->btnTank1_toggle = false;
							m_pContext->btnTank1_rgb = Vector3(0.0f, 0.0f, 0.0f);
						}
						else {
							m_pContext->btnTank1_toggle = true;
							m_pContext->btnTank1_rgb = Vector3(1.0f, 0.0f, 0.5f);
						}
					}
				}
				// Button Tank
				if(m_pContext->btnTank1_toggle)
				{
					Matrix4x4& base = CharacterControl::Components::ClientGameObjectManagerAddon::tankSNarray[0]->m_base;
					Vector3 pos = base.getPos();
					// button 0: TANK_DOWN, 1: TANK_UP, 2: TANK_RIGHT, 3: TANK_LEFT
					
					OnClick_TankButtons(m_pContext, uiName, xPos, yPos, index2_char_length, &pos, base);
				}
				// Button Tank2
				if (strcmp(uiName, "TANK2_BUTTONS") == 0)
				{
					if(IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
					{
						if(m_pContext->btnTank2_toggle) 
						{
							m_pContext->btnTank2_toggle = false;
							m_pContext->btnTank2_rgb = Vector3(0.0f, 0.0f, 0.0f);
						}
						else 
						{
							m_pContext->btnTank2_toggle = true;
							m_pContext->btnTank2_rgb = Vector3(1.0f, 0.0f, 0.5f);
						}
					}
				}
				if(m_pContext->btnTank2_toggle)
				{
					Matrix4x4& base = CharacterControl::Components::ClientGameObjectManagerAddon::tankSNarray[1]->m_base;
					Vector3 pos = base.getPos();
					OnClick_TankButtons(m_pContext, uiName, xPos, yPos, index2_char_length, &pos, base);
				}
				// Button Tank3
				if (strcmp(uiName, "TANK3_BUTTONS") == 0)
				{
					if(IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
					{
						if(m_pContext->btnTank3_toggle) {
							m_pContext->btnTank3_toggle = false;
							m_pContext->btnTank3_rgb = Vector3(0.0f, 0.0f, 0.0f);
						}
						else {
							m_pContext->btnTank3_toggle = true;
							m_pContext->btnTank3_rgb = Vector3(1.0f, 0.0f, 0.5f);
						}
					}
				}
				if(m_pContext->btnTank3_toggle)
				{
					Matrix4x4& base = CharacterControl::Components::ClientGameObjectManagerAddon::tankSNarray[2]->m_base;
					Vector3 pos = base.getPos();
					OnClick_TankButtons(m_pContext, uiName, xPos, yPos, index2_char_length, &pos, base);
				}

			}
			// if there is a SOLDIER in the scene
			if(CharacterControl::Components::SoldierNPCMovementSM::m_soldier_movement_sm != nullptr)
			{
				if (strcmp(uiName, "SOLDIER1_BUTTONS") == 0)
				{
					if(IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
					{
						if (m_pContext->btnSol1_toggle)
						{
							m_pContext->btnSol1_toggle = false;
							m_pContext->btnSol1_rgb = Vector3(0.0f, 0.0f, 0.0f);
						}
						else
						{
							m_pContext->btnSol1_toggle = true;
							m_pContext->btnSol1_rgb = Vector3(1.0f, 0.0f, 0.5f);
						}
					}
				}
				// if Soldier toggle in ON
				if(m_pContext->btnSol1_toggle)
				{
					Matrix4x4& base = CharacterControl::Components::SoldierNPCMovementSM::m_soldier_movement_sm->getParentsSceneNode()->m_base;
					Vector3 pos = base.getPos();
					OnClick_SoldierButtons(m_pContext, uiName, xPos, yPos, index2_char_length, &pos, base);
				}
			}
			// if Cam toggle is ON
			if(m_pContext->btnCam_toggle)
			{
				OnClick_CameraButtons(m_pContext, uiName, xPos, yPos, index2_char_length);
			}
			// Button Text Colors
			if (strcmp(uiName, "TEXT1_RED") == 0)
				OnClick_TextColorButtons(m_pContext, xPos, yPos, index2_char_length, Vector3(1.0f, 0.0f, 0.0f));
			else if (strcmp(uiName, "TEXT1_GREEN") == 0)
				OnClick_TextColorButtons(m_pContext, xPos, yPos, index2_char_length, Vector3(0.0f, 1.0f, 0.0f));
			else if (strcmp(uiName, "TEXT1_BLUE") == 0)
				OnClick_TextColorButtons(m_pContext, xPos, yPos, index2_char_length, Vector3(0.0f, 0.0f, 1.0f));

			// Button Toggle Debug
			else if(strcmp(uiName, "TOGGLE_DEBUG_INFO") == 0)
			{
				if(IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
				{
					if(m_pContext->toggleDebugInfo)
						m_pContext->toggleDebugInfo = false;
					else
						m_pContext->toggleDebugInfo = true;
				} 
			}
			// Button Toggle Camera
			else if(strcmp(uiName, "CAMERA_BUTTONS") == 0)
			{
				if(IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
				{
					if (m_pContext->btnCam_toggle)
					{
						m_pContext->btnCam_toggle = false;
						m_pContext->btnCam_rgb = Vector3(0.0f, 0.0f, 0.0f);
					}
					else 
					{
						m_pContext->btnCam_toggle = true;
						m_pContext->btnCam_rgb = Vector3(1.0f, 0.0f, 1.0f);
					}
				}
			}
			else if(strcmp(uiName, "LIGHT_BUTTONS") == 0)
			{
				if(IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
				{
					if(m_pContext->btnLight_toggle)
					{
						m_pContext->btnLight_toggle = false;
						m_pContext->btnLight_rgb = Vector3(0.0f, 0.0f, 0.0f);
					}
					else
					{
						m_pContext->btnLight_toggle = true;
						m_pContext->btnLight_rgb = Vector3(1.0f, 0.0f, 0.5f);
					}
				} 
			}
			// if Light toggle is ON
			if(m_pContext->btnLight_toggle)
			{
				if(strcmp(uiName, "SPOT_LIGHT") == 0 && spotlightClickedOnce == false)
				{
					if(IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
					{
						m_pContext->getGameObjectManager()->button_CREATE_LIGHT(2);
						spotlightClickedOnce = true;
					}
				}
				else if(strcmp(uiName, "DIR_LIGHT") == 0 && dirlightClickedOnce == false)
				{
					if(IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
					{
						m_pContext->getGameObjectManager()->button_CREATE_LIGHT(1);
						dirlightClickedOnce = true;
					}
				}
				else if(strcmp(uiName, "POINT_LIGHT") == 0 && pointlightClickedOnce == false)
				{
					if(IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
					{
						m_pContext->getGameObjectManager()->button_CREATE_LIGHT(0);
						pointlightClickedOnce = true;
					}
				}
			}
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
		Event_KEY_A_HELD *keyEvent = (Event_KEY_A_HELD*)pEvt;
		float move = 1.0f;
		if(keyEvent->m_move != NULL)
			move = keyEvent->m_move;
		Vector3 relativeMovement(-1.0f * move,0.0f,0.0f);

		flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;

		m_pQueueManager->add(h, QT_GENERAL);
	}

	else if (Event_KEY_S_HELD::GetClassId() == pEvt->getClassId())
	{
		Handle h("EVENT", sizeof(Event_FLY_CAMERA));
		Event_FLY_CAMERA *flyCameraEvt = new(h) Event_FLY_CAMERA ;
		Event_KEY_S_HELD *keyEvent = (Event_KEY_S_HELD*)pEvt;
		float move = 1.0f;
		if(keyEvent->m_move != NULL)
			move = keyEvent->m_move;
		Vector3 relativeMovement(0.0f,0.0f, -1.0f * move);

		flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;

		m_pQueueManager->add(h, QT_GENERAL);

	}

	else if (Event_KEY_D_HELD::GetClassId() == pEvt->getClassId())

	{
		Handle h("EVENT", sizeof(Event_FLY_CAMERA));
		Event_KEY_D_HELD *keyEvent = (Event_KEY_D_HELD*)pEvt;
		Event_FLY_CAMERA *flyCameraEvt = new(h) Event_FLY_CAMERA ;
		float move = 1.0f;
		if(keyEvent->m_move != NULL)
			move = keyEvent->m_move;
		Vector3 relativeMovement(move,0.0f,0.0f);
		flyCameraEvt->m_relativeMove = relativeMovement * Debug_Fly_Speed * m_frameTime;
		m_pQueueManager->add(h, QT_GENERAL);
	}

	else if (Event_KEY_W_HELD::GetClassId() == pEvt->getClassId())
	{
		Handle h("EVENT", sizeof(Event_FLY_CAMERA));
		Event_KEY_W_HELD *keyEvent = (Event_KEY_W_HELD*)pEvt;
		Event_FLY_CAMERA *flyCameraEvt = new(h) Event_FLY_CAMERA ;
		float move = 1.0f;
		if(keyEvent->m_move != NULL)
			move = keyEvent->m_move;
		Vector3 relativeMovement(0.0f,0.0f,move);

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

void DefaultGameControls::OnClick_TankButtons(PE::GameContext* m_pContext, const char* uiKey, float xPos, float yPos, float index2_char_length, Vector3* pos, Matrix4x4& base)
{
	// Check if the cursor is within the button bounds
	if(!IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
		return;

	Vector3 newPos = *pos; // Copy the current position
	// Update the position based on the button pressed
	if (strcmp(uiKey, "TANK1_DOWN") == 0)
		pos->m_y -= 0.001f;
	else if (strcmp(uiKey, "TANK1_UP") == 0)
		pos->m_y += 0.001f;
	else if (strcmp(uiKey, "TANK1_LEFT") == 0)
		pos->m_x -= 0.001f;
	else if (strcmp(uiKey, "TANK1_RIGHT") == 0)
		pos->m_x += 0.001f;

	// Set the new position
	base.setPos(*pos);
}
void DefaultGameControls::OnClick_TextColorButtons(PE::GameContext* m_pContext, float xPos, float yPos, float index2_char_length, Vector3 color)
{
	// Check if the cursor is within the button bounds
	if (!IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
		return;

	// Set the color
	m_pContext->text_rgb_1 = color;
}
void DefaultGameControls::OnClick_SoldierButtons(PE::GameContext* m_pContext, const char* uiKey, float xPos, float yPos, float index2_char_length, Vector3* pos, Matrix4x4& base)
{
	// Check if the cursor is within the button bounds
	if (!IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
		return;
	// Perform the action based on the button pressed
	if (strcmp(uiKey, "SOLDIER1_DOWN") == 0)
		base.setPos(Vector3(pos->m_x, pos->m_y - 0.0001f, pos->m_z));
	else if (strcmp(uiKey, "SOLDIER1_UP") == 0)
		base.setPos(Vector3(pos->m_x, pos->m_y + 0.0001f, pos->m_z));
	else if (strcmp(uiKey, "SOLDIER1_LEFT") == 0)
		base.setPos(Vector3(pos->m_x - 0.0001f, pos->m_y, pos->m_z));
	else if (strcmp(uiKey, "SOLDIER1_RIGHT") == 0)
		base.setPos(Vector3(pos->m_x + 0.0001f, pos->m_y, pos->m_z));
}
void DefaultGameControls::OnClick_CameraButtons(PE::GameContext* m_pContext, const char* uiKey, float xPos, float yPos, float index2_char_length)
{
    // Check if the cursor is within the button bounds
    if (!IsCursorWithinBounds(m_pContext, xPos, yPos, index2_char_length))
        return;

    // Create and configure the event based on the button pressed
    if (strcmp(uiKey, "CAMERA_FWD") == 0)
    {
			Handle h("EVENT", sizeof(Event_KEY_W_HELD));
			Event_KEY_W_HELD *event = new (h) Event_KEY_W_HELD;
			event->m_move = 0.01f;
			m_pContext->m_button = 1; 
			m_pQueueManager->add(h, Events::QT_INPUT);
    }
    else if (strcmp(uiKey, "CAMERA_LEFT") == 0)
    {
			Handle h("EVENT", sizeof(Event_KEY_A_HELD));
			Event_KEY_A_HELD *event = new (h) Event_KEY_A_HELD;
			event->m_move = 0.01f; 
			m_pContext->m_button = 2; 
			m_pQueueManager->add(h, Events::QT_INPUT);
    }
    else if (strcmp(uiKey, "CAMERA_BACK") == 0)
    {
			Handle h("EVENT", sizeof(Event_KEY_S_HELD));
			Event_KEY_S_HELD *event = new (h) Event_KEY_S_HELD;
			event->m_move = 0.01f; 
			m_pContext->m_button = 3; 
			m_pQueueManager->add(h, Events::QT_INPUT);
    }
    else if (strcmp(uiKey, "CAMERA_RIGHT") == 0)
    {
		  Handle h("EVENT", sizeof(Event_KEY_D_HELD));
			Event_KEY_D_HELD *event = new (h) Event_KEY_D_HELD;
			event->m_move = 0.01f;
			m_pContext->m_button = 4; 
			m_pQueueManager->add(h, Events::QT_INPUT);
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

