#include "terminal.h"

window_struct terminal = {
    .name = "Terminal",
    .id = 0,
    .pos_x = 0,
    .pos_y = 0,
    .width = 500,
    .height = 350,
    .visible = true,
    .focused = true,
    .is_dragging = false,
    .drag_offset_x = 0,
    .drag_offset_y = 0
};