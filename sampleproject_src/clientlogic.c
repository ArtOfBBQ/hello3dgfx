#include "clientlogic.h"

typedef struct SliderRequest {
    char label[64];
    int32_t sliderback_object_id;
    int32_t pin_object_id;
    float min_float_value;
    float max_float_value;
    int32_t min_int_value;
    int32_t max_int_value;
    int32_t * linked_int;
    float * linked_float;
} SliderRequest;

typedef struct SliderTitle {
    char title[64];
    int32_t object_id;
    float unoffset_y_screenspace;
} SliderTitle;

#define SLIDERS_SIZE 98
#define SLIDERTITLES_SIZE (SLIDERS_SIZE / 13)+1
static SliderRequest * slider_requests = NULL;
static SliderTitle slider_titles[SLIDERTITLES_SIZE];
static bool32_t update_slider_positions_and_label_vals = true;
static bool32_t full_redraw_sliders = false;

static void slider_callback_request_update(void) {
    update_slider_positions_and_label_vals = true;
}

static void save_particle_stats(void) {
    char writables_path[256];
    writables_path[0] = '\0';
    
    platform_get_writables_path(
        /* char * recipient: */
            writables_path,
        /* const uint32_t recipient_size: */
            256);
    
    char dir_sep[4];
    platform_get_directory_separator(dir_sep);
    
    char writables_filepath[256];
    strcpy_capped(writables_filepath, 256, writables_path);
    strcat_capped(writables_filepath, 256, dir_sep);
    strcat_capped(writables_filepath, 256, "particlestats.txt");
    
    if (platform_file_exists(writables_filepath)) {
        platform_delete_file(writables_filepath);
    }
    
    uint32_t good = 0;
    
    char * output = malloc_from_managed(1000000);
    
    output[0] = '\0';
    
    /*
    Reminder: Zpolygon struct layout
    
    float        xyz[3];
    float        xyz_angle[3];
    float        bonus_rgb[3];
    float        xyz_multiplier[3]; // determines width/height/depth
    float        xyz_offset[3];
    float        scale_factor;
    unsigned int ignore_lighting;
    unsigned int ignore_camera;
    float        simd_padding[6];
    */
    
    typedef struct DumpableStat {
        char name[128];
        GPUPolygon * source;
    } DumpableStat;
    
    DumpableStat * dumpable_stats = malloc_from_managed(
        sizeof(DumpableStat) * 10);
    
    dumpable_stats[0].source = &particle_effects[0].zpolygon_gpu;
    strcpy_capped(dumpable_stats[0].name, 128, "zpolygon_gpu");
    dumpable_stats[1].source = &particle_effects[0].gpustats_initial_random_add_1;
    strcpy_capped(dumpable_stats[1].name, 128, "gpustats_initial_random_add_1");
    dumpable_stats[2].source = &particle_effects[0].gpustats_initial_random_add_2;
    strcpy_capped(dumpable_stats[2].name, 128, "gpustats_initial_random_add_2");
    dumpable_stats[3].source = &particle_effects[0].gpustats_pertime_random_add_1;
    strcpy_capped(dumpable_stats[3].name, 128, "gpustats_pertime_random_add_1");
    dumpable_stats[4].source = &particle_effects[0].gpustats_pertime_random_add_2;
    strcpy_capped(dumpable_stats[4].name, 128, "gpustats_pertime_random_add_2");
    dumpable_stats[5].source = &particle_effects[0].gpustats_pertime_add;
    strcpy_capped(dumpable_stats[5].name, 128, "gpustats_pertime_add");
    dumpable_stats[6].source = &particle_effects[0].gpustats_perexptime_add;
    strcpy_capped(dumpable_stats[6].name, 128, "gpustats_perexptime_add");
    
    for (uint32_t stat_i = 0; stat_i < 7; stat_i++) {
        for (uint32_t m = 0; m < 3; m++) {
            strcat_capped(output, 1000000, "particle->");
            strcat_capped(output, 1000000, dumpable_stats[stat_i].name);
            strcat_capped(output, 1000000, ".xyz[");
            strcat_uint_capped(output, 1000000, m);
            strcat_capped(output, 1000000, "] = ");
            strcat_float_capped(
                output, 1000000, dumpable_stats[stat_i].source->xyz[m]);
            strcat_capped(output, 1000000, "\n");
        }
        for (uint32_t m = 0; m < 3; m++) {
            strcat_capped(output, 1000000, "particle->");
            strcat_capped(output, 1000000, dumpable_stats[stat_i].name);
            strcat_capped(output, 1000000, ".xyz_angle[");
            strcat_uint_capped(output, 1000000, m);
            strcat_capped(output, 1000000, "] = ");
            strcat_float_capped(
                output, 1000000, dumpable_stats[stat_i].source->xyz_angle[m]);
            strcat_capped(output, 1000000, "\n");
        }
        for (uint32_t m = 0; m < 3; m++) {
            strcat_capped(output, 1000000, "particle->");
            strcat_capped(output, 1000000, dumpable_stats[stat_i].name);
            strcat_capped(output, 1000000, ".bonus_rgb[");
            strcat_uint_capped(output, 1000000, m);
            strcat_capped(output, 1000000, "] = ");
            strcat_float_capped(
                output, 1000000, dumpable_stats[stat_i].source->bonus_rgb[m]);
            strcat_capped(output, 1000000, "\n");
        }
        for (uint32_t m = 0; m < 3; m++) {
            strcat_capped(output, 1000000, "particle->");
            strcat_capped(output, 1000000, dumpable_stats[stat_i].name);
            strcat_capped(output, 1000000, ".xyz_multiplier[");
            strcat_uint_capped(output, 1000000, m);
            strcat_capped(output, 1000000, "] = ");
            strcat_float_capped(
                output, 1000000, dumpable_stats[stat_i].source->xyz_multiplier[m]);
            strcat_capped(output, 1000000, "\n");
        }
        for (uint32_t m = 0; m < 3; m++) {
            strcat_capped(output, 1000000, "particle->");
            strcat_capped(output, 1000000, dumpable_stats[stat_i].name);
            strcat_capped(output, 1000000, ".xyz_offset[");
            strcat_uint_capped(output, 1000000, m);
            strcat_capped(output, 1000000, "] = ");
            strcat_float_capped(
                output, 1000000, dumpable_stats[stat_i].source->xyz_offset[m]);
            strcat_capped(output, 1000000, "\n");
        }
        strcat_capped(output, 1000000, "particle->");
        strcat_capped(output, 1000000, dumpable_stats[stat_i].name);
        strcat_capped(output, 1000000, ".scale_factor = ");
        strcat_float_capped(
            output, 1000000,dumpable_stats[stat_i].source->scale_factor);
        strcat_capped(output, 1000000, "\n");
    }
    
    strcat_capped(output, 1000000, "particle->lifespan = ");
    strcat_uint_capped(
        output, 1000000, (uint32_t)particle_effects[0].particle_lifespan);
    strcat_capped(output, 1000000, "\n");
    
    strcat_capped(output, 1000000, "particle->particle_spawns_per_second = ");
    strcat_uint_capped(
        output, 1000000, particle_effects[0].particle_spawns_per_second);
    strcat_capped(output, 1000000, "\n");
    
    strcat_capped(output, 1000000, "particle->pause_between_spawns = ");
    strcat_uint_capped(
        output, 1000000, (uint32_t)particle_effects[0].pause_between_spawns);
    strcat_capped(output, 1000000, "\n");
    
    for (uint32_t m = 0; m < 4; m++) {
        strcat_capped(output, 1000000, "particle->zpolygon_material.rgba[");
        strcat_uint_capped(output, 1000000, m);
        strcat_capped(output, 1000000, "] = ");
        strcat_float_capped(
            output,
            1000000,
            (uint32_t)particle_effects[0].zpolygon_material.rgba[m]);
        strcat_capped(output, 1000000, "\n");
    }
    
    platform_write_file(
        /* const char * filepath_destination: */
            writables_filepath,
        /* const char * output: */
            output,
        /* const uint32_t output_size: */
            get_string_length(output),
        /* uint32_t * good: */
            &good);
    assert(good);
    
    platform_open_folder_in_window_if_possible(writables_path);
    free_from_managed(output);
}

