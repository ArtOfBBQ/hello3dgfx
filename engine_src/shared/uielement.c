#include "uielement.h"

static int32_t currently_sliding_touchable_id = -1;
static int32_t currently_sliding_object_id = -1;

typedef struct ActiveUIElement {
    int32_t touchable_id;
    int32_t object_id;
    int32_t object_id_2;
    bool32_t clickable;
    bool32_t slideable;
    bool32_t deleted;
    float slider_width;
    float slider_min;
    float slider_max;
    float * slider_linked_value;
    char interaction_sound_filename[128];
} ActiveUIElement;

NextUIElementSettings * next_ui_element_settings = NULL;

#define ACTIVE_UI_ELEMENTS_SIZE 300
uint32_t active_ui_elements_size = 0;
ActiveUIElement * active_ui_elements = NULL;

static ActiveUIElement * next_active_ui_element(void) {
    for (uint32_t i = 0; i < active_ui_elements_size; i++) {
        if (active_ui_elements[i].deleted) {
            return &active_ui_elements[i];
        }
    }
    
    active_ui_elements[active_ui_elements_size].deleted = true;
    active_ui_elements_size += 1;
    return &active_ui_elements[active_ui_elements_size - 1];
}

void init_ui_elements(void) {
    next_ui_element_settings = (NextUIElementSettings *)
        malloc_from_unmanaged(sizeof(NextUIElementSettings));
    
    next_ui_element_settings->slider_width_screenspace  = 100;
    next_ui_element_settings->slider_height_screenspace =  40;
    next_ui_element_settings->ignore_lighting = true;
    next_ui_element_settings->ignore_camera = false;
    next_ui_element_settings->button_background_texturearray_i = -1;
    next_ui_element_settings->button_background_texture_i = -1;
    next_ui_element_settings->slider_background_texturearray_i = -1;
    next_ui_element_settings->slider_background_texture_i = -1;
    next_ui_element_settings->slider_pin_texturearray_i = -1;
    next_ui_element_settings->slider_pin_texture_i = -1;
    next_ui_element_settings->interaction_sound_filename[0] = '\0';
    
    active_ui_elements = (ActiveUIElement *)malloc_from_unmanaged(
        sizeof(ActiveUIElement) * ACTIVE_UI_ELEMENTS_SIZE);
}

