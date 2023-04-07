#include "scheduled_animations.h"

ScheduledAnimation * scheduled_animations;
uint32_t scheduled_animations_size = 0;

void init_scheduled_animations(void) {
    scheduled_animations = (ScheduledAnimation *)malloc_from_unmanaged(
        sizeof(ScheduledAnimation) * SCHEDULED_ANIMATIONS_ARRAYSIZE);
}

void construct_scheduled_animation(
    ScheduledAnimation * to_construct)
{
    to_construct->affected_object_id = -1;
    
    to_construct->set_texture_array_i = false;
    to_construct->new_texture_array_i = -1;
    to_construct->set_texture_i = false;
    to_construct->new_texture_i = -1;
    to_construct->destroy_even_waiting_duplicates = false;
    
    to_construct->final_x_known = false;
    to_construct->final_y_known = false;
    to_construct->final_z_known = false;
    to_construct->delta_x_per_second = 0.0f;
    to_construct->delta_y_per_second = 0.0f;
    to_construct->delta_z_per_second = 0.0f;
    to_construct->delta_z_per_second = 0.0f;
    
    to_construct->final_x_angle_known = false;
    to_construct->x_rotation_per_second = 0.0f;
    to_construct->final_y_angle_known = false;
    to_construct->y_rotation_per_second = 0.0f;
    to_construct->final_z_angle_known = false;
    to_construct->z_rotation_per_second = 0.0f;
    
    to_construct->final_width_known = false;
    to_construct->final_height_known = false;
    to_construct->delta_width_per_second = 0.0f;
    to_construct->delta_height_per_second = 0.0f;
    
    to_construct->final_scale_known = false;
    to_construct->delta_scale_per_second = 0.0f;
    
    for (uint32_t i = 0; i < 4; i++) {
        to_construct->final_rgba_known[i] = false;
        to_construct->rgba_delta_per_second[i] = 0.0f;
    }
    
    for (uint32_t i = 0; i < 3; i++) {
        to_construct->final_rgb_bonus_known[i] = false;
        to_construct->rgb_bonus_delta_per_second[i] = 0.0f;
    }
    
    to_construct->wait_before_each_run = 0;
    to_construct->remaining_wait_before_next_run = 0;
    to_construct->duration_microseconds = 1000000;
    to_construct->remaining_microseconds = 0; // gets set when scheduling
    to_construct->runs = 1;
    to_construct->delete_object_when_finished = false;
    to_construct->deleted = false;
    to_construct->clientlogic_callback_when_finished_id = -1;
}

void request_scheduled_animation(
    ScheduledAnimation * to_add)
{
    log_assert(to_add != NULL);
    log_assert(!to_add->deleted);
    
    if (
        to_add->clientlogic_callback_when_finished_id < 0)
    {
        log_assert(to_add->affected_object_id >= 0);
        
        bool32_t found_target = false;
        
        for (
            uint32_t i = 0;
            i < zlights_to_apply_size;
            i++)
        {
            if (
                !zlights_to_apply[i].deleted &&
                zlights_to_apply[i].object_id == to_add->affected_object_id)
            {
                found_target = true;
            }
        }
        
        for (
            uint32_t zp_i = 0;
            zp_i < zpolygons_to_render_size;
            zp_i++)
        {
            if (
                !zpolygons_to_render[zp_i].deleted &&
                zpolygons_to_render[zp_i].object_id == to_add->affected_object_id)
            {
                found_target = true;
            }
        }
    }
    to_add->remaining_microseconds = to_add->duration_microseconds;
    
    log_assert(
        scheduled_animations_size < SCHEDULED_ANIMATIONS_ARRAYSIZE);
    
    if (to_add->remaining_wait_before_next_run < 1) {
        delete_conflicting_animations(
            to_add,
            -1);
    }
    
    for (
        int32_t i = 0;
        i < (int32_t)scheduled_animations_size;
        i++)
    {
        if (scheduled_animations[i].deleted)
        {
            scheduled_animations[i] = *to_add;
            return;
        }
    }
    
    log_assert(
        SCHEDULED_ANIMATIONS_ARRAYSIZE > scheduled_animations_size);
    
    scheduled_animations[scheduled_animations_size] = *to_add;
    log_assert(!scheduled_animations[scheduled_animations_size].deleted);
    scheduled_animations_size += 1;
    log_assert(
        SCHEDULED_ANIMATIONS_ARRAYSIZE
            > scheduled_animations_size);
}

