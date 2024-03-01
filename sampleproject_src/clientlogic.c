#include "clientlogic.h"

typedef struct SliderRequest {
    char label[64];
    float min_value;
    float max_value;
    float * linked_value;
} SliderRequest;

#define SLIDERS_SIZE 78
static SliderRequest * slider_requests = NULL;
static char slider_titles[(SLIDERS_SIZE / 13)+1][64];

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
    
    uint32_t good = 0;
    char * output = "I'm a particle stat list!\nHere's some stats <3";
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
}

void client_logic_startup(void) {
    
    slider_requests = malloc_from_unmanaged(
        sizeof(SliderRequest) * SLIDERS_SIZE);
    
    strcpy_capped(slider_titles[0], 64, "per time add");
    strcpy_capped(slider_requests[0].label, 64, "X:");
    slider_requests[0].min_value = -5.0f;
    slider_requests[0].max_value =  5.0f;
    slider_requests[0].linked_value =
        &particle_effects[0].gpustats_pertime_add.xyz[0];
    
    strcpy_capped(slider_requests[1].label, 64, "Y:");
    slider_requests[1].min_value = -5.0f;
    slider_requests[1].max_value =  5.0f;
    slider_requests[1].linked_value =
        &particle_effects[0].gpustats_pertime_add.xyz[1];
    
    strcpy_capped(slider_requests[2].label, 64, "Z:");
    slider_requests[2].min_value = -2.5f;
    slider_requests[2].max_value =  2.5f;
    slider_requests[2].linked_value =
        &particle_effects[0].gpustats_pertime_add.xyz[2];
    
    strcpy_capped(slider_requests[3].label, 64, "X rot:");
    slider_requests[3].min_value = -2.7f;
    slider_requests[3].max_value =  2.7f;
    slider_requests[3].linked_value =
        &particle_effects[0].gpustats_pertime_add.xyz_angle[0];
    
    strcpy_capped(slider_requests[4].label, 64, "Y rot:");
    slider_requests[4].min_value = -2.7f;
    slider_requests[4].max_value =  2.7f;
    slider_requests[4].linked_value =
        &particle_effects[0].gpustats_pertime_add.xyz_angle[1];
    
    strcpy_capped(slider_requests[5].label, 64, "Z rot:");
    slider_requests[5].min_value = -2.7f;
    slider_requests[5].max_value =  2.7f;
    slider_requests[5].linked_value =
        &particle_effects[0].gpustats_pertime_add.xyz_angle[2];
    
    strcpy_capped(slider_requests[6].label, 64, "+R:");
    slider_requests[6].min_value = -2.0f;
    slider_requests[6].max_value =  2.0f;
    slider_requests[6].linked_value =
        &particle_effects[0].gpustats_pertime_add.bonus_rgb[0];
    
    strcpy_capped(slider_requests[7].label, 64, "+G:");
    slider_requests[7].min_value = -2.0f;
    slider_requests[7].max_value =  2.0f;
    slider_requests[7].linked_value =
        &particle_effects[0].gpustats_pertime_add.bonus_rgb[1];
    
    strcpy_capped(slider_requests[8].label, 64, "+B:");
    slider_requests[8].min_value = -2.0f;
    slider_requests[8].max_value =  2.0f;
    slider_requests[8].linked_value =
        &particle_effects[0].gpustats_pertime_add.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[9].label, 64, "+Width:");
    slider_requests[9].min_value = -0.25f;
    slider_requests[9].max_value =  0.25f;
    slider_requests[9].linked_value =
        &particle_effects[0].gpustats_pertime_add.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[10].label, 64, "+Height:");
    slider_requests[10].min_value = -0.25f;
    slider_requests[10].max_value =  0.25f;
    slider_requests[10].linked_value =
        &particle_effects[0].gpustats_pertime_add.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[11].label, 64, "+Depth:");
    slider_requests[11].min_value = -0.25f;
    slider_requests[11].max_value =  0.25f;
    slider_requests[11].linked_value =
        &particle_effects[0].gpustats_pertime_add.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[12].label, 64, "+Scale:");
    slider_requests[12].min_value = -0.50f;
    slider_requests[12].max_value =  0.50f;
    slider_requests[12].linked_value =
        &particle_effects[0].gpustats_pertime_add.scale_factor;
    
    strcpy_capped(slider_titles[1], 64, "init rand add 1");
    strcpy_capped(slider_requests[13].label, 64, "X:");
    slider_requests[13].min_value = -5.0f;
    slider_requests[13].max_value =  5.0f;
    slider_requests[13].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.xyz[0];
    
    strcpy_capped(slider_requests[14].label, 64, "Y:");
    slider_requests[14].min_value = -5.0f;
    slider_requests[14].max_value =  5.0f;
    slider_requests[14].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.xyz[1];
    
    strcpy_capped(slider_requests[15].label, 64, "Z:");
    slider_requests[15].min_value = -2.5f;
    slider_requests[15].max_value =  2.5f;
    slider_requests[15].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.xyz[2];
    
    strcpy_capped(slider_requests[16].label, 64, "X rot:");
    slider_requests[16].min_value = -2.7f;
    slider_requests[16].max_value =  2.7f;
    slider_requests[16].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_angle[0];
    
    strcpy_capped(slider_requests[17].label, 64, "Y rot:");
    slider_requests[17].min_value = -2.7f;
    slider_requests[17].max_value =  2.7f;
    slider_requests[17].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_angle[1];
    
    strcpy_capped(slider_requests[18].label, 64, "Z rot:");
    slider_requests[18].min_value = -2.7f;
    slider_requests[18].max_value =  2.7f;
    slider_requests[18].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_angle[2];
    
    strcpy_capped(slider_requests[19].label, 64, "+R:");
    slider_requests[19].min_value = -2.0f;
    slider_requests[19].max_value =  2.0f;
    slider_requests[19].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.bonus_rgb[0];
    
    strcpy_capped(slider_requests[20].label, 64, "+G:");
    slider_requests[20].min_value = -2.0f;
    slider_requests[20].max_value =  2.0f;
    slider_requests[20].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.bonus_rgb[1];
    
    strcpy_capped(slider_requests[21].label, 64, "+B:");
    slider_requests[21].min_value = -2.0f;
    slider_requests[21].max_value =  2.0f;
    slider_requests[21].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[22].label, 64, "+Width:");
    slider_requests[22].min_value = -0.25f;
    slider_requests[22].max_value =  0.25f;
    slider_requests[22].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[23].label, 64, "+Height:");
    slider_requests[23].min_value = -0.25f;
    slider_requests[23].max_value =  0.25f;
    slider_requests[23].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[24].label, 64, "+Depth:");
    slider_requests[24].min_value = -0.25f;
    slider_requests[24].max_value =  0.25f;
    slider_requests[24].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[25].label, 64, "+Scale:");
    slider_requests[25].min_value = -0.50f;
    slider_requests[25].max_value =  0.50f;
    slider_requests[25].linked_value =
        &particle_effects[0].gpustats_initial_random_add_1.scale_factor;
    
    strcpy_capped(slider_titles[2], 64, "init rand add 2");
    strcpy_capped(slider_requests[26].label, 64, "X:");
    slider_requests[26].min_value = -5.0f;
    slider_requests[26].max_value =  5.0f;
    slider_requests[26].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.xyz[0];
    
    strcpy_capped(slider_requests[27].label, 64, "Y:");
    slider_requests[27].min_value = -5.0f;
    slider_requests[27].max_value =  5.0f;
    slider_requests[27].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.xyz[1];
    
    strcpy_capped(slider_requests[28].label, 64, "Z:");
    slider_requests[28].min_value = -2.5f;
    slider_requests[28].max_value =  2.5f;
    slider_requests[28].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.xyz[2];
    
    strcpy_capped(slider_requests[29].label, 64, "X rot:");
    slider_requests[29].min_value = -2.7f;
    slider_requests[29].max_value =  2.7f;
    slider_requests[29].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_angle[0];
    
    strcpy_capped(slider_requests[30].label, 64, "Y rot:");
    slider_requests[30].min_value = -2.7f;
    slider_requests[30].max_value =  2.7f;
    slider_requests[30].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_angle[1];
    
    strcpy_capped(slider_requests[31].label, 64, "Z rot:");
    slider_requests[31].min_value = -2.7f;
    slider_requests[31].max_value =  2.7f;
    slider_requests[31].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_angle[2];
    
    strcpy_capped(slider_requests[32].label, 64, "+R:");
    slider_requests[32].min_value = -2.0f;
    slider_requests[32].max_value =  2.0f;
    slider_requests[32].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.bonus_rgb[0];
    
    strcpy_capped(slider_requests[33].label, 64, "+G:");
    slider_requests[33].min_value = -2.0f;
    slider_requests[33].max_value =  2.0f;
    slider_requests[33].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.bonus_rgb[1];
    
    strcpy_capped(slider_requests[34].label, 64, "+B:");
    slider_requests[34].min_value = -2.0f;
    slider_requests[34].max_value =  2.0f;
    slider_requests[34].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[35].label, 64, "+Width:");
    slider_requests[35].min_value = -0.25f;
    slider_requests[35].max_value =  0.25f;
    slider_requests[35].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[36].label, 64, "+Height:");
    slider_requests[36].min_value = -0.25f;
    slider_requests[36].max_value =  0.25f;
    slider_requests[36].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[37].label, 64, "+Depth:");
    slider_requests[37].min_value = -0.25f;
    slider_requests[37].max_value =  0.25f;
    slider_requests[37].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[38].label, 64, "+Scale:");
    slider_requests[38].min_value = -0.50f;
    slider_requests[38].max_value =  0.50f;
    slider_requests[38].linked_value =
        &particle_effects[0].gpustats_initial_random_add_2.scale_factor;
    
    strcpy_capped(slider_titles[3], 64, "+/time rand 1");
    strcpy_capped(slider_requests[39].label, 64, "X:");
    slider_requests[39].min_value = -5.0f;
    slider_requests[39].max_value =  5.0f;
    slider_requests[39].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz[0];
    
    strcpy_capped(slider_requests[40].label, 64, "Y:");
    slider_requests[40].min_value = -5.0f;
    slider_requests[40].max_value =  5.0f;
    slider_requests[40].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz[1];
    
    strcpy_capped(slider_requests[41].label, 64, "Z:");
    slider_requests[41].min_value = -2.5f;
    slider_requests[41].max_value =  2.5f;
    slider_requests[41].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz[2];
    
    strcpy_capped(slider_requests[42].label, 64, "X rot:");
    slider_requests[42].min_value = -2.7f;
    slider_requests[42].max_value =  2.7f;
    slider_requests[42].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_angle[0];
    
    strcpy_capped(slider_requests[43].label, 64, "Y rot:");
    slider_requests[43].min_value = -2.7f;
    slider_requests[43].max_value =  2.7f;
    slider_requests[43].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_angle[1];
    
    strcpy_capped(slider_requests[44].label, 64, "Z rot:");
    slider_requests[44].min_value = -2.7f;
    slider_requests[44].max_value =  2.7f;
    slider_requests[44].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_angle[2];
    
    strcpy_capped(slider_requests[45].label, 64, "+R:");
    slider_requests[45].min_value = -2.0f;
    slider_requests[45].max_value =  2.0f;
    slider_requests[45].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.bonus_rgb[0];
    
    strcpy_capped(slider_requests[46].label, 64, "+G:");
    slider_requests[46].min_value = -2.0f;
    slider_requests[46].max_value =  2.0f;
    slider_requests[46].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.bonus_rgb[1];
    
    strcpy_capped(slider_requests[47].label, 64, "+B:");
    slider_requests[47].min_value = -2.0f;
    slider_requests[47].max_value =  2.0f;
    slider_requests[47].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[48].label, 64, "+Width:");
    slider_requests[48].min_value = -0.25f;
    slider_requests[48].max_value =  0.25f;
    slider_requests[48].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[49].label, 64, "+Height:");
    slider_requests[49].min_value = -0.25f;
    slider_requests[49].max_value =  0.25f;
    slider_requests[49].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[50].label, 64, "+Depth:");
    slider_requests[50].min_value = -0.25f;
    slider_requests[50].max_value =  0.25f;
    slider_requests[50].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[51].label, 64, "+Scale:");
    slider_requests[51].min_value = -0.50f;
    slider_requests[51].max_value =  0.50f;
    slider_requests[51].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_1.scale_factor;
    
    strcpy_capped(slider_titles[4], 64, "+/time rand 2");
    strcpy_capped(slider_requests[52].label, 64, "X:");
    slider_requests[52].min_value = -5.0f;
    slider_requests[52].max_value =  5.0f;
    slider_requests[52].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz[0];
    
    strcpy_capped(slider_requests[53].label, 64, "Y:");
    slider_requests[53].min_value = -5.0f;
    slider_requests[53].max_value =  5.0f;
    slider_requests[53].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz[1];
    
    strcpy_capped(slider_requests[54].label, 64, "Z:");
    slider_requests[54].min_value = -2.5f;
    slider_requests[54].max_value =  2.5f;
    slider_requests[54].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz[2];
    
    strcpy_capped(slider_requests[55].label, 64, "X rot:");
    slider_requests[55].min_value = -2.7f;
    slider_requests[55].max_value =  2.7f;
    slider_requests[55].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_angle[0];
    
    strcpy_capped(slider_requests[56].label, 64, "Y rot:");
    slider_requests[56].min_value = -2.7f;
    slider_requests[56].max_value =  2.7f;
    slider_requests[56].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_angle[1];
    
    strcpy_capped(slider_requests[57].label, 64, "Z rot:");
    slider_requests[57].min_value = -2.7f;
    slider_requests[57].max_value =  2.7f;
    slider_requests[57].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_angle[2];
    
    strcpy_capped(slider_requests[58].label, 64, "+R:");
    slider_requests[58].min_value = -2.0f;
    slider_requests[58].max_value =  2.0f;
    slider_requests[58].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.bonus_rgb[0];
    
    strcpy_capped(slider_requests[59].label, 64, "+G:");
    slider_requests[59].min_value = -2.0f;
    slider_requests[59].max_value =  2.0f;
    slider_requests[59].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.bonus_rgb[1];
    
    strcpy_capped(slider_requests[60].label, 64, "+B:");
    slider_requests[60].min_value = -2.0f;
    slider_requests[60].max_value =  2.0f;
    slider_requests[60].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[61].label, 64, "+Width:");
    slider_requests[61].min_value = -0.25f;
    slider_requests[61].max_value =  0.25f;
    slider_requests[61].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[62].label, 64, "+Height:");
    slider_requests[62].min_value = -0.25f;
    slider_requests[62].max_value =  0.25f;
    slider_requests[62].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[63].label, 64, "+Depth:");
    slider_requests[63].min_value = -0.25f;
    slider_requests[63].max_value =  0.25f;
    slider_requests[63].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[64].label, 64, "+Scale:");
    slider_requests[64].min_value = -0.50f;
    slider_requests[64].max_value =  0.50f;
    slider_requests[64].linked_value =
        &particle_effects[0].gpustats_pertime_random_add_2.scale_factor;
    
    strcpy_capped(slider_titles[5], 64, "+/time^2");
    strcpy_capped(slider_requests[65].label, 64, "X:");
    slider_requests[65].min_value = -5.0f;
    slider_requests[65].max_value =  5.0f;
    slider_requests[65].linked_value =
        &particle_effects[0].gpustats_perexptime_add.xyz[0];
    
    strcpy_capped(slider_requests[66].label, 64, "Y:");
    slider_requests[66].min_value = -5.0f;
    slider_requests[66].max_value =  5.0f;
    slider_requests[66].linked_value =
        &particle_effects[0].gpustats_perexptime_add.xyz[1];
    
    strcpy_capped(slider_requests[67].label, 64, "Z:");
    slider_requests[67].min_value = -2.5f;
    slider_requests[67].max_value =  2.5f;
    slider_requests[67].linked_value =
        &particle_effects[0].gpustats_perexptime_add.xyz[2];
    
    strcpy_capped(slider_requests[68].label, 64, "X rot:");
    slider_requests[68].min_value = -2.7f;
    slider_requests[68].max_value =  2.7f;
    slider_requests[68].linked_value =
        &particle_effects[0].gpustats_perexptime_add.xyz_angle[0];
    
    strcpy_capped(slider_requests[69].label, 64, "Y rot:");
    slider_requests[69].min_value = -2.7f;
    slider_requests[69].max_value =  2.7f;
    slider_requests[69].linked_value =
        &particle_effects[0].gpustats_perexptime_add.xyz_angle[1];
    
    strcpy_capped(slider_requests[70].label, 64, "Z rot:");
    slider_requests[70].min_value = -2.7f;
    slider_requests[70].max_value =  2.7f;
    slider_requests[70].linked_value =
        &particle_effects[0].gpustats_perexptime_add.xyz_angle[2];
    
    strcpy_capped(slider_requests[71].label, 64, "+R:");
    slider_requests[71].min_value = -2.0f;
    slider_requests[71].max_value =  2.0f;
    slider_requests[71].linked_value =
        &particle_effects[0].gpustats_perexptime_add.bonus_rgb[0];
    
    strcpy_capped(slider_requests[72].label, 64, "+G:");
    slider_requests[72].min_value = -2.0f;
    slider_requests[72].max_value =  2.0f;
    slider_requests[72].linked_value =
        &particle_effects[0].gpustats_perexptime_add.bonus_rgb[1];
    
    strcpy_capped(slider_requests[73].label, 64, "+B:");
    slider_requests[73].min_value = -2.0f;
    slider_requests[73].max_value =  2.0f;
    slider_requests[73].linked_value =
        &particle_effects[0].gpustats_perexptime_add.bonus_rgb[2];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[74].label, 64, "+Width:");
    slider_requests[74].min_value = -0.25f;
    slider_requests[74].max_value =  0.25f;
    slider_requests[74].linked_value =
        &particle_effects[0].gpustats_perexptime_add.xyz_multiplier[0];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[75].label, 64, "+Height:");
    slider_requests[75].min_value = -0.25f;
    slider_requests[75].max_value =  0.25f;
    slider_requests[75].linked_value =
        &particle_effects[0].gpustats_perexptime_add.xyz_multiplier[1];
    
    // xyz_multiplier
    strcpy_capped(slider_requests[76].label, 64, "+Depth:");
    slider_requests[76].min_value = -0.25f;
    slider_requests[76].max_value =  0.25f;
    slider_requests[76].linked_value =
        &particle_effects[0].gpustats_perexptime_add.xyz_multiplier[2];
    
    // scale_factor
    strcpy_capped(slider_requests[77].label, 64, "+Scale:");
    slider_requests[77].min_value = -0.50f;
    slider_requests[77].max_value =  0.50f;
    slider_requests[77].linked_value =
        &particle_effects[0].gpustats_perexptime_add.scale_factor;
    
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
    
    assert(particle_effects_size == 1);
    
    next_ui_element_settings->slider_background_rgba[0] = 0.2f;
    next_ui_element_settings->slider_background_rgba[1] = 0.0f;
    next_ui_element_settings->slider_background_rgba[2] = 0.0f;
    request_int_slider(
        /* const int32_t background_object_id: */
            next_ui_element_object_id(),
        /* const int32_t pin_object_id: */
            next_ui_element_object_id(),
        /* const float x_screenspace: */
            window_globals->window_width -
                next_ui_element_settings->slider_width_screenspace -
                    (14 * 3),
        /* const float y_screenspace: */
            window_globals->window_height - (14 * 5) + (3 * 22),
        /* const float z: */
            0.75f,
        /* const float min_value: */
            1,
        /* const float max_value: */
            1000,
        /* int32_t * linked_value: */
            (int32_t *)&particle_effects[0].particle_spawns_per_second);
    font_height = 14;
    request_label_renderable(-1, "spawns/sec",
        /* const float left_pixelspace: */
            (window_globals->window_width -
                next_ui_element_settings->slider_width_screenspace) -
                (font_height * 3) +
                (next_ui_element_settings->slider_width_screenspace / 2) +
                (font_height / 2),
        /* const float top_pixelspace: */
            window_globals->window_height - (14 * 5) + (3 * 22) +
                (font_height / 2),
        /* const float z: */
            0.75f,
        /* const float max_width: */
            10000,
        /* const uint32_t ignore_camera: */
            false);
    
    next_ui_element_settings->slider_background_rgba[0] = 0.8f;
    next_ui_element_settings->slider_background_rgba[1] = 0.8f;
    next_ui_element_settings->slider_background_rgba[2] = 0.8f;
    request_int_slider(
        /* const int32_t background_object_id: */
            next_ui_element_object_id(),
        /* const int32_t pin_object_id: */
            next_ui_element_object_id(),
        /* const float x_screenspace: */
            window_globals->window_width -
                next_ui_element_settings->slider_width_screenspace -
                    (14 * 3),
        /* const float y_screenspace: */
            window_globals->window_height - (14 * 3) + (3 * 22),
        /* const float z: */
            0.75f,
        /* const float min_value: */
            1,
        /* const float max_value: */
            5000000,
        /* int32_t * linked_value: */
            (int32_t *)&particle_effects[0].particle_lifespan);
    request_label_renderable(-1, "lifespan",
        /* const float left_pixelspace: */
            (window_globals->window_width -
                next_ui_element_settings->slider_width_screenspace) -
                (font_height * 3) +
                (next_ui_element_settings->slider_width_screenspace / 2) +
                (font_height / 2),
        /* const float top_pixelspace: */
            window_globals->window_height - (14 * 3) + (3 * 22) +
                (font_height / 2),
        /* const float z: */
            0.75f,
        /* const float max_width: */
            10000,
        /* const uint32_t ignore_camera: */
            false);
    
    next_ui_element_settings->slider_background_rgba[0] = 0.8f;
    next_ui_element_settings->slider_background_rgba[1] = 0.6f;
    next_ui_element_settings->slider_background_rgba[2] = 0.6f;
    request_int_slider(
        /* const int32_t background_object_id: */
            next_ui_element_object_id(),
        /* const int32_t pin_object_id: */
            next_ui_element_object_id(),
        /* const float x_screenspace: */
            window_globals->window_width -
                next_ui_element_settings->slider_width_screenspace -
                    (14 * 3),
        /* const float y_screenspace: */
            window_globals->window_height - (14 * 1) + (3 * 22),
        /* const float z: */
            0.75f,
        /* const float min_value: */
            1,
        /* const float max_value: */
            5000000,
        /* int32_t * linked_value: */
            (int32_t *)&particle_effects[0].pause_between_spawns);
    request_label_renderable(-1, "pause/spawn",
        /* const float left_pixelspace: */
            (window_globals->window_width -
                next_ui_element_settings->slider_width_screenspace) -
                (font_height * 3) +
                (next_ui_element_settings->slider_width_screenspace / 2) +
                (font_height / 2),
        /* const float top_pixelspace: */
            window_globals->window_height - (14 * 1) + (3 * 22) +
                (font_height / 2),
        /* const float z: */
            0.75f,
        /* const float max_width: */
            10000,
        /* const uint32_t ignore_camera: */
            false);
    
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
        
        request_float_slider(
            /* const int32_t background_object_id: */
                next_ui_element_object_id(),
            /* const int32_t pin_object_id: */
                next_ui_element_object_id(),
            /* const float x_screenspace: */
                window_globals->window_width -
                    next_ui_element_settings->slider_width_screenspace -
                        (font_height * 3),
            /* const float y_screenspace: */
                window_globals->window_height - (font_height * 2) - (i * 22),
            /* const float z: */
                0.75f,
            /* const float min_value: */
                slider_requests[i].min_value,
            /* const float max_value: */
                slider_requests[i].max_value,
            /* float * linked_value: */
                slider_requests[i].linked_value);
        
        if (i % 13 == 0 && i < 78) {
            float prev_font_height = font_height;
            font_height = 20;
            font_color[0] = next_ui_element_settings->slider_background_rgba[0];
            font_color[1] = next_ui_element_settings->slider_background_rgba[1];
            font_color[2] = next_ui_element_settings->slider_background_rgba[2];
            font_color[3] = 1.0f;
            
            request_label_renderable(
                /* const int32_t with_object_id: */
                    1000,
                /* const char * text_to_draw: */
                    slider_titles[i / 13],
                /* const float left_pixelspace: */
                    window_globals->window_width -
                        next_ui_element_settings->slider_width_screenspace -
                        (next_ui_element_settings->slider_width_screenspace / 2) -
                            ((prev_font_height * 9)/2),
                /* const float top_pixelspace: */
                    window_globals->window_height -
                        (prev_font_height * 2.0f) -
                        ((i + 7.0f) * 22.0f),
                /* const float z: */
                    0.75f,
                /* const float max_width: */
                    500,
                /* const uint32_t ignore_camera: */
                    0);
            ScheduledAnimation * anim = next_scheduled_animation();
            anim->affected_object_id = 1000;
            anim->final_z_angle_known = true;
            anim->final_z_angle = 1.58f;
            anim->duration_microseconds = 1;
            commit_scheduled_animation(anim);
        }
    }
    
    // save button;
    next_ui_element_settings->ignore_camera = true;
    next_ui_element_settings->ignore_lighting = true;
    next_ui_element_settings->button_width_screenspace = 75.0f;
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
            "Save",
        /* const float x_screenspace: */
            (next_ui_element_settings->button_width_screenspace / 2) + 140,
        /* const float y_screenspace: */
            (next_ui_element_settings->button_height_screenspace / 2) + 10,
        /* const float z: */
            1.00f,
        /* void (* funtion_pointer)(void): */
            save_particle_stats);
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
    
    delete_zpolygon_object(1001);
    
    next_ui_element_settings->ignore_camera = false;
    next_ui_element_settings->ignore_lighting = true;
    for (uint32_t i = 0; i < SLIDERS_SIZE; i++) {
        char label_and_num[128];
        strcpy_capped(label_and_num, 128, slider_requests[i].label);
        strcat_capped(label_and_num, 128, " ");
        strcat_float_capped(
            label_and_num,
            128,
            *slider_requests[i].linked_value);
        
        request_label_renderable(
            /* const int32_t with_object_id: */
                1001,
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
                    (font_height / 2),
            /* const float z: */
                0.75f,
            /* const float max_width: */
                500,
            /* const uint32_t ignore_camera: */
                next_ui_element_settings->ignore_camera);
    }
    
    client_handle_keypresses(microseconds_elapsed);
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
}

void client_logic_shutdown(void) {
    // Your application shutdown code goes here!
}