void ui_elements_handle_touches(uint64_t ms_elapsed)
{
    (void)ms_elapsed;
    
    if (
        !user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].handled)
    {
        if (
            user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].
                touchable_id >= 0 &&
            user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].
                touchable_id < LAST_UI_TOUCHABLE_ID)
        {
            for (
                uint32_t i = 0;
                i < active_ui_elements_size;
                i++)
            {
                if (
                    !active_ui_elements[i].deleted &&
                    user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].
                        touchable_id ==
                    active_ui_elements[i].touchable_id)
                {
                    currently_sliding_object_id =
                        active_ui_elements[i].object_id_2;
                    currently_sliding_touchable_id =
                        active_ui_elements[i].touchable_id;
                    
                    for (
                        uint32_t zp_i = 0;
                        zp_i < zpolygons_to_render->size;
                        zp_i++)
                    {
                        if (zpolygons_to_render->cpu_data[zp_i].object_id ==
                            currently_sliding_object_id)
                        {
                            zpolygons_to_render->gpu_data[zp_i].scale_factor =
                                1.05f;
                        }
                    }
                    
                    ScheduledAnimation * bump_pin = next_scheduled_animation();
                    bump_pin->affected_object_id = currently_sliding_object_id;
                    bump_pin->final_scale_known = true;
                    bump_pin->final_scale = 1.0f;
                    bump_pin->remaining_microseconds = 200000;
                    commit_scheduled_animation(bump_pin);
                    
                    if (
                        active_ui_elements[i].
                            interaction_sound_filename[0] != '\0')
                    {
                        // add_audio();
                        // platform_play_sound_resource(
                        //    active_ui_elements[i].interaction_sound_filename);
                    }
                    
                    user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].
                        handled = true;
                }
            }
        }
    }
    
    if (
        currently_sliding_touchable_id >= 0 &&
        currently_sliding_object_id >= 0)
    {
        int32_t ui_elem_i = -1;
        for (
            ui_elem_i = 0;
            ui_elem_i < (int32_t)active_ui_elements_size;
            ui_elem_i++)
        {
            if (
                !active_ui_elements[ui_elem_i].deleted &&
                user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].
                    touchable_id ==
                active_ui_elements[ui_elem_i].touchable_id)
            {
                user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_START].
                    handled = true;
                break;
            }
        }
        
        if (ui_elem_i >= 0) {
            
            for (
                uint32_t zp_i = 0;
                zp_i < zpolygons_to_render->size;
                zp_i++)
            {
                if (
                    zpolygons_to_render->cpu_data[zp_i].object_id ==
                        currently_sliding_object_id)
                {
                    // set slider value
                    float new_x_offset =
                        screenspace_x_to_x(
                            user_interactions[
                                INTR_PREVIOUS_MOUSE_OR_TOUCH_MOVE].screen_x,
                            zpolygons_to_render->gpu_data[zp_i].xyz[0]) -
                        zpolygons_to_render->gpu_data[zp_i].xyz[0];
                    
                    if (
                        new_x_offset <
                            -active_ui_elements[ui_elem_i].slider_width / 2)
                    {
                        new_x_offset =
                            -active_ui_elements[ui_elem_i].slider_width / 2;
                    }
                    
                    if (
                        new_x_offset >
                            active_ui_elements[ui_elem_i].slider_width / 2)
                    {
                        new_x_offset =
                            active_ui_elements[ui_elem_i].slider_width / 2;
                    }
                    
                    zpolygons_to_render->gpu_data[zp_i].xyz_offset[0] =
                        new_x_offset;
                    
                    float slider_pct = (new_x_offset /
                        active_ui_elements[ui_elem_i].slider_width) + 0.5f;
                    
                    log_assert(
                        active_ui_elements[ui_elem_i].slider_linked_value !=
                            NULL);
                    *(active_ui_elements[ui_elem_i].slider_linked_value) =
                        active_ui_elements[ui_elem_i].slider_min +
                            ((active_ui_elements[ui_elem_i].slider_max -
                                active_ui_elements[ui_elem_i].slider_min) *
                                    slider_pct);
                }
            }
        }
    }
    
    if (
        !user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_END].handled &&
        currently_sliding_touchable_id >= 0 &&
        currently_sliding_object_id >= 0)
    {
        user_interactions[INTR_PREVIOUS_TOUCH_OR_LEFTCLICK_END].handled = true;
        
        currently_sliding_object_id = -1;
        currently_sliding_touchable_id = -1;
    }
}