void delete_conflicting_animations(
    ScheduledAnimation * priority_anim,
    const int32_t self_index)
{
    float float_threshold = 0.0001f;
    
    for (
        int32_t i = 0;
        i < (int32_t)scheduled_animations_size;
        i++)
    {
        if (
            (scheduled_animations[i].remaining_wait_before_next_run > 0 &&
                !priority_anim->destroy_even_waiting_duplicates) ||
            scheduled_animations[i].deleted ||
            scheduled_animations[i].clientlogic_callback_when_finished_id
                != -1 ||
            (scheduled_animations[i].affected_object_id !=
                priority_anim->affected_object_id) ||
            i == self_index ||
            scheduled_animations[i].runs != 1)
        {
            continue;
        }
        
        if (
            priority_anim->final_z_known &&
            scheduled_animations[i].final_z_known)
        {
            scheduled_animations[i].deleted = true;
        }
        
        if (
            (
            !priority_anim->final_z_known &&
            (
                priority_anim->delta_z_per_second < -float_threshold ||
                priority_anim->delta_z_per_second >  float_threshold
            )) &&
            (
            !scheduled_animations[i].final_z_known &&
            (
                scheduled_animations[i].delta_z_per_second < -float_threshold ||
                scheduled_animations[i].delta_z_per_second >  float_threshold
            )))
        {
            scheduled_animations[i].deleted = true;
        }
        
        if (
            priority_anim->final_x_known &&
            scheduled_animations[i].final_x_known)
        {
            scheduled_animations[i].deleted = true;
        }
        
        if (
            (
            !priority_anim->final_x_known &&
            (
                priority_anim->delta_x_per_second < -float_threshold ||
                priority_anim->delta_x_per_second >  float_threshold
            )) &&
            (
            !scheduled_animations[i].final_x_known &&
            (
                scheduled_animations[i].delta_x_per_second < -float_threshold ||
                scheduled_animations[i].delta_x_per_second >  float_threshold
            )))
        {
            scheduled_animations[i].deleted = true;
        }
        
        if (
            priority_anim->final_y_known &&
            scheduled_animations[i].final_y_known)
        {
            scheduled_animations[i].deleted = true;
        }
        
        if (
            (
            !priority_anim->final_y_known &&
            (
                priority_anim->delta_y_per_second < -float_threshold ||
                priority_anim->delta_y_per_second >  float_threshold
            )) &&
            (
            !scheduled_animations[i].final_y_known &&
            (
                scheduled_animations[i].delta_y_per_second < -float_threshold ||
                scheduled_animations[i].delta_y_per_second >  float_threshold
            )))
        {
            scheduled_animations[i].deleted = true;
        }
        
        if (
            priority_anim->final_y_angle_known &&
            scheduled_animations[i].final_y_angle_known)
        {
            scheduled_animations[i].deleted = true;
        }
        
        if (
            (
            !priority_anim->final_y_angle_known &&
            (
                priority_anim->y_rotation_per_second < -float_threshold ||
                priority_anim->y_rotation_per_second >  float_threshold
            )) &&
            (
            !scheduled_animations[i].final_y_angle_known &&
            (
                scheduled_animations[i].y_rotation_per_second < -float_threshold ||
                scheduled_animations[i].y_rotation_per_second >  float_threshold
            )))
        {
            scheduled_animations[i].deleted = true;
        }
    }
}

