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

void blitObj(const obj& object) {    
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;

     // Calculate the next power of 2 for texture width and height
    float texture_width = static_cast<float>(nextPowerOf2(static_cast<int>(object.imgX)));
    float texture_height = static_cast<float>(nextPowerOf2(static_cast<int>(object.imgY)));

    // Update texture size to next power of 2
    pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, PVR_TXRFMT_ARGB4444, (int)texture_width, (int)texture_height, object.texture_pointer, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
    vert.oargb = 0;
    vert.flags = PVR_CMD_VERTEX;

    // Set half dimensions using the original image size
    float half_width = object.imgX / 2.0f;
    float half_height = object.imgY / 2.0f;

    // Calculate UV coordinates based on the original image size and texture size
    float u_max = object.imgX / texture_width;
    float v_max = object.imgY / texture_height;

    // Top-left vertex
    vert.x = object.x - half_width;
    vert.y = object.y - half_height;
    vert.z = 1.0f;
    vert.u = 0.0f;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Top-right vertex
    vert.x = object.x + half_width;
    vert.y = object.y - half_height;
    vert.u = u_max;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-left vertex
    vert.x = object.x - half_width;
    vert.y = object.y + half_height;
    vert.u = 0.0f;
    vert.v = v_max;
    pvr_prim(&vert, sizeof(vert));

    // Bottom-right vertex
    vert.flags = PVR_CMD_VERTEX_EOL;
    vert.x = object.x + half_width;
    vert.y = object.y + half_height;
    vert.u = u_max;
    vert.v = v_max;
    pvr_prim(&vert, sizeof(vert));
    
}
