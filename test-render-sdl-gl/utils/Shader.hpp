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
#include <string>
#include <memory>
#include <stdexcept>
#include <filesystem>
#include <fstream>

#include "Error.hpp"

class InvalidShader {
public:
	enum class Step {
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

class ValidShader {
public:
	using Uniform = std::optional<GLuint>;
	
    ValidShader(const GLuint program) noexcept;
    ~ValidShader(void) noexcept;
	
	ValidShader(const ValidShader&) = delete;
	ValidShader operator=(const ValidShader&) = delete;

	[[nodiscard]]
	GLuint program() noexcept;

	[[nodiscard]]
    Uniform uniform(const std::string& name) noexcept;
	[[nodiscard]]
	Uniform uniform_block_index(const std::string &name) noexcept;
    void activate() noexcept;
	void set_mat4(const std::string &name, const glm::mat4 value);
	void set_int(const std::string &name, const int value);
	void set_float(const std::string &name, const float value);
	void set_vec3(const std::string &name, const glm::vec3 value);
	void set_vec3(const std::string &name, const float x, const float y, const float z);
	void set_vec4(const std::string &name, const glm::vec4 value);
	void set_vec4(const std::string &name,
				  const float x,
				  const float y,
				  const float z,
				  const float w);

private:
    GLuint m_program{0};
};

using Shader = std::variant<std::unique_ptr<ValidShader>, InvalidShader>;

class ShaderBuilder 
{
public:
	[[nodiscard]]
	static Shader produce(const char* vertex_source, const char* fragment_source) noexcept;

	[[nodiscard]]
	static Shader produce(const std::string& vertex_source, const std::string& fragment_source) noexcept;

	[[nodiscard]]	
	static std::optional<std::string> file_slurp(const std::filesystem::path& path) noexcept;
};


Shader ShaderBuilder::produce(const std::string& vertex_source, const std::string& fragment_source) noexcept
{
	return produce(vertex_source.c_str(), fragment_source.c_str());
}

Shader ShaderBuilder::produce(const char* vertex_source, const char* fragment_source) noexcept
{
    int success;
    std::array<char, 512> infoLog;

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_source, NULL);
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

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(program, infoLog.size(), NULL, infoLog.data());
		return InvalidShader(InvalidShader::Step::Program, std::string(infoLog.data()));
    }

    return std::make_unique<ValidShader>(program);
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
	
GLuint ValidShader::program() noexcept
{
    return m_program;
}

void ValidShader::activate() noexcept
{
    glUseProgram(m_program);
	GL_THROW_ON_ERROR();
}

ValidShader::Uniform ValidShader::uniform(const std::string &name) noexcept
{         
    const auto location = glGetUniformLocation(m_program, name.c_str());
    if (location == -1)
        return {};
    return {location};
}
	
ValidShader::Uniform ValidShader::uniform_block_index(const std::string &name) noexcept
{
	const auto location = glGetUniformBlockIndex(m_program, name.c_str());
    if (location == GL_INVALID_INDEX) {
        return std::nullopt;
	}
	return location;
}

void ValidShader::set_mat4(const std::string &name, const glm::mat4 value) 
{
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error(name + " uniform does not exist!");
    glUniformMatrix4fv(*location, 1, GL_FALSE, glm::value_ptr(value)); 
}

void ValidShader::set_int(const std::string &name, const int value) 
{
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error("Uniform '" + name + "' does not exist!");
    glUniform1i(*location, value); 
}

void ValidShader::set_float(const std::string &name, const float value) 
{
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error("Uniform '" + name + "' does not exist!");
    glUniform1f(*location, value); 
}

void ValidShader::set_vec3(const std::string &name, const glm::vec3 value) 
{
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error("Uniform '" + name + "' does not exist!");
    glUniform3f(*location, value.x, value.y, value.z); 
}

void ValidShader::set_vec3(const std::string &name,
						   const float x,
						   const float y,
						   const float z)
{
	set_vec3(name, glm::vec3(x, y, z));
}

void ValidShader::set_vec4(const std::string &name, const glm::vec4 value) 
{
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error("Uniform '" + name + "' does not exist!");
    glUniform4f(*location, value.x, value.y, value.z, value.w); 
}

void ValidShader::set_vec4(const std::string &name,
						   const float x,
						   const float y,
						   const float z,
						   const float w)
{
	set_vec4(name, glm::vec4(x, y, z, w));
}