static float particle_y_offset = 0;
static int32_t slider_labels_object_id = -1;
void client_logic_startup(void) {
    
    slider_labels_object_id = next_nonui_object_id();
    
    slider_requests = malloc_from_unmanaged(
        sizeof(SliderRequest) * SLIDERS_SIZE);
    
    for (uint32_t i = 0; i < SLIDERS_SIZE; i++) {
        slider_requests[i].linked_float = NULL;
        slider_requests[i].linked_int = NULL;
    }
    
    strcpy_capped(slider_titles[0].title, 64, "per time add");
    strcpy_capped(slider_requests[0].label, 64, "X:");
    slider_requests[0].min_float_value = -5.0f;
    slider_requests[0].max_float_value =  5.0f;
    slider_requests[0].linked_float =
        &particle_effects[0].gpustats_pertime_add.xyz[0];
    
    strcpy_capped(slider_requests[1].label, 64, "Y:");
    slider_requests[1].min_float_value = -5.0f;
    slider_requests[1].max_float_value =  5.0f;
    slider_requests[1].linked_float =
        &particle_effects[0].gpustats_pertime_add.xyz[1];
    
    strcpy_capped(slider_requests[2].label, 64, "Z:");
    slider_requests[2].min_float_value = -2.5f;
    slider_requests[2].max_float_value =  2.5f;
    slider_requests[2].linked_float =
        &particle_effects[0].gpustats_pertime_add.xyz[2];
    
    strcpy_capped(slider_requests[3].label, 64, "X rot:");
    slider_requests[3].min_float_value = -2.7f;
    slider_requests[3].max_float_value =  2.7f;
    slider_requests[3].linked_float =
        &particle_effects[0].gpustats_pertime_add.xyz_angle[0];
    
    strcpy_capped(slider_requests[4].label, 64, "Y rot:");
    slider_requests[4].min_float_value = -2.7f;
    slider_requests[4].max_float_value =  2.7f;
    slider_requests[4].linked_float =
        &particle_effects[0].gpustats_pertime_add.xyz_angle[1];
    
    strcpy_capped(slider_requests[5].label, 64, "Z rot:");
    slider_requests[5].min_float_value = -2.7f;
    slider_requests[5].max_float_value =  2.7f;
    slider_requests[5].linked_float =
        &particle_effects[0].gpustats_pertime_add.xyz_angle[2];
    
    strcpy_capped(slider_requests[6].label, 64, "+R:");
    slider_requests[6].min_float_value = -2.0f;
    slider_requests[6].max_float_value =  2.0f;
    slider_requests[6].linked_float =
        &particle_effects[0].gpustats_pertime_add.bonus_rgb[0];
    
    strcpy_capped(slider_requests[7].label, 64, "+G:");
    slider_requests[7].min_float_value = -2.0f;
    slider_requests[7].max_float_value =  2.0f;
    slider_requests[7].linked_float =
        &particle_effects[0].gpustats_pertime_add.bonus_rgb[1];
    
    strcpy_capped(slider_requests[8].label, 64, "+B:");
    slider_requests[8].min_float_value = -2.0f;
    slider_requests[8].max_float_value =  2.0f;
    slider_requests[8].linked_float =
        &particle_effects[0].gpustats_pertime_add.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[9].label, 64, "+Width:");
    slider_requests[9].min_float_value = -0.25f;
    slider_requests[9].max_float_value =  0.25f;
    slider_requests[9].linked_float =
        &particle_effects[0].gpustats_pertime_add.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[10].label, 64, "+Height:");
    slider_requests[10].min_float_value = -0.25f;
    slider_requests[10].max_float_value =  0.25f;
    slider_requests[10].linked_float =
        &particle_effects[0].gpustats_pertime_add.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[11].label, 64, "+Depth:");
    slider_requests[11].min_float_value = -0.25f;
    slider_requests[11].max_float_value =  0.25f;
    slider_requests[11].linked_float =
        &particle_effects[0].gpustats_pertime_add.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[12].label, 64, "+Scale:");
    slider_requests[12].min_float_value = -0.50f;
    slider_requests[12].max_float_value =  0.50f;
    slider_requests[12].linked_float =
        &particle_effects[0].gpustats_pertime_add.scale_factor;
    
    strcpy_capped(slider_titles[1].title, 64, "init rand add 1");
    strcpy_capped(slider_requests[13].label, 64, "X:");
    slider_requests[13].min_float_value = -5.0f;
    slider_requests[13].max_float_value =  5.0f;
    slider_requests[13].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.xyz[0];
    
    strcpy_capped(slider_requests[14].label, 64, "Y:");
    slider_requests[14].min_float_value = -5.0f;
    slider_requests[14].max_float_value =  5.0f;
    slider_requests[14].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.xyz[1];
    
    strcpy_capped(slider_requests[15].label, 64, "Z:");
    slider_requests[15].min_float_value = -2.5f;
    slider_requests[15].max_float_value =  2.5f;
    slider_requests[15].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.xyz[2];
    
    strcpy_capped(slider_requests[16].label, 64, "X rot:");
    slider_requests[16].min_float_value = -2.7f;
    slider_requests[16].max_float_value =  2.7f;
    slider_requests[16].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_angle[0];
    
    strcpy_capped(slider_requests[17].label, 64, "Y rot:");
    slider_requests[17].min_float_value = -2.7f;
    slider_requests[17].max_float_value =  2.7f;
    slider_requests[17].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_angle[1];
    
    strcpy_capped(slider_requests[18].label, 64, "Z rot:");
    slider_requests[18].min_float_value = -2.7f;
    slider_requests[18].max_float_value =  2.7f;
    slider_requests[18].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_angle[2];
    
    strcpy_capped(slider_requests[19].label, 64, "+R:");
    slider_requests[19].min_float_value = -2.0f;
    slider_requests[19].max_float_value =  2.0f;
    slider_requests[19].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.bonus_rgb[0];
    
    strcpy_capped(slider_requests[20].label, 64, "+G:");
    slider_requests[20].min_float_value = -2.0f;
    slider_requests[20].max_float_value =  2.0f;
    slider_requests[20].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.bonus_rgb[1];
    
    strcpy_capped(slider_requests[21].label, 64, "+B:");
    slider_requests[21].min_float_value = -2.0f;
    slider_requests[21].max_float_value =  2.0f;
    slider_requests[21].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[22].label, 64, "+Width:");
    slider_requests[22].min_float_value = -0.25f;
    slider_requests[22].max_float_value =  0.25f;
    slider_requests[22].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[23].label, 64, "+Height:");
    slider_requests[23].min_float_value = -0.25f;
    slider_requests[23].max_float_value =  0.25f;
    slider_requests[23].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[24].label, 64, "+Depth:");
    slider_requests[24].min_float_value = -0.25f;
    slider_requests[24].max_float_value =  0.25f;
    slider_requests[24].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[25].label, 64, "+Scale:");
    slider_requests[25].min_float_value = -0.50f;
    slider_requests[25].max_float_value =  0.50f;
    slider_requests[25].linked_float =
        &particle_effects[0].gpustats_initial_random_add_1.scale_factor;
    
    strcpy_capped(slider_titles[2].title, 64, "init rand add 2");
    strcpy_capped(slider_requests[26].label, 64, "X:");
    slider_requests[26].min_float_value = -5.0f;
    slider_requests[26].max_float_value =  5.0f;
    slider_requests[26].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.xyz[0];
    
    strcpy_capped(slider_requests[27].label, 64, "Y:");
    slider_requests[27].min_float_value = -5.0f;
    slider_requests[27].max_float_value =  5.0f;
    slider_requests[27].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.xyz[1];
    
    strcpy_capped(slider_requests[28].label, 64, "Z:");
    slider_requests[28].min_float_value = -2.5f;
    slider_requests[28].max_float_value =  2.5f;
    slider_requests[28].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.xyz[2];
    
    strcpy_capped(slider_requests[29].label, 64, "X rot:");
    slider_requests[29].min_float_value = -2.7f;
    slider_requests[29].max_float_value =  2.7f;
    slider_requests[29].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_angle[0];
    
    strcpy_capped(slider_requests[30].label, 64, "Y rot:");
    slider_requests[30].min_float_value = -2.7f;
    slider_requests[30].max_float_value =  2.7f;
    slider_requests[30].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_angle[1];
    
    strcpy_capped(slider_requests[31].label, 64, "Z rot:");
    slider_requests[31].min_float_value = -2.7f;
    slider_requests[31].max_float_value =  2.7f;
    slider_requests[31].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_angle[2];
    
    strcpy_capped(slider_requests[32].label, 64, "+R:");
    slider_requests[32].min_float_value = -2.0f;
    slider_requests[32].max_float_value =  2.0f;
    slider_requests[32].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.bonus_rgb[0];
    
    strcpy_capped(slider_requests[33].label, 64, "+G:");
    slider_requests[33].min_float_value = -2.0f;
    slider_requests[33].max_float_value =  2.0f;
    slider_requests[33].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.bonus_rgb[1];
    
    strcpy_capped(slider_requests[34].label, 64, "+B:");
    slider_requests[34].min_float_value = -2.0f;
    slider_requests[34].max_float_value =  2.0f;
    slider_requests[34].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[35].label, 64, "+Width:");
    slider_requests[35].min_float_value = -0.25f;
    slider_requests[35].max_float_value =  0.25f;
    slider_requests[35].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[36].label, 64, "+Height:");
    slider_requests[36].min_float_value = -0.25f;
    slider_requests[36].max_float_value =  0.25f;
    slider_requests[36].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[37].label, 64, "+Depth:");
    slider_requests[37].min_float_value = -0.25f;
    slider_requests[37].max_float_value =  0.25f;
    slider_requests[37].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[38].label, 64, "+Scale:");
    slider_requests[38].min_float_value = -0.50f;
    slider_requests[38].max_float_value =  0.50f;
    slider_requests[38].linked_float =
        &particle_effects[0].gpustats_initial_random_add_2.scale_factor;
    
    strcpy_capped(slider_titles[3].title, 64, "+/time rand 1");
    strcpy_capped(slider_requests[39].label, 64, "X:");
    slider_requests[39].min_float_value = -5.0f;
    slider_requests[39].max_float_value =  5.0f;
    slider_requests[39].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz[0];
    
    strcpy_capped(slider_requests[40].label, 64, "Y:");
    slider_requests[40].min_float_value = -5.0f;
    slider_requests[40].max_float_value =  5.0f;
    slider_requests[40].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz[1];
    
    strcpy_capped(slider_requests[41].label, 64, "Z:");
    slider_requests[41].min_float_value = -2.5f;
    slider_requests[41].max_float_value =  2.5f;
    slider_requests[41].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz[2];
    
    strcpy_capped(slider_requests[42].label, 64, "X rot:");
    slider_requests[42].min_float_value = -2.7f;
    slider_requests[42].max_float_value =  2.7f;
    slider_requests[42].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_angle[0];
    
    strcpy_capped(slider_requests[43].label, 64, "Y rot:");
    slider_requests[43].min_float_value = -2.7f;
    slider_requests[43].max_float_value =  2.7f;
    slider_requests[43].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_angle[1];
    
    strcpy_capped(slider_requests[44].label, 64, "Z rot:");
    slider_requests[44].min_float_value = -2.7f;
    slider_requests[44].max_float_value =  2.7f;
    slider_requests[44].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_angle[2];
    
    strcpy_capped(slider_requests[45].label, 64, "+R:");
    slider_requests[45].min_float_value = -2.0f;
    slider_requests[45].max_float_value =  2.0f;
    slider_requests[45].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.bonus_rgb[0];
    
    strcpy_capped(slider_requests[46].label, 64, "+G:");
    slider_requests[46].min_float_value = -2.0f;
    slider_requests[46].max_float_value =  2.0f;
    slider_requests[46].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.bonus_rgb[1];
    
    strcpy_capped(slider_requests[47].label, 64, "+B:");
    slider_requests[47].min_float_value = -2.0f;
    slider_requests[47].max_float_value =  2.0f;
    slider_requests[47].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[48].label, 64, "+Width:");
    slider_requests[48].min_float_value = -0.25f;
    slider_requests[48].max_float_value =  0.25f;
    slider_requests[48].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[49].label, 64, "+Height:");
    slider_requests[49].min_float_value = -0.25f;
    slider_requests[49].max_float_value =  0.25f;
    slider_requests[49].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[50].label, 64, "+Depth:");
    slider_requests[50].min_float_value = -0.25f;
    slider_requests[50].max_float_value =  0.25f;
    slider_requests[50].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[51].label, 64, "+Scale:");
    slider_requests[51].min_float_value = -0.50f;
    slider_requests[51].max_float_value =  0.50f;
    slider_requests[51].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_1.scale_factor;
    
    strcpy_capped(slider_titles[4].title, 64, "+/time rand 2");
    strcpy_capped(slider_requests[52].label, 64, "X:");
    slider_requests[52].min_float_value = -5.0f;
    slider_requests[52].max_float_value =  5.0f;
    slider_requests[52].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz[0];
    
    strcpy_capped(slider_requests[53].label, 64, "Y:");
    slider_requests[53].min_float_value = -5.0f;
    slider_requests[53].max_float_value =  5.0f;
    slider_requests[53].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz[1];
    
    strcpy_capped(slider_requests[54].label, 64, "Z:");
    slider_requests[54].min_float_value = -2.5f;
    slider_requests[54].max_float_value =  2.5f;
    slider_requests[54].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz[2];
    
    strcpy_capped(slider_requests[55].label, 64, "X rot:");
    slider_requests[55].min_float_value = -2.7f;
    slider_requests[55].max_float_value =  2.7f;
    slider_requests[55].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_angle[0];
    
    strcpy_capped(slider_requests[56].label, 64, "Y rot:");
    slider_requests[56].min_float_value = -2.7f;
    slider_requests[56].max_float_value =  2.7f;
    slider_requests[56].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_angle[1];
    
    strcpy_capped(slider_requests[57].label, 64, "Z rot:");
    slider_requests[57].min_float_value = -2.7f;
    slider_requests[57].max_float_value =  2.7f;
    slider_requests[57].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_angle[2];
    
    strcpy_capped(slider_requests[58].label, 64, "+R:");
    slider_requests[58].min_float_value = -2.0f;
    slider_requests[58].max_float_value =  2.0f;
    slider_requests[58].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.bonus_rgb[0];
    
    strcpy_capped(slider_requests[59].label, 64, "+G:");
    slider_requests[59].min_float_value = -2.0f;
    slider_requests[59].max_float_value =  2.0f;
    slider_requests[59].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.bonus_rgb[1];
    
    strcpy_capped(slider_requests[60].label, 64, "+B:");
    slider_requests[60].min_float_value = -2.0f;
    slider_requests[60].max_float_value =  2.0f;
    slider_requests[60].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[61].label, 64, "+Width:");
    slider_requests[61].min_float_value = -0.25f;
    slider_requests[61].max_float_value =  0.25f;
    slider_requests[61].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[62].label, 64, "+Height:");
    slider_requests[62].min_float_value = -0.25f;
    slider_requests[62].max_float_value =  0.25f;
    slider_requests[62].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[63].label, 64, "+Depth:");
    slider_requests[63].min_float_value = -0.25f;
    slider_requests[63].max_float_value =  0.25f;
    slider_requests[63].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[64].label, 64, "+Scale:");
    slider_requests[64].min_float_value = -0.50f;
    slider_requests[64].max_float_value =  0.50f;
    slider_requests[64].linked_float =
        &particle_effects[0].gpustats_pertime_random_add_2.scale_factor;
    
    strcpy_capped(slider_titles[5].title, 64, "+/time^2");
    strcpy_capped(slider_requests[65].label, 64, "X:");
    slider_requests[65].min_float_value = -5.0f;
    slider_requests[65].max_float_value =  5.0f;
    slider_requests[65].linked_float =
        &particle_effects[0].gpustats_perexptime_add.xyz[0];
    
    strcpy_capped(slider_requests[66].label, 64, "Y:");
    slider_requests[66].min_float_value = -5.0f;
    slider_requests[66].max_float_value =  5.0f;
    slider_requests[66].linked_float =
        &particle_effects[0].gpustats_perexptime_add.xyz[1];
    
    strcpy_capped(slider_requests[67].label, 64, "Z:");
    slider_requests[67].min_float_value = -2.5f;
    slider_requests[67].max_float_value =  2.5f;
    slider_requests[67].linked_float =
        &particle_effects[0].gpustats_perexptime_add.xyz[2];
    
    strcpy_capped(slider_requests[68].label, 64, "X rot:");
    slider_requests[68].min_float_value = -2.7f;
    slider_requests[68].max_float_value =  2.7f;
    slider_requests[68].linked_float =
        &particle_effects[0].gpustats_perexptime_add.xyz_angle[0];
    
    strcpy_capped(slider_requests[69].label, 64, "Y rot:");
    slider_requests[69].min_float_value = -2.7f;
    slider_requests[69].max_float_value =  2.7f;
    slider_requests[69].linked_float =
        &particle_effects[0].gpustats_perexptime_add.xyz_angle[1];
    
    strcpy_capped(slider_requests[70].label, 64, "Z rot:");
    slider_requests[70].min_float_value = -2.7f;
    slider_requests[70].max_float_value =  2.7f;
    slider_requests[70].linked_float =
        &particle_effects[0].gpustats_perexptime_add.xyz_angle[2];
    
    strcpy_capped(slider_requests[71].label, 64, "+R:");
    slider_requests[71].min_float_value = -2.0f;
    slider_requests[71].max_float_value =  2.0f;
    slider_requests[71].linked_float =
        &particle_effects[0].gpustats_perexptime_add.bonus_rgb[0];
    
    strcpy_capped(slider_requests[72].label, 64, "+G:");
    slider_requests[72].min_float_value = -2.0f;
    slider_requests[72].max_float_value =  2.0f;
    slider_requests[72].linked_float =
        &particle_effects[0].gpustats_perexptime_add.bonus_rgb[1];
    
    strcpy_capped(slider_requests[73].label, 64, "+B:");
    slider_requests[73].min_float_value = -2.0f;
    slider_requests[73].max_float_value =  2.0f;
    slider_requests[73].linked_float =
        &particle_effects[0].gpustats_perexptime_add.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[74].label, 64, "+Width:");
    slider_requests[74].min_float_value = -0.25f;
    slider_requests[74].max_float_value =  0.25f;
    slider_requests[74].linked_float =
        &particle_effects[0].gpustats_perexptime_add.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[75].label, 64, "+Height:");
    slider_requests[75].min_float_value = -0.25f;
    slider_requests[75].max_float_value =  0.25f;
    slider_requests[75].linked_float =
        &particle_effects[0].gpustats_perexptime_add.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[76].label, 64, "+Depth:");
    slider_requests[76].min_float_value = -0.25f;
    slider_requests[76].max_float_value =  0.25f;
    slider_requests[76].linked_float =
        &particle_effects[0].gpustats_perexptime_add.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[77].label, 64, "+Scale:");
    slider_requests[77].min_float_value = -0.50f;
    slider_requests[77].max_float_value =  0.50f;
    slider_requests[77].linked_float =
        &particle_effects[0].gpustats_perexptime_add.scale_factor;
    
    strcpy_capped(slider_titles[6].title, 64, "Base Mesh");
    strcpy_capped(slider_requests[78].label, 64, "X:");
    slider_requests[78].min_float_value = -5.0f;
    slider_requests[78].max_float_value =  5.0f;
    slider_requests[78].linked_float =
        &particle_effects[0].zpolygon_gpu.xyz[0];
    
    strcpy_capped(slider_requests[79].label, 64, "Y:");
    slider_requests[79].min_float_value = -5.0f;
    slider_requests[79].max_float_value =  5.0f;
    slider_requests[79].linked_float =
        &particle_effects[0].zpolygon_gpu.xyz[1];
    
    strcpy_capped(slider_requests[80].label, 64, "Z:");
    slider_requests[80].min_float_value = -2.5f;
    slider_requests[80].max_float_value =  2.5f;
    slider_requests[80].linked_float =
        &particle_effects[0].zpolygon_gpu.xyz[2];
    
    strcpy_capped(slider_requests[81].label, 64, "X rot:");
    slider_requests[81].min_float_value = -2.7f;
    slider_requests[81].max_float_value =  2.7f;
    slider_requests[81].linked_float =
        &particle_effects[0].zpolygon_gpu.xyz_angle[0];
    
    strcpy_capped(slider_requests[82].label, 64, "Y rot:");
    slider_requests[82].min_float_value = -2.7f;
    slider_requests[82].max_float_value =  2.7f;
    slider_requests[82].linked_float =
        &particle_effects[0].zpolygon_gpu.xyz_angle[1];
    
    strcpy_capped(slider_requests[83].label, 64, "Z rot:");
    slider_requests[83].min_float_value = -2.7f;
    slider_requests[83].max_float_value =  2.7f;
    slider_requests[83].linked_float =
        &particle_effects[0].zpolygon_gpu.xyz_angle[2];
    
    strcpy_capped(slider_requests[84].label, 64, "+R:");
    slider_requests[84].min_float_value = -2.0f;
    slider_requests[84].max_float_value =  2.0f;
    slider_requests[84].linked_float =
        &particle_effects[0].zpolygon_gpu.bonus_rgb[0];
    
    strcpy_capped(slider_requests[85].label, 64, "+G:");
    slider_requests[85].min_float_value = -2.0f;
    slider_requests[85].max_float_value =  2.0f;
    slider_requests[85].linked_float =
        &particle_effects[0].zpolygon_gpu.bonus_rgb[1];
    
    strcpy_capped(slider_requests[86].label, 64, "+B:");
    slider_requests[86].min_float_value = -2.0f;
    slider_requests[86].max_float_value =  2.0f;
    slider_requests[86].linked_float =
        &particle_effects[0].zpolygon_gpu.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[87].label, 64, "+Width:");
    slider_requests[87].min_float_value = -0.25f;
    slider_requests[87].max_float_value =  0.25f;
    slider_requests[87].linked_float =
        &particle_effects[0].zpolygon_gpu.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[88].label, 64, "+Height:");
    slider_requests[88].min_float_value = -0.25f;
    slider_requests[88].max_float_value =  0.25f;
    slider_requests[88].linked_float =
        &particle_effects[0].zpolygon_gpu.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[89].label, 64, "+Depth:");
    slider_requests[89].min_float_value = -0.25f;
    slider_requests[89].max_float_value =  0.25f;
    slider_requests[89].linked_float =
        &particle_effects[0].zpolygon_gpu.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[90].label, 64, "+Scale:");
    slider_requests[90].min_float_value = -0.50f;
    slider_requests[90].max_float_value =  0.50f;
    slider_requests[90].linked_float =
        &particle_effects[0].zpolygon_gpu.scale_factor;
    
    // scale_factor
    strcpy_capped(slider_requests[91].label, 64, "Lifespan: ");
    slider_requests[91].min_int_value =        1;
    slider_requests[91].max_int_value =  4500000;
    slider_requests[91].linked_int    =
        (int32_t *)&particle_effects[0].particle_lifespan;
    
    strcpy_capped(slider_requests[92].label, 64, "Spawns/sec: ");
    slider_requests[92].min_int_value =        1;
    slider_requests[92].max_int_value =     3000;
    slider_requests[92].linked_int    =
        (int32_t *)&particle_effects[0].particle_spawns_per_second;
    
    strcpy_capped(slider_requests[93].label, 64, "Pause: ");
    slider_requests[93].min_int_value =        1;
    slider_requests[93].max_int_value =  9000000;
    slider_requests[93].linked_int    =
        (int32_t *)&particle_effects[0].pause_between_spawns;
    
    // Materials
    strcpy_capped(slider_requests[94].label, 64, "Material R:");
    slider_requests[94].min_float_value =  0.0f;
    slider_requests[94].max_float_value =  1.0f;
    slider_requests[94].linked_float =
        &particle_effects[0].zpolygon_material.rgba[0];
    
    strcpy_capped(slider_requests[95].label, 64, "Material G:");
    slider_requests[95].min_float_value =  0.0f;
    slider_requests[95].max_float_value =  1.0f;
    slider_requests[95].linked_float =
        &particle_effects[0].zpolygon_material.rgba[1];
    
    strcpy_capped(slider_requests[96].label, 64, "Material B:");
    slider_requests[96].min_float_value =  0.0f;
    slider_requests[96].max_float_value =  1.0f;
    slider_requests[96].linked_float =
        &particle_effects[0].zpolygon_material.rgba[2];
    
    strcpy_capped(slider_requests[97].label, 64, "Material A:");
    slider_requests[97].min_float_value =  0.0f;
    slider_requests[97].max_float_value =  1.0f;
    slider_requests[97].linked_float =
        &particle_effects[0].zpolygon_material.rgba[3];
    
    init_PNG_decoder(malloc_from_managed, free_from_managed, memset, memcpy);
    
    const char * fontfile = "font.png";
    if (platform_resource_exists("font.png")) {
        register_new_texturearray_by_splitting_file(
            /* filename : */ fontfile,
            /* rows     : */ 10,
            /* columns  : */ 10);
    }
    
    char * textures[1];
    textures[0] = "blob1.png";
    register_new_texturearray_from_files(
        /* const char ** filenames: */
            (const char **)textures,
        /* const uint32_t filenames_size: */
            1);
    
    zLightSource * light = next_zlight();
    light->RGBA[0]       =  0.50f;
    light->RGBA[1]       =  0.15f;
    light->RGBA[2]       =  0.15f;
    light->RGBA[3]       =  1.00f;
    light->ambient       =  1.00f;
    light->diffuse       =  1.00f;
    light->reach         =  5.00f;
    light->x             = -2.00f;
    light->y             =  0.50f;
    light->z             =  0.75f;
    commit_zlight(light);
    
    for (uint32_t i = 0; i < SLIDERS_SIZE; i++) {
        if (i % 13 == 0 && i < 92) {
            slider_titles[i / 13].object_id = next_nonui_object_id();
        }
        
        slider_requests[i].sliderback_object_id = next_ui_element_object_id();
        slider_requests[i].pin_object_id = next_ui_element_object_id();;
    }
    
    full_redraw_sliders = true;
}

