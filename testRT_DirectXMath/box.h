#pragma once

#include "rtweekend.h"

#include "aacrect.h"
#include "hittable_list.h"

class box : public hittable {
public:
    box() = default;
    box(const point3& p0, const point3& p1, shared_ptr<material> ptr);

    virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const override;

    virtual bool bounding_box(float time0, float time1, aabb& output_box) const override {
        output_box = aabb(box_min, box_max);
        return true;
    }

public:
    point3 box_min;
    point3 box_max;
    hittable_list sides;
};

box::box(const point3& p0, const point3& p1, shared_ptr<material> ptr) {
    box_min = p0;
    box_max = p1;

    sides.add(make_shared<xy_rect>(XMVectorGetX(p0), XMVectorGetX(p1), XMVectorGetY(p0), XMVectorGetY(p1), XMVectorGetZ(p1), ptr));
    sides.add(make_shared<xy_rect>(XMVectorGetX(p0), XMVectorGetX(p1), XMVectorGetY(p0), XMVectorGetY(p1), XMVectorGetZ(p0), ptr));

    sides.add(make_shared<xz_rect>(XMVectorGetX(p0), XMVectorGetX(p1), XMVectorGetZ(p0), XMVectorGetZ(p1), XMVectorGetY(p1), ptr));
    sides.add(make_shared<xz_rect>(XMVectorGetX(p0), XMVectorGetX(p1), XMVectorGetZ(p0), XMVectorGetZ(p1), XMVectorGetY(p0), ptr));

    sides.add(make_shared<yz_rect>(XMVectorGetY(p0), XMVectorGetY(p1), XMVectorGetZ(p0), XMVectorGetZ(p1), XMVectorGetX(p1), ptr));
    sides.add(make_shared<yz_rect>(XMVectorGetY(p0), XMVectorGetY(p1), XMVectorGetZ(p0), XMVectorGetZ(p1), XMVectorGetX(p0), ptr));
}

bool box::hit(const Ray& r, float t_min, float t_max, hit_record& rec) const {
    return sides.hit(r, t_min, t_max, rec);
}
