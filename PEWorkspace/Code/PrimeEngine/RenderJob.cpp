#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"
#include "RenderJob.h"
#include "PrimeEngine/Scene/DrawList.h"

#if APIABSTRACTION_IOS
#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>
#endif

extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#include "PrimeEngine/Game/Client/ClientGame.h"
#include "PrimeEngine/Events/StandardEvents.h" // Include the file that defines the Event_CREATE_LIGHT event

// #include "CharacterControl/Tank/ClientTank.h"
// #include "CharacterControl/ClientGameObjectManagerAddon.h"
#include "CharacterControl/Characters/SoldierNPC.h"
#include "CharacterControl/Characters/SoldierNPCMovementSM.h"
// keyboard mouse
#include "PrimeEngine/Events/StandardKeyboardEvents.h"
#include "PrimeEngine/APIAbstraction/DirectX9/DX9_KeyboardMouse/DX9_KeyboardMouse.h"
// Add Imgui library
#include "PrimeEngine/Import/imgui/imgui.h"
#include "PrimeEngine/Import/imgui/backend/imgui_impl_dx9.h"
#include "PrimeEngine/Import/imgui/backend/imgui_impl_win32.h"
namespace PE {

using namespace Events;
using namespace Components;

void drawThreadFunctionJob(void *params)
{
	GameContext *pContext = static_cast<PE::GameContext *>(params);

	g_drawThreadInitializationLock.lock();
	//initialization here..
	g_drawThreadInitialized = true;
	g_drawThreadExited = false;
	g_drawThreadInitializationLock.unlock();

	//acquire rendring thread lock so that we can sleep on it until game thread wakes us up
	g_drawThreadLock.lock();

	// now we can signal main thread that this thread is initialized
	g_drawThreadInitializedCV.signal();
    while (1)
    {
		runDrawThreadSingleFrameThreaded(*pContext);
		if (g_drawThreadExited)
			return;
    }
    return;
}

void runDrawThreadSingleFrameThreaded(PE::GameContext &ctx)
{
	while (!g_drawThreadCanStart && !g_drawThreadShouldExit)
    {
		bool success = g_drawCanStartCV.sleep();
        assert(success);
    }
	g_drawThreadCanStart = false; // set to false for next frame

	//PEINFO("Draw thread got g_drawThreadLock\n");

	if (g_drawThreadShouldExit)
	{
		//right now game thread is waiting on this thread to finish
		g_drawThreadLock.unlock();
		g_drawThreadExited = true;
		return;
	}

	runDrawThreadSingleFrame(ctx);
}

void runDrawThreadSingleFrame(PE::GameContext &ctx)
{
	int threadOwnershipMask = 0;
	
	ctx.getGPUScreen()->AcquireRenderContextOwnership(threadOwnershipMask);

	#if PE_ENABLE_GPU_PROFILING
	Timer t;
	PE::Profiling::Profiler::Instance()->startEventQuery(Profiling::Group_DrawThread, IRenderer::Instance()->getDevice(), t.GetTime(), "DrawThread");
	#endif

	#if APIABSTRACTION_D3D9
		//IRenderer::Instance()->getDevice()->SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	#elif APIABSTRACTION_D3D11
		D3D11Renderer *pD3D11Renderer = static_cast<D3D11Renderer *>(ctx.getGPUScreen());
		ID3D11Device *pDevice = pD3D11Renderer->m_pD3DDevice;
		ID3D11DeviceContext *pDeviceContext = pD3D11Renderer->m_pD3DContext;

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	#endif

	bool renderShadowMap = false;
	#if !APIABSTRACTION_IOS && !APIABSTRACTION_PS3 && !PE_PLAT_IS_PSVITA /* && !PE_API_IS_D3D11*/
		renderShadowMap = true;
	#endif
	if (renderShadowMap)
	{
		// for shadow mapping:
		EffectManager::Instance()->setShadowMapRenderTarget();

		DrawList::ZOnlyInstanceReadOnly()->optimize();

		ctx.getGPUScreen()->ReleaseRenderContextOwnership(threadOwnershipMask);

		// the render context is acquired and release inside of this function
		DrawList::ZOnlyInstanceReadOnly()->do_RENDER_Z_ONLY(NULL, threadOwnershipMask);

		ctx.getGPUScreen()->AcquireRenderContextOwnership(threadOwnershipMask);

		EffectManager::Instance()->endCurrentRenderTarget();
	}

	IRenderer::checkForErrors("renderjob update start\n");

	IRenderer::RenderMode renderMode = ctx.getGPUScreen()->m_renderMode;

	// test Camera Instance
	CameraSceneNode *pcam = CameraManager::Instance()->getActiveCamera()->getCamSceneNode();
	Matrix4x4 pcam_world = pcam->m_worldTransform;
	Vector3 pcam_pos = pcam_world.getPos();

	// Render Imgui window
	bool show_demo_window = true;

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	// draw your imgui window
	ImGui::Begin("Prime Engine", &show_demo_window);
	ImGui::SeparatorText("Camera Movement");
    if (ImGui::Button("FORWARD (W)"))  // W
		ctx.imgui_wasd = 1;
	else if (ImGui::Button("LEFT (A)")) // A
		ctx.imgui_wasd = 2;
	
    else if (ImGui::Button("BACK (S)"))	// S
		ctx.imgui_wasd = 3;
	
    else if (ImGui::Button("RIGHT (D)"))	// D
		ctx.imgui_wasd = 4;
	// else 
		// ctx.imgui_wasd = 0;
	if (ImGui::Button("STOP"))	
		ctx.imgui_wasd = 0;

	ImGui::SeparatorText("Camera Position");
	ImGui::Text("X: %f", pcam_pos.m_x);
	ImGui::SameLine(); ImGui::Text(" Y: %f", pcam_pos.m_y);
	ImGui::SameLine(); ImGui::Text(" Z: %f", pcam_pos.m_z);
	// PEINFO("CAMERA - WORLD POS: %f %f %f\n", pcam_pos.getX(), pcam_pos.getY(), pcam_pos.getZ());

	ImGui::SeparatorText("Mouse Movement");
	ImGuiIO& io = ImGui::GetIO();
	// Display inputs submitted to ImGuiIO
	if (ImGui::IsMousePosValid())
		ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
	else
		ImGui::Text("Mouse pos: <INVALID>");
	ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
	ImGui::Text("Mouse down:");
	if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        ImGui::SameLine(); ImGui::Text("True");
	}
	else {
		ImGui::SameLine(); ImGui::Text("False");
	}

