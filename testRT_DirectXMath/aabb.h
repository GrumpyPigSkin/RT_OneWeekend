#pragma once

#include "rtweekend.h"

class aabb {
public:
    aabb() = default;
    aabb(const point3& a, const point3& b) { minimum = a; maximum = b; }

    point3 min() const { return minimum; }
    point3 max() const { return maximum; }

    bool hit(const Ray& r, float t_min, float t_max) const {

        {
            auto invD = 1.0f / XMVectorGetX(r.direction());
            auto t0 = (XMVectorGetX(min()) - XMVectorGetX(r.origin())) * invD;
            auto t1 = (XMVectorGetX(max()) - XMVectorGetX(r.origin())) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }

        {
            auto invD = 1.0f / XMVectorGetY(r.direction());
            auto t0 = (XMVectorGetY(min()) - XMVectorGetY(r.origin())) * invD;
            auto t1 = (XMVectorGetY(max()) - XMVectorGetY(r.origin())) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }

        {
            auto invD = 1.0f / XMVectorGetZ(r.direction());
            auto t0 = (XMVectorGetZ(min()) - XMVectorGetZ(r.origin())) * invD;
            auto t1 = (XMVectorGetZ(max()) - XMVectorGetZ(r.origin())) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }

        return true;
    }

    point3 minimum = {};
    point3 maximum = {};
};

aabb surrounding_box(aabb box0, aabb box1) {
    point3 small{ XMVectorMin(box0.minimum, box1.minimum) };
    point3 big{ XMVectorMax(box0.maximum, box1.maximum) };
    return aabb(small, big);
}

