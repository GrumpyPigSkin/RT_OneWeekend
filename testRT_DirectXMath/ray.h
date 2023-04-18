#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Ray {
public:
    Ray() = default;
    Ray(const XMVECTOR& origin, const XMVECTOR& direction, float time = 0.f)
        : orig(origin), dir(direction), tm(time)
    {}

    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }
    float time() const { return tm; }

    point3 at(float t) const {
        return orig + point3{ t,t,t,t } *dir;
    }

public:
    point3 orig;
    vec3 dir;
    float tm;
};