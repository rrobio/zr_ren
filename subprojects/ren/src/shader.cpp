#include "shader.hpp"

#include <fstream>
#include <sstream>

namespace ren {

auto check_compile(GLuint shader, Shader::Type type, std::string_view name)
	-> bool
{
	GLint success;
	char info_log[1024];
	if (type != Shader::Type::PROGRAM) {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, nullptr, info_log);
			std::cerr << "ERROR (" << name << "): " << (int)type
					  << " Shader compilation error: " << info_log << '\n';
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 512, nullptr, info_log);
			std::cerr << "ERROR (" << name
					  << "): Shader linking error: " << info_log << '\n';
		}
	}

	return success;
};

Shader::Shader(std::filesystem::path const& vertex_path, std::filesystem::path const& fragment_path,
			   std::filesystem::path const& geometry_path)
{
	std::ifstream vertex_file(vertex_path, std::ios::ate);
	std::ifstream fragment_file(fragment_path, std::ios::ate);
	std::ifstream geometry_file;

	assert(vertex_file.is_open() && fragment_file.is_open());
	if (!geometry_path.empty()) {
		geometry_file.open(geometry_path, std::ios::ate);
		assert(geometry_file.is_open());
	}

	size_t v_size = static_cast<size_t>(vertex_file.tellg());
	size_t f_size = static_cast<size_t>(fragment_file.tellg());
	vertex_file.seekg(0);
	fragment_file.seekg(0);

	size_t g_size {};
	if (!geometry_path.empty()) {
		g_size = static_cast<size_t>(geometry_file.tellg());
		geometry_file.seekg(0);
	}

	std::vector<char> v_code_buffer(v_size);
	vertex_file.read(v_code_buffer.data(), v_size);
	std::string v_string(v_code_buffer.cbegin(), v_code_buffer.cend());
	vertex_file.close();

	std::vector<char> f_code_buffer(f_size);
	fragment_file.read(f_code_buffer.data(), f_size);
	std::string f_string(f_code_buffer.cbegin(), f_code_buffer.cend());
	fragment_file.close();

	std::string g_string;
	if (!geometry_path.empty()) {
		std::vector<char> g_code_buffer(g_size);
		geometry_file.read(g_code_buffer.data(), g_size);
		g_string = std::string(g_code_buffer.cbegin(), g_code_buffer.cend());
		geometry_file.close();
	}
	GLuint vertex, fragment, geometry;

	char const* v_code = v_string.c_str();
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_code, nullptr);
	glCompileShader(vertex);

	char const* f_code = f_string.c_str();
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_code, nullptr);
	glCompileShader(fragment);

	char const* g_code;
	if (!geometry_path.empty()) {
		g_code = g_string.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &g_code, nullptr);
		glCompileShader(geometry);
	}

	if (!check_compile(vertex, Shader::Type::VERTEX, vertex_path.string())) {
		m_success = false;
		return;

	}
	if (!check_compile(fragment, Shader::Type::FRAGMENT, fragment_path.string())) {
		m_success = false;
		return;
	}

	if (!geometry_path.empty()) {
		if (!check_compile(geometry, Shader::Type::GEOMETRY, geometry_path.string())){
			m_success = false;
			return;
		}
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (!geometry_path.empty()) glAttachShader(ID, geometry);
	glLinkProgram(ID);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (!geometry_path.empty()) glDeleteShader(geometry);

	if (!check_compile(ID, Shader::Type::PROGRAM, vertex_path.string())){
		m_success = false;
		return;
	}

	m_success = true;
}
} // namespace ren
