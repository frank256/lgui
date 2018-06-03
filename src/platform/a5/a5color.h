/*   _                _
*   | |              (_)
*   | |  __ _  _   _  _
*   | | / _` || | | || |
*   | || (_| || |_| || |
*   |_| \__, | \__,_||_|
*        __/ |
*       |___/
*
* Copyright (c) 2015 frank256
*
* License (BSD):
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice, this
*    list of conditions and the following disclaimer in the documentation and/or
*    other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LGUI_A5_COLOR_H
#define LGUI_A5_COLOR_H

#include <allegro5/allegro_color.h>

namespace lgui {

using Color =  ALLEGRO_COLOR;

/** Constructs a color value from rgb byte values (0-255). */
inline Color rgb_i(unsigned char r, unsigned char g, unsigned char b) {
    return al_map_rgb(r, g, b);
}

/** Constructs a color value from rgba byte values (0-255). */
inline Color rgba_i(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    return al_map_rgba(r, g, b, a);
}

/** Constructs a color value from rgb float values (0.0-1.0). */
inline Color rgb(float r, float g, float b) {
    return al_map_rgb_f(r, g, b);
}

/** Constructs a color value from rgba float values (0.0-1.0). */
inline Color rgba(float r, float g, float b, float a) {
    return al_map_rgba_f(r, g, b, a);
}

/** Constructs a color value from rgba float values, premultiplying rgb by a. */
inline Color rgba_premult(float r, float g, float b, float a) {
    return al_map_rgba_f(r*a, g*a, b*a, a);
}

/** Constructs a color from a float greyscale value (0.0-1.0), premultiplying the alpha value. */
inline Color grey_premult(float g, float a) {
    return al_map_rgba_f(g*a, g*a, g*a, a);
}

/** Constructs a color from a float greyscale value (0.0-1.0). */
inline Color grey(float g) {
    return al_map_rgb_f(g, g, g);
}


/** Multiplies the alpha value of the color given with the given alpha value, returning the result as a
 * new color. */
inline Color col_mult_alpha(Color col, float alpha) {
    return rgba_premult(col.r, col.g, col.b, col.a*alpha);
}

/** Changes the alpha value of a color given as a byte (0-255), returning the result as a new color. */
inline Color col_set_alpha_i(Color col, unsigned char nalpha)
{
    unsigned char r, g, b;
    al_unmap_rgb(col, &r, &g, &b);
    return al_map_rgba(r, g, b, nalpha);
}

/** Return the alpha value of the given color as a byte value (0-255). */
inline int col_get_alpha_i(Color col)
{
    unsigned char alpha, r, g, b;
    al_unmap_rgba(col, &r, &g, &b, &alpha);
    return alpha;
}

/** Return the alpha value of the given color as a float value (0.0-1.0). */
inline float col_get_alpha(Color col)
{
    float alpha, r, g, b;
    al_unmap_rgba_f(col, &r, &g, &b, &alpha);
    return alpha;
}

/** Multiply the given color's rgb values with the given float value, returning a new color.*/
inline Color rgb_mult(const Color& col, float i)
{
    Color c;
    c.a = col.a;
    c.r = col.r * i;
    c.g = col.g * i;
    c.b = col.b * i;
    return c;
}

/** Decompose the given color into its rgb components (as bytes). */
inline void col_get_rgb_i(Color col, unsigned char& r, unsigned char& g, unsigned char& b)
{
    al_unmap_rgb(col, &r, &g, &b);
}

/** Decompose the given color into its rgba components (as bytes). */
inline void col_get_rgba_i(Color col, unsigned char& r, unsigned char& g, unsigned char& b,
                         unsigned char& a)
{
    al_unmap_rgba(col, &r, &g, &b, &a);
}

/** Retuns the inverse (rgb) color of the given color. */
inline Color invert_color(Color col)  {
    return rgb(1.0-col.r, 1.0-col.g, 1.0-col.b);
}

/** Retuns the inverse color of the given color, also inverting the alpha channel. */
inline Color invert_color_with_alpha(Color col)  {
    return rgba(1.0-col.r, 1.0-col.g, 1.0-col.b, 1.0-col.a);
}

/** Constructs a color from a HTML-style hex string. Lengths of 6 and 8 characters (the latter including the
 * alpha  channel) are supported. A leading # is permitted. */
inline Color rgb_from_hex(const char* colstr) {
    float r, g, b, a = 1.0;
    char const* ptr = colstr;
    unsigned long rgb;
    if (*ptr == '#') ptr++;
    int len = strlen(ptr);
    rgb = strtoul(ptr, NULL, 16);
    if(len <= 6) {
        r = (rgb >> 16) / 255.0;
        g = ((rgb >> 8) & 255) / 255.0;
        b = (rgb & 255) / 255.0;
    }
    else {
        r = (rgb >> 24) / 255.0;
        g = ((rgb >> 16) & 255) / 255.0;
        b = ((rgb >> 8) & 255) / 255.0;
        a = (rgb & 255) / 255.0;
    }
    return rgba(r, g, b, a);
}

}

#endif // LGUI_A5_COLOR_H

