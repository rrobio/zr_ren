#include <iostream>
#include <thread>

#include "rtweekend.hpp"

#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "sphere.hpp"

color ray_color(ray const& r, hittable const& world, int depth)
{
	hit_record rec;

	if (depth <= 0) return color(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec)) {
		// point3 target = rec.p + rec.normal + random_in_unit_sphere();
		// point3 target = rec.p + rec.normal +
		// random_in_hemisphere(rec.normal); point3 target = rec.p + rec.normal
		// + random_unit_vector(); return 0.5 * ray_color(ray(rec.p, target -
		// rec.p), world, depth-1);
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return color(0, 0, 0);
	}
	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main()
{
	auto const aspect_ratio = 16.0 / 9.0;
	int const image_width = 400;
	int const image_height = static_cast<int>(image_width / aspect_ratio);
	int const samples_per_pixel = 100;
	int const max_depth = 50;

	auto R = cos(pi / 4);

	hittable_list world;

	auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left = make_shared<dielectric>(1.5);
	auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	world.add(
		make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(
		make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
	world.add(
		make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.45, material_left));
	world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));

	camera cam(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 20,
			   aspect_ratio);

	auto n_threads = 6;
	std::vector<std::thread> threads(n_threads);

	std::vector<std::vector<color>> colors(image_width, std::vector<color>(image_height));

	auto loop = [image_width, image_height, &world, &cam, max_depth,
				 &colors](int start, int stop) {
		for (int j = stop; j >= start; --j) {
			for (int i = 0; i < image_width; ++i) {
				color pixel_color(0, 0, 0);
				for (int s = 0; s < samples_per_pixel; ++s) {
					auto u = (i + random_double()) / (image_width - 1);
					auto v = (j + random_double()) / (image_height - 1);
					ray r = cam.get_ray(u, v);
					pixel_color += ray_color(r, world, max_depth);
				}
				colors.at(i).at(j) = pixel_color;
			}
		}
	};
	int step = image_height / n_threads;
	int cur_step = 0;
	for (int i = 0; i < n_threads; i++) {
		auto start = cur_step;
		auto stop = cur_step + step;
		if (stop > image_height)
			stop = image_height;
		threads.at(i) = std::thread(loop, start, stop);
		cur_step += step;
	}
	for (auto& t : threads)
		t.join();
	std::cerr << "\nWriting file.\n";
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
	for (int i = image_height - 1; i >= 0; i--) {
		for (int j = 0; j < image_width; j++) {
			write_color(std::cout, colors.at(j).at(i), samples_per_pixel);
		}
	}

	std::cerr << "Done.\n";
}
