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


    //check de error si la ventana falla
    if (window == NULL) {
        std::cout << "Error a crear la ventana GLFW ";
        std::cout << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    gladLoadGL(); //Carga GLAD y configura OpenGl

    glViewport(0, 0, 800, 800); //area de renderizado de la ventana x=0, y=0, a x=800, y=800 

    // --- GENERACIÓN DINÁMICA DE LA GRILLA ---
    const float STEP = 0.1f; // Medida mínima del cuadrado (0.1 en espacio NDC [-1, 1])
    std::vector<GLfloat> gridVertices;

    // OPCIÓN 1: LÍNEAS HORIZONTALES Y VERTICALES CONTINUAS
    for (float pos = -1.0f; pos <= 1.0f + 0.0001f; pos += STEP)
    {
        // Línea Vertical (x constante, va de y=-1 a y=1)
        gridVertices.push_back(pos);   gridVertices.push_back(-1.0f); gridVertices.push_back(0.0f);   gridVertices.push_back(0.4f); gridVertices.push_back(0.4f); gridVertices.push_back(0.4f);
        gridVertices.push_back(pos);   gridVertices.push_back(1.0f);  gridVertices.push_back(0.0f);   gridVertices.push_back(0.4f); gridVertices.push_back(0.4f); gridVertices.push_back(0.4f);

        // Línea Horizontal (y constante, va de x=-1 a x=1)
        gridVertices.push_back(-1.0f); gridVertices.push_back(pos);   gridVertices.push_back(0.0f);   gridVertices.push_back(0.4f); gridVertices.push_back(0.4f); gridVertices.push_back(0.4f);
        gridVertices.push_back(1.0f);  gridVertices.push_back(pos);   gridVertices.push_back(0.0f);   gridVertices.push_back(0.4f); gridVertices.push_back(0.4f); gridVertices.push_back(0.4f);
    }

    /*
    // OPCIÓN 2: GENERACIÓN CUADRADO POR CUADRADO INDEPENDIENTE
    for (float x = -1.0f; x < 1.0f - 0.0001f; x += STEP)
    {
        for (float y = -1.0f; y < 1.0f - 0.0001f; y += STEP)
        {
            float xNext = x + STEP;
            float yNext = y + STEP;
            float r = 0.4f, g = 0.4f, b = 0.4f;

            // Lado Inferior
            gridVertices.push_back(x);     gridVertices.push_back(y);     gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);
            gridVertices.push_back(xNext); gridVertices.push_back(y);     gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);

            // Lado Derecho
            gridVertices.push_back(xNext); gridVertices.push_back(y);     gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);
            gridVertices.push_back(xNext); gridVertices.push_back(yNext); gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);

            // Lado Superior
            gridVertices.push_back(xNext); gridVertices.push_back(yNext); gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);
            gridVertices.push_back(x);     gridVertices.push_back(yNext); gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);

            // Lado Izquierdo
            gridVertices.push_back(x);     gridVertices.push_back(yNext); gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);
            gridVertices.push_back(x);     gridVertices.push_back(y);     gridVertices.push_back(0.0f); gridVertices.push_back(r); gridVertices.push_back(g); gridVertices.push_back(b);
        }
    }
    */


    //GLfloat vertices[] = {  //matriz de vertices con posicion -1<< x,y <<+1
        //                 COORDENADAS                /          COLORES
           // CUADRADO
              //-0.5f,  -0.5f * float(sqrt(3)) / 2,      0.0f,  0.8f, 0.3f,  0.02f, //esquina izquierda baja
              //0.5f,  -0.5f * float(sqrt(3)) / 2,      0.0f,  0.8f, 0.3f,  0.02f, //esquina derecha baja
              //-0.5f,  0.5f * float(sqrt(3)) / 2,      0.0f,  0.8f, 0.3f,  0.02f, //esquina izquierda alta
              //0.5f,  0.5f * float(sqrt(3)) / 2,      0.0f,  0.8f, 0.3f,  0.02f, //esquina derecha alta

         //TRIANGULO
           //-0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, 
            //0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f,
            //0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f,
           //-0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f,
            //0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f,
            //0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f


    //};

    //GLuint indices[] = {
         //0, 3, 5, //triangulo izquierdo
         //3, 2, 4, // triangulo derecho
         //5, 4, 1 // triangulo superior


         //0, 1, 2, //cuadrado
         //1, 3, 2  //cuadrado

    //};

    Shader shaderProgram("Shaders/default.vert", "Shaders/default.frag");


    // Genera un objeto de matriz de vértices (VAO) y lo vincula
    VAO VAO1;
    VAO1.Bind();

    // Genera un VBO con el vector dinámico de la grilla
    VBO VBO1(gridVertices.data(), gridVertices.size() * sizeof(GLfloat));

    // Genera un objeto de búfer de vértices y lo vincula a los vértices
    //VBO VBO1(vertices, sizeof(vertices));
    // Genera un objeto de búfer de elementos y lo vincula a los índices.
    //EBO EBO1(indices, sizeof(indices));

    // Vincula atributos de VBO, como coordenadas y colores, al VAO.
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    //  Desune todas para evitar modificarlas accidentalmente.
    VAO1.Unbind();
    VBO1.Unbind();
    //EBO1.Unbind();



    while (!glfwWindowShouldClose(window))
    {
        // color de fondo
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        // limpia el back buffer y le da un color
        glClear(GL_COLOR_BUFFER_BIT);
        // le dice al programa de opengl que shader usar
        shaderProgram.Activate();
        // libera el VAO para wue opengl sepa que debe usarlo
        VAO1.Bind();
        // Dibujar primitivas, número de índices, tipo de datos de los índices, índice de los índices
        //glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0); //(triangulos)
        glDrawArrays(GL_LINES, 0, gridVertices.size() / 6);
        // cambia el back con el front buffer
        glfwSwapBuffers(window);
        // encargado de los eventos del GLWF
        glfwPollEvents();
    }

    // elimina todos los objetos creados
    VAO1.Delete();
    VBO1.Delete();
    //EBO1.Delete();
    shaderProgram.Delete();
    // Cierre la ventana antes de finalizar el programa.
    glfwDestroyWindow(window);
    // Finaliza GLFW antes de terminar el programa.
    glfwTerminate();
    return 0;
}