void request_float_slider(
    const int32_t background_object_id,
    const int32_t pin_object_id,
    const float x_screenspace,
    const float y_screenspace,
    const float z,
    const float min_value,
    const float max_value,
    float * linked_value)
{
    log_assert(next_ui_element_settings != NULL);
    log_assert(background_object_id != pin_object_id);
    
    PolygonRequest slider_back;
    request_next_zpolygon(&slider_back);
    construct_quad_around(
        /* const float mid_x: */
            screenspace_x_to_x(x_screenspace, z),
        /* const float mid_y: */
            screenspace_y_to_y(y_screenspace, z),
        /* const float z: */
            z,
        /* const float width: */
            screenspace_width_to_width(
                next_ui_element_settings->slider_width_screenspace,
                z),
        /* const float height: */
            screenspace_height_to_height(
                next_ui_element_settings->slider_height_screenspace,
                z),
        /* zPolygon * recipient: */
            &slider_back);
    
    slider_back.cpu_data->object_id = background_object_id;
    
    slider_back.gpu_material[0].texturearray_i =
        next_ui_element_settings->slider_background_texturearray_i;
    slider_back.gpu_material[0].texture_i =
        next_ui_element_settings->slider_background_texture_i;
    slider_back.gpu_material[0].rgba[0] = 0.75f;
    slider_back.gpu_material[0].rgba[1] = 1.00f;
    slider_back.gpu_material[0].rgba[2] = 1.00f;
    slider_back.gpu_material[0].rgba[3] = 1.00f;
    
    slider_back.gpu_data->ignore_lighting =
        next_ui_element_settings->ignore_lighting;
    slider_back.gpu_data->ignore_camera =
        next_ui_element_settings->ignore_camera;
    log_assert(slider_back.cpu_data->visible);
    log_assert(!slider_back.cpu_data->committed);
    log_assert(!slider_back.cpu_data->deleted);
    commit_zpolygon_to_render(&slider_back);
    
    PolygonRequest slider_pin;
    request_next_zpolygon(&slider_pin);
    float pin_z = z - 0.005f;
    construct_quad_around(
        /* const float mid_x: */
            screenspace_x_to_x(x_screenspace, pin_z),
        /* const float mid_y: */
            screenspace_y_to_y(y_screenspace, pin_z),
        /* const float z: */
            pin_z,
        /* const float width: */
            screenspace_width_to_width(
                next_ui_element_settings->pin_width_screenspace,
                pin_z),
        /* const float height: */
            screenspace_height_to_height(
                next_ui_element_settings->pin_height_screenspace,
                pin_z),
        /* zPolygon * recipient: */
            &slider_pin);
    
    slider_pin.cpu_data->object_id = pin_object_id;
    
    if (*linked_value < min_value) {
        *linked_value = min_value;
    }
    if (*linked_value > max_value) {
        *linked_value = max_value;
    }
    
    float initial_slider_progress =
        (*linked_value - min_value) / (max_value - min_value);
    float initial_x_offset_screenspace =
        -(next_ui_element_settings->slider_width_screenspace / 2) +
        (initial_slider_progress *
            next_ui_element_settings->slider_width_screenspace);
    slider_pin.gpu_data->xyz_offset[0] =
        screenspace_width_to_width(initial_x_offset_screenspace, pin_z);
    
    slider_pin.gpu_data->xyz_offset[1] = 0.0f;
    
    slider_pin.gpu_material[0].texturearray_i =
        next_ui_element_settings->slider_pin_texturearray_i;
    slider_pin.gpu_material[0].texture_i =
        next_ui_element_settings->slider_pin_texture_i;
    slider_pin.gpu_material[0].rgba[0] = 1.0f;
    slider_pin.gpu_material[0].rgba[1] = 1.0f;
    slider_pin.gpu_material[0].rgba[2] = 1.0f;
    slider_pin.gpu_material[0].rgba[3] = 1.0f;
    
    slider_pin.gpu_data->ignore_lighting =
        next_ui_element_settings->ignore_lighting;
    slider_pin.gpu_data->ignore_camera =
        next_ui_element_settings->ignore_camera;
    slider_pin.cpu_data->touchable_id = next_ui_element_touchable_id();
    
    ActiveUIElement * next_active_element = next_active_ui_element();
    next_active_element->object_id = background_object_id;
    next_active_element->object_id_2 = pin_object_id;
    next_active_element->touchable_id = slider_pin.cpu_data->touchable_id;
    next_active_element->slider_width = 
        screenspace_width_to_width(
            next_ui_element_settings->slider_width_screenspace,
            z);
    next_active_element->slider_min = min_value;
    next_active_element->slider_max = max_value;
    next_active_element->slideable = true;
    next_active_element->deleted = false;
    next_active_element->slider_linked_value = linked_value;
    strcpy_capped(
        next_active_element->interaction_sound_filename,
        128,
        next_ui_element_settings->interaction_sound_filename);
    commit_zpolygon_to_render(&slider_pin);
}

void unregister_ui_element_with_object_id(
    const int32_t with_object_id)
{
    for (uint32_t i = 0; i < active_ui_elements_size; i++) {
        if (active_ui_elements[i].object_id == with_object_id) {
            active_ui_elements[i].deleted = true;
        }
    }
}