void client_logic_threadmain(int32_t threadmain_id) {
    switch (threadmain_id) {
        default:
            log_append("unhandled threadmain_id: ");
            log_append_int(threadmain_id);
            log_append("\n");
    }
}

void client_logic_animation_callback(
    const int32_t callback_id,
    const float arg_1,
    const float arg_2,
    const int32_t arg_3)
{
    #ifndef LOGGER_IGNORE_ASSERTS
    char unhandled_callback_id[256];
    strcpy_capped(
        unhandled_callback_id,
        256,
        "unhandled client_logic_animation_callback: ");
    strcat_int_capped(
        unhandled_callback_id,
        256,
        callback_id);
    strcat_capped(
        unhandled_callback_id,
        256,
        ". Find in clientlogic.c -> client_logic_animation_callback\n");
    log_append(unhandled_callback_id);
    log_dump_and_crash(unhandled_callback_id);
    #endif
}

static void client_handle_keypresses(
    uint64_t microseconds_elapsed)
{
    float elapsed_mod = (float)((double)microseconds_elapsed / (double)16666);
    float cam_speed = 0.1f * elapsed_mod;
    float cam_rotation_speed = 0.05f * elapsed_mod;
    
    if (keypress_map[TOK_KEY_OPENSQUAREBRACKET] == true)
    {
        particle_y_offset -= 15.0f;
        update_slider_positions_and_label_vals = true;
    }
    
    if (keypress_map[TOK_KEY_CLOSESQUAREBRACKET] == true)
    {
        particle_y_offset += 15.0f;
        update_slider_positions_and_label_vals = true;
    }
    
    if (keypress_map[TOK_KEY_LEFTARROW] == true)
    {
        camera.x -= cam_speed;
    }
    
    if (keypress_map[TOK_KEY_RIGHTARROW] == true)
    {
        camera.x += cam_speed;
    }
    
    if (keypress_map[TOK_KEY_DOWNARROW] == true)
    {
        camera.y -= cam_speed;
    }
    
    if (keypress_map[TOK_KEY_UPARROW] == true)
    {
        camera.y += cam_speed;
    }
    
    if (keypress_map[TOK_KEY_A] == true) {
        camera.x_angle += cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_Z] == true) {
        camera.z_angle -= cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_X] == true) {
        camera.z_angle += cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_Q] == true) {
        camera.x_angle -= cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_W] == true) {
        camera.y_angle -= cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_S] == true) {
        camera.y_angle += cam_rotation_speed;
    }
    
    if (keypress_map[TOK_KEY_L] == true) {
        keypress_map[TOK_KEY_L] = false;
        LineParticle * lines = next_lineparticle_effect();
        PolygonRequest lines_polygon;
        lines_polygon.cpu_data = &lines->zpolygon_cpu;
        lines_polygon.gpu_data = &lines->zpolygon_gpu;
        lines_polygon.gpu_material = &lines->zpolygon_material;
        construct_quad(
            /* const float left_x: */
                0.0f,
            /* const float bottom_y: */
                0.0f,
            /* const float z: */
                0.5f,
            /* const float width: */
                screenspace_width_to_width(75.0f, 0.5f),
            /* const float height: */
                screenspace_height_to_height(75.0f, 0.5f),
            /* PolygonRequest * stack_recipient: */
                &lines_polygon);
        lines_polygon.gpu_data->ignore_camera = false;
        lines_polygon.gpu_data->ignore_lighting = true;
        
        lines->zpolygon_material.texturearray_i = 1;
        lines->zpolygon_material.texture_i = 0;
        
        lines_polygon.cpu_data->committed = true;
        lines->waypoint_duration[0] = 1250000;
        lines->waypoint_x[0] = screenspace_x_to_x(
            /* const float screenspace_x: */
                0,
            /* const float given_z: */
                0.5f);
        lines->waypoint_y[0] = screenspace_y_to_y(
            /* const float screenspace_y: */
                0,
            /* const float given_z: */
                0.5f);
        lines->waypoint_z[0] = 0.5f;
        lines->waypoint_r[0] = 0.8f;
        lines->waypoint_g[0] = 0.1f;
        lines->waypoint_b[0] = 0.1f;
        lines->waypoint_a[0] = 1.0f;
        lines->waypoint_scalefactor[0] = 1.0f;
        lines->waypoint_duration[0] = 350000;
        
        lines->waypoint_x[1] = screenspace_x_to_x(
            /* const float screenspace_x: */
                window_globals->window_width,
            /* const float given_z: */
                0.5f);
        lines->waypoint_y[1] = screenspace_y_to_y(
            /* const float screenspace_y: */
                0,
            /* const float given_z: */
                0.5f);
        lines->waypoint_z[1] = 0.5f;
        
        lines->waypoint_r[1] = 0.4f;
        lines->waypoint_g[1] = 0.8f;
        lines->waypoint_b[1] = 0.2f;
        lines->waypoint_a[1] = 1.0f;
        lines->waypoint_scalefactor[1] = 0.85f;
        lines->waypoint_duration[1] = 350000;
                
        lines->trail_delay = 500000;
        lines->waypoints_size = 2;
        lines->particle_count = 50;
        lines->particle_zangle_variance_pct = 15;
        lines->particle_rgb_variance_pct = 15;
        lines->particle_scalefactor_variance_pct = 35;
        commit_lineparticle_effect(lines);
    }
    
    if (keypress_map[TOK_KEY_T] == true) {
        // particle effect for object id 321
        for (uint32_t zp_i = 0; zp_i < zpolygons_to_render->size; zp_i++) {
            if (zpolygons_to_render->cpu_data[zp_i].object_id != 321) {
                continue;
            }
            
            ShatterEffect * shatter = next_shatter_effect();
            shatter->zpolygon_to_shatter_cpu =
                zpolygons_to_render->cpu_data[zp_i];
            zpolygons_to_render->cpu_data[zp_i].deleted = true;
            shatter->zpolygon_to_shatter_gpu =
                zpolygons_to_render->gpu_data[zp_i];
            shatter->zpolygon_to_shatter_material =
                zpolygons_to_render->gpu_materials[zp_i * MAX_MATERIALS_SIZE];
            
            shatter->longest_random_delay_before_launch = 5000000;
            shatter->linear_direction[0] = 0.8f;
            shatter->linear_direction[1] = 0.1f;
            shatter->linear_direction[2] = 0.1f;
            shatter->linear_distance_per_second = 0.05f;
            shatter->exploding_distance_per_second = 0.05f;
            commit_shatter_effect(shatter);
        }
    }
    
    if (keypress_map[TOK_KEY_BACKSLASH] == true) {
        // / key
        camera.z -= 0.01f;
    }
    
    if (keypress_map[TOK_KEY_UNDERSCORE] == true) {
        camera.z += 0.01f;
    }
}

