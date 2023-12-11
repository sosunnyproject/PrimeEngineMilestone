// APIAbstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "../Lua/LuaEnvironment.h"

// Sibling/Children includes
#include "GameObjectManager.h"
#include "../Sound/SoundManager.h"

#include "PrimeEngine/Scene/Skeleton.h"
#include "PrimeEngine/Scene/MeshInstance.h"
#include "PrimeEngine/Scene/SkeletonInstance.h"
#include <random>
#include "PrimeEngine/Scene/MeshManager.h"
#include "CharacterControl/ClientGameObjectManagerAddon.h"
#include "CharacterControl/CharacterControlContext.h"

namespace PE {
namespace Components {

using namespace PE::Events;

PE_IMPLEMENT_CLASS1(GameObjectManager, Component);

// Singleton ------------------------------------------------------------------

// Constructor -------------------------------------------------------------
GameObjectManager::GameObjectManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself), m_luaGameObjectTableRef(LUA_NOREF)
, Networkable(context, this, Networkable::s_NetworkId_GameObjectManager) // pre-assigned network id
{
}

	// Methods      ------------------------------------------------------------
void GameObjectManager::addDefaultComponents()
{
	Component::addDefaultComponents();

	addComponent(m_pContext->getLuaEnvironment()->getHandle());

	PE_REGISTER_EVENT_HANDLER(Event_SET_DEBUG_TARGET_HANDLE, GameObjectManager::do_SET_DEBUG_TARGET_HANDLE);
	PE_REGISTER_EVENT_HANDLER(Event_CONSTRUCT_SOUND, GameObjectManager::do_CONSTRUCT_SOUND);

	PE_REGISTER_EVENT_HANDLER(Event_CREATE_LIGHT, GameObjectManager::do_CREATE_LIGHT);
	PE_REGISTER_EVENT_HANDLER(Event_CREATE_MESH, GameObjectManager::do_CREATE_MESH);
	PE_REGISTER_EVENT_HANDLER(Event_CREATE_SKELETON, GameObjectManager::do_CREATE_SKELETON);
	PE_REGISTER_EVENT_HANDLER(Event_CREATE_ANIM_SET, GameObjectManager::do_CREATE_ANIM_SET);

	createGameObjectTableIfDoesntExist();
}

// Individual events -------------------------------------------------------
void GameObjectManager::do_SET_DEBUG_TARGET_HANDLE(Events::Event *pEvt)
{
	Event_SET_DEBUG_TARGET_HANDLE *pRealEvt = (Event_SET_DEBUG_TARGET_HANDLE *)(pEvt);

	Component::s_debuggedComponent = pRealEvt->m_hDebugTarget;
	Component::s_debuggedEvent = pRealEvt->m_debugEvent;
}

void GameObjectManager::do_CREATE_LIGHT(Events::Event *pEvt)
{
	Event_CREATE_LIGHT *pRealEvt = (Event_CREATE_LIGHT *)(pEvt);

	bool haveObject = false;
	Handle exisitngObject;

	// this goes into lua
	if(pRealEvt->m_sentByLua)
		putGameObjectTableIOnStack();

	if (!pRealEvt->m_peuuid.isZero() && pRealEvt->m_sentByLua)
	{
		PEINFO("m_peuuid: %d", pRealEvt->m_peuuid);
		// have a valid peeuid for the object. need to check if have one already

		haveObject = m_pContext->getLuaEnvironment()->checkTableValueByPEUUIDFieldExists(pRealEvt->m_peuuid);
		if (haveObject)
		{
			LuaEnvironment::popHandleFromTableOnStackAndPopTable(m_pContext->getLuaEnvironment()->L, exisitngObject);
			m_lastAddedObjHandle = exisitngObject;
		}
		else
		{
			// pop nil
			if(pRealEvt->m_sentByLua)
				m_pContext->getLuaEnvironment()->pop();
		}
	}

	// This gets called after int Event_CREATE_LIGHT::l_Construct(lua_State* luaVM)
	if (!haveObject)
	{
		Handle hLight("LIGHT", sizeof(Light));

		Light *pLight = new(hLight) Light(
			*m_pContext,
			m_arena,
			hLight,
			pRealEvt->m_pos, //Position
			pRealEvt->m_u, 
			pRealEvt->m_v, 
			pRealEvt->m_n, //Direction (z-axis)
			pRealEvt->m_ambient, //Ambient
			pRealEvt->m_diffuse, //Diffuse
			pRealEvt->m_spec, //Specular
			pRealEvt->m_att, //Attenuation (x, y, z)
			pRealEvt->m_spotPower, // Spot Power
			pRealEvt->m_range, //Range
			pRealEvt->m_isShadowCaster, //Whether or not it casts shadows
			(PrimitiveTypes::Int32)(pRealEvt->m_type) //0 = point, 1 = directional, 2 = spot
		);
		if(pRealEvt->m_sentByLua)
			pLight->addDefaultComponents();
		RootSceneNode::Instance()->m_lights.add(hLight);
		RootSceneNode::Instance()->addComponent(hLight);

		if(pRealEvt->m_sentByLua)
			m_pContext->getLuaEnvironment()->pushHandleAsFieldAndSet(pRealEvt->m_peuuid, hLight);
			
		m_lastAddedObjHandle = hLight;
	}
	else
	{
		// already have this object
		
		// need to reset the orientation
		// and light source settings
		Light *pLight = exisitngObject.getObject<Light>();
		
		pLight->m_base.setPos(pRealEvt->m_pos);
		pLight->m_base.setU(pRealEvt->m_u);
		pLight->m_base.setV(pRealEvt->m_v);
		pLight->m_base.setN(pRealEvt->m_n);
		

		pLight->m_cbuffer.pos = pLight->m_base.getPos();
		pLight->m_cbuffer.dir = pLight->m_base.getN();

		pLight->m_cbuffer.ambient = pRealEvt->m_ambient;
		pLight->m_cbuffer.diffuse = pRealEvt->m_diffuse;
		pLight->m_cbuffer.spec = pRealEvt->m_spec;
		pLight->m_cbuffer.att = pRealEvt->m_att;
		pLight->m_cbuffer.spotPower = pRealEvt->m_spotPower;
		pLight->m_cbuffer.range = pRealEvt->m_range;
		pLight->isTheShadowCaster = pRealEvt->m_isShadowCaster;
	}

	// pop the game object table
	if (pRealEvt->m_sentByLua)
		m_pContext->getLuaEnvironment()->pop();
}

// bypass all LUA stuff
void GameObjectManager::button_CREATE_LIGHT(float light_type)
{
		Handle *h = new Handle("EVENT", sizeof(Event_CREATE_LIGHT));
		Event_CREATE_LIGHT *pEvt = new(*h) Event_CREATE_LIGHT();
		pEvt->m_pos = Vector3( 9.62f, 2.9f, -0.9f);
		pEvt->m_u = Vector3(1.0f, 0.0f, 0.0f);
		pEvt->m_v = Vector3(0.0f, 0.0f, 0.9f);
		pEvt->m_n = Vector3(0.0f, -0.99f, 0.6f);

		pEvt->m_type = light_type;
		//0 = point, 1 = directional, 2 = spot
		if(light_type == 0) {
			pEvt->m_diffuse = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
			pEvt->m_ambient = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else if(light_type == 1) {
			pEvt->m_diffuse = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
			pEvt->m_ambient = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		else if(light_type == 2) {
			pEvt->m_diffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			pEvt->m_ambient = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
		}

		pEvt->m_spec = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
		pEvt->m_att = Vector3(0.03f, 0.05f, 0.03f);
		pEvt->m_spotPower = 1.0f;
		pEvt->m_range = 50.0f;
		pEvt->m_isShadowCaster = 0;
		
		std::random_device rd;  // obtain a random number from hardware
		std::mt19937 eng(rd()); // seed the generator
		std::uniform_int_distribution<> distr(100000001, 999999999); // define the range

		// trying to bypass LUA peuuid; random number, non-zero, not the same ones
		PEUUID myUUID;
		PrimitiveTypes::UInt32 myValue = 0; // distr(eng);
		PrimitiveTypes::UInt32 v1 = 0; // distr(eng);
		PrimitiveTypes::UInt32 v2 = 0; // distr(eng);
		PrimitiveTypes::UInt32 v3 = 0; //distr(eng);
		myUUID.set(myValue, v1, v2, v3);
		pEvt->m_peuuid = myUUID;
		pEvt->m_sentByLua = false;	// to escape any Lua related functions and errors
		PEINFO("pEvt mpeuuid: %d", pEvt->m_peuuid);
		// memorypool.h error
		// Events::EventQueueManager::Instance()->add(pEvt, Events::QT_GENERAL);
		// adding the event to the Queue creates allocate/block memory error
		// let's just call create-light function directly
		// and manually release the event handle.
		do_CREATE_LIGHT(pEvt);
		h->release();
}
//error
void GameObjectManager::button_CREATE_SKELETON()
{
	Handle *h = new Handle("EVENT", sizeof(Event_CREATE_SKELETON));
	Event_CREATE_SKELETON *pEvt = new(*h) Event_CREATE_SKELETON(m_pContext->m_gameThreadThreadOwnershipMask);
	
	pEvt->m_sentByLua = false;
	const char* name = "soldier_Soldier_Skeleton.skela";
	const char* package = "Soldier";
	StringOps::writeToString(name, pEvt->m_skelFilename, 255);
	StringOps::writeToString(package, pEvt->m_package, 255);

	pEvt->hasCustomOrientation = true;
	pEvt->m_pos = Vector3(0.0f, 0.0f, 0.0f);
	pEvt->m_u = Vector3(1.0f, 0.0f, 0.0f);
	pEvt->m_v = Vector3(0.0f, 1.0f, 0.0f);
	pEvt->m_n = Vector3(0.0f, 0.0f, 1.0f);	

	// trying to bypass LUA peuuid; random number, non-zero, not the same ones
	std::random_device rd;  // obtain a random number from hardware
	std::mt19937 eng(rd()); // seed the generator
	std::uniform_int_distribution<> distr(100000001, 999999999); // define the range
	PEUUID myUUID;
	PrimitiveTypes::UInt32 myValue = distr(eng);
	PrimitiveTypes::UInt32 v1 = distr(eng);
	PrimitiveTypes::UInt32 v2 = distr(eng);
	PrimitiveTypes::UInt32 v3 = distr(eng);
	myUUID.set(myValue, v1, v2, v3);
	pEvt->m_peuuid = myUUID;

	do_CREATE_SKELETON(pEvt);
	h->release();
}
//error
void GameObjectManager::button_CREATE_TANKS()
{
	CharacterControl::CharacterControlContext* pGameCtx = static_cast<CharacterControl::CharacterControlContext*>(m_pContext->m_pGameSpecificContext);

	for (int i = 0; i < 6; ++i)
	{
		((CharacterControl::Components::ClientGameObjectManagerAddon*)(pGameCtx->getGameObjectManagerAddon()))->createTank(
		i, m_pContext->m_gameThreadThreadOwnershipMask);
	}
}

void GameObjectManager::do_CREATE_SKELETON(Events::Event *pEvt)
{
	Events::Event_CREATE_SKELETON *pRealEvent = (Events::Event_CREATE_SKELETON *)(pEvt);
	bool haveObject = false;
	Handle exisitngObject;

	// lua
	if(pRealEvent->m_sentByLua)
		putGameObjectTableIOnStack();

	if (!pRealEvent->m_peuuid.isZero() && pRealEvent->m_sentByLua)
	{
		// have a valid peeuid for the object. need to check if have one already

		haveObject = m_pContext->getLuaEnvironment()->checkTableValueByPEUUIDFieldExists(pRealEvent->m_peuuid);
		if (haveObject)
		{
			LuaEnvironment::popHandleFromTableOnStackAndPopTable(m_pContext->getLuaEnvironment()->L, exisitngObject);
			m_lastAddedObjHandle = exisitngObject;
		}
		else
		{
			// pop nil
			m_pContext->getLuaEnvironment()->pop();
		}
	}

	if (!haveObject)
	{
		// need to acquire redner context for this code to execute thread-safe
		m_pContext->getGPUScreen()->AcquireRenderContextOwnership(pRealEvent->m_threadOwnershipMask);

		PE::Handle hSkelInstance("SkeletonInstance", sizeof(SkeletonInstance));
		SkeletonInstance *pSkelInstance = new(hSkelInstance) SkeletonInstance(*m_pContext, m_arena, hSkelInstance, Handle());
		if (pRealEvent->m_sentByLua)
			pSkelInstance->addDefaultComponents();

		if(pRealEvent->m_sentByLua)
			pSkelInstance->initFromFiles(pRealEvent->m_skelFilename, pRealEvent->m_package, pRealEvent->m_threadOwnershipMask);
		else {
			Handle h = m_pContext->getMeshManager()->getAsset(pRealEvent->m_skelFilename, pRealEvent->m_package, pRealEvent->m_threadOwnershipMask);
			static int allowedEvts[] = {0};
			h.getObject<Component>()->addComponent(m_hMyself, &allowedEvts[0]);	
			h.release();
		}
		// pSkelInstance->initFromFiles(pRealEvent->m_skelFilename, pRealEvent->m_package, pRealEvent->m_threadOwnershipMask);

		m_pContext->getGPUScreen()->ReleaseRenderContextOwnership(pRealEvent->m_threadOwnershipMask);
		
		if (pRealEvent->hasCustomOrientation)
		{
			// need to create a scene node for this mesh
			Handle hSN("SCENE_NODE", sizeof(SceneNode));
			SceneNode *pSN = new(hSN) SceneNode(*m_pContext, m_arena, hSN);
			if(pRealEvent->m_sentByLua)
				pSN->addDefaultComponents();

			pSN->m_base.setPos(pRealEvent->m_pos);
			pSN->m_base.setU(pRealEvent->m_u);
			pSN->m_base.setV(pRealEvent->m_v);
			pSN->m_base.setN(pRealEvent->m_n);

			pSN->addComponent(hSkelInstance);

			RootSceneNode::Instance()->addComponent(hSN);
		}
		else
		{
			RootSceneNode::Instance()->addComponent(hSkelInstance);
		}
		if(pRealEvent->m_sentByLua)
			m_pContext->getLuaEnvironment()->pushHandleAsFieldAndSet(pRealEvent->m_peuuid, hSkelInstance);
	
		m_lastAddedObjHandle = hSkelInstance;
		m_lastAddedSkelInstanceHandle = hSkelInstance;
	}
	else
	{
		// already have this object
		// only care about orientation
		if (pRealEvent->hasCustomOrientation)
		{
			// need to reset the orientation
			// try finding scene node
			SkeletonInstance *pSkelInstance = exisitngObject.getObject<SkeletonInstance>();
			Handle hSN = pSkelInstance->getFirstParentByType<SceneNode>();
			if (hSN.isValid())
			{
				SceneNode *pSN = hSN.getObject<SceneNode>();
				pSN->m_base.setPos(pRealEvent->m_pos);
				pSN->m_base.setU(pRealEvent->m_u);
				pSN->m_base.setV(pRealEvent->m_v);
				pSN->m_base.setN(pRealEvent->m_n);
			}
		}
	}
	
	// pop the game object table
	if (pRealEvent->m_sentByLua)
		m_pContext->getLuaEnvironment()->pop();
}

void GameObjectManager::do_CREATE_ANIM_SET(Events::Event *pEvt)
{
	Events::Event_CREATE_ANIM_SET *pRealEvent = (Events::Event_CREATE_ANIM_SET *)(pEvt);
	bool haveObject = false;
	Handle exisitngObject;

	putGameObjectTableIOnStack();

	if (!haveObject)
	{
		// need to acquire redner context for this code to execute thread-safe
		m_pContext->getGPUScreen()->AcquireRenderContextOwnership(pRealEvent->m_threadOwnershipMask);

		PEASSERT(m_lastAddedSkelInstanceHandle.isValid(), "Adding anim set, so we need a skeleton instance");
		m_lastAddedSkelInstanceHandle.getObject<SkeletonInstance>()->setAnimSet(pRealEvent->animSetFilename, pRealEvent->m_package);
		m_pContext->getGPUScreen()->ReleaseRenderContextOwnership(pRealEvent->m_threadOwnershipMask);
	}
	// pop the game object table
	m_pContext->getLuaEnvironment()->pop();
}

void GameObjectManager::do_CREATE_MESH(Events::Event *pEvt)
{
	Events::Event_CREATE_MESH *pRealEvent = (Events::Event_CREATE_MESH *)(pEvt);
	
	bool haveObject = false;
	bool haveOtherObject = false;
	Handle exisitngObject;

	// this goes into lua
	putGameObjectTableIOnStack();

	if (!pRealEvent->m_peuuid.isZero())
	{
		// have a valid peeuid for the object. need to check if have one already
		
		haveObject = m_pContext->getLuaEnvironment()->checkTableValueByPEUUIDFieldExists(pRealEvent->m_peuuid);
		if (haveObject)
		{
			LuaEnvironment::popHandleFromTableOnStackAndPopTable(m_pContext->getLuaEnvironment()->L, exisitngObject);
		}
		else
		{
			// pop nil
			m_pContext->getLuaEnvironment()->pop();
		}

		if (haveObject)
		{
			Component *pExisiting = exisitngObject.getObject<Component>();

			if (!pExisiting->isInstanceOf<MeshInstance>())
			{
				haveObject = false; // objects can have same id if they are different types, like skeleton + mesh
				haveOtherObject = true;
			}
		}
	}

	if (!haveObject)
	{
	
		// need to acquire redner context for this code to execute thread-safe
		m_pContext->getGPUScreen()->AcquireRenderContextOwnership(pRealEvent->m_threadOwnershipMask);
		
			PE::Handle hMeshInstance("MeshInstance", sizeof(MeshInstance));
			MeshInstance *pMeshInstance = new(hMeshInstance) MeshInstance(*m_pContext, m_arena, hMeshInstance);
			pMeshInstance->addDefaultComponents();

			pMeshInstance->initFromFile(pRealEvent->m_meshFilename, pRealEvent->m_package, pRealEvent->m_threadOwnershipMask);
			
		
		m_pContext->getGPUScreen()->ReleaseRenderContextOwnership(pRealEvent->m_threadOwnershipMask);
		
		// we need to add this mesh to a scene node or to an existing skeleton
		if (pMeshInstance->hasSkinWeights())
		{
			// this mesh has skin weights, so it should belong to a skeleton. assume the last added skeleton is skeleton we need
			PEASSERT(m_lastAddedSkelInstanceHandle.isValid(), "Adding skinned mesh, so we need a skeleton instance");
			m_lastAddedSkelInstanceHandle.getObject<Component>()->addComponent(hMeshInstance);
		}
		else
		{
			if (pRealEvent->hasCustomOrientation)
			{
				// need to create a scene node for this mesh
				Handle hSN("SCENE_NODE", sizeof(SceneNode));
				SceneNode *pSN = new(hSN) SceneNode(*m_pContext, m_arena, hSN);
				pSN->addDefaultComponents();

				pSN->addComponent(hMeshInstance);

				RootSceneNode::Instance()->addComponent(hSN);
				pSN->m_base.setPos(pRealEvent->m_pos);
				pSN->m_base.setU(pRealEvent->m_u);
				pSN->m_base.setV(pRealEvent->m_v);
				pSN->m_base.setN(pRealEvent->m_n);
			}
			else
			{
				RootSceneNode::Instance()->addComponent(hMeshInstance);
			}
		}

		if (!haveOtherObject)
			m_pContext->getLuaEnvironment()->pushHandleAsFieldAndSet(pRealEvent->m_peuuid, hMeshInstance);
	}
	else
	{
		// already have this object
		// only care about orientation
		if (pRealEvent->hasCustomOrientation)
		{
			// need to reset the orientation
			// try finding scene node
			MeshInstance *pMeshInstance = exisitngObject.getObject<MeshInstance>();
			Handle hSN = pMeshInstance->getFirstParentByType<SceneNode>();
			if (hSN.isValid())
			{
				SceneNode *pSN = hSN.getObject<SceneNode>();
				pSN->m_base.setPos(pRealEvent->m_pos);
				pSN->m_base.setU(pRealEvent->m_u);
				pSN->m_base.setV(pRealEvent->m_v);
				pSN->m_base.setN(pRealEvent->m_n);
			}
		}
	}

	// pop the game object table
	m_pContext->getLuaEnvironment()->pop();
}



void GameObjectManager::do_CONSTRUCT_SOUND(Events::Event *pEvt)
{
	Event_CONSTRUCT_SOUND *pRealEvent = (Event_CONSTRUCT_SOUND *)(pEvt);
	SoundManager::Construct(*m_pContext, m_arena, pRealEvent->m_waveBankFilename);
}

void GameObjectManager::createGameObjectTableIfDoesntExist()
{
	if (m_luaGameObjectTableRef == LUA_NOREF)
		m_luaGameObjectTableRef = LuaEnvironment::createTableOnTopOfStackAndStoreReference(m_pContext->getLuaEnvironment()->L);

}
void GameObjectManager::putGameObjectTableIOnStack()
{
	if (m_luaGameObjectTableRef == LUA_NOREF)
		createGameObjectTableIfDoesntExist();
	
	LuaEnvironment::putTableOnTopOfStackByReference(m_luaGameObjectTableRef, m_pContext->getLuaEnvironment()->L);
}

	
}; // namespace Components
}; //namespace PE