void request_fade_and_destroy(
    const int32_t object_id,
    const uint64_t wait_before_first_run,
    const uint64_t duration_microseconds)
{
    log_assert(duration_microseconds > 0);
    log_assert(object_id >= 0);
    
    // register scheduled animation
    ScheduledAnimation modify_alpha;
    construct_scheduled_animation(&modify_alpha);
    modify_alpha.affected_object_id = object_id;
    modify_alpha.remaining_wait_before_next_run = wait_before_first_run;
    modify_alpha.duration_microseconds = duration_microseconds;
    modify_alpha.rgba_delta_per_second[1] = -0.5f;
    modify_alpha.rgba_delta_per_second[2] = -0.5f;
    modify_alpha.final_rgba_known[3] = true;
    modify_alpha.final_rgba[3] = 0.0f;
    modify_alpha.delete_object_when_finished = true;
    request_scheduled_animation(&modify_alpha);
}

void request_fade_to(
    const int32_t object_id,
    const uint64_t wait_before_first_run,
    const uint64_t duration_microseconds,
    const float target_alpha)
{
    log_assert(object_id >= 0);
    
    // register scheduled animation
    ScheduledAnimation modify_alpha;
    construct_scheduled_animation(&modify_alpha);
    modify_alpha.affected_object_id = object_id;
    modify_alpha.remaining_wait_before_next_run = wait_before_first_run;
    modify_alpha.duration_microseconds = duration_microseconds;
    modify_alpha.final_rgba_known[3] = true;
    modify_alpha.final_rgba[3] = target_alpha;
    request_scheduled_animation(&modify_alpha);
}

