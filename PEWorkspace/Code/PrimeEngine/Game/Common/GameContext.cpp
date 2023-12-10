
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "GameContext.h"

namespace PE
{
 GameContext::GameContext()
    {
      // Initialize and assign values to the variables
      m_button = 0;
      imgui_wasd = 0;
      text_rgb_1 = Vector3(1.0f, 0.0f, 0.0f);
      text_rgb_2 = Vector3(0.0f, 1.0f, 0.0f);
      text_rgb_3 = Vector3(0.0f, 0.0f, 1.0f);
      g_cursorPos = {0, 0}; // Initialize POINT structure
      toggleDebugInfo = false;
      btnTank1_toggle = false;
      btnSol1_toggle = false;
      btnCam_toggle = false;
    }
}; // namespace PE
