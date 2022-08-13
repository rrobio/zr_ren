// clang-format off
#include "glad/glad.h"
#include <GLFW/glfw3.h>
// clang-format on

#include <iostream>
#include <cassert>
#include <array>
#include <functional>
#include <memory>
#include <random>
#include <filesystem>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// clang-format off
#include "window.hpp"
#include "input.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "resource_manager.hpp"
#include "texture.hpp"
#include "cubemap.hpp"
#include "log.hpp"
#include "material.hpp"
// clang-format on

int const screen_width = 1024;
int const screen_height = 768;
int const channel_count = 4;
float const screen_aspect =
	static_cast<float>(screen_width) / static_cast<float>(screen_height);
int const shadow_width = 1440;
int const shadow_height = 1440;
float const shadow_aspect =
	static_cast<float>(shadow_width) / static_cast<float>(shadow_height);

bool should_close = false;
bool debug = false;

inline double random_double()
{
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

void render_scene(ren::Shader& shader, std::vector<ren::Object> const& world)
{
	shader.use();
	for (auto const& obj : world) {
		shader.set("model", obj.model());
		obj.draw();
	}
}
void toggle_debug(ren::Window& window, ImGuiIO& io)
{
	auto const io_flags = ImGuiConfigFlags_NoMouse;
	if (!debug) {
		window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		io.ConfigFlags &= ~io_flags;
		debug = true;
	}
	else {
		window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		io.ConfigFlags |= io_flags;
		debug = false;
	}
}

auto get_root_directory() -> std::filesystem::path {
    auto path = std::filesystem::canonical("/proc/self/exe");
    do {
        path = path.parent_path();
    } while (!path.parent_path().string().ends_with("/zr"));

    return path.parent_path();
}

int main()
{
    //auto current_working_directory = std::filesystem::current_path();
    auto const root_directory = get_root_directory();
    auto const ren_directory = root_directory / "subprojects/ren";
    std::cout << ren_directory << "\n";

	auto window = ren::Window("ren", screen_width, screen_height, false);
	window.set_input_mode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	window.set_input_mode(GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	auto cam =
		ren::Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.f, 0.f, 0.f),
					glm::vec3(0.0f, 1.0f, 0.0f), screen_aspect);

	std::vector<ren::Object> world {};
	world.push_back(ren::create_plane());
	for (size_t i = 0; i < 10; i++) { world.push_back(ren::create_sphere()); }

	auto solid_shader = ren::Shader(ren_directory / "shaders/solid_color.vert",
									ren_directory / "shaders/solid_color.frag");
	assert(solid_shader.success());
	auto depth_shader = ren::Shader(ren_directory / "shaders/point_shadow_depth.vert",
									ren_directory / "shaders/point_shadow_depth.frag",
									ren_directory / "shaders/point_shadow_depth.geom");
	assert(depth_shader.success());
	auto shadow_shader =
		ren::Shader(ren_directory / "shaders/shadows.vert", ren_directory / "shaders/shadows.frag");
	assert(shadow_shader.success());
	glEnable(GL_DEPTH_TEST);

	auto no_tex_img = ren::Image(ren_directory / "res/tex/no-tex.png");
	auto no_tex = ren::Texture(no_tex_img);

	auto light_pos = glm::vec3(-2.f, 9.f, -1.f);

	ImGui::CreateContext();
	auto& io = ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL(window.get_ptr(), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	ren::Log::init();
	bool p_open = true;

	auto plane_model = glm::scale(glm::mat4(1.0f), glm::vec3(15.f, 1.f, 15.f));
	plane_model = glm::translate(plane_model, glm::vec3(0.f, -5.f, 0.f));

	auto light_sphere = ren::create_sphere();

	auto world_model = glm::scale(glm::mat4(1), glm::vec3(30.f, 30.f, 30.f));

	auto material = std::make_shared<ren::Material>();
	material->diffuse = glm::vec3(0.2f, 0.5f, 0.1f);
	world[0].set_model(plane_model);
	for (size_t i = 1; i < world.size(); i++) {
		auto x = (random_double() * 2 - 1) * 5;
		auto y = (random_double() * 2 - 1) * 5;
		auto z = (random_double() * 2 - 1) * 5;
		auto cube_model = glm::translate(glm::mat4(1), glm::vec3(x, y, z));

		world[i].set_model(cube_model);
	}
	//  RT ---------------------
	int const DATA_SIZE = screen_width * screen_height * channel_count;

	GLuint pbo;
	glGenBuffers(1, &pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
	void* mapped_buffer = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	GLubyte* image_data = new GLubyte[DATA_SIZE];

	GLuint rt_tex;
	glGenTextures(1, &rt_tex);
	glBindTexture(GL_TEXTURE_2D, rt_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screen_width, screen_height, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)image_data);
	glBindTexture(GL_TEXTURE_2D, 0);

	// \RT ---------------------
	GLuint depth_map_FBO;
	glGenFramebuffers(1, &depth_map_FBO);

	GLuint depth_cubemap;
	glGenTextures(1, &depth_cubemap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);
	for (GLuint i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
					 shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT,
					 GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_FBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	float near_plane = 1.0f;
	float far_plane = 25.0f;
	glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), shadow_aspect,
											 near_plane, far_plane);
	std::vector<glm::mat4> shadow_transforms;
	shadow_transforms.push_back(
		shadow_proj
		* glm::lookAt(light_pos, light_pos + glm::vec3(1.0f, 0.0f, 0.0f),
					  glm::vec3(0.0f, -1.0f, 0.0f)));
	shadow_transforms.push_back(
		shadow_proj
		* glm::lookAt(light_pos, light_pos + glm::vec3(-1.0f, 0.0f, 0.0f),
					  glm::vec3(0.0f, -1.0f, 0.0f)));
	shadow_transforms.push_back(
		shadow_proj
		* glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 1.0f, 0.0f),
					  glm::vec3(0.0f, 0.0f, 1.0f)));
	shadow_transforms.push_back(
		shadow_proj
		* glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, -1.0f, 0.0f),
					  glm::vec3(0.0f, 0.0f, -1.0f)));
	shadow_transforms.push_back(
		shadow_proj
		* glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, 1.0f),
					  glm::vec3(0.0f, -1.0f, 0.0f)));
	shadow_transforms.push_back(
		shadow_proj
		* glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, -1.0f),
					  glm::vec3(0.0f, -1.0f, 0.0f)));

	auto const speed = 0.05f;
	auto keymap = std::make_shared<ren::Keymap>();
	keymap->set_bind({GLFW_KEY_ESCAPE, []() { should_close = true; }});
	keymap->set_bind({GLFW_KEY_W, [&cam, speed]() {
						  cam.move(glm::vec3(0.f, 0.f, 1.f), speed);
					  }});
	keymap->set_bind({GLFW_KEY_A, [&cam, speed]() {
						  cam.move(glm::vec3(-1.f, 0.f, 0.f), speed);
					  }});
	keymap->set_bind({GLFW_KEY_S, [&cam, speed]() {
						  cam.move(glm::vec3(0.f, 0.f, -1.f), speed);
					  }});
	keymap->set_bind({GLFW_KEY_D, [&cam, speed]() {
						  cam.move(glm::vec3(1.f, 0.f, 0.f), speed);
					  }});
	keymap->set_bind({GLFW_KEY_E, [&cam, speed]() {
						  cam.move(glm::vec3(0.f, 1.f, 0.f), speed);
					  }});
	keymap->set_bind({GLFW_KEY_Q, [&cam, speed]() {
						  cam.move(glm::vec3(0.f, -1.f, 0.f), speed);
					  }});
	keymap->set_bind(
		{GLFW_KEY_F1, [&window, &io]() { toggle_debug(window, io); }});
	keymap->set_bind({GLFW_KEY_R, [&world, &image_data, &cam]() {
									  //					  render_scene_rt(world, cam, image_data);
					  }});
	window.set_keymap(keymap);

	while (!should_close) {
		glClearColor(0.3f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto ticks = glfwGetTime();

		light_pos = glm::vec3(glm::sin(ticks) * 3, 10.f, glm::cos(ticks) * 3);

		auto light_model = glm::translate(glm::mat4(1.f), light_pos);

		light_sphere.set_model(light_model);
		shadow_transforms[0] =
			(shadow_proj
			 * glm::lookAt(light_pos, light_pos + glm::vec3(1.0f, 0.0f, 0.0f),
						   glm::vec3(0.0f, -1.0f, 0.0f)));
		shadow_transforms[1] =
			(shadow_proj
			 * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0f, 0.0f, 0.0f),
						   glm::vec3(0.0f, -1.0f, 0.0f)));
		shadow_transforms[2] =
			(shadow_proj
			 * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 1.0f, 0.0f),
						   glm::vec3(0.0f, 0.0f, 1.0f)));
		shadow_transforms[3] =
			(shadow_proj
			 * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, -1.0f, 0.0f),
						   glm::vec3(0.0f, 0.0f, -1.0f)));
		shadow_transforms[4] =
			(shadow_proj
			 * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, 1.0f),
						   glm::vec3(0.0f, -1.0f, 0.0f)));
		shadow_transforms[5] =
			(shadow_proj
			 * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, -1.0f),
						   glm::vec3(0.0f, -1.0f, 0.0f)));

		auto proj =
			glm::perspective(glm::radians(90.0f), screen_aspect, 0.1f, 100.f);
		auto view = cam.view();

		// solid_shader.set("projection", proj);
		// solid_shader.set("view", view);
		// render_scene(solid_shader, world);
		// 1. render scene to depth cubemap
		// --------------------------------
		glViewport(0, 0, shadow_width, shadow_height);
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_FBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		depth_shader.use();
		for (unsigned int i = 0; i < 6; ++i)
			depth_shader.set(
				("shadow_matrices[" + std::to_string(i) + "]").c_str(),
				shadow_transforms[i]);
		depth_shader.set("far_plane", far_plane);
		depth_shader.set("light_pos", light_pos);
		render_scene(depth_shader, world);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. render scene as normal
		// -------------------------
		glViewport(0, 0, screen_width, screen_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shadow_shader.use();
		shadow_shader.set("projection", proj);
		shadow_shader.set("view", view);
		// set lighting uniforms
		shadow_shader.set("light.position", light_pos);
		shadow_shader.set("view_pos", cam.pos());
		// shadow_shader.set<int>("shadows", true); // enable/disable shadows by
		// pressing 'SPACE'
		shadow_shader.set("far_plane", far_plane);

		shadow_shader.set<GLuint>("diffuse_texture", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, no_tex.id);

		shadow_shader.set<GLuint>("depth_map", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);
		render_scene(shadow_shader, world);
		solid_shader.use();
		solid_shader.set<glm::mat4>("projection", proj);
		solid_shader.set<glm::mat4>("view", view);
		solid_shader.set<glm::mat4>("model", light_model);
		solid_shader.set<glm::vec3>("color", {1.f, 1.f, 1.f});
		light_sphere.draw();

		window.poll_events();
		window.exec_keymap();
		cam.rotate_offset(window.get_cursor_pos());

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		auto frametime = glfwGetTime() - ticks;
		ImGui::Text("FPS: %f", ImGui::GetIO().Framerate);
		ImGui::Text("frametime: %f", frametime);
		ren::Log::the().draw("Log");

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		window.swap_buffers();
	}
	ren::Log::destroy();
	delete[] image_data;
}