	ImGui::SeparatorText("Text Color: RGB");
	if(ImGui::ColorEdit4("Important Text", (float*)&t1, ImGuiColorEditFlags_Float | 0)){
		ctx.text_rgb_1 = Vector3(t1[0], t1[1], t1[2]);
	}
	if(ImGui::ColorEdit4("Interactive Text", (float*)&t2, ImGuiColorEditFlags_Float | 0)){
		ctx.text_rgb_2 = Vector3(t2[0], t2[1], t2[2]);
	}
	if(ImGui::ColorEdit4("General Text", (float*)&t3, ImGuiColorEditFlags_Float | 0)){
		ctx.text_rgb_3 = Vector3(t3[0], t3[1], t3[2]);
	}
	
	ImGui::SeparatorText("Soldier Position");

	if (CharacterControl::Components::SoldierNPCMovementSM::m_soldier_movement_sm != nullptr) {
		Matrix4x4& base = CharacterControl::Components::SoldierNPCMovementSM::m_soldier_movement_sm->getParentsSceneNode()->m_base;
		Vector3 pos = base.getPos();
		if(	ImGui::InputFloat3("Soldier Position XYZ", (float*)&soldier_posf)) {
			base.setPos(Vector3(soldier_posf[0], soldier_posf[1], soldier_posf[2]));
		}
	}

	/*
	if(ImGui::SliderFloat3("Important Text", t1, 0.0f, 1.0f)){
		ctx.text_rgb_1 = Vector3(t1[0], t1[1], t1[2]);
	}
	*/

	// if(ImGui::Button("Create Light")){
	// 	// Create a light
	// 	Handle h("EVENT", sizeof(Event_CREATE_LIGHT));
	// 	Event_CREATE_LIGHT *pEvent = new(h) Event_CREATE_LIGHT();
	// 	Events::EventQueueManager::Instance()->add(pEvent, Events::QT_GENERAL);
	// }
	// for (CharacterControl::Components::TankController* tankController : CharacterControl::Components::ClientGameObjectManagerAddon::tanks) {
    // 	PEINFO("////RENDERJOB::: TankController: %f\n", tankController->m_spawnPos.m_x);
	// 	// Do something with tankController
	// }

	ImGui::End();

	ImGui::EndFrame();

