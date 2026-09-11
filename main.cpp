#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

int main()
{
    // Inicializar GLFW
    glfwInit();

    // especificar version de OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Ventana y sus parametros
    GLFWwindow* window = glfwCreateWindow(800, 800, "Mi primer ventana", NULL, NULL);

    // check de error si la ventana falla
    if (window == NULL) {
        std::cout << "Error al crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Carga GLAD y configura OpenGL (con chequeo de error)
    if (!gladLoadGL()) {
        std::cout << "Error al inicializar GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 800); // area de renderizado de la ventana x=0, y=0, a x=800, y=800

    // --- GENERACIÓN DINÁMICA DE LA GRILLA (celdas rellenas hechas de 2 triángulos c/u) ---
    const float STEP = 0.1f;   // Tamaño de cada celda, en espacio NDC [-1, 1]
    const float MARGIN = 0.015f; // Espacio entre celdas para que se vea la separación de la grilla

    // Cuántas celdas hay por lado
    const int numCells = static_cast<int>((2.0f / STEP) + 0.5f);

    std::vector<GLfloat> gridVertices; // posición (3) + color (3) por vértice

    // Agrega un solo vértice (posición + color) al buffer
    auto addVertex = [&](float x, float y, float r, float g, float b)
        {
            gridVertices.push_back(x); gridVertices.push_back(y); gridVertices.push_back(0.0f);
            gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);
        };

    // Agrega una celda (cuadrado) como 2 triángulos, dadas sus 4 esquinas en orden
    auto addCell = [&](float x0, float y0, float x1, float y1, float r, float g, float b)
        {
            // Triángulo 1: inferior-izquierda, inferior-derecha, superior-izquierda
            addVertex(x0, y0, r, g, b);
            addVertex(x1, y0, r, g, b);
            addVertex(x0, y1, r, g, b);
            // Triángulo 2: inferior-derecha, superior-derecha, superior-izquierda
            addVertex(x1, y0, r, g, b);
            addVertex(x1, y1, r, g, b);
            addVertex(x0, y1, r, g, b);
        };

    // Genera cada celda de la grilla, encogida por MARGIN para que se note la separación
    for (int j = 0; j < numCells; j++)
    {
        float y0 = -1.0f + j * STEP + MARGIN;
        float y1 = -1.0f + (j + 1) * STEP - MARGIN;
        for (int i = 0; i < numCells; i++)
        {
            float x0 = -1.0f + i * STEP + MARGIN;
            float x1 = -1.0f + (i + 1) * STEP - MARGIN;
            addCell(x0, y0, x1, y1, 0.4f, 0.4f, 0.4f);
        }
    }

    Shader shaderProgram("default.vert", "default.frag");

    // Genera un objeto de matriz de vértices (VAO) y lo vincula
    VAO VAO1;
    VAO1.Bind();

    // Genera un VBO con los vértices de la grilla
    VBO VBO1(gridVertices.data(), gridVertices.size() * sizeof(GLfloat));

    // Vincula atributos de VBO, como coordenadas y colores, al VAO.
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    // Desune todas para evitar modificarlas accidentalmente.
    VAO1.Unbind();
    VBO1.Unbind();

    // Número de vértices a dibujar (se calcula una sola vez fuera del loop)
    const GLsizei numVertices = static_cast<GLsizei>(gridVertices.size() / 6);

    while (!glfwWindowShouldClose(window))
    {
        // color de fondo
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        // limpia el back buffer y le da un color
        glClear(GL_COLOR_BUFFER_BIT);
        // le dice al programa de opengl que shader usar
        shaderProgram.Activate();
        // vincula el VAO para que opengl sepa que debe usarlo
        VAO1.Bind();
        // Dibujar los triángulos de cada celda
        glDrawArrays(GL_TRIANGLES, 0, numVertices);
        // cambia el back con el front buffer
        glfwSwapBuffers(window);
        // encargado de los eventos del GLFW
        glfwPollEvents();
    }

    // elimina todos los objetos creados
    VAO1.Delete();
    VBO1.Delete();
    shaderProgram.Delete();
    // Cierra la ventana antes de finalizar el programa.
    glfwDestroyWindow(window);
    // Finaliza GLFW antes de terminar el programa.
    glfwTerminate();
    return 0;
}