static void resolve_single_animation_effects(
    ScheduledAnimation * anim,
    const uint64_t elapsed_this_run,
    const uint64_t remaining_microseconds_at_start_of_run)
{
    log_assert(remaining_microseconds_at_start_of_run >= elapsed_this_run);
    
    // TODO: remove debugging code
    if (anim->y_rotation_per_second > 0.0f) {
        log_assert(1);
    }
    
    if (anim->deleted) { return; }
    
    bool32_t found_at_least_one = false;
    
    // apply effects
    for (
        int32_t l_i = (int32_t)zlights_to_apply_size - 1;
        l_i >= 0;
        l_i--)
    {
        if (
            zlights_to_apply[l_i].object_id ==
                anim->affected_object_id &&
            !zlights_to_apply[l_i].deleted)
        {
            found_at_least_one = true;
            
            if (!anim->final_x_known) {
                zlights_to_apply[l_i].x +=
                    (anim->delta_x_per_second * elapsed_this_run) / 1000000;
            } else {
                float diff_x = anim->final_mid_x - zlights_to_apply[l_i].x;
                zlights_to_apply[l_i].x +=
                    diff_x /
                        ((float)remaining_microseconds_at_start_of_run /
                            elapsed_this_run);
            }
            
            if (!anim->final_y_known) {
                zlights_to_apply[l_i].y +=
                    ((anim->delta_y_per_second
                        * elapsed_this_run)
                            / 1000000);
            } else {
                float diff_y = anim->final_mid_y - zlights_to_apply[l_i].y;
                zlights_to_apply[l_i].y +=
                    diff_y /
                        ((float)remaining_microseconds_at_start_of_run /
                            elapsed_this_run);
            }
            
            if (!anim->final_z_known) {
                zlights_to_apply[l_i].z +=
                    ((anim->delta_z_per_second
                        * elapsed_this_run)
                            / 1000000);
            } else {
                float diff_z = anim->final_mid_z - zlights_to_apply[l_i].z;
                zlights_to_apply[l_i].z +=
                    diff_z /
                        ((float)remaining_microseconds_at_start_of_run /
                            elapsed_this_run);
            }
            
            if (!anim->final_reach_known) {
                zlights_to_apply[l_i].reach +=
                    ((anim->delta_reach_per_second
                        * elapsed_this_run)
                            / 1000000);
            } else {
                float diff_reach = anim->final_reach -
                    zlights_to_apply[l_i].reach;
                
                zlights_to_apply[l_i].reach +=
                    diff_reach /
                        ((float)remaining_microseconds_at_start_of_run /
                            elapsed_this_run);
            }
            
            for (
                uint32_t c = 0;
                c < 4;
                c++)
            {
                if (!anim->final_rgba_known[c]) {
                    zlights_to_apply[l_i].RGBA[c] +=
                        (anim->rgba_delta_per_second[c]
                            * elapsed_this_run)
                                / 1000000;
                } else {
                    float cur_val =
                        zlights_to_apply[l_i].RGBA[c];
                    float delta_val =
                        anim->final_rgba[c] - cur_val;
                    
                    zlights_to_apply[l_i].RGBA[c] +=
                        delta_val /
                            ((float)remaining_microseconds_at_start_of_run /
                                elapsed_this_run);
                }
            }
        }
    }
    
    for (
        uint32_t zp_i = 0;
        zp_i < zpolygons_to_render_size;
        zp_i++)
    {
        if (
            zpolygons_to_render[zp_i].object_id != anim->affected_object_id)
        {
            continue;
        }
        
        found_at_least_one = true;
        
        if (!anim->final_x_known) {
                zpolygons_to_render[zp_i].x +=
                    ((anim->delta_x_per_second
                        * elapsed_this_run)
                            / 1000000);
        } else {
            float diff_x = anim->final_mid_x -
                zpolygons_to_render[zp_i].x;
            zpolygons_to_render[zp_i].x +=
                diff_x /
                    ((float)remaining_microseconds_at_start_of_run /
                        elapsed_this_run);
        }
        
        if (!anim->final_y_known) {
                zpolygons_to_render[zp_i].y +=
                    ((anim->delta_y_per_second
                        * elapsed_this_run)
                            / 1000000);
        } else {
            float diff_y = anim->final_mid_y -
                zpolygons_to_render[zp_i].y;
            zpolygons_to_render[zp_i].y +=
                diff_y /
                    ((float)remaining_microseconds_at_start_of_run /
                        elapsed_this_run);
        }
        
        if (!anim->final_z_known) {
                zpolygons_to_render[zp_i].z +=
                    ((anim->delta_z_per_second
                        * elapsed_this_run)
                            / 1000000);
        } else {
            float diff_z = anim->final_mid_z - zpolygons_to_render[zp_i].z;
            zpolygons_to_render[zp_i].z +=
                diff_z /
                    ((float)remaining_microseconds_at_start_of_run /
                        elapsed_this_run);
        }
        
        if (!anim->final_x_angle_known) {
            zpolygons_to_render[zp_i].x_angle +=
                (anim->x_rotation_per_second
                    * elapsed_this_run)
                        / 1000000;
        } else {
            float diff_x_angle = anim->final_x_angle - zpolygons_to_render[zp_i].x_angle;
            zpolygons_to_render[zp_i].x_angle +=
                diff_x_angle /
                    ((float)remaining_microseconds_at_start_of_run /
                        elapsed_this_run);
        }
        
        if (!anim->final_y_angle_known) {
            if (anim->y_rotation_per_second > 0.1f) {
                log_assert(1);
            }
            zpolygons_to_render[zp_i].y_angle +=
                (anim->y_rotation_per_second
                    * elapsed_this_run)
                        / 1000000;
        } else {
            if (anim->y_rotation_per_second > 0.1f) {
                log_assert(1);
            }
            
            float diff_y_angle = anim->final_y_angle - zpolygons_to_render[zp_i].y_angle;
            zpolygons_to_render[zp_i].y_angle +=
                diff_y_angle /
                    ((float)remaining_microseconds_at_start_of_run /
                        elapsed_this_run);
        }
        
        if (!anim->final_z_angle_known) {
            zpolygons_to_render[zp_i].z_angle +=
                (anim->z_rotation_per_second
                    * elapsed_this_run)
                        / 1000000;
        } else {
            float diff_z_angle = anim->final_z_angle - zpolygons_to_render[zp_i].z_angle;
            zpolygons_to_render[zp_i].z_angle +=
                diff_z_angle /
                    ((float)remaining_microseconds_at_start_of_run /
                        elapsed_this_run);
        }
        
        if (!anim->final_width_known) {
            if (anim->delta_width_per_second != 0.0f) {
                float cur_width = zpolygon_get_width(&zpolygons_to_render[zp_i]);
            
                float bonus_width = (anim->delta_width_per_second
                    * elapsed_this_run)
                        / 1000000;
                
                zpolygon_scale_width_only_given_z(
                    /* zPolygon * to_scale: */ &zpolygons_to_render[zp_i],
                    /* const float new_width: */ cur_width + bonus_width,
                    /* const float when_observed_at_z: */ 1.0f);
            }
        } else {
            float cur_width = zpolygon_get_width(&zpolygons_to_render[zp_i]);
            
            float diff_width = anim->final_width - cur_width;
            
            float new_width = cur_width +
                (diff_width /
                    ((float)remaining_microseconds_at_start_of_run /
                        elapsed_this_run));
            
            zpolygon_scale_width_only_given_z(
                /* zPolygon * to_scale: */ &zpolygons_to_render[zp_i],
                /* const float new_width: */ new_width,
                /* const float when_observed_at_z: */ 1.0f);
        }
        
        if (!anim->final_scale_known) {
            zpolygons_to_render[zp_i].scale_factor +=
                (anim->delta_scale_per_second *
                    elapsed_this_run) / 1000000;
        } else {
            float diff_scale =
                anim->final_scale - zpolygons_to_render[zp_i].scale_factor;
            zpolygons_to_render[zp_i].scale_factor +=
                diff_scale
                    / ((float)remaining_microseconds_at_start_of_run
                        / elapsed_this_run);
        }
        
        if (anim->set_texture_array_i || anim->set_texture_i) {
            for (
                uint32_t tri_i = 0;
                tri_i < zpolygons_to_render[zp_i].triangles_size;
                tri_i++)
            {
                if (anim->set_texture_array_i) {
                    zpolygons_to_render[zp_i].triangles[tri_i].texturearray_i =
                        anim->new_texture_array_i;
                }
                if (anim->set_texture_i) {
                    zpolygons_to_render[zp_i].triangles[tri_i].texture_i =
                        anim->new_texture_i;
                }
            }
        }
        
        for (
            uint32_t c = 0;
            c < 4;
            c++)
        {
            if (!anim->final_rgba_known[c]) {
                for (uint32_t tri_i = 0; tri_i < zpolygons_to_render[zp_i].triangles_size; tri_i++) {
                    float delta = ((anim->rgba_delta_per_second[c]
                            * elapsed_this_run)
                        / 1000000);
                    zpolygons_to_render[zp_i].triangles[tri_i].color[c] +=
                        delta;
                }
            } else {
                
                for (
                    uint32_t tri_i = 0;
                    tri_i < zpolygons_to_render[zp_i].triangles_size;
                    tri_i++)
                {
                    float cur_val =
                        zpolygons_to_render[zp_i].triangles[tri_i].color[c];
                    float delta_val = anim->final_rgba[c] - cur_val;
                    
                    zpolygons_to_render[zp_i].triangles[tri_i].color[c] +=
                        delta_val /
                            ((float)remaining_microseconds_at_start_of_run /
                                elapsed_this_run);
                }
            }
        }
        
        for (
            uint32_t c = 0;
            c < 3;
            c++)
        {
            if (!anim->final_rgb_bonus_known[c]) {
                float delta = ((anim->rgb_bonus_delta_per_second[c]
                        * elapsed_this_run)
                    / 1000000);
                    zpolygons_to_render[zp_i].rgb_bonus[c] +=
                        delta;
            } else {
                float cur_val = zpolygons_to_render[zp_i].rgb_bonus[c];
                float delta_val = anim->final_rgb_bonus[c] - cur_val;
                
                zpolygons_to_render[zp_i].rgb_bonus[c] +=
                    delta_val /
                        ((float)remaining_microseconds_at_start_of_run /
                            elapsed_this_run);
            }
        }
    }    
}

