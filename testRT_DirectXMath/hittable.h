#pragma once

#include <DirectXMath.h>
#include "Ray.h"
#include "aabb.h"
#include <iostream>

class material;

struct hit_record {
    DirectX::XMVECTOR p = {};
    DirectX::XMVECTOR normal = {};
    shared_ptr<material> mat_ptr;
    float t = 0.f;
    float u;
    float v;
    bool front_face = false;

    inline void set_face_normal(const Ray& r, const vec3& outward_normal) {
        front_face = XMVectorGetX(XMVector3Dot(r.direction(), outward_normal)) < 0.f;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(float time0, float time1, aabb& output_box) const = 0;
    virtual ~hittable() = default;
};

class translate : public hittable {
public:
    translate(shared_ptr<hittable> p, const vec3& displacement)
        : ptr(p), offset(displacement) {}

    virtual bool hit(
        const Ray& r, float t_min, float t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;

public:
    shared_ptr<hittable> ptr;
    vec3 offset;
};

bool translate::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {
    Ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (!ptr->hit(moved_r, t_min, t_max, rec))
        return false;

    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);

    return true;
}

bool translate::bounding_box(float time0, float time1, aabb& output_box) const {
    if (!ptr->bounding_box(time0, time1, output_box))
        return false;

    output_box = aabb(
        output_box.min() + offset,
        output_box.max() + offset);

    return true;
}

class rotate_y : public hittable {
public:
    rotate_y(shared_ptr<hittable> p, float angle);

    virtual bool hit(
        const Ray& r, float t_min, float t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override {
        output_box = bbox;
        return hasbox;
    }

public:
    shared_ptr<hittable> ptr;
    float sin_theta;
    float cos_theta;
    bool hasbox;
    aabb bbox;
};
rotate_y::rotate_y(shared_ptr<hittable> p, float angle) : ptr(p) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);

    vec3 min = { infinity, infinity, infinity };
    vec3 max = { -infinity, -infinity, -infinity };

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                float x = i * XMVectorGetX(bbox.max()) + (1.f - i) * XMVectorGetX(bbox.min());
                float y = j * XMVectorGetY(bbox.max()) + (1.f - j) * XMVectorGetY(bbox.min());
                float z = k * XMVectorGetZ(bbox.max()) + (1.f - k) * XMVectorGetZ(bbox.min());

                float newx = cos_theta * x + sin_theta * z;
                float newz = -sin_theta * x + cos_theta * z;

                vec3 tester = { newx, y, newz };

                min = XMVectorMin(min, tester);
                max = XMVectorMin(max, tester);
            }
        }
    }
    bbox = aabb(min, max);
}

bool rotate_y::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {
    auto origin = r.origin();
    auto direction = r.direction();

    origin = { cos_theta * XMVectorGetX(r.origin()) - sin_theta * XMVectorGetZ(r.origin()),  XMVectorGetY(origin), XMVectorGetZ(origin) };
    origin = { XMVectorGetX(origin) , XMVectorGetY(origin),  sin_theta * XMVectorGetX(r.origin()) + cos_theta * XMVectorGetZ(r.origin()) };
    direction = { cos_theta * XMVectorGetX(r.direction()) - sin_theta * XMVectorGetZ(r.direction()), XMVectorGetY(direction), XMVectorGetZ(direction) };
    direction = { XMVectorGetX(direction),  XMVectorGetY(direction),  sin_theta * XMVectorGetX(r.direction()) + cos_theta * XMVectorGetZ(r.direction()) };

    Ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    auto p = rec.p;
    auto normal = rec.normal;

    p = { cos_theta * XMVectorGetX(rec.p) + sin_theta * XMVectorGetZ(rec.p), XMVectorGetY(p), XMVectorGetZ(p)};
    p = { XMVectorGetX(p) ,XMVectorGetY(p), -sin_theta * XMVectorGetX(rec.p) + cos_theta * XMVectorGetZ(rec.p) };
    normal = { cos_theta * XMVectorGetX(rec.normal) + sin_theta * XMVectorGetZ(rec.normal) , XMVectorGetY(normal), XMVectorGetZ(normal) };
    normal = { XMVectorGetX(normal), XMVectorGetY(normal), -sin_theta * XMVectorGetX(rec.normal) + cos_theta * XMVectorGetZ(rec.normal) };

    rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}