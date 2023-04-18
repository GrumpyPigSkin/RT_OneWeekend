
#include "rtweekend.h"
#include "hittable_list.h"
#include "sphere.h"
#include <iostream>
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "aacrect.h"
#include "write_color.h"
#include "constant_medium.h"
#include "box.h"
#include "bvh.h"
#include <thread>
#include <atomic>
#include <chrono>
using namespace DirectX;
using namespace std::chrono_literals;

/*
double length() const {
    return sqrt(length_squared());
}
double length_squared() const {
    return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
}
*/

color ray_color(const Ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color{ 0.f, 0.f, 0.f };

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001f, infinity, rec))
        return background;

    Ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(color{ 0.2f, 0.3f, 0.1f }, color{ 0.9f, 0.9f, 0.9f });
    world.add(make_shared<sphere>(point3{ 0, -1000, 0 }, 1000, make_shared<lambertian>(checker)));


    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_float();
            point3 center{ a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float() };

            //if (XMVectorGetX(XMVector3Length(center - point3{ 4.f, 0.2f, 0.f })) > 0.9f) {
                //shared_ptr<material> sphere_material;

                /*if (choose_mat < 0.8f) {*/
                    // diffuse
                    /*auto albedo = random_vector() * random_vector();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3{ 0, random_float(0.f, .5f), 0.f };
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0f, 1.0f, 0.2f, sphere_material));*/
                //}
                //else if (choose_mat < 0.95f) {
                //    // metal
                //    auto albedo = random_vector(0.5f, 1.f);
                //    auto fuzz = random_float(0.f, 0.5f);
                //    sphere_material = make_shared<metal>(albedo, fuzz);
                //    world.add(make_shared<sphere>(center, 0.2f, sphere_material));
                //}
                //else {
                //    // glass
                //    sphere_material = make_shared<dielectric>(1.5f);
                //    world.add(make_shared<sphere>(center, 0.2f, sphere_material));
                //}
            //}
            shared_ptr<material> sphere_material;
            auto albedo = random_vector() * random_vector();
            sphere_material = make_shared<lambertian>(albedo);
            auto center2 = center + vec3{ 0, random_float(0.f, .5f), 0.f };
            world.add(make_shared<moving_sphere>(
                center, center2, 0.0f, 1.0f, 0.2f, sphere_material));
        }
    }

    auto material1 = make_shared<dielectric>(1.5f);
    world.add(make_shared<sphere>(point3{ 0.f, 1.f, 0.f }, 1.0f, material1));

    auto material2 = make_shared<lambertian>(color{ 0.4f, 0.2f, 0.1f });
    world.add(make_shared<sphere>(point3{ -4.f, 1.f, 0.f }, 1.0f, material2));

    auto material3 = make_shared<metal>(color{ 0.7f, 0.6f, 0.5f }, 0.0f);
    world.add(make_shared<sphere>(point3{ 4.f, 1.f, 0.f }, 1.0f, material3));

    return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
}

hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(color{ 0.2f, 0.3f, 0.1f }, color{ 0.9f, 0.9f, 0.9f });

    objects.add(make_shared<sphere>(point3{ 0.f, -10.f, 0.f }, 10.f, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3{ 0.f, 10.f, 0.f }, 10.f, make_shared<lambertian>(checker)));

    return objects;
}

hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3{ 0.f, -1000.f, 0.f }, 1000.f, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3{ 0.f, 2.f, 0.f }, 2.f, make_shared<lambertian>(pertext)));

    return objects;
}

hittable_list earth() {
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3{ 0.f, 0.f, 0.f }, 2.f, earth_surface);

    return hittable_list(globe);
}

hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4.f);
    auto albedo = random_vector() * random_vector();
    auto red = make_shared<lambertian>(color{ .65f, .05f, .05f });
    auto white = make_shared<lambertian>(color{ .73f, .73f, .73f });
    auto green = make_shared<lambertian>(color{ .12f, .45f, .15f });
    auto light = make_shared<diffuse_light>(color{ 15.f, 15.f, 15.f });

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    auto sphere_material = make_shared<lambertian>(albedo);
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<sphere>(point3{ 0.f, -1000.f, 0.f }, 1000.f, sphere_material));
    objects.add(make_shared<sphere>(point3{ 0.f, 2.f, 0.f }, 2.f, make_shared<lambertian>(pertext)));

    //auto difflight = make_shared<diffuse_light>(color{ 4.f, 4.f, 4.f });
    //objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));

    return objects;
}

hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(color     {.65f, .05f, .05f});
    auto white = make_shared<lambertian>(color   {.73f, .73f, .73f});
    auto green = make_shared<lambertian>(color   {.12f, .45f, .15f});
    auto light = make_shared<diffuse_light>(color{15.f, 15.f, 15.f});

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    
    shared_ptr<hittable> box1 = make_shared<box>(point3{ 0, 0, 0 }, point3{ 165, 330, 165 }, white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3{ 265, 0, 295 });
    objects.add(box1);

    
    shared_ptr<hittable> box2 = make_shared<box>(point3{ 0, 0, 0 }, point3{ 165, 165, 165 }, white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3{ 130, 0, 65 });
    objects.add(box2);


    return objects;
}

hittable_list cornell_smoke() {
    hittable_list objects;

    auto red = make_shared<lambertian>(color{ .65f, .05f, .05f });
    auto white = make_shared<lambertian>(color{ .73f, .73f, .73f });
    auto green = make_shared<lambertian>(color{ .12f, .45f, .15f });
    auto light = make_shared<diffuse_light>(color{ 15.f, 15.f, 15.f });

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3{ 0, 0, 0 }, point3{ 165, 330, 165 }, white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3{ 265, 0, 295 });
    


    shared_ptr<hittable> box2 = make_shared<box>(point3{ 0, 0, 0 }, point3{ 165, 165, 165 }, white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3{ 130, 0, 65 });


    objects.add(make_shared<constant_medium>(box1, 0.01f, color{ 0, 0, 0 }));
    objects.add(make_shared<constant_medium>(box2, 0.01f, color{ 1, 1, 1 }));

    return objects;
}

