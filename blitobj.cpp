#include <kos.h>
#include <cstdlib>  // For rand()
#include "blitobj.hpp"
#include "loadobj.hpp"
#include <cmath>

// Constants for movement
const float ENEMY_BASE_SPEED = 2.0f;
const float ENEMY_AMPLITUDE = 10.0f;
const float ENEMY_FREQUENCY = 0.05f;
const float MAX_X_THRESHOLD = 320.0f;




int nextPowerOf2(int num) {
    int power = 1;
    while (power < num) {
        power <<= 1;  // Multiply by 2
    }
    return power;
}

void blitObj(const obj& object, float rotation_degrees) {    
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

    // Calculate the next power of 2 for texture width and height
    float texture_width = static_cast<float>(nextPowerOf2(static_cast<int>(object.imgX)));
    float texture_height = static_cast<float>(nextPowerOf2(static_cast<int>(object.imgY)));

    // Update texture size to next power of 2
    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, 
                     (int)texture_width, (int)texture_height, 
                     object.texture_pointer, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    // Convert rotation to radians
    float rotation_rad = (rotation_degrees * M_PI) / 180.0f;
    float cos_rot = cosf(rotation_rad);
    float sin_rot = sinf(rotation_rad);

    // Set half dimensions using the original image size
    float half_width = object.imgX / 2.0f;
    float half_height = object.imgY / 2.0f;

    // Calculate UV coordinates based on the original image size and texture size
    float u_max = object.imgX / texture_width;
    float v_max = object.imgY / texture_height;

    // Define the four corners relative to center before rotation
    float corners[4][2] = {
        {-half_width, -half_height},  // Top-left
        {half_width, -half_height},   // Top-right
        {-half_width, half_height},   // Bottom-left
        {half_width, half_height}     // Bottom-right
    };

    // UV coordinates for the corners
    float uvs[4][2] = {
        {0.0f, 0.0f},    // Top-left
        {u_max, 0.0f},   // Top-right
        {0.0f, v_max},   // Bottom-left
        {u_max, v_max}   // Bottom-right
    };

    // Draw the rotated quad
    for (int i = 0; i < 4; i++) {
        // Rotate the corner position
        float rotated_x = corners[i][0] * cos_rot - corners[i][1] * sin_rot;
        float rotated_y = corners[i][0] * sin_rot + corners[i][1] * cos_rot;

        // Translate to final position
        float final_x = object.x + rotated_x;
        float final_y = object.y + rotated_y;

        vert.flags = (i == 3) ? PVR_CMD_VERTEX_EOL : PVR_CMD_VERTEX;
        vert.x = final_x;
        vert.y = final_y;
        vert.z = 1.0f;
        vert.u = uvs[i][0];
        vert.v = uvs[i][1];
        vert.argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
        vert.oargb = 0;
        pvr_prim(&vert, sizeof(vert));
    }
}