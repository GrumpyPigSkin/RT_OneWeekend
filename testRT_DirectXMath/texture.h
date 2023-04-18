#pragma once
#include "rtweekend.h"
#include "perlin.h"
#include <iostream>

class texture {
public:
    virtual color value(float u, float v, const point3& p) const = 0;
    virtual ~texture() = default;
};

class solid_color : public texture {
public:
    solid_color() = default;
    solid_color(color c) : color_value(c) {}

    solid_color(float red, float green, float blue)
        : solid_color(color{ red, green, blue }) {}

    virtual color value(float u, float v, const vec3& p) const override {
        return color_value;
    }

private:
    color color_value;
};

class checker_texture : public texture {
public:
    checker_texture() {}

    checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd)
        : even(_even), odd(_odd) {}

    checker_texture(color c1, color c2)
        : even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

    virtual color value(float u, float v, const point3& p) const override {
        float sines = sin(10 * XMVectorGetX(p)) * sin(10 * XMVectorGetY(p)) * sin(10 * XMVectorGetZ(p));
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    shared_ptr<texture> odd;
    shared_ptr<texture> even;
};

#include "perlin.h"

class noise_texture : public texture {
public:
    noise_texture() = default;
    noise_texture(float sc) : scale(sc) {}

    virtual color value(float u, float v, const point3& p) const override {
        return color{ 1.f, 1.f, 1.f } *0.5f * (1.f + sin(scale * XMVectorGetZ(p) + 10.f * noise.turb(p)));
    }

public:
    perlin noise = {};
    float scale = 0.f;
};

class image_texture : public texture {
public:
    const static int bytes_per_pixel = 3;

    image_texture()
        : data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

    image_texture(const char* filename) {
        auto components_per_pixel = bytes_per_pixel;

        data = stbi_load(
            filename, &width, &height, &components_per_pixel, components_per_pixel);

        if (!data) {
            std::cerr << "ERROR: Could not load texture image file '" << filename << "'.\n";
            width = height = 0;
        }

        bytes_per_scanline = bytes_per_pixel * width;
    }

    ~image_texture() {
        delete data;
    }

    virtual color value(float u, float v, const vec3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (data == nullptr)
            return color{ 0.f, 1.f, 1.f };

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = clamp(u, 0.0f, 1.0f);
        v = 1.0f - clamp(v, 0.0f, 1.0f);  // Flip V to image coordinates

        auto i = static_cast<int>(u * width);
        auto j = static_cast<int>(v * height);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= width)  i = width - 1;
        if (j >= height) j = height - 1;

        const float color_scale = 1.0f / 255.0f;
        auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;

        return color{ color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2] };
    }

private:
    unsigned char* data;
    int width = 0, height = 0;
    int bytes_per_scanline = 0;
};
