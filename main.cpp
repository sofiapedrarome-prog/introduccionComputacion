#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ShaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"


int main()
{
	glfwInit();
	// version de OPENGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// GLFW usando Core profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// coordenadas de los vertices del triangulo (x,y,z(no hay))
	GLfloat vertices[] = {
	-0.5f, -0.5f, 0.0f, // 0 left
	 0.5f, -0.5f, 0.0f, // 1 right
	 0.0f,  0.5f, 0.0f, // 2 top

	 0.0f, -0.5f, 0.0f, // 3 mid inferior
	-0.25f, 0.0f, 0.0f, // 4 mid izquierdo
	 0.25f, 0.0f, 0.0f  // 5 mid derecho
	};

	GLuint indices[] = {
		0, 4, 3, // triángulo abajo-izquierda
		3, 1, 5, // triángulo abajo-derecha
		4, 5, 2  // triángulo arriba
	};

	// creamos el objeto 800 por 800 pxls con nomvbre "OpenGL Window"
	GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL Window", nullptr, nullptr);
	// ve si ;la ventana se creo correctamente
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
	// especifica las demiensiones de la ventana de visualizacion x=0, y=0, ancho=800, alto=600
	glViewport(0, 0, 800, 800);	

	Shader shaderProgram("default.vert", "default.frag");
	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));
	VAO1.LinkVBO(VBO1, 0);
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// creamos el Vertex Array Object y el Vertex Buffer Object
	GLuint VAO,VBO, EBO;
	// generamos el VAO, VBO y EBO solo con un objeto de cada uno
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	
	//unimos el VBO al VAO y le pasamos los vertices del triangulo
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//unimos el EBO al VAO y le pasamos los indices del triangulo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// especificamos como OpenGL debe interpretar los vertices del triangulo
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//vertice posicion 0
	glEnableVertexAttribArray(0);

	// desunimos el VBO, VAO y EBO  
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	// color del fondo de la ventana (RGB + alpha)
	glClearColor(0.7f, 0.3f, 0.5f, 1.0f);
	// limpia el buffer de color y lo pinta con el color especificado en glClearColor
	glClear(GL_COLOR_BUFFER_BIT);
	// intercambia el buffer de color (dibuja en la ventana)
	glfwSwapBuffers(window);

	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.7f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//dice que programa de shader usar 
		shaderProgram.Activate();
		//une el VAO al contexto de OpenGL
		VAO1.Bind();
		// dibuja el triangulo con los vertices totales e indices especificados
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
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