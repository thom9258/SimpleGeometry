#pragma once

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <SDL_opengl.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <optional>
#include <variant>
#include <array>
#include <string_view>
#include <memory>
#include <stdexcept>
#include <filesystem>
#include <functional>
#include <format>
#include <iostream>
#include <fstream>

#include "Error.hpp"

class InvalidShader {
public:
	enum class Step {
		FileLoad,
		Vertex,
		Fragment,
		Program,
	};
	
	InvalidShader()
		: InvalidShader(Step::Vertex, "?")
	{}
	InvalidShader(const Step where, const std::string what)
		: where(where), what(what)
	{}

	const Step where;
	const std::string what;
};

class ActivatedShader 
{
public:
	explicit ActivatedShader(GLuint program) noexcept;
	~ActivatedShader();

	using Uniform = std::optional<GLuint>;
	[[nodiscard]]
	Uniform uniform(const std::string_view name) noexcept;

	void set_mat4(const std::string_view name, const glm::mat4 value);
	void set_int(const std::string_view name, const int value);
	void set_float(const std::string_view name, const float value);
	void set_vec3(const std::string_view name, const glm::vec3 value);
	void set_vec3(const std::string_view name, const float x, const float y, const float z);
	void set_vec4(const std::string_view name, const glm::vec4 value);
	void set_vec4(const std::string_view name,
				  const float x,
				  const float y,
				  const float z,
				  const float w);
private:
	GLuint m_activated_program;
};
	

class ValidShader 
{
public:
	ValidShader(const GLuint program) noexcept;
	~ValidShader(void) noexcept;
	
	ValidShader(ValidShader&& rhs)
	{
		std::swap(m_program, rhs.m_program);
	}

	ValidShader& operator=(ValidShader&& rhs)
	{
		std::swap(m_program, rhs.m_program);
		return *this;
	}

	ValidShader(const ValidShader&) = delete;
	ValidShader& operator=(const ValidShader&) = delete;

	using DoWithActivatedShader = std::function<void(ActivatedShader&)>;
	void with_activated(DoWithActivatedShader&& do_with);
	
private:
    GLuint m_program{0};
};

using Shader = std::variant<ValidShader, InvalidShader>;

class ShaderBuilder 
{
public:
	[[nodiscard]]
	static Shader produce(const char* vertex_source,
						  const char* fragment_source) noexcept;

	[[nodiscard]]
	static Shader produce(const std::string& vertex_source,
						  const std::string& fragment_source) noexcept;

	[[nodiscard]]
	static Shader slurp_produce(const std::filesystem::path& vertex_path,
								const std::filesystem::path& fragment_path) noexcept;


	[[nodiscard]]	
	static std::optional<std::string> file_slurp(const std::filesystem::path& path) noexcept;
};

Shader ShaderBuilder::slurp_produce(const std::filesystem::path& vertex_path,
									const std::filesystem::path& fragment_path) noexcept
{

	std::cout << std::format("Producing shader from: {} and {}",
							 vertex_path.string(),
							 fragment_path.string()) 
			  << std::endl;
	const auto vertex_source = file_slurp(vertex_path);
	if (!vertex_source)
		return InvalidShader(InvalidShader::Step::FileLoad,
							 std::string("Could not Load vertex source from path:")
							 + vertex_path.string());

	const auto fragment_source = file_slurp(fragment_path);
	if (!fragment_source)
		return InvalidShader(InvalidShader::Step::FileLoad,
							 std::string("Could not Load fragment source from path:")
							 + fragment_path.string());

	return produce(*vertex_source, *fragment_source);
}

Shader ShaderBuilder::produce(const std::string& vertex_source, const std::string& fragment_source) noexcept
{
	return produce(vertex_source.c_str(), fragment_source.c_str());
}

