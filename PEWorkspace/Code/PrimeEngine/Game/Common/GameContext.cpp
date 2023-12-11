
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
      btnTank1_rgb = Vector3(0.0f, 0.0f, 0.0f);
      btnTank2_rgb = Vector3(0.0f, 0.0f, 0.0f);
      btnTank3_rgb = Vector3(0.0f, 0.0f, 0.0f);
      btnCam_rgb = Vector3(0.0f, 0.0f, 0.0f);
      btnLight_rgb = Vector3(0.0f, 0.0f, 0.0f);
      btnSol1_rgb = Vector3(0.0f, 0.0f, 0.0f);
      g_cursorPos = {0, 0}; // Initialize POINT structure
      toggleDebugInfo = false;
      btnTank1_toggle = false;
      btnTank2_toggle = false;
      btnTank3_toggle = false;
      btnSol1_toggle = false;
      btnCam_toggle = false;
      btnLight_toggle = false;
    }
}; // namespace PE