void resolve_animation_effects(const uint64_t microseconds_elapsed) {
    
    ScheduledAnimation * anim;
    for (
        int32_t animation_i = (int32_t)(scheduled_animations_size - 1);
        animation_i >= 0;
        animation_i--)
    {
        // pointer for abbreviation
        anim = &scheduled_animations[animation_i];
        
        if (anim->deleted) {
            if (animation_i == (int32_t)scheduled_animations_size - 1) {
                scheduled_animations_size -= 1;
            }
            continue;
        }
        
        uint64_t actual_elapsed = microseconds_elapsed;
        
        if (anim->remaining_wait_before_next_run > 0) {
            if (actual_elapsed >= anim->remaining_wait_before_next_run) {
                actual_elapsed -= anim->remaining_wait_before_next_run;
                anim->remaining_wait_before_next_run = 0;
                
                delete_conflicting_animations(anim, animation_i);
            } else {
                anim->remaining_wait_before_next_run -= actual_elapsed;
                continue;
            }
        }
        
        uint64_t actual_elapsed_this_run = actual_elapsed;
        uint64_t remaining_microseconds_at_start_of_run =
            anim->remaining_microseconds;
        bool32_t delete_after_this_run = false;
        
        if (anim->remaining_microseconds > actual_elapsed) {
            anim->remaining_microseconds -= actual_elapsed;
        } else {
            
            actual_elapsed_this_run = anim->remaining_microseconds;
            
            // delete or set up next run
            if (anim->runs > 1 || anim->runs == 0) {
                
                uint64_t excess_from_last_run_mcrs =
                    (actual_elapsed - anim->remaining_microseconds);
                
                while (excess_from_last_run_mcrs > 0) {
                    if (anim->runs > 1) {
                        anim->runs -= 1;
                    } else {
                        log_assert(anim->runs == 0);
                    }
                    
                    anim->remaining_wait_before_next_run =
                        anim->wait_before_each_run;
                    
                    if (
                        anim->remaining_wait_before_next_run >=
                            excess_from_last_run_mcrs)
                    {
                        anim->remaining_wait_before_next_run -=
                            excess_from_last_run_mcrs;
                        excess_from_last_run_mcrs = 0;
                    } else {
                        excess_from_last_run_mcrs -=
                            anim->remaining_wait_before_next_run;
                        anim->remaining_wait_before_next_run = 0;
                    }
                    
                    anim->remaining_microseconds =
                        anim->duration_microseconds;
                    
                    if (anim->remaining_microseconds >=
                        excess_from_last_run_mcrs)
                    {
                        anim->remaining_microseconds -=
                            excess_from_last_run_mcrs;
                        excess_from_last_run_mcrs = 0;
                    } else {
                        excess_from_last_run_mcrs -=
                            anim->remaining_microseconds;
                    }
                }
            } else {
                delete_after_this_run = true;
            }
        }
        
        if (actual_elapsed_this_run > 0) {
            resolve_single_animation_effects(
                anim,
                actual_elapsed_this_run,
                remaining_microseconds_at_start_of_run);
        }
        
        if (delete_after_this_run) {
            
            anim->deleted = true;
            if (animation_i == (int32_t)scheduled_animations_size) {
                scheduled_animations_size -= 1;
            }
            
            if (anim->clientlogic_callback_when_finished_id >= 0)  {
                client_logic_animation_callback(
                    anim->clientlogic_callback_when_finished_id);
            }
            
            if (anim->delete_object_when_finished) {
                
                for (
                    int32_t l_i = (int32_t)zlights_to_apply_size - 1;
                    l_i >= 0;
                    l_i--)
                {
                    if (
                        zlights_to_apply[l_i].object_id ==
                            anim->affected_object_id)
                    {
                        zlights_to_apply[l_i].deleted = true;
                        
                        if (l_i == (int32_t)zlights_to_apply_size - 1)
                        {
                            zlights_to_apply_size--;
                        }
                    }
                }
                
                delete_zpolygon_object(anim->affected_object_id);
                
                delete_particle_effect(anim->affected_object_id);
            }
        }
    }
}

