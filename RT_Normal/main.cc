//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"

#include "box.h"
#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "sphere.h"
#include "texture.h"

#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;



color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}


hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            //if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                //shared_ptr<material> sphere_material;

                /*if (choose_mat < 0.8) {*/
                    // diffuse
                    /*auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));*/
                //} else if (choose_mat < 0.95) {
                //    // metal
                //    auto albedo = color::random(0.5, 1);
                //    auto fuzz = random_double(0, 0.5);
                //    sphere_material = make_shared<metal>(albedo, fuzz);
                //    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                //} else {
                //    // glass
                //    sphere_material = make_shared<dielectric>(1.5);
                //    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                //}
            //}
            shared_ptr<material> sphere_material;
            auto albedo = color::random() * color::random();
            sphere_material = make_shared<lambertian>(albedo);
            auto center2 = center + vec3(0, random_double(0, .5), 0);
            world.add(make_shared<moving_sphere>(
                center, center2, 0.0, 1.0, 0.2, sphere_material));

        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
}


hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    objects.add(make_shared<sphere>(point3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}


hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    return objects;
}


hittable_list earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);

    return hittable_list(globe);
}


hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    objects.add(make_shared<sphere>(point3(0,7,0), 2, difflight));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}


hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    objects.add(box2);

    return objects;
}


hittable_list cornell_smoke() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));

    shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));

    objects.add(make_shared<constant_medium>(box1, 0.01, color(0,0,0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, color(1,1,1)));

    return objects;
}


hittable_list final_scene() {
    hittable_list boxes1;
    hittable_list objects;

    auto white = make_shared<lambertian>(color{ .73f, .73f, .73f });
    auto random_col = make_shared<lambertian>(color{ random_double(), 0, random_double() });

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            shared_ptr<hittable> t = make_shared<sphere>(point3{ (float)(i * 50), 0.f, (float)(j * 50) }, random_double(30.f, 80.f), random_col);
            objects.add(make_shared<constant_medium>(t, 0.01f, color{ random_double_messenne(), 0, random_double_messenne() }));
        }
    }

    //hittable_list objects;

    auto light = make_shared<diffuse_light>(color(10, 10, 10));
    objects.add(make_shared<xz_rect>(123, 323, 147, 312, 554, light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30,0,0);
    auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400,200,400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    //auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
            vec3(-100,270,395)
        )
    );

    return objects;
}

void DrawPixel(int nSamplesPerPixel, int nImageWidth, int nImageHeight, int startHeight, int endHeight, int nMaxDepth, hittable_list world, camera cam, std::vector<color>& oColours, color background, std::atomic<int>& scan_lines_remianing)
{
    
    
    for (int i = startHeight; i <= endHeight; i++)
    {
        scan_lines_remianing--;
        for (int j = 0; j < nImageWidth; j++)
        {
            color pixelColour{ 0.f, 0.f, 0.f };
            for (int k = 0; k < nSamplesPerPixel; k++)
            {
                auto u = (j + random_double()) / (nImageWidth - 1);
                auto v = (i + random_double()) / (nImageHeight - 1);
                ray r = cam.get_ray(u, v);
                pixelColour += ray_color(r, background, world, nMaxDepth);
            }

            auto r = pixelColour.x();
            auto g = pixelColour.y();
            auto b = pixelColour.z();

            // Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
            if (r != r) r = 0.0;
            if (g != g) g = 0.0;
            if (b != b) b = 0.0;

            // Divide the color by the number of samples and gamma-correct for gamma=2.0.
            auto scale = 1.0 / nSamplesPerPixel;
            r = sqrt(scale * r);
            g = sqrt(scale * g);
            b = sqrt(scale * b);

            color out = { static_cast<double>(256 * clamp(r, 0.0, 0.999)),
                static_cast<double>(256 * clamp(g, 0.0, 0.999)),
                static_cast<double>(256 * clamp(b, 0.0, 0.999)) };

            oColours.push_back(out);
        }
    }

}

void print_lines_remaining(std::atomic<int>& scan_lines_remaining)
{

    while (scan_lines_remaining > 0)
    {
        std::cerr << "\rScanlines remaining: " << scan_lines_remaining << ' ' << std::flush;
        std::this_thread::sleep_for(1000ms);

    }
}


