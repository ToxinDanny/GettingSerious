#include <Windows.h>
#include <d2d1_1.h>
#include <windowsx.h>

#include "slab-arena.h"
#include "ecs/ecs.h"

#include "../deps/clay_renderer_gdi.c"
#define CLAY_IMPLEMENTATION
#include "ui.h"

static linear_arena_t* global_arena;
static slab_arena_t* game_arena;
static command_queue_t* cmd_queue;

typedef struct game_rect {
  float left;
  float right;
  float top;
  float bottom;
  BOOL is_initialized;
} game_rect_t;

static game_rect_t gameRect = {};

HFONT fonts[1];

BOOL didAllocConsole = FALSE;
BOOL ui_debug_mode = FALSE;
void HandleClayErrors(Clay_ErrorData errorData) {
  if (!didAllocConsole) {
    didAllocConsole = AllocConsole();
  }
  printf("Handle Clay Errors: %s\r\n", errorData.errorText.chars);
}

void CenterWindow(HWND hwnd_self) {
  HWND hwnd_parent;
  RECT rw_self, rc_parent, rw_parent;
  int xpos, ypos;
  hwnd_parent = GetParent(hwnd_self);
  if (NULL == hwnd_parent)
    hwnd_parent = GetDesktopWindow();
  GetWindowRect(hwnd_parent, &rw_parent);
  GetClientRect(hwnd_parent, &rc_parent);
  GetWindowRect(hwnd_self, &rw_self);
  xpos = rw_parent.left + (rc_parent.right + rw_self.left - rw_self.right) / 2;
  ypos = rw_parent.top + (rc_parent.bottom + rw_self.top - rw_self.bottom) / 2;
  SetWindowPos(hwnd_self, NULL, xpos, ypos, 0, 0,
               SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {

  switch (uMsg) {
    case WM_CREATE:
      CenterWindow(hwnd);
      break;

    case WM_DESTROY:
      slab_arena_destroy(game_arena);
      PostQuitMessage(0);
      break;

    case WM_SIZE: 
      RECT r = {0};
      if (GetClientRect(hwnd, &r)) {
        Clay_Dimensions dim = (Clay_Dimensions){.height = r.bottom - r.top,
                                                .width = r.right - r.left};
        Clay_SetLayoutDimensions(dim);
      }

      Clay_ElementData gameContainer =
          Clay_GetElementData(Clay_GetElementId(CLAY_STRING("GameContainer")));

      if (gameContainer.found == TRUE) {
        gameRect.left = gameContainer.boundingBox.x;
        gameRect.right =
            gameContainer.boundingBox.x + gameContainer.boundingBox.width;
        gameRect.top = gameContainer.boundingBox.y;
        gameRect.bottom =
            gameContainer.boundingBox.y + gameContainer.boundingBox.height;
      }

      InvalidateRect(hwnd, NULL, false);
      break;

    case WM_RBUTTONUP:
    case WM_LBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MOUSEMOVE: 
      short mouseX = GET_X_LPARAM(lParam);
      short mouseY = GET_Y_LPARAM(lParam);
      short mouseButtons = LOWORD(wParam);
      Clay_SetPointerState((Clay_Vector2){mouseX, mouseY}, mouseButtons & 0b01);
      InvalidateRect(hwnd, NULL, false); 
      break;
    
    case WM_KEYDOWN:
      if (VK_ESCAPE == wParam) {
        DestroyWindow(hwnd);
        break;
      }

      if (wParam == VK_F12) {
        Clay_SetDebugModeEnabled(ui_debug_mode = !ui_debug_mode);
       
      }

      InvalidateRect(hwnd, NULL, false); 
      break;
      
    case WM_PAINT:
      Clay_RenderCommandArray renderCommands = RenderBaseLayout(cmd_queue);
      Clay_Win32_Render(hwnd, renderCommands, fonts);

      if(gameRect.is_initialized == TRUE)
        break;
      
      gameContainer =
          Clay_GetElementData(Clay_GetElementId(CLAY_STRING("GameContainer")));
      
      if (gameContainer.found == TRUE) {
        gameRect.left = gameContainer.boundingBox.x;
        gameRect.right =
            gameContainer.boundingBox.x + gameContainer.boundingBox.width;
        gameRect.top = gameContainer.boundingBox.y;
        gameRect.bottom =
            gameContainer.boundingBox.y + gameContainer.boundingBox.height;
      }
      break;

    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

  return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR lpCmdLine, int nCmdShow) {

  global_arena = linear_arena_init(1<<27, 8);
  game_arena = slab_arena_init(1<<27, 8);
  cmd_queue = cmd_queue_init(global_arena);
  entity_manager_t entity_manager;

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
  message.message = WM_NULL;
  
  while (message.message != WM_QUIT) {

    if(PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
      DispatchMessage(&message);

    else {

      while(cmd_queue->count > 0){
        COMMAND cmd = dequeue_cmd(cmd_queue);

        switch (cmd) {
        case CREATE_CMD:
          
          break;
          
        case CREATE_N_CMD:
          break;
          
        case DELETE_CMD:
          // TODO: Element clicked deletion.
          // Get element id, convert to entity id, find correct slot and free from game_arena.
          break;
          
        case DESTROY_CMD:
          slab_arena_destroy(game_arena);
          game_arena = slab_arena_init(1<<27, 8);
          break;

        default:
          break;
        }
      }
    }
  }

  return 0;
}
