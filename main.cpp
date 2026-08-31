#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

int main()
{
	glfwInit();
	// version de OPENGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// GLFW usando Core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// generamos los vertices e indices de una rejilla de cuadrados de 0.1
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	float squareSize = 0.1f;
	float start = -1.0f;
	int divisions = 20; // 20 cuadrados de 0.1 cubren de -1 a 1

	GLuint indexCount = 0;

	for (int row = 0; row < divisions; row++)
	{
		for (int col = 0; col < divisions; col++)
		{
			float x = start + col * squareSize;
			float y = start + row * squareSize;

			// 4 vertices de este cuadrado
			vertices.push_back(x);               vertices.push_back(y);               vertices.push_back(0.0f); // abajo-izq
			vertices.push_back(x + squareSize);  vertices.push_back(y);               vertices.push_back(0.0f); // abajo-der
			vertices.push_back(x + squareSize);  vertices.push_back(y + squareSize);  vertices.push_back(0.0f); // arriba-der
			vertices.push_back(x);               vertices.push_back(y + squareSize);  vertices.push_back(0.0f); // arriba-izq

			// 2 triangulos de este cuadrado
			indices.push_back(indexCount + 0);
			indices.push_back(indexCount + 1);
			indices.push_back(indexCount + 2);

			indices.push_back(indexCount + 0);
			indices.push_back(indexCount + 2);
			indices.push_back(indexCount + 3);

			indexCount += 4;
		}
	}

	// creamos el objeto 800 por 800 pxls con nombre "OpenGL Window"
	GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Window", nullptr, nullptr);
	// ve si la ventana se creo correctamente
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// introduce la ventana en el contexto de OpenGL
	glfwMakeContextCurrent(window);
	// carga todas las funciones de OpenGL con glad
	gladLoadGL();
	// especifica las dimensiones de la ventana de visualizacion x=0, y=0, ancho=800, alto=800
	glViewport(0, 0, 800, 800);

	Shader shaderProgram("default.vert", "default.frag");

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices.data(), (GLsizeiptr)(vertices.size() * sizeof(GLfloat)));
	EBO EBO1(indices.data(), (GLsizeiptr)(indices.size() * sizeof(GLuint)));

	VAO1.LinkVBO(VBO1, 0);
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	while (!glfwWindowShouldClose(window))
	{
		// color del fondo de la ventana (RGB + alpha)
		glClearColor(0.7f, 0.3f, 0.5f, 1.0f);
		// limpia el buffer de color y lo pinta con el color especificado en glClearColor
		glClear(GL_COLOR_BUFFER_BIT);

		// dice que programa de shader usar
		shaderProgram.Activate();
		// une el VAO al contexto de OpenGL
		VAO1.Bind();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // modo de dibujo en lineas
		// dibuja la rejilla con los vertices totales e indices especificados
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// borramos el VAO, VBO, EBO y el programa de shader
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window);
	// termina GLFW
	glfwTerminate();
	return 0;
}