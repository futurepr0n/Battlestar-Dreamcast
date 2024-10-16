#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <kos.h>
#include "loadobj.hpp"

void draw_background(void);
void draw_ship(const obj& player);
void draw_scrolling_background(float x_offset);
void draw_starfield(float x1, float y1, float z1, float a, float r, float g, float b, int c, float xs, float ys);
void draw_scene(void);

#endif // RENDERER_HPP