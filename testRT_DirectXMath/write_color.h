#pragma once

#include <iostream>
#include "rtweekend.h"

color write_color(std::ostream& out, color pixel_color, int samples_per_pixel) {
    // Write the translated [0,255] value of each color component.
    float r = XMVectorGetX(pixel_color);
    float g = XMVectorGetY(pixel_color);
    float b = XMVectorGetZ(pixel_color);

    // Divide the color by the number of samples.
    float scale = 1.0f / static_cast<float>(samples_per_pixel);
    r = sqrtf(scale * r);
    g = sqrtf(scale * g);
    b = sqrtf(scale * b);

     //Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0f, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0f, 0.999f)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0f, 0.999f)) << '\n';

    return color{ static_cast<float>(256 * clamp(r, 0.0f, 0.999f)),
        static_cast<float>(256 * clamp(g, 0.0f, 0.999f)),
        static_cast<float>(256 * clamp(b, 0.0f, 0.999f))};

}
