/* Schedule "Set and forget" animations */

#ifndef SCHEDULED_ANIMATION_H
#define SCHEDULED_ANIMATION_H

#include "common.h"
#include "bitmap_renderer.h" // for texquads_to_render
#include "zpolygon.h"

void resolve_animation_effects(uint64_t microseconds_elapsed);

typedef struct ScheduledAnimation {
    uint32_t affected_object_id;
    bool32_t affects_camera_not_object;
    float delta_x_per_second;
    float delta_y_per_second;
    float delta_z_per_second;
    float x_rotation_per_second;
    float y_rotation_per_second;
    float z_rotation_per_second;
    bool32_t center_while_scaling; // keep the center while
                                   // shrinking/enlarging.
                                   // false to hold the left
                                   // & top instead
    float width_factor_per_second; // 1.0f to leave width as is
    float height_factor_per_second; // 1.03f to increase by 3%
                                    // of original height per
                                    // second
    float rgba_delta_per_second[4];
    uint64_t wait_first_microseconds;
    uint64_t remaining_microseconds;
    bool32_t delete_object_when_finished;
    bool32_t deleted;
} ScheduledAnimation;

void construct_scheduled_animation(
    ScheduledAnimation * to_construct);

void request_scheduled_animation(ScheduledAnimation * to_add);

void request_fade_and_destroy(
    const uint32_t object_id,
    const uint64_t duration_microseconds);

void request_fade_to(
    const uint32_t object_id,
    const uint64_t duration_microseconds,
    const float target_alpha);

void request_move_to(
    const uint32_t object_id,
    const uint64_t duration_microseconds,
    const bool32_t ignore_target_mid_x,
    const float target_mid_x,
    const bool32_t ignore_target_mid_y,
    const float target_mid_y);

void request_dud_dance(
    const uint32_t object_id);

void request_bump_animation(
    const uint32_t object_id);

#endif

