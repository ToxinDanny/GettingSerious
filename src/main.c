#include <Windows.h>
#include <d2d1_1.h>
#include <stdint.h>
#include <windowsx.h>

#include "ecs/ecs.h"
#include "linear-arena.h"
#include "slab-arena.h"

#include "../deps/clay_renderer_gdi.c"
#define CLAY_IMPLEMENTATION
#include "ui.h"

#define WIDTH 800
#define HEIGHT 600

linear_arena_t *global_arena;
slab_arena_t *game_arena;
command_queue_t *cmd_queue;
entity_manager_t *em;
window_rect_t game_rect = {};
RECT window_rc = {};
HDC memHdc;
HBITMAP memBitmap;

/**************** Took from clay gdi example ****************/
HFONT fonts[1];
BOOL didAllocConsole = FALSE;
BOOL ui_debug_mode = FALSE;
void HandleClayErrors(Clay_ErrorData errorData);
void CenterWindow(HWND hwnd_self);
/************************************************************/

void InitMemory();
void InitUi();
void InitWindowClass(HINSTANCE hInstance);

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
    if (GetClientRect(hwnd, &window_rc)) {
      Clay_Dimensions dim = (Clay_Dimensions){.height = window_rc.bottom - window_rc.top,
                                              .width = window_rc.right - window_rc.left};
      Clay_SetLayoutDimensions(dim);

    }

    Clay_ElementData gameContainer =
        Clay_GetElementData(Clay_GetElementId(CLAY_STRING("GameContainer")));

    if (gameContainer.found == TRUE) {
      game_rect.left = gameContainer.boundingBox.x;
      game_rect.right =
          gameContainer.boundingBox.x + gameContainer.boundingBox.width;
      game_rect.top = gameContainer.boundingBox.y;
      game_rect.bottom =
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
    PAINTSTRUCT ps;
    BeginPaint(hwnd, &ps);
    EndPaint(hwnd, &ps);
    break;

  default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

  return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
                    LPWSTR lpCmdLine, int nCmdShow) {

  InitMemory();

  InitUi();

  InitWindowClass(hInstance);

  HWND hWindow = CreateWindow("GettingSerious", "GettingSerious, for real",
                              WS_OVERLAPPEDWINDOW, 100, 100, WIDTH, HEIGHT,
                              NULL, NULL, hInstance, 0);

  if (!hWindow)
    return -1;

  GetClientRect(hWindow, &window_rc);
  ShowWindow(hWindow, nCmdShow);  

  HDC hdc = GetDC(hWindow);
  memHdc = CreateCompatibleDC(hdc);
  memBitmap = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);
  SelectObject(memHdc, memBitmap);
  ReleaseDC(hWindow, hdc);

  HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));

  MSG message;
  message.message = WM_NULL;

  while (message.message != WM_QUIT) {

    if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
      
      TranslateMessage(&message);
      DispatchMessage(&message);

    } else {
      
      while (cmd_queue->count > 0) {
        if(game_rect.is_initialized == FALSE)
          break;

        COMMAND cmd = dequeue_cmd(cmd_queue);

        switch (cmd) {
        case CREATE_CMD:
          create_entity(game_arena, em, 0);
          break;

        case CREATE_N_CMD:
          break;

        case DELETE_CMD:
          // TODO: Element clicked deletion.
          // Get element id, convert to entity id, find correct slot and free
          // from game_arena.
          break;

        case DESTROY_CMD:
          slab_arena_destroy(game_arena);
          game_arena = slab_arena_init(1 << 27, 8);
          break;

        default:
          break;
        }
      }

      FillRect(memHdc, &window_rc, (HBRUSH)GetStockObject(BLACK_BRUSH)); // clear window

      if (em->count > 0) {
        upd_entity_pos(em);
      }
      
      Clay_RenderCommandArray renderCommands = RenderBaseLayout(cmd_queue);
      Clay_Win32_Render(&memHdc, &window_rc, renderCommands, fonts);

      if (game_rect.is_initialized == FALSE) {

        Clay_ElementData gameContainer = Clay_GetElementData(
            Clay_GetElementId(CLAY_STRING("GameContainer")));

        if (gameContainer.found == TRUE) {
          game_rect.left = gameContainer.boundingBox.x;
          game_rect.right =
              gameContainer.boundingBox.x + gameContainer.boundingBox.width;
          game_rect.top = gameContainer.boundingBox.y;
          game_rect.bottom =
              gameContainer.boundingBox.y + gameContainer.boundingBox.height;

          game_rect.is_initialized = TRUE;  
        }
      }

      SelectObject(memHdc, hBrush);

      for (int i = 0; i < em->count; i++) {

        uint32_t r = (uint32_t)(2.0f * em->shapes[i]->shape.circle.radius);

        uint32_t left = (uint32_t)em->positions[i]->x - r;
        uint32_t top = (uint32_t)em->positions[i]->y - r;
        uint32_t right = left + 2 * r;
        uint32_t bottom = top + 2 * r;

        Ellipse(memHdc, left, top, right, bottom);
      }

      HDC hdc = GetDC(hWindow);
      int w = window_rc.right - window_rc.left;
      int h = window_rc.bottom - window_rc.top;
      BOOL ok = BitBlt(hdc, 0, 0, w, h, memHdc, 0, 0, SRCCOPY);
      ReleaseDC(hWindow, hdc);
    }
  }

  return 0;
}

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

void InitMemory() {
  global_arena = linear_arena_init(1 << 27, 8);
  game_arena = slab_arena_init(1 << 27, 8);
  cmd_queue = cmd_queue_init(global_arena);
  em = entity_manager_init(global_arena);
  em->game_rect = &game_rect;
  em->count = 0;
}

void InitUi() {
  uint64_t clayRequiredMemory = Clay_MinMemorySize();
  Clay_Arena clayMemory = {
      .nextAllocation = NULL,
      .capacity = clayRequiredMemory,
      .memory = (char *)malloc(clayRequiredMemory),
  };

  Clay_Dimensions clayDimensions = {.width = WIDTH, .height = HEIGHT};
  Clay_ErrorHandler errHandler = {.errorHandlerFunction = HandleClayErrors,
                                  .userData = NULL};
  Clay_Initialize(clayMemory, clayDimensions, errHandler);

  Clay_Win32_SetRendererFlags(CLAYGDI_RF_ALPHABLEND | CLAYGDI_RF_SMOOTHCORNERS);

  fonts[0] = Clay_Win32_SimpleCreateFont("../resources/Roboto-Regular.ttf",
                                         "Roboto", -11, FW_NORMAL);
  Clay_SetMeasureTextFunction(Clay_Win32_MeasureText, fonts);
}

void InitWindowClass(HINSTANCE hInstance){
  WNDCLASSEX windowClass;
  ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

  windowClass.cbSize = sizeof(WNDCLASSEX);
  windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
  windowClass.hInstance = hInstance;
  windowClass.lpfnWndProc = WindowProc;
  windowClass.lpszClassName = "GettingSerious";
  windowClass.style = CS_HREDRAW | CS_VREDRAW;

  RegisterClassEx(&windowClass);
}
