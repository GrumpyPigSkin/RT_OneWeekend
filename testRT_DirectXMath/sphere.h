#pragma once

#include "hittable.h"
#include "ray.h"
#include "aabb.h"
#include <DirectXMath.h>

using point3 = DirectX::XMVECTOR;
using vec3 = DirectX::XMVECTOR;

class sphere : public hittable {
public:
    sphere() = default;
    sphere(point3 cen, float r, shared_ptr<material> m)
        : center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(
        const Ray& r, float t_min, float t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override;

public:
    point3 center = {};
    float radius = 0.f;
    shared_ptr<material> mat_ptr;

private:

    static void get_sphere_uv(const point3& p, float& u, float& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        float theta = acosf(-XMVectorGetY(p));
        float phi = atan2f(-XMVectorGetZ(p), XMVectorGetX(p)) + pi;

        u = phi / (2.f * pi);
        v = theta / pi;
    }
};

bool sphere::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    float a = XMVectorGetX(XMVector3Dot(r.direction(), r.direction()));
    float half_b = XMVectorGetX(XMVector3Dot(oc, r.direction()));
    float c = XMVectorGetX(XMVector3Dot(oc,oc)) - radius * radius;

    float discriminant = half_b * half_b - a * c;
    if (discriminant < 0.f) return false;
    float sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    float root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool sphere::bounding_box(float time0, float time1, aabb& output_box) const {
    output_box = aabb(
        center - vec3{ radius, radius, radius },
        center + vec3{ radius, radius, radius} );
    return true;
}