	bool disableScreenSpaceEffects = renderMode == IRenderer::RenderMode_DefaultNoPostProcess;
	if (!disableScreenSpaceEffects)
    {
		// set render target: GlowTargetTextureGPU
        EffectManager::Instance()->setTextureAndDepthTextureRenderTargetForGlow();
         
        assert(DrawList::InstanceReadOnly() != DrawList::Instance());
        DrawList::InstanceReadOnly()->optimize();
                
		// set global shader value (applied to all draw calls) for shadow map texture
		if (renderShadowMap)
			EffectManager::Instance()->createSetShadowMapShaderValue(DrawList::InstanceReadOnly());

		ctx.getGPUScreen()->ReleaseRenderContextOwnership(threadOwnershipMask);

		// all mesh - draw calls
		DrawList::InstanceReadOnly()->do_RENDER(NULL, threadOwnershipMask);
		ctx.getGPUScreen()->AcquireRenderContextOwnership(threadOwnershipMask);

		EffectManager::Instance()->endCurrentRenderTarget();

		#if APIABSTRACTION_D3D9
			//IRenderer::Instance()->getDevice()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		#elif APIABSTRACTION_D3D11
			pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		#endif

		// sets render target to separated glow texture
		EffectManager::Instance()->drawGlowSeparationPass();
		EffectManager::Instance()->endCurrentRenderTarget();

        // First glow path into another texture with horizontal glow
                
        // Draw Effects
        // horizontal glow into 2nd glow target
        EffectManager::Instance()->drawFirstGlowPass();
		EffectManager::Instance()->endCurrentRenderTarget();

        // from second glow target to FinishedGlowTargetTexture

		EffectManager::Instance()->drawSecondGlowPass();
		EffectManager::Instance()->endCurrentRenderTarget();

		bool drawMotionBlur = renderMode == IRenderer::RenderMode_DefaultGlow;
		if (drawMotionBlur)
		{
			//draw back into main back buffer render target
			EffectManager::Instance()->drawMotionBlur();
			
			// Render IMGUI
			ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			
			EffectManager::Instance()->endCurrentRenderTarget();
		}
		else
		{
			bool debugGlowRenderTarget = renderMode == IRenderer::RenderMode_DebugGlowRT;
			bool drawSeparatedGlow = renderMode == IRenderer::RenderMode_DebugSeparatedGlow;
			bool drawGlow1stPass = renderMode == IRenderer::RenderMode_DebugGlowHorizontalBlur;
			bool drawGlow2ndPass = renderMode == IRenderer::RenderMode_DebugGlowVerticalBlurCombine;
			bool drawShadowRenderTarget = renderMode == IRenderer::RenderMode_DebugShadowRT;

			EffectManager::Instance()->debugDrawRenderTarget(debugGlowRenderTarget, drawSeparatedGlow, drawGlow1stPass, drawGlow2ndPass, drawShadowRenderTarget);
			// Render IMGUI
			ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

			EffectManager::Instance()->endCurrentRenderTarget();
		}
    }
    else
    {
        // use simple rendering
        // the render target here is the same as end result of motion blur
        EffectManager::Instance()->setTextureAndDepthTextureRenderTargetForDefaultRendering();
                
        assert(DrawList::InstanceReadOnly() != DrawList::Instance());
                
		DrawList::InstanceReadOnly()->optimize();
		
		#if PE_PLAT_IS_PSVITA
			EffectManager::Instance()->drawFullScreenQuad();
		#endif
		
		// set global shader value (applied to all draw calls) for shadow map texture
		if (renderShadowMap)
			EffectManager::Instance()->createSetShadowMapShaderValue(DrawList::InstanceReadOnly());

		ctx.getGPUScreen()->ReleaseRenderContextOwnership(threadOwnershipMask);

        DrawList::InstanceReadOnly()->do_RENDER(NULL, threadOwnershipMask);

		ctx.getGPUScreen()->AcquireRenderContextOwnership(threadOwnershipMask);

		// Render IMGUI
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        // Required when rendering to backbuffer directly. also done in drawMotionBlur_EndScene() since it is the last step of post process
		EffectManager::Instance()->endCurrentRenderTarget();
    }
            
	ctx.getGPUScreen()->endFrame();

    // Flip screen
	ctx.getGPUScreen()->swap(false);
    PE::IRenderer::checkForErrors("");

			
	#if PE_ENABLE_GPU_PROFILING
		Timer::TimeType time = t.TickAndGetCurrentTime();
		// only perform flush when gpu profiling is enabled, swap() above will call present which should flush when PE_DETAILED_GPU_PROFILING = 0 == vsync enabled.
		Profiling::Profiler::Instance()->update(Profiling::Group_DrawCalls, PE_DETAILED_GPU_PROFILING, true, time, t);
		Profiling::Profiler::Instance()->update(Profiling::Group_DrawThread, PE_DETAILED_GPU_PROFILING, true, time, t);
	#endif

	ctx.getGPUScreen()->ReleaseRenderContextOwnership(threadOwnershipMask);
}


}; // namespace PE

