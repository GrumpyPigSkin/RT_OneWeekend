#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <DirectXMath.h>
#include <random>
#include "rtw_stb_image.h"

using namespace DirectX;

// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;
using vec3 = DirectX::XMVECTOR;
using point3 = DirectX::XMVECTOR;
using color = DirectX::XMVECTOR;

// Constants

const float infinity = std::numeric_limits<float>::infinity();
const float pi = 3.14159265f;

// Utility Functions

inline float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0f;
}

inline float random_float()
{
    return std::rand() / (RAND_MAX + 1.0f);
}

inline float random_float(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_float();
}

inline int random_int(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_float(static_cast<float>(min), static_cast<float>(max + 1)));
}

inline float clamp(float x, float min, float max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Common Headers

inline vec3 random_vector()
{
    return { random_float(), random_float(), random_float() };
}

inline vec3 random_vector(float min, float max)
{
    return { random_float(min,max), random_float(min,max), random_float(min,max) };
}

inline vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random_vector(-1.f, 1.f);
        if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(p,p)) >= 1.f) 
            continue;
        return p;
    }
}

inline vec3 random_unit_vector() {
    return DirectX::XMVector3Normalize(random_in_unit_sphere());
}

inline vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(in_unit_sphere, normal)) > 0.0f) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
    {
        return -in_unit_sphere;
    }
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3{random_float(-1, 1), random_float(-1, 1), 0};
        if (XMVectorGetX(XMVector3Dot(p,p)) >= 1) continue;
        return p;
    }
}

inline bool near_zero(const vec3& vec)
{
    float s = 1e-8f;
    vec3 v = XMVectorAbs(vec);
    return (int)XMVectorGetX(XMVectorLessOrEqual(v, vec3{s,s,s,s}));
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2.f * DirectX::XMVector3Dot(v, n) * n;
}

vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat) {
    vec3 dot = XMVector3Dot(-uv, n);
    auto cos_theta = fmin(XMVectorGetX(dot), 1.0f);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
    vec3 temp = XMVector3Dot(r_out_perp, r_out_perp);
    vec3 r_out_parallel = -sqrtf(fabs(1.0f - XMVectorGetX(temp))) * n;
    return r_out_perp + r_out_parallel;
}

#include "ray.h"
