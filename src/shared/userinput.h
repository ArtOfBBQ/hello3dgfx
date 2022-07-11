#ifndef USERINPUT_H
#define USERINPUT_H

#include "common.h"
#include "zpolygon.h"
#include "platform_layer.h"
#include "bitmap_renderer.h"

#define KEYPRESS_MAP_SIZE 1000

typedef struct Touch {
    int32_t touchable_id;
    float start_x;
    float start_y;
    uint64_t started_at;
    float current_x;
    float current_y;
    bool32_t finished;
    uint64_t finished_at;
    bool32_t handled;
} Touch;

extern Touch current_touch;

typedef struct MouseEvent {
    float screenspace_x;
    float screenspace_y;
    int32_t touchable_id;
    uint64_t happened_at;
    bool32_t handled;
} MouseEvent;

extern MouseEvent last_mouse_down;
extern MouseEvent last_mouse_up;
extern MouseEvent last_right_mouse_down;
extern MouseEvent last_right_mouse_up;
extern MouseEvent last_mouse_move;

extern bool32_t keypress_map[KEYPRESS_MAP_SIZE];

void buffer_mousedown(
    float screenspace_x,
    float screenspace_y);
void buffer_mouseup(
    float screenspace_x,
    float screenspace_y);
void buffer_mousemove(
    float screenspace_x,
    float screenspace_y);

void register_keyup(uint32_t key_id);
void register_keydown(uint32_t key_id);
void register_touchstart(const float x, const float y);
void register_touchend(const float x, const float y);

#endif