void request_dud_dance(const uint32_t object_id)
{
    uint64_t step_size = 60000;
    for (
        uint64_t wait_first = 0;
        wait_first < step_size * 8;
        wait_first += step_size)
    {
        ScheduledAnimation move_request;
        construct_scheduled_animation(&move_request);
        move_request.affected_object_id = (int32_t)object_id;
        move_request.remaining_wait_before_next_run = wait_first;
        move_request.duration_microseconds = step_size;
        move_request.delta_x_per_second =
            wait_first % (step_size * 2) == 0 ?
                0.07f : -0.07f;
        move_request.delta_y_per_second =
            wait_first % (step_size * 2) == 0 ?
                0.07f : -0.07f;
        request_scheduled_animation(
            &move_request);
    }
}

void request_bump_animation(
    const uint32_t object_id,
    const uint32_t wait)
{
    uint64_t duration = 200000;
    
    ScheduledAnimation embiggen_request;
    construct_scheduled_animation(&embiggen_request);
    embiggen_request.affected_object_id = (int32_t)object_id;
    embiggen_request.remaining_wait_before_next_run = wait;
    embiggen_request.duration_microseconds = duration / 2;
    embiggen_request.final_scale_known = true;
    embiggen_request.final_scale = 1.35f;
    // embiggen_request.final_yscale_known = true;
    // embiggen_request.final_yscale = 1.35f;
    request_scheduled_animation(&embiggen_request);
    
    ScheduledAnimation revert_request;
    construct_scheduled_animation(&revert_request);
    revert_request.affected_object_id = (int32_t)object_id;
    revert_request.remaining_wait_before_next_run = wait + duration / 2;
    revert_request.duration_microseconds = duration / 2;
    revert_request.final_scale_known = true;
    revert_request.final_scale = 1.0f;
    // revert_request.final_yscale_known = true;
    // revert_request.final_yscale = 1.0f;
    request_scheduled_animation(&revert_request);
}

