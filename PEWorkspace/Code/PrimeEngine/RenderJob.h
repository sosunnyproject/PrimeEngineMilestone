#ifndef __Render_Job_H__
#define __Render_Job_H__

#include "PrimeEngine/Render/IRenderer.h"

#include "PrimeEngineIncludes.h"

namespace PE {

	void drawThreadFunctionJob(void *params);

	void runDrawThreadSingleFrame(PE::GameContext &ctx);

	void runDrawThreadSingleFrameThreaded(PE::GameContext &ctx);

    static float t1[4] = {0.5f, 0.5f, 0.5f, 1.0f};
	static float t2[4] = {0.5f, 0.5f, 0.8f, 1.0f};
	static float t3[4] = {0.5f, 0.8f, 0.5f, 1.0f};
}; // namespace PE

#endif