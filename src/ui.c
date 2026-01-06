#include "ui.h"
#include "command_queue.h"
#include <Windows.h>

Clay_Color PRIMARY_COLOR = {21, 59, 80, 255};        // Yale Blue
Clay_Color PRIMARY_COLOR_BW = {48, 54, 51, 255};     // Metalgun Gray
Clay_Color SECONDARY_COLOR = {149, 217, 218, 255};   // Pearl Aqua
Clay_Color SECONDARY_COLOR_2 = {36, 100, 134, 255};  // Baltic Blue
Clay_Color SECONDARY_COLOR_3 = {201, 227, 241, 255}; // Pale Sky
Clay_Color ALTERNATIVE_COLOR = {254, 127, 45, 255};  // Pumpkin Spice

static command_queue_t* cmd_queue;

void HandleButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData) {

  COMMAND cmd;

  if(elementId.id == CLAY_ID("CreateButton").id)
    cmd = CREATE_CMD;

  if(elementId.id == CLAY_ID("CreateNButton").id)
    cmd = CREATE_N_CMD;

  if(elementId.id == CLAY_ID("DeleteButton").id)
    cmd = DELETE_CMD;

  if(elementId.id == CLAY_ID("DestroyButton").id)
    cmd = DESTROY_CMD;

  if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
    enqueue_cmd(cmd_queue, cmd);
  }
}

Clay_RenderCommandArray RenderBaseLayout(command_queue_t* queue) {

  cmd_queue = queue;

  Clay_BeginLayout();

  Clay_Sizing layoutExpand = {.width = CLAY_SIZING_GROW(0),
                              .height = CLAY_SIZING_GROW(0)};

  CLAY(CLAY_ID("OuterContainer"),
       {.backgroundColor = PRIMARY_COLOR,
        .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM,
                   .sizing = layoutExpand,
                   .padding = CLAY_PADDING_ALL(16),
                   .childGap = 16}}) {

    CLAY(CLAY_ID("GameContainer"),
         {
             .layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                                   .height = CLAY_SIZING_PERCENT(0.6)},
                        .padding = CLAY_PADDING_ALL(16)},
             .backgroundColor = PRIMARY_COLOR,
             .border = {.color = SECONDARY_COLOR, .width = {.bottom = 1}},
         }) {}

    CLAY(CLAY_ID("ControlContainer"),
         {.layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                                .height = CLAY_SIZING_PERCENT(0.4)},
                     .childGap = 16}}) {

      CLAY(CLAY_ID("ButtonPanel"),
           {.layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM,
                       .sizing = {.width = CLAY_SIZING_PERCENT(0.4),
                                  .height = CLAY_SIZING_GROW(0)},
                       .padding = CLAY_PADDING_ALL(16),
                       .childGap = 16,
                       .childAlignment =
                           (Clay_ChildAlignment){CLAY_ALIGN_X_CENTER,
                                                 CLAY_ALIGN_Y_CENTER}},
            .backgroundColor = PRIMARY_COLOR,
            .border = {.color = SECONDARY_COLOR, .width = {0, 1, 0, 0, 0}}}) {

        CLAY(CLAY_ID("CreateButton"),
             {.layout = {.sizing = {.width = CLAY_SIZING_FIT(200, 1000),
                                    .height = CLAY_SIZING_FIT(50, 250)},
                         .padding = CLAY_PADDING_ALL(16),
                         .childAlignment =
                             (Clay_ChildAlignment){CLAY_ALIGN_X_CENTER,
                                                   CLAY_ALIGN_Y_CENTER}},
              .backgroundColor = ALTERNATIVE_COLOR,
              .cornerRadius = CLAY_CORNER_RADIUS(8)}) {

          Clay_OnHover(HandleButtonInteraction, NULL);

          CLAY_TEXT(
              CLAY_STRING("Create instance"),
              CLAY_TEXT_CONFIG(
                  {.fontId = 0, .fontSize = 24, .textColor = PRIMARY_COLOR, .textAlignment = (Clay_TextAlignment){CLAY_TEXT_ALIGN_CENTER}}));
        }
        {
        }

        CLAY(CLAY_ID("DestroyButton"),
             {.layout = {.sizing = {.width = CLAY_SIZING_FIT(200, 1000),
                                    .height = CLAY_SIZING_FIT(50, 250)},
                         .padding = CLAY_PADDING_ALL(16),
                         .childAlignment =
                             (Clay_ChildAlignment){CLAY_ALIGN_X_CENTER,
                                                   CLAY_ALIGN_Y_CENTER}},
              .backgroundColor = ALTERNATIVE_COLOR,
              .cornerRadius = CLAY_CORNER_RADIUS(8)}) {

          Clay_OnHover(HandleButtonInteraction, NULL);

          CLAY_TEXT(
              CLAY_STRING("Destroy Arena"),
              CLAY_TEXT_CONFIG(
                  {.fontId = 0, .fontSize = 24, .textColor = PRIMARY_COLOR, .textAlignment = (Clay_TextAlignment){CLAY_TEXT_ALIGN_CENTER}}));
        }
        {
        }
      }

      CLAY(CLAY_ID("DiagPanel"),
           {.layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                                  .height = CLAY_SIZING_GROW(0)},

                       .padding = CLAY_PADDING_ALL(16)},
            .backgroundColor = PRIMARY_COLOR,
            .border = {.color = SECONDARY_COLOR, .width = {0, 1, 0, 0, 0}}}) {}
    }
  }

  Clay_RenderCommandArray renderCommands = Clay_EndLayout();
  return renderCommands;
}
