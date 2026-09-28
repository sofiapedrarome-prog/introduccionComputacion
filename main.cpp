#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
/* Algoritmo de Bresenham(variante con parámetro de decisión p, para 0 <= dy <= dx)
   dx = x1-x0 ; dy = y1-y0
   if dx != 0:
       y = y0 ; p = 2*dy - dx
       for i in range(dx+1):
           putPixel(x0+i, y)
          if p >= 0: y = y+1 ; p = p - 2*dx
        p = p + 2*dy; */  

// confiuracion de pixeles independientes 
const int GRID_COLS = 40; // Número de columnas (eje X)
const int GRID_ROWS = 40; // Número de filas (eje Y)

struct PixelColor {
    float r, g, b;
};

// Matriz para modificar el color de cada píxel de forma independiente
PixelColor pixelGrid[GRID_ROWS][GRID_COLS];

// Función para cambiar el color de un píxel específico (columna, fila)
void setPixelColor(int col, int row, float r, float g, float b) {
    if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS) {
        pixelGrid[row][col] = { r, g, b };
    }
}


Shader* gShader = nullptr;
VAO* gVAO = nullptr;
GLsizei gVertexCount = 0;
// Función para renderizar la grilla de píxeles
void render(GLFWwindow* window) {
    if (!gShader || !gVAO) return;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    gShader->Activate();
    gVAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, gVertexCount);
    glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    std::cout << "Nuevo tamaño: " << width << "x" << height << std::endl;
    glViewport(0, 0, width, height);
    render(window);   // redibuja mientras se arrastra el borde

}

// BRESENHAM 
void drawLineBresenham(int x0, int y0, int x1, int y1, float r, float g, float b) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        // Pintar el píxel en la posición actual
        setPixelColor(x0, y0, r, g, b);

        // Si llegó al punto final, termina
        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Función auxiliar para pintar los 8 octantes simétricos
void drawCircle8Points(int xc, int yc, int x, int y, float r, float g, float b) {
    setPixelColor(xc + x, yc + y, r, g, b);
    setPixelColor(xc - x, yc + y, r, g, b);
    setPixelColor(xc + x, yc - y, r, g, b);
    setPixelColor(xc - x, yc - y, r, g, b);
    setPixelColor(xc + y, yc + x, r, g, b);
    setPixelColor(xc - y, yc + x, r, g, b);
    setPixelColor(xc + y, yc - x, r, g, b);
    setPixelColor(xc - y, yc - x, r, g, b);
}

void drawCircleBresenham(int xc, int yc, int r_radius, float r, float g, float b) {
    int x = 0;
    int y = r_radius;
    int d = 3 - 2 * r_radius; // Parámetro de decisión inicial

    drawCircle8Points(xc, yc, x, y, r, g, b);

    while (y >= x) {
        x++;

        // Chequear si el punto está dentro o fuera del círculo ideal
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else {
            d = d + 4 * x + 6;
        }

        drawCircle8Points(xc, yc, x, y, r, g, b);
    }
}

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
	// Configura la función de callback para el redimensionamiento de la ventana
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Carga GLAD y configura OpenGL (con chequeo de error)
    if (!gladLoadGL()) {
        std::cout << "Error al inicializar GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 800); // area de renderizado de la ventana x=0, y=0, a x=800, y=800

    // inicializacion de colores base de cada pixel
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            pixelGrid[r][c] = { 0.2f, 0.2f, 0.2f }; // Color base gris oscuro
        }
    }

    // (x1, y1) hasta el píxel (x2, y2)
    //drawLineBresenham(10, 10, 19, 19, 1.0f, 0.0f, 0.0f);

    // dibujar un círculo usando el algoritmo de Bresenham
    drawCircleBresenham(20, 20, 12, 1.0f, 0.0f, 0.0f);


	// generaciom de vertices para la grilla de pixeles
    const float STEP = 0.1f; // Medida mínima del cuadrado
    std::vector<GLfloat> gridVertices;

    // Generar cuadrados como dos triángulos (triángulos llenos) y colorear por posición
    float stepX = 2.0f / GRID_COLS;
    float stepY = 2.0f / GRID_ROWS;
    float padding = 0.002f; // Margen para ver la separación


    for (int row = 0; row < GRID_ROWS; ++row) {
        for (int col = 0; col < GRID_COLS; ++col) {

            //tamaño de pixeles con un pequeño espacio de separacion 
            float x = -1.0f + col * stepX + padding;
            float y = -1.0f + row * stepY + padding;
            float xNext = -1.0f + (col + 1) * stepX - padding;
            float yNext = -1.0f + (row + 1) * stepY - padding;

            // Tomar el color INDIVIDUAL asignado a este píxel
            float r = pixelGrid[row][col].r;
            float g = pixelGrid[row][col].g;
            float b = pixelGrid[row][col].b;

            // Triángulo 1: (x,y), (xNext,y), (xNext,yNext)
            gridVertices.push_back(x);     gridVertices.push_back(y);     gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);
            gridVertices.push_back(xNext); gridVertices.push_back(y);     gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);
            gridVertices.push_back(xNext); gridVertices.push_back(yNext); gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);

            // Triángulo 2: (x,y), (xNext,yNext), (x,yNext)
            gridVertices.push_back(x);     gridVertices.push_back(y);     gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);
            gridVertices.push_back(xNext); gridVertices.push_back(yNext); gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);
            gridVertices.push_back(x);     gridVertices.push_back(yNext); gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);
        }
    }

    Shader shaderProgram("default.vert", "default.frag");

    // Genera un objeto de matriz de vértices (VAO) y lo vincula
    VAO VAO1;
    VAO1.Bind();

   

    VBO VBO1(gridVertices.data(), gridVertices.size() * sizeof(GLfloat));

    // Vincula atributos de VBO, como coordenadas y colores, al VAO.
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    // Desune todas para evitar modificarlas accidentalmente.
    VAO1.Unbind();
    VBO1.Unbind();

	// Configura la función de callback para el redimensionamiento de la ventana
    gShader = &shaderProgram;
    gVAO = &VAO1;
    gVertexCount = static_cast<GLsizei>(gridVertices.size() / 6);

    while (!glfwWindowShouldClose(window))
    {
        // color de fondo
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // limpia el back buffer y le da un color
        glClear(GL_COLOR_BUFFER_BIT);
        // le dice al programa de opengl que shader usar
        shaderProgram.Activate();
        // libera el VAO para wue opengl sepa que debe usarlo
        VAO1.Bind();
        // Dibujar como triángulos para obtener cuadrados llenos (6 vértices por cuadrado)
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(gridVertices.size() / 6));
        // cambia el back con el front buffer
        glfwSwapBuffers(window);
        // encargado de los eventos del GLWF
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