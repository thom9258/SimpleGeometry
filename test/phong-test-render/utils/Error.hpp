#pragma once

const char* gl_error_string(const GLenum err)
{
	switch (err) {
	case GL_NO_ERROR: return "GL_NO_ERROR";
	case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
	case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
	//case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
	//case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
	default:  return "GL_UNDEFINED_ERROR";
	}
}

void gl_throw_on_error(const char* file, unsigned int line)
{
	const auto err = glGetError();
	if (err == GL_NO_ERROR)
		return;
	throw std::runtime_error(std::string("GL error [L:") 
							 + std::to_string(line) + " F:" + file + "]: "
							 + gl_error_string(err));
}

#define GL_THROW_ON_ERROR() gl_throw_on_error(__FILE__, __LINE__)
