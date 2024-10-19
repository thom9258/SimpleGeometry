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

// helper type for the visitor #4
template<class... Ts> struct variant_switch : Ts... { using Ts::operator()...; };

// explicit deduction guide (not needed as of C++20)
template<class... Ts> variant_switch(Ts...) -> variant_switch<Ts...>;

class Shader {
public:
	enum class Step {
		Vertex,
		Fragment,
		Program,
	};

	struct Error {
		Shader::Step where;
		std::string what;

		Error(const Shader::Step where, const std::string what)
			: where(where), what(what)
		{}
	};

	using Uptr = std::unique_ptr<Shader>;
	using ShaderOrError = std::variant<Shader::Uptr, Error>;
	using Uniform = std::optional<GLuint>;
	
	[[nodiscard]]
    static ShaderOrError create(const char* vertex_source,
							    const char* fragment_source);

	[[nodiscard]]	
	static std::optional<std::string> file_slurp(const std::filesystem::path& path) noexcept;


    explicit Shader(const GLuint program) noexcept;
    ~Shader(void) noexcept;

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
	void set_vec4(const std::string &name, const glm::vec4 value);

private:
    GLuint m_program{0};
};

Shader::Shader(const GLuint program) noexcept
    : m_program(program) 
{
}

Shader::~Shader(void) noexcept 
{
    glDeleteShader(m_program);
}

Shader::ShaderOrError Shader::create(const char* vertex_source, const char* fragment_source) 
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
		return Error(Shader::Step::Vertex, std::string(infoLog.data()));
    }
    
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_source, NULL);
    glCompileShader(fragment);
	GL_THROW_ON_ERROR();

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success) {
		glDeleteShader(vertex);
        glGetShaderInfoLog(fragment, infoLog.size(), NULL, infoLog.data());
		return Error(Shader::Step::Fragment, std::string(infoLog.data()));
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
		return Error(Shader::Step::Program, std::string(infoLog.data()));
    }

    return std::make_unique<Shader>(program);
}

std::optional<std::string> Shader::file_slurp(const std::filesystem::path& path) noexcept
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
	
GLuint Shader::program() noexcept
{
    return m_program;
}

void Shader::activate() noexcept
{
    glUseProgram(m_program);
	GL_THROW_ON_ERROR();
}

Shader::Uniform Shader::uniform(const std::string &name) noexcept
{         
    const auto location = glGetUniformLocation(m_program, name.c_str());
    if (location == -1)
        return {};
    return {location};
}
	
Shader::Uniform Shader::uniform_block_index(const std::string &name) noexcept
{
	const auto location = glGetUniformBlockIndex(m_program, name.c_str());
    if (location == GL_INVALID_INDEX) {
        return std::nullopt;
	}
	return location;
}

void Shader::set_mat4(const std::string &name, const glm::mat4 value) {
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error(name + " uniform does not exist!");
    glUniformMatrix4fv(*location, 1, GL_FALSE, glm::value_ptr(value)); 
}

void Shader::set_int(const std::string &name, const int value) {
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error("Uniform '" + name + "' does not exist!");
    glUniform1i(*location, value); 
}

void Shader::set_float(const std::string &name, const float value) {
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error("Uniform '" + name + "' does not exist!");
    glUniform1f(*location, value); 
}

void Shader::set_vec3(const std::string &name, const glm::vec3 value) {
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error("Uniform '" + name + "' does not exist!");
    glUniform3f(*location, value.x, value.y, value.z); 
}

void Shader::set_vec4(const std::string &name, const glm::vec4 value) {
    const auto location = uniform(name);
    if (!location)
		throw std::runtime_error("Uniform '" + name + "' does not exist!");
    glUniform4f(*location, value.x, value.y, value.z, value.w); 
}
