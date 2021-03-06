#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include <string>
#include <map>
#include <vector>
#include <glm/glm.hpp>

namespace wagl {

	class ShaderProgram
	{
	public:
		struct Uniform {
			Uniform() {
			}

			Uniform(GLuint program, GLint location, GLenum type, GLsizei size);

			void operator=(bool rhs) const;
			void operator=(GLint rhs) const;
			void operator=(GLuint rhs) const;
			void operator=(GLfloat rhs) const;


			void operator=(const glm::vec3& rhs) const;
			void operator=(const glm::vec2& rhs) const;

			void operator=(const glm::mat4& rhs) const;

			GLuint program;
			GLenum type;
			GLsizei size;
			GLint location;

			static Uniform invalid;
		};

		struct Attribute {
			Attribute() {
			}

			Attribute(GLint location, GLenum type, GLsizei size)
				: type(type), size(size), location(location) {
			}
			GLenum type;
			GLsizei size;
			GLint location;
		};

		ShaderProgram(Shader<GL_VERTEX_SHADER>* vertex, Shader<GL_FRAGMENT_SHADER>* fragment);
		ShaderProgram(Shader<GL_VERTEX_SHADER>* vertex, Shader<GL_GEOMETRY_SHADER>* geometry, Shader<GL_FRAGMENT_SHADER>* fragment);
		~ShaderProgram();


		const Attribute& GetAttribute(const int index) const;
		const Attribute& GetAttribute(const std::string name) const;
		bool hasAttribute(const std::string attribute) const;
		const Uniform& Getuniform(const std::string uniform) const;

		void use();

	private:

		void registerUniforms();
		void registerAttributes();
		void link();

		static bool isSamplerType(GLenum type);

		std::map<std::string, Uniform> uniforms;
		std::vector<Attribute> attributes;
		std::map<std::string, int> attributeIndexMap;

		GLuint vertexShader;
		GLuint fragmentShader;
		GLuint geometryShader;

		GLuint program{};
	};

}