hittable_list final_scene() {

    hittable_list boxes1;

    hittable_list objects;

    auto white = make_shared<lambertian>(color{ .73f, .73f, .73f });

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            shared_ptr<hittable> t = make_shared<sphere>(point3{ (float)(i * 50), 0.f, (float)(j * 50) }, random_float(30.f, 80.f), white);
            objects.add(make_shared<constant_medium>(t, 0.01f, color{ random_float(), 0, random_float() }));
        }
    }

    shared_ptr<hittable> box1 = make_shared<sphere>(point3{ 0.f, 0.f, 0.f }, 50.f, white);
    shared_ptr<hittable> box2 = make_shared<sphere>(point3{ 0.f, 0.f, 0.f }, 50.f, white);


    objects.add(make_shared<constant_medium>(box1, 0.01f, color{ 0.f, 0.f, 0.f }));

    auto light = make_shared<diffuse_light>(color{ 10.f, 10.f, 10.f });
    objects.add(make_shared<xz_rect>(123.f, 323.f, 147.f, 312.f, 554.f, light));

    auto center1 = point3{ 400.f, 400.f, 200.f };
    auto center2 = center1 + vec3{ 30.f, 0.f, 0.f };
    auto moving_sphere_material = make_shared<lambertian>(color{ 0.7f, 0.3f, 0.1f });
    objects.add(make_shared<moving_sphere>(center1, center2, 0.f, 1.f, 50.f, moving_sphere_material));

    objects.add(make_shared<sphere>(point3{ 260.f, 150.f, 45.f }, 50.f, make_shared<dielectric>(1.5f)));
    objects.add(make_shared<sphere>(
        point3{ 0.f, 150.f, 145.f }, 50.f, make_shared<metal>(color{ 0.8f, 0.8f, 0.9f }, 1.0f)
        ));

    auto boundary = make_shared<sphere>(point3{ 360.f, 150.f, 145.f }, 70.f, make_shared<dielectric>(1.5f));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2f, color{ 0.2f, 0.4f, 0.9f }));
    boundary = make_shared<sphere>(point3{ 0.f, 0.f, 0.f }, 5000.f, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001f, color{ 1, 1, 1 }));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    objects.add(make_shared<sphere>(point3{ 400.f, 200.f, 400.f }, 100.f, emat));
    auto pertext = make_shared<noise_texture>(0.1f);
    objects.add(make_shared<sphere>(point3{ 220.f, 280.f, 300.f }, 80.f, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(random_vector(0, 165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
        vec3{ -100, 270, 395}
        )
    );
    

    return objects;
}

void DrawPixel(int nSamplesPerPixel, int nImageWidth, int nImageHeight, int startHeight, int endHeight, int nMaxDepth, hittable_list& world, camera& cam, std::vector<color>& oColours, color& background, std::atomic<int>& scan_lines_remianing)
{
    XMVECTOR clamp_min = { 0.f,0.f,0.f,0.f };
    XMVECTOR clamp_max = { 0.999f,0.999f,0.999f,0.999f };
    for (int i = startHeight; i <= endHeight; i++)
    {
        scan_lines_remianing--;
        for (int j = 0; j < nImageWidth; j++)
        {
            color pixelColour{ 0.f, 0.f, 0.f };
            for (int k = 0; k < nSamplesPerPixel; k++)
            {
                auto u = (j + random_float()) / (nImageWidth - 1);
                auto v = (i + random_float()) / (nImageHeight - 1);
                Ray r = cam.get_ray(u, v);
                pixelColour += ray_color(r, background, world, nMaxDepth);
            }

            float scale = 1.0f / static_cast<float>(nSamplesPerPixel);
            color out = XMVectorSqrt(pixelColour * scale);

            out = 256.f * XMVectorClamp(out, clamp_min, clamp_max);
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
int main()
{
    float aspect_ratio = 16.0f / 9.0f;
    int image_width = 600;
    int samples_per_pixel = 100;
    const int max_depth = 50;
    

    float R = cosf(pi / 4.f);
    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    float vfov = 40.0f;
    float aperture = 0.0f;
    color background{ 0.f, 0.f, 0.f };
    
    switch (6) {
    case 1:
        world = random_scene();
        lookfrom = point3{ 13.f, 2.f, 3.f };
        lookat = point3{ 0.f, 0.f, 0.f };
        background = color{ 0.70f, 0.80f, 1.00f };
        vfov = 20.0f;
        aperture = 0.1f;
        break;
    case 2:
        world = two_spheres();
        lookfrom = point3{ 13.f, 2.f, 3.f };
        background = color{ 0.70f, 0.80f, 1.00f };
        lookat = point3{ 0.f, 0.f, 0.f };
        vfov = 20.0f;
        break;

    case 3:
        world = two_perlin_spheres();
        background = color{ 0.70f, 0.80f, 1.00f };
        lookfrom = point3{ 13.f, 2.f, 3.f };
        lookat = point3{0.f, 0.f, 0.f};
        vfov = 20.0f;
        break;
    case 4:
        world = earth();
        background = color{ 0.70f, 0.80f, 1.00f };
        lookfrom = point3{ 13.f, 2.f, 3.f };
        lookat = point3{ 0.f, 0.f, 0.f };
        vfov = 20.0f;
        break;
    case 5:
        world = simple_light();
        samples_per_pixel = 400;
        background = color{ 0.f, 0.f, 0.f };
        lookfrom = point3{ 26.f, 3.f, 6.f };
        lookat = point3{ 0.f, 2.f, 0.f };
        vfov = 20.0f;
        break;
    
    case 6:
        world = cornell_box();
        aspect_ratio = 1.0f;
        image_width = 300;
        samples_per_pixel = 400;
        background = color{ 0.f, 0.f, 0.f };
        lookfrom = point3{ 278.f, 278.f, -800.f };
        lookat = point3{ 278.f, 278.f, 0.f };
        vfov = 40.0f;
        break;
    case 7:
        world = cornell_smoke();
        aspect_ratio = 1.0f;
        image_width = 600;
        samples_per_pixel = 1000;
        background = color{ 0.f, 0.f, 0.f };
        lookfrom = point3{ 278.f, 278.f, -800.f };
        lookat = point3{ 278.f, 278.f, 0.f };
        vfov = 40.0f;
        break;
    default:
    case 8:
        world = final_scene();
        aspect_ratio = 1.0f;
        image_width = 800;
        samples_per_pixel = 10000;
        background = color{ 0.f, 0.f, 0.f };
        lookfrom = point3{ 478.f, 278.f, -800.f };
        lookat = point3{ 278.f, 278.f, 0.f };
        vfov = 40.0f;
        break;
    }

    vec3 vup{ 0.f, 1.f, 0.f };

    const int image_height = static_cast<int>(image_width / aspect_ratio);
    float dist_to_focus = 10.0f;
    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0f, 1.0f);

    auto t1 = std::chrono::high_resolution_clock::now();

    // Render

    //std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    //for (int j = image_height - 1; j >= 0; --j)
    //{
    //    std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    //    for (int i = 0; i < image_width; ++i)
    //    {
    //        color pixel_color = { 0.f, 0.f, 0.f };
    //        for (int s = 0; s < samples_per_pixel; ++s) {
    //            auto u = (i + random_float()) / (image_width - 1);
    //            auto v = (j + random_float()) / (image_height - 1);
    //            Ray r = cam.get_ray(u, v);
    //            pixel_color += ray_color(r, background, world, max_depth);
    //        }
    //        //write_color(std::cout, pixel_color, samples_per_pixel);
    //    }
    //}

    //std::cerr << "\nDone.\n";

    std::atomic<int> scan_lines_remaining = image_height;

    const int num_threads = std::thread::hardware_concurrency();
    std::vector<std::vector<color>> thread_outputs(num_threads);
    std::vector<std::thread> threads(num_threads);
    for (size_t i = 0; i < num_threads; i++)
    {
        size_t k = i + 1;
        int threadHeight = ceil(image_height / num_threads);
        int threadIndex = threadHeight * i;
        int endHeight = threadHeight + threadIndex;
        int startHeight = endHeight - threadHeight;
        thread_outputs[i].reserve(threadHeight* image_width);
        threads[i] = std::thread(&DrawPixel, samples_per_pixel, image_width, image_height, startHeight, endHeight, max_depth, 
            std::ref(world), std::ref(cam), std::ref(thread_outputs[i]), std::ref(background), std::ref(scan_lines_remaining));
    }

    //threads[num_threads] = std::thread(print_lines_remaining, std::ref(scan_lines_remaining));
    
    for (size_t i = 0; i < num_threads; i++)
    {
        threads[i].join();
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    auto t3 = std::chrono::duration_cast<std::chrono::seconds> (t2 - t1);

    std::cerr << "Time Taken: " << t3 << std::endl;

    //std::vector<color> finished;
    //for (size_t i = 0; i < num_threads; i++)
    //{
    //    finished.insert(finished.end(), thread_outputs[i].begin(), thread_outputs[i].end());
    //}

    //std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
    //for (int i = finished.size() - 1; i >= 0; i--)
    //{
    //   std::cout << (int)XMVectorGetX(finished[i]) << ' ' << (int)XMVectorGetY(finished[i]) << ' ' << (int)XMVectorGetZ(finished[i]) << std::endl;
    //}

    std::cerr << "Done" << std::flush;

}

