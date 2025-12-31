#include <Windows.h>
#include <d2d1_1.h>

#include "../deps/clay_renderer_gdi.c"

#define CLAY_IMPLEMENTATION
#include "../deps/clay.h"

#include "ui.h"

HFONT fonts[1];

BOOL didAllocConsole = FALSE;
BOOL ui_debug_mode = FALSE;
void HandleClayErrors(Clay_ErrorData errorData) {
  if (!didAllocConsole) {
    didAllocConsole = AllocConsole();
  }
  printf("Handle Clay Errors: %s\r\n", errorData.errorText.chars);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {

  switch (uMsg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    case WM_SIZE: // resize events
    {
      RECT r = {0};
      if (GetClientRect(hwnd, &r)) {
        Clay_Dimensions dim = (Clay_Dimensions){.height = r.bottom - r.top,
                                                .width = r.right - r.left};
        Clay_SetLayoutDimensions(dim);
      }
      InvalidateRect(hwnd, NULL, false); // force a wm_paint event
      break;
    }
    
    case WM_KEYDOWN:
      if (VK_ESCAPE == wParam) {
        DestroyWindow(hwnd);
        break;
      }

      if (wParam == VK_F12) {
        Clay_SetDebugModeEnabled(ui_debug_mode = !ui_debug_mode);
       
      }

      InvalidateRect(hwnd, NULL, false); // force a wm_paint event
      break;
      
    case WM_PAINT:
      Clay_RenderCommandArray renderCommands = CreateBaseLayout();
      Clay_Win32_Render(hwnd, renderCommands, fonts);
      break;

    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

  return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR lpCmdLine, int nCmdShow) {

  uint64_t clayRequiredMemory = Clay_MinMemorySize();
  Clay_Arena clayMemory = {
      .nextAllocation = NULL,
      .capacity = clayRequiredMemory,
      .memory = (char *)malloc(clayRequiredMemory),
  };

  Clay_Dimensions clayDimensions = {.width = 800, .height = 600};
  Clay_ErrorHandler errHandler = {.errorHandlerFunction = HandleClayErrors, .userData = NULL};
  Clay_Initialize(clayMemory, clayDimensions, errHandler);

  Clay_Win32_SetRendererFlags(CLAYGDI_RF_ALPHABLEND | CLAYGDI_RF_SMOOTHCORNERS);

  fonts[0] = Clay_Win32_SimpleCreateFont("../resources/Roboto-Regular.ttf", "Roboto", -11, FW_NORMAL);
  Clay_SetMeasureTextFunction(Clay_Win32_MeasureText, fonts);

  WNDCLASSEX windowClass;
  ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

  windowClass.cbSize = sizeof(WNDCLASSEX);
  windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
  windowClass.hInstance = hInstance;
  windowClass.lpfnWndProc = WindowProc;
  windowClass.lpszClassName = "GettingSerious";
  windowClass.style = CS_HREDRAW | CS_VREDRAW;

  RegisterClassEx(&windowClass);

  HWND hWindow = CreateWindow("GettingSerious", "GettingSerious, for real",
                              WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, NULL,
                              NULL, hInstance, 0);

  if (!hWindow)
    return -1;

  ShowWindow(hWindow, nCmdShow);

  MSG message;
  while (GetMessage(&message, NULL, 0, 0)) {

    DispatchMessage(&message);
  }

  return 0;
}