void client_logic_update(uint64_t microseconds_elapsed)
{
    request_fps_counter(microseconds_elapsed);
    
    client_handle_keypresses(microseconds_elapsed);
    
    if (full_redraw_sliders) {
        font_height = 14;
        for (uint32_t i = 0; i < SLIDERS_SIZE; i++) {
            if (i % 13 == 0 && i < 91) {
                slider_titles[i / 13].unoffset_y_screenspace =
                    window_globals->window_height -
                        (font_height * 2.0f) -
                        ((i + 7.0f) * 22.0f);
            }
        }
        
        ParticleEffect particles;
        construct_particle_effect(&particles);
        
        particles.zpolygon_material.rgba[0] = 0.3f;
        particles.zpolygon_material.rgba[1] = 0.3f;
        particles.zpolygon_material.rgba[2] = 0.3f;
        particles.zpolygon_material.rgba[3] = 1.0f;
        
        particles.zpolygon_cpu.mesh_id              =      1; // hardcoded cube
        particles.zpolygon_gpu.xyz[0]               =   0.0f;
        particles.zpolygon_gpu.xyz[1]               =   0.0f;
        particles.zpolygon_gpu.xyz[2]               =   2.0f;
        particles.gpustats_pertime_add.xyz[0]       =  0.00f;
        particles.gpustats_pertime_add.xyz[1]       =  0.00f;
        particles.gpustats_pertime_add.xyz[2]       =  0.00f;
        particles.gpustats_pertime_add.xyz_angle[0] =  0.00f;
        particles.gpustats_pertime_add.xyz_angle[1] =  0.00f;
        particles.gpustats_pertime_add.xyz_angle[2] =  0.00f;
        particles.gpustats_pertime_add.bonus_rgb[0] =  0.00f;
        particles.gpustats_pertime_add.bonus_rgb[1] =  0.00f;
        particles.gpustats_pertime_add.bonus_rgb[2] =  0.00f;
        
        particles.gpustats_pertime_random_add_1.xyz[0] =  0.0f;
        particles.gpustats_pertime_random_add_1.xyz[1] =  0.0f;
        particles.gpustats_pertime_random_add_1.xyz[2] =  0.0f;
        particles.gpustats_pertime_random_add_2.xyz[0] =  0.0f;
        particles.gpustats_pertime_random_add_2.xyz[1] =  0.0f;
        particles.gpustats_pertime_random_add_2.xyz[2] =  0.0f;
        
        particles.particle_spawns_per_second   =    1000;
        particles.particle_lifespan            = 2500000;
        particles.random_texturearray_i[0]     =      -1;
        particles.random_texture_i[0]          =      -1;
        particles.random_textures_size         =       1;
        request_particle_effect(&particles);
        
        next_ui_element_settings->slider_width_screenspace         =   200;
        next_ui_element_settings->slider_height_screenspace        =    15;
        next_ui_element_settings->pin_width_screenspace            =    20;
        next_ui_element_settings->pin_height_screenspace           =    20;
        next_ui_element_settings->ignore_lighting                  =  true;
        next_ui_element_settings->ignore_camera                    = false;
        next_ui_element_settings->slider_background_texturearray_i =    -1;
        next_ui_element_settings->slider_background_texture_i      =    -1;
        next_ui_element_settings->slider_pin_texturearray_i        =    -1;
        next_ui_element_settings->slider_pin_texture_i             =    -1;
        next_ui_element_settings->slider_background_rgba[0]        =  0.4f;
        next_ui_element_settings->slider_background_rgba[1]        =  0.0f;
        next_ui_element_settings->slider_background_rgba[2]        =  0.9f;
        next_ui_element_settings->slider_background_rgba[3]        =  1.0f;
        next_ui_element_settings->slider_pin_rgba[0]               =  0.5f;
        next_ui_element_settings->slider_pin_rgba[1]               =  1.0f;
        next_ui_element_settings->slider_pin_rgba[2]               =  0.0f;
        next_ui_element_settings->slider_pin_rgba[3]               =  1.0f;
        next_ui_element_settings->ignore_camera                    = true;
        next_ui_element_settings->ignore_lighting                  = true;
        assert(particle_effects_size == 1);
        
        next_ui_element_settings->slider_slid_funcptr =
            slider_callback_request_update;
        
        for (uint32_t i = 0; i < SLIDERS_SIZE; i++) {
            font_height   =   14;
            font_color[0] = 0.5f;
            font_color[1] = 1.0f;
            font_color[2] = 0.0f;
            font_color[3] = 1.0f;
            
            next_ui_element_settings->slider_background_rgba[0] =
                (i / 13) * 0.15f;
            next_ui_element_settings->slider_background_rgba[1] =
                (i / 26) * 0.30f;
            next_ui_element_settings->slider_background_rgba[2] =
                1.0f - ((i / 13) * 0.15f);
            
            float slider_x_screenspace = window_globals->window_width -
                next_ui_element_settings->slider_width_screenspace -
                    (font_height * 3);
            
            float slider_y_screenspace = window_globals->window_height -
                (font_height * 2) -
                (i * 22) +
                particle_y_offset;
            
            if (slider_requests[i].linked_float != NULL) {
                request_float_slider(
                    /* const int32_t background_object_id: */
                        slider_requests[i].sliderback_object_id,
                    /* const int32_t pin_object_id: */
                        slider_requests[i].pin_object_id,
                    /* const float x_screenspace: */
                        slider_x_screenspace,
                    /* const float y_screenspace: */
                        slider_y_screenspace,
                    /* const float z: */
                        0.75f,
                    /* const float min_value: */
                        slider_requests[i].min_float_value,
                    /* const float max_value: */
                        slider_requests[i].max_float_value,
                    /* float * linked_value: */
                        slider_requests[i].linked_float);
            } else {
                log_assert(slider_requests[i].linked_int != NULL);
                
                request_int_slider(
                    /* const int32_t background_object_id: */
                        slider_requests[i].sliderback_object_id,
                    /* const int32_t pin_object_id: */
                        slider_requests[i].pin_object_id,
                    /* const float x_screenspace: */
                        slider_x_screenspace,
                    /* const float y_screenspace: */
                        slider_y_screenspace,
                    /* const float z: */
                        0.75f,
                    /* const int32_t min_value: */
                        slider_requests[i].min_int_value,
                    /* const int32_t max_value: */
                        slider_requests[i].max_int_value,
                    /* float * linked_value: */
                        slider_requests[i].linked_int);
            }
            
            if (i % 13 == 0 && i < 91) {
                float prev_font_height = 14;
                font_height = 20;
                font_color[0] = next_ui_element_settings->slider_background_rgba[0];
                font_color[1] = next_ui_element_settings->slider_background_rgba[1];
                font_color[2] = next_ui_element_settings->slider_background_rgba[2];
                font_color[3] = 1.0f;
                
                request_label_renderable(
                    /* const int32_t with_object_id: */
                        slider_titles[i / 13].object_id,
                    /* const char * text_to_draw: */
                        slider_titles[i / 13].title,
                    /* const float left_pixelspace: */
                        window_globals->window_width -
                            next_ui_element_settings->slider_width_screenspace -
                            (next_ui_element_settings->slider_width_screenspace / 2) -
                                ((prev_font_height * 9)/2),
                    /* const float top_pixelspace: */
                        slider_titles[i / 13].unoffset_y_screenspace +
                            particle_y_offset,
                    /* const float z: */
                        0.75f,
                    /* const float max_width: */
                        500,
                    /* const uint32_t ignore_camera: */
                        true);
                
                ScheduledAnimation * anim = next_scheduled_animation();
                anim->affected_object_id = slider_titles[i / 13].object_id;
                anim->final_z_angle_known = true;
                anim->final_z_angle = 1.58f;
                anim->duration_microseconds = 1;
                commit_scheduled_animation(anim);
            }
        }
        
        next_ui_element_settings->slider_slid_funcptr = NULL;
        
        // save button;
        font_height = 14;
        next_ui_element_settings->ignore_camera = true;
        next_ui_element_settings->ignore_lighting = true;
        next_ui_element_settings->button_width_screenspace = 115.0f;
        next_ui_element_settings->button_height_screenspace = 40.0f;
        next_ui_element_settings->button_background_rgba[0] = 0.2f;
        next_ui_element_settings->button_background_rgba[1] = 0.3f;
        next_ui_element_settings->button_background_rgba[2] = 1.0f;
        next_ui_element_settings->button_background_rgba[3] = 1.0f;
        next_ui_element_settings->button_background_texturearray_i = -1;
        next_ui_element_settings->button_background_texture_i = -1;
        request_button(
            /* const int32_t button_object_id: */
                next_ui_element_object_id(),
            /* const char * label: */
                "Save to .txt",
            /* const float x_screenspace: */
                (next_ui_element_settings->button_width_screenspace / 2) + 140,
            /* const float y_screenspace: */
                (next_ui_element_settings->button_height_screenspace / 2) + 10,
            /* const float z: */
                1.00f,
            /* void (* funtion_pointer)(void): */
                save_particle_stats);
        
        full_redraw_sliders = false;
    }
    
    if (!update_slider_positions_and_label_vals) { return; }
    
    delete_zpolygon_object(slider_labels_object_id);
    update_slider_positions_and_label_vals = false;
    next_ui_element_settings->ignore_camera = true;
    next_ui_element_settings->ignore_lighting = true;
    font_height = 14;
    font_color[0] = 0.5f;
    font_color[1] = 1.0f;
    font_color[2] = 0.5f;
    font_color[3] = 1.0f;
    for (uint32_t i = 0; i < SLIDERS_SIZE; i++) {
        
        for (
            uint32_t zp_i = 0;
            zp_i < zpolygons_to_render->size;
            zp_i++)
        {
            if (i % 13 == 0) {
                if (slider_titles[i / 13].object_id ==
                    zpolygons_to_render->cpu_data[zp_i].object_id)
                {
                    zpolygons_to_render->gpu_data[zp_i].xyz[1] =
                        screenspace_y_to_y(
                            slider_titles[i / 13].unoffset_y_screenspace +
                                particle_y_offset,
                            0.75f);
                }
            }
            
            if (
                (zpolygons_to_render->cpu_data[zp_i].object_id ==
                    slider_requests[i].pin_object_id) ||
                (zpolygons_to_render->cpu_data[zp_i].object_id ==
                    slider_requests[i].sliderback_object_id))
            {
                zpolygons_to_render->gpu_data[zp_i].xyz[1] =
                    screenspace_y_to_y(
                        window_globals->window_height -
                            (font_height * 2) -
                            (i * 22) +
                            particle_y_offset,
                        0.75f);
            }
        }
        
        char label_and_num[128];
        strcpy_capped(label_and_num, 128, slider_requests[i].label);
        strcat_capped(label_and_num, 128, " ");
        
        if (slider_requests[i].linked_float != NULL) {
            strcat_float_capped(
            label_and_num,
            128,
            *slider_requests[i].linked_float);
        } else {
            log_assert(slider_requests[i].linked_int != NULL);
            strcat_int_capped(
            label_and_num,
            128,
            *slider_requests[i].linked_int);
        }
        
        request_label_renderable(
            /* const int32_t with_object_id: */
                slider_labels_object_id,
            /* const char * text_to_draw: */
                label_and_num,
            /* const float left_pixelspace: */
                (window_globals->window_width -
                    next_ui_element_settings->slider_width_screenspace) -
                    (font_height * 3) +
                    (next_ui_element_settings->slider_width_screenspace / 2) +
                    (font_height / 2),
            /* const float top_pixelspace: */
                window_globals->window_height -
                    (font_height * 2) -
                    (i * 22) +
                    (font_height / 2) +
                    particle_y_offset,
            /* const float z: */
                0.75f,
            /* const float max_width: */
                500,
            /* const uint32_t ignore_camera: */
                next_ui_element_settings->ignore_camera);
    }
}

void client_logic_evaluate_terminal_command(
    char * command,
    char * response,
    const uint32_t response_cap)
{
    if (are_equal_strings(command, "EXAMPLE COMMAND")) {
        strcpy_capped(response, response_cap, "Hello from clientlogic!");
        return;
    }
    
    strcpy_capped(
        response,
        response_cap,
        "Unrecognized command - see client_logic_evaluate_terminal_command() "
        "in clientlogic.c");
}

void client_logic_window_resize(
    const uint32_t new_height,
    const uint32_t new_width)
{
    // You're notified that the window is resized!
    full_redraw_sliders = true;
    update_slider_positions_and_label_vals = true;
}

void client_logic_shutdown(void) {
    // Your application shutdown code goes here!
}
