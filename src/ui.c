#include "ui.h"

Clay_RenderCommandArray CreateBaseLayout() {
  Clay_BeginLayout();

  Clay_Sizing layoutExpand = {.width = CLAY_SIZING_GROW(0),
                              .height = CLAY_SIZING_GROW(0)};

  CLAY(CLAY_ID("OuterContainer"),
       {.backgroundColor = {48, 54, 51, 255},
        .layout = {.layoutDirection = CLAY_TOP_TO_BOTTOM,
                   .sizing = layoutExpand,
                   .padding = CLAY_PADDING_ALL(16),
                   .childGap = 16}}) {

    CLAY(CLAY_ID("GameContainer"),
         {.layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                                .height = CLAY_SIZING_PERCENT(0.6)},
                     .padding = CLAY_PADDING_ALL(16)},
          .backgroundColor = {136, 141, 167, 255},
          .cornerRadius = (Clay_CornerRadius){5, 5, 5, 5}}) {}

    CLAY(CLAY_ID("ControlContainer"),
         {.layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                                .height = CLAY_SIZING_PERCENT(0.4)},
                     .childGap = 16
                     }}) {

      CLAY(CLAY_ID("ButtonPanel"),
           {.layout = {.sizing = {.width = CLAY_SIZING_PERCENT(0.4),
                                  .height = CLAY_SIZING_GROW(0)},
                       .padding = CLAY_PADDING_ALL(16)},
            .backgroundColor = {136, 141, 167, 255},
            .cornerRadius = (Clay_CornerRadius){5, 5, 5, 5}}) {}

      CLAY(CLAY_ID("DiagPanel"),

           {.layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                                  .height = CLAY_SIZING_GROW(0)},

                       .padding = CLAY_PADDING_ALL(16)},
            .backgroundColor = {136, 141, 167, 255},
            .cornerRadius = (Clay_CornerRadius){5, 5, 5, 5}}) {}
    }
  }

  Clay_RenderCommandArray renderCommands = Clay_EndLayout();
  return renderCommands;
}