int main() {

    // Image

    auto aspect_ratio = 16.0 / 9.0;
    int image_width = 600;
    int samples_per_pixel = 100;
    int max_depth = 50;

    // World

    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;
    color background(0,0,0);

    switch (1) {
        case 1:
            world = random_scene();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            aperture = 0.1;
            break;

        case 2:
            world = two_spheres();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;

        case 3:
            world = two_perlin_spheres();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(13,2,3);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;

        case 4:
            world = earth();
            background = color(0.70, 0.80, 1.00);
            lookfrom = point3(0,0,12);
            lookat = point3(0,0,0);
            vfov = 20.0;
            break;

        case 5:
            world = simple_light();
            samples_per_pixel = 400;
            lookfrom = point3(26,3,6);
            lookat = point3(0,2,0);
            vfov = 20.0;
            break;

        default:
        case 6:
            world = cornell_box();
            aspect_ratio = 1.0;
            image_width = 600;
            samples_per_pixel = 100;
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;

        case 7:
            world = cornell_smoke();
            aspect_ratio = 1.0;
            image_width = 600;
            samples_per_pixel = 200;
            lookfrom = point3(278, 278, -800);
            lookat = point3(278, 278, 0);
            vfov = 40.0;
            break;

        case 8:
            world = final_scene();
            aspect_ratio = 1.0;
            image_width = 800;
            samples_per_pixel = 100;
            lookfrom = point3(478, 278, -600);
            lookat = point3(278, 278, 0);
            //background = color(0.50, 0.0, .50);
            vfov = 40.0;
            break;
    }

    // Camera

    const vec3 vup(0,1,0);
    const auto dist_to_focus = 10.0;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    // Render

    auto t1 = std::chrono::high_resolution_clock::now();

    //std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    //for (int j = image_height-1; j >= 0; --j) {
    //    std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    //    for (int i = 0; i < image_width; ++i) {
    //        color pixel_color(0,0,0);
    //        for (int s = 0; s < samples_per_pixel; ++s) {
    //            auto u = (i + random_double()) / (image_width-1);
    //            auto v = (j + random_double()) / (image_height-1);
    //            ray r = cam.get_ray(u, v);
    //            pixel_color += ray_color(r, background, world, max_depth);
    //        }
    //        write_color(std::cout, pixel_color, samples_per_pixel);
    //    }
    //}

    std::atomic<int> scan_lines_remaining = image_height;

    const int num_threads = std::thread::hardware_concurrency();
    std::vector<std::vector<color>> thread_outputs(num_threads);
    std::vector<std::thread> threads(num_threads + 1);
    for (size_t i = 0; i < num_threads; i++)
    {
        int k = i + 1;
        int threadHeight = ceilf(image_height / num_threads);
        int threadIndex = threadHeight * i;
        int endHeight = threadHeight + threadIndex;
        int startHeight = endHeight - threadHeight;
        thread_outputs[i].reserve(threadHeight* image_width);
        threads[i] = std::thread(&DrawPixel, samples_per_pixel, image_width, image_height, startHeight, endHeight, max_depth, world, cam, std::ref(thread_outputs[i]), background, std::ref(scan_lines_remaining));

    }

    threads[num_threads] = std::thread(print_lines_remaining, std::ref(scan_lines_remaining));

    for (size_t i = 0; i < num_threads + 1; i++)
    {
        threads[i].join();
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    auto t3 = std::chrono::duration_cast<std::chrono::seconds> (t2 - t1);

    std::cerr << "Time Taken: " << t3 << std::endl;

    std::vector<color> finished;
    for (size_t i = 0; i < num_threads; i++)
    {
        finished.insert(finished.end(), thread_outputs[i].begin(), thread_outputs[i].end());
    }
    
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
    for (int i = finished.size() - 1; i >= 0; i--)
    {
        std::cout << (int)finished[i].x() << ' ' << (int)finished[i].y() << ' ' << (int)finished[i].z() << std::endl;
    }

    std::cerr << "\nDone.\n";
    
}
