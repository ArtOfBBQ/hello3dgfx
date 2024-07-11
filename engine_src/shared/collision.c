#include "collision.h"

// evaporate printf statements if COLLISION_SILENCE is set
#ifndef COLLISION_SILENCE
#define col_printf(...) printf(__VA_ARGS__)
#else
#define col_printf(...)
#endif

int point_hits_AArect(
    const float point[2],
    const float rect_bounds_min[2],
    const float rect_bounds_max[2])
{
    return
        point[0] >= rect_bounds_min[0] &&
        point[1] >= rect_bounds_min[1] &&
        point[0] <= rect_bounds_max[0] &&
        point[1] <= rect_bounds_max[1];
}

// returns distance to collision, or FLT_MAX if no hit, or
// a negative float if hit is behind the ray
float ray_hits_AArect(
    const float ray_origin[2],
    const float ray_direction[2],
    const float rect_bounds_min[2],
    const float rect_bounds_max[2],
    float * collision_recipient)
{
    /*
                y       -----
               5|       |   |
               4|     / |   |
               3|   /   -----
               2| /    4
               1|
    -x ------------------------- x
                |1 2 3 4 5 6 7
                |
                |
                |
                |
               -y
    
    FOR EACH BOUNDING PLANE (so in 2D there are 4 - it's the
    left/right/top/bottom of the box extended as planes)
    1. Find the absolute distance along that axis from the ray
      origin to rect (Just simple subtraction)
    2. Next, we need to find the distance if we were traveling
      along the ray
        
      If we're looking for the distance to a y-aligned plane, we
      can just read off the X difference
     
      If we're looking for the distance to an x-aligned plane, we
      can read off the Y difference
      
      Then we can deduce the total distance to plane collision
      E.g. if direction is [2.0f,3.0f] and the distance on the Y
      plane was 9.0f, then we took 9.0f/3.0f = 3.0 steps, and we
      must take 3.0 steps in the X direction also
      
    3. I suppose we now have to move along the ray, and check if
       that point is touching not only the plane but also the
       bounding box itself?
    4. Store that result (distance) and move on to the next
    END FOR
    
    Next, if there are any candidates, we take the smallest
    distance (that was >= 0.0f). That must be the closest collision
    */
    
    col_printf(
        "Box is at: [%f,%f] to [%f, %f]\n",
        rect_bounds_min[0],
        rect_bounds_min[1],
        rect_bounds_max[0],
        rect_bounds_max[1]);
    
    float nearest_dist_found = FLT_MAX;
    
    for (int plane_i = 0; plane_i < 4; plane_i++) {
        int axis_i = plane_i / 2;
        
        const float * bounds = (plane_i % 2 == 1) ?
            rect_bounds_min :
            rect_bounds_max;
        
        float axis_diff =
            bounds[axis_i] - ray_origin[axis_i];
        
        float steps_taken = axis_diff / ray_direction[0];
        col_printf("Plane %i steps taken: %f\n", plane_i,
            steps_taken);
        
        float collision_point[2];
        collision_point[0] = ray_origin[0];
        collision_point[1] = ray_origin[1];
        
        collision_point[0] += (ray_direction[0] * steps_taken);
        collision_point[1] += (ray_direction[1] * steps_taken);
        
        col_printf("Plane %i collision point: [%f, %f]\n", plane_i,
            collision_point[0], collision_point[1]);
        
        if (
            point_hits_AArect(
                collision_point,
                rect_bounds_min,
                rect_bounds_max))
        {
            float dist_ray_to_hit =
                (
                (ray_origin[0] - collision_point[0]) *
                (ray_origin[0] - collision_point[0])) +
                (
                (ray_origin[1] - collision_point[1]) *
                (ray_origin[1] - collision_point[1]));
            
            if (dist_ray_to_hit <= nearest_dist_found) {
                nearest_dist_found = dist_ray_to_hit;
                collision_recipient[0] = collision_point[0];
                collision_recipient[1] = collision_point[1];
            }
            
            col_printf(
                "Plane %i collided after: %f\n",
                plane_i,
                dist_ray_to_hit);
        } else {
            col_printf(
                "No collision found for plane %i\n",
                plane_i);
        }
    }
    
    return sqrtf(nearest_dist_found);
}

