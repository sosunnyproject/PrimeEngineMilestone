#ifndef __PYENGINE_2_0_TEXTSCENENODE_H__
#define __PYENGINE_2_0_TEXTSCENENODE_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "../Utils/Array/Array.h"
#include "SceneNode.h"
#include <cstring>
//#define USE_DRAW_COMPONENT

namespace PE {
namespace Components {
struct TextSceneNode : public SceneNode
{
	PE_DECLARE_CLASS(TextSceneNode);

	// Constructor -------------------------------------------------------------
	TextSceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);

	virtual ~TextSceneNode() {}

	void setSelfAndMeshAssetEnabled(bool enabled);

	// Component ------------------------------------------------------------

	virtual void addDefaultComponents();
	
	// Individual events -------------------------------------------------------

	PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_GATHER_DRAWCALLS);
	virtual void do_PRE_GATHER_DRAWCALLS(Events::Event *pEvt);

	enum DrawType
	{
		InWorld,
		InWorldFacingCamera,
		Overlay2D,
		Overlay2D_3DPos
	};
	void loadFromString_needsRC(const char *str, DrawType drawType, int &threadOwnershipMask);

	DrawType m_drawType;
	float m_scale;
	Handle m_hMyTextMesh;
	Handle m_hMyTextMeshInstance;
	float m_cachedAspectRatio;

	bool m_canBeRecreated;
	Vector3 m_rgb;
	Vector2 g_pos2D;
	char m_str[256];
	char m_uiName[256];
	int m_strLen;
	
}; // class TextSceneNode

}; // namespace Components
}; // namespace PE
#endif
