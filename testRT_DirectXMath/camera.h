#pragma once

#include "rtweekend.h"

class camera {
public:
    camera(
        point3 lookfrom,
        point3 lookat,
        vec3   vup,
        float vfov, // vertical field-of-view in degrees
        float aspect_ratio,
        float aperture,
        float focus_dist,
        float _time0 = 0.f,
        float _time1 = 0.f
    ) {
        float theta = degrees_to_radians(vfov);
        float h = tanf(theta / 2.f);
        float viewport_height = 2.f * h;
        float viewport_width = aspect_ratio * viewport_height;

        w = XMVector3Normalize(lookfrom - lookat);
        u = XMVector3Normalize(XMVector3Cross(vup, w));
        v = XMVector3Cross(w, u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal / 2.f - vertical / 2.f - focus_dist * w;
        lens_radius = aperture / 2.f;
        time0 = _time0;
        time1 = _time1;
    }

    Ray get_ray(float s, float t) const {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * XMVectorGetX(rd) + v * XMVectorGetY(rd);

        return Ray(
            origin + offset,
            lower_left_corner + s * horizontal + t * vertical - origin - offset,
            random_float(time0, time1)
        );
    }

private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    float lens_radius;
    float time0, time1;
};