void delete_all_movement_animations_targeting(
    const int32_t object_id)
{
    for (uint32_t i = 0; i < scheduled_animations_size; i++) {
        if (scheduled_animations[i].affected_object_id == (int32_t)object_id &&
            (scheduled_animations[i].final_x_known ||
            scheduled_animations[i].final_y_known))
        {
            scheduled_animations[i].deleted = true;
        }
    }
}

void delete_all_rgba_animations_targeting(
    const int32_t object_id)
{
    for (uint32_t i = 0; i < scheduled_animations_size; i++) {
        if (scheduled_animations[i].affected_object_id == (int32_t)object_id &&
            (scheduled_animations[i].final_rgba_known[0] ||
             scheduled_animations[i].final_rgba_known[1] ||
             scheduled_animations[i].final_rgba_known[2] ||
             scheduled_animations[i].final_rgba_known[3]))
        {
            scheduled_animations[i].deleted = true;
        }
    }
}

void delete_all_animations_targeting(const int32_t object_id) {
    for (uint32_t i = 0; i < scheduled_animations_size; i++) {
        if (scheduled_animations[i].affected_object_id == object_id) {
            scheduled_animations[i].deleted = true;
        }
    }
}

void delete_all_repeatforever_animations(void) {
    for (uint32_t i = 0; i < scheduled_animations_size; i++) {
        if (scheduled_animations[i].runs == 0) {
            scheduled_animations[i].deleted = true;
        }
    }
}