Shader ShaderBuilder::produce(const char* vertex_source,
							  const char* fragment_source) noexcept
{
	GL_THROW_ON_ERROR();

    int success;
    std::array<char, 512> infoLog;

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	GL_THROW_ON_ERROR();
	
    glShaderSource(vertex, 1, &vertex_source, NULL);
	GL_THROW_ON_ERROR();
    glCompileShader(vertex);
	GL_THROW_ON_ERROR();
	
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex, infoLog.size(), NULL, infoLog.data());
		return InvalidShader(InvalidShader::Step::Vertex, std::string(infoLog.data()));
    }
    
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source, NULL);
    glCompileShader(fragment);
	GL_THROW_ON_ERROR();

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success) {
		glDeleteShader(vertex);
        glGetShaderInfoLog(fragment, infoLog.size(), NULL, infoLog.data());
		return InvalidShader(InvalidShader::Step::Fragment, std::string(infoLog.data()));
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
	GL_THROW_ON_ERROR();

    glDeleteShader(vertex);
    glDeleteShader(fragment); 
	GL_THROW_ON_ERROR();

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(program, infoLog.size(), NULL, infoLog.data());
		return InvalidShader(InvalidShader::Step::Program, std::string(infoLog.data()));
    }

	GL_THROW_ON_ERROR();
    return ValidShader(program);
}

auto get_valid_shader_or_throw(Shader&& shader)
	-> ValidShader
{
	if (std::holds_alternative<InvalidShader>(shader))
		throw std::runtime_error(std::get<InvalidShader>(shader).what);
	return std::move(std::get<ValidShader>(shader));
}

std::optional<std::string> ShaderBuilder::file_slurp(const std::filesystem::path& path) noexcept
{
	std::error_code err;
	if (!std::filesystem::is_regular_file(path, err))
		return std::nullopt;

	std::ifstream t(path.string());
	std::string str;

	t.seekg(0, std::ios::end);   
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),
				std::istreambuf_iterator<char>());

	return str;
}

ValidShader::ValidShader(const GLuint program) noexcept
    : m_program(program) 
{}

ValidShader::~ValidShader(void) noexcept 
{
    glDeleteShader(m_program);
}
	
void ValidShader::with_activated(DoWithActivatedShader&& do_with)
{
	auto activated = ActivatedShader(m_program);
	std::invoke(do_with, activated);
}

ActivatedShader::ActivatedShader(const GLuint program) noexcept
{
    glUseProgram(program);
	GL_THROW_ON_ERROR();
	m_activated_program = program;
}

ActivatedShader::~ActivatedShader()
{
    glUseProgram(0);
}

ActivatedShader::Uniform ActivatedShader::uniform(const std::string_view name) noexcept
{         
    const auto location = glGetUniformLocation(m_activated_program, name.data());
    if (location == -1)
        return {};
    return {location};
}
	
void ActivatedShader::set_mat4(const std::string_view name, const glm::mat4 value) 
{
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error(std::string(name) + " uniform does not exist!");
    glUniformMatrix4fv(*location, 1, GL_FALSE, glm::value_ptr(value)); 
}

void ActivatedShader::set_int(const std::string_view name, const int value) 
{
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error(std::string(name) + " uniform does not exist!");
    glUniform1i(*location, value); 
}

void ActivatedShader::set_float(const std::string_view name, const float value) 
{
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error(std::string(name) + " uniform does not exist!");
    glUniform1f(*location, value); 
}

void ActivatedShader::set_vec3(const std::string_view name, const glm::vec3 value) 
{
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error(std::string(name) + " uniform does not exist!");
    glUniform3f(*location, value.x, value.y, value.z); 
}

void ActivatedShader::set_vec3(const std::string_view name,
							   const float x,
							   const float y,
							   const float z)
{
	set_vec3(name, glm::vec3(x, y, z));
}

void ActivatedShader::set_vec4(const std::string_view name, const glm::vec4 value) 
{
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error(std::string(name) + " uniform does not exist!");
    glUniform4f(*location, value.x, value.y, value.z, value.w); 
}

void ActivatedShader::set_vec4(const std::string_view name,
							   const float x,
							   const float y,
							   const float z,
							   const float w)
{
	set_vec4(name, glm::vec4(x, y, z, w));
}
