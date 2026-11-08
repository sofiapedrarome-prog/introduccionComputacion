#include <glad/glad.h> //carga de funciones de OpenGL
#include <GLFW/glfw3.h> //ventana, teclado y mouse
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h" // dibujan panel
#include "imgui/imgui_impl_opengl3.h"
#include <iostream>
#include <vector>  
#include <cstdlib> // para abs()
#include <cmath>   // para sqrt()
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

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
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    gShader->Activate();
    gVAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, gVertexCount);
    glfwSwapBuffers(window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { // cambia segun el tamaño de la ventana
    std::cout << "Nuevo tamaño: " << width << "x" << height << std::endl;
    glViewport(0, 0, width, height);
    render(window);   // llama render para actualizar la ventana con el nuevo tamaño
}

// línea de Bresenham en cualquier dirección (los 8 octantes)
void drawLineBresenham(int x0, int y0, int x1, int y1, float r, float g, float b) {
    int dx = abs(x1 - x0);  // abs pq no importa la direccion
    int dy = abs(y1 - y0);

    // Sentido de avance en cada eje
    int sx = (x0 < x1) ? 1 : -1; // si x0 < x1, sx = 1 (avanza a la derecha), sino sx = -1 (avanza a la izquierda)
    int sy = (y0 < y1) ? 1 : -1; // si y0 < y1, sy = 1 (avanza hacia arriba), sino sy = -1 (avanza hacia abajo)

    if (dx >= dy) {
        // Línea "horizontal": X es el eje principal, Y avanza según p
        int p = 2 * dy - dx;
        int y = y0;
        for (int i = 0; i <= dx; i++) {
            setPixelColor(x0 + i * sx, y, r, g, b);
            if (p >= 0) {
                y += sy;
                p -= 2 * dx;
            }
            p += 2 * dy;
        }
    }
    else {
        // Línea "vertical": Y es el eje principal, X avanza según p
        int p = 2 * dx - dy;
        int x = x0;
        for (int i = 0; i <= dy; i++) {
            setPixelColor(x, y0 + i * sy, r, g, b);
            if (p >= 0) {
                x += sx;
                p -= 2 * dy;
            }
            p += 2 * dx;
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

// interacción con el mouse 

GLuint gVBOid = 0;        // ID del VBO para poder actualizarlo después de cada clic 
bool hasStartPoint = false; // ¿ya se hizo el primer clic?
int startCol = 0, startRow = 0;

int currentTool = 1;// herramienta activa (0 = línea, 1 = círculo) y color de dibujo
float drawColor[3] = { 1.0f, 0.0f, 0.0f };

// regenera los vértices desde pixelGrid y los sube al VBO existente
void updateGridBuffer() { // pasa los colores de pixelGrid al VBO para que se vean en pantalla
    std::vector<GLfloat> v;
    float stepX = 2.0f / GRID_COLS;
    float stepY = 2.0f / GRID_ROWS;
    float padding = 0.002f;

    for (int row = 0; row < GRID_ROWS; ++row) {
        for (int col = 0; col < GRID_COLS; ++col) {
            float x = -1.0f + col * stepX + padding;
            float y = -1.0f + row * stepY + padding;
            float xNext = -1.0f + (col + 1) * stepX - padding;
            float yNext = -1.0f + (row + 1) * stepY - padding;
            float r = pixelGrid[row][col].r;
            float g = pixelGrid[row][col].g;
            float b = pixelGrid[row][col].b;

            // cuadrado 

            // Triángulo 1
            v.insert(v.end(), { x,     y,     0.0f, r, g, b });
            v.insert(v.end(), { xNext, y,     0.0f, r, g, b });
            v.insert(v.end(), { xNext, yNext, 0.0f, r, g, b });
            // Triángulo 2
            v.insert(v.end(), { x,     y,     0.0f, r, g, b });
            v.insert(v.end(), { xNext, yNext, 0.0f, r, g, b });
            v.insert(v.end(), { x,     yNext, 0.0f, r, g, b });
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, gVBOid);
    glBufferSubData(GL_ARRAY_BUFFER, 0, v.size() * sizeof(GLfloat), v.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// de clic del mouse
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action != GLFW_PRESS) return;

    // si el clic es sobre el panel de ImGui, no pintar en la grilla
    if (ImGui::GetIO().WantCaptureMouse) return;

    // Clic derecho: limpiar toda la grilla
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        for (int r = 0; r < GRID_ROWS; ++r)
            for (int c = 0; c < GRID_COLS; ++c)
                pixelGrid[r][c] = { 0.4f, 0.6f, 0.2f };
        hasStartPoint = false;
        updateGridBuffer();
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        // Posición del cursor y tamaño actual de la ventana (sirve aunque se redimensione)
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int w, h; //w ancho ventana, h alto 
        glfwGetWindowSize(window, &w, &h);

        // Convertir píxeles de pantalla -> celda de la grilla
        // (en pantalla Y crece hacia abajo; en la grilla la fila 0 está abajo)
        int col = static_cast<int>(xpos / w * GRID_COLS);
        int row = GRID_ROWS - 1 - static_cast<int>(ypos / h * GRID_ROWS);
        if (col < 0) col = 0;
        if (col >= GRID_COLS) col = GRID_COLS - 1;
        if (row < 0) row = 0;
        if (row >= GRID_ROWS) row = GRID_ROWS - 1;

        if (!hasStartPoint) {
            // Primer clic: punto inicial (línea) o centro (círculo), en blanco
            startCol = col;
            startRow = row;
            hasStartPoint = true;
            setPixelColor(col, row, 1.0f, 1.0f, 1.0f);
        }
        else {
            if (currentTool == 0) {
                // línea desde el punto inicial hasta este clic
                drawLineBresenham(startCol, startRow, col, row,
                    drawColor[0], drawColor[1], drawColor[2]);
            }
            else {
                // círculo, el radio es la distancia del centro a este clic
                int dx = col - startCol;
                int dy = row - startRow;
                int radio = static_cast<int>(std::sqrt(static_cast<double>(dx * dx + dy * dy)) + 0.5); // pitagoras
                drawCircleBresenham(startCol, startRow, radio,
                    drawColor[0], drawColor[1], drawColor[2]);
            }
            hasStartPoint = false;
        }
        updateGridBuffer();
    }
}

int main()
{
    if (!glfwInit()) { // comprobar si glfw inicio correctamente 
        std::cout << "Error al inicializar GLFW" << std::endl;
        return -1;
    }

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

    // callback para los clics del mouse
    glfwSetMouseButtonCallback(window, mouse_button_callback);


    // Carga GLAD y configura OpenGL (con chequeo de error)
    if (!gladLoadGL()) {
        std::cout << "Error al inicializar GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 800); // area de renderizado de la ventana x=0, y=0, a x=800, y=800

    // inicializar ImGui (después de los callbacks, para que los encadene)
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true); // dibuja sobre ventana
    ImGui_ImplOpenGL3_Init("#version 330");

    // inicializacion de colores base de cada pixel
    for (int r = 0; r < GRID_ROWS; ++r) {
        for (int c = 0; c < GRID_COLS; ++c) {
            pixelGrid[r][c] = { 0.5f, 0.5f, 0.5f }; // Color base
        }
    } // <-- llave que faltaba

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

    // guardar el ID del VBO para actualizarlo con cada clic
    gVBOid = VBO1.ID;

    // Configura la función de callback para el redimensionamiento de la ventana
    gShader = &shaderProgram;
    gVAO = &VAO1;
    gVertexCount = static_cast<GLsizei>(gridVertices.size() / 6);

    while (!glfwWindowShouldClose(window))
    {
        // iniciar frame de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // panel de herramientas
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Herramientas");
        if (ImGui::RadioButton("Linea", currentTool == 0)) {
            currentTool = 0;
            hasStartPoint = false;
        }
        if (ImGui::RadioButton("Circulo", currentTool == 1)) {
            currentTool = 1;
            hasStartPoint = false;
        }
        ImGui::ColorEdit3("Color", drawColor);
        if (currentTool == 0)
            ImGui::Text(hasStartPoint ? "Clic en el punto final" : "Clic en el punto inicial");
        else
            ImGui::Text(hasStartPoint ? "Clic para marcar el radio" : "Clic en el centro");
        ImGui::Text("Clic derecho: limpiar");
        ImGui::End();

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

        // dibujar ImGui encima de la grilla
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // cambia el back con el front buffer
        glfwSwapBuffers(window);
        // encargado de los eventos del GLWF
        glfwPollEvents();
    }

    // cerrar ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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