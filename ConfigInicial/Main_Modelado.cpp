//Practica 04
//Marquez Martinez Fatima Athziri 
//Fecha de entrega: 12 de septiembre de 2026
//No. de cuenta: 319159596

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shaders
#include "Shader.h"

void Inputs(GLFWwindow* window);

const GLint WIDTH = 800, HEIGHT = 600;
float movX = 0.0f;
float movY = 0.0f;
float movZ = -3.0f; 
float rot = 3.0f;  
float rotX = 15.0f; 

int main() {
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica04.Marquez Martinez Fatima Athiziri ", nullptr, nullptr);
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    if (nullptr == window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;

    if (GLEW_OK != glewInit()) {
        std::cout << "Failed to initialise GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader ourShader("Shader/core.vs", "Shader/core.frag");

    // Arreglo de vertices de 1x1x1
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Como quitamos el color del arreglo,stride es de 3 posiciones
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)screenWidth / (GLfloat)screenHeight, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window))
    {
        Inputs(window);
        glfwPollEvents();

        // Color de fondo 
        glClearColor(0.45f, 0.75f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.Use();

        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(movX, movY, movZ));
        // Inclinación Arriba/Abajo (Eje X) con la tecla Arriba/Abajo
        view = glm::rotate(view, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, glm::radians(rot), glm::vec3(0.0f, 1.0f, 0.0f));


        GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
        GLint projecLoc = glGetUniformLocation(ourShader.Program, "projection");
        GLint colorLoc = glGetUniformLocation(ourShader.Program, "blockColor");


        glUniformMatrix4fv(projecLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


        glBindVertexArray(VAO);

        glm::mat4 model;

        // CONSTRUCCIÓN DEL GATITO 

        // Cuerpo Principal 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.63f, 0.63f, 0.67f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Oreja Izquierda 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.35f, 0.55f, 0.35f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.47f, 0.47f, 0.51f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Oreja Derecha 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.35f, 0.55f, 0.35f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.47f, 0.47f, 0.51f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        //  Rayas de la espalda
        // Raya 1 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.51f, -0.3f));
        model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.15f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Raya 2 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.51f, 0.0f));
        model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.15f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.51f, 0.3f));
        model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.15f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Hocico 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.525f));
        model = glm::scale(model, glm::vec3(0.4f, 0.2f, 0.08f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Nariz 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.12f, 0.56f));
        model = glm::scale(model, glm::vec3(0.12f, 0.08f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 1.0f, 0.68f, 0.77f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Ojo Izquierdo
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.25f, 0.05f, 0.51f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 1.0f, 0.8f, 0.0f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Ojo Derecho
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.25f, 0.05f, 0.51f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Pupila Izquierda 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.22f, 0.08f, 0.52f));
        model = glm::scale(model, glm::vec3(0.14f, 0.14f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.1f, 0.1f, 0.1f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Pupila Derecha 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.28f, 0.08f, 0.52f));
        model = glm::scale(model, glm::vec3(0.14f, 0.14f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

        // Brillo Izquierdo 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.18f, 0.12f, 0.53f));
        model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Brillo Derecho 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.32f, 0.12f, 0.53f));
        model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // Patas
        float legScale = 0.2f;
        glUniform3f(colorLoc, 0.63f, 0.63f, 0.67f);

        // Pata Frontal Izquierda
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.4f, -0.55f, 0.4f));
        model = glm::scale(model, glm::vec3(legScale, 0.4f, legScale));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Pata Frontal Derecha
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.4f, -0.55f, 0.4f));
        model = glm::scale(model, glm::vec3(legScale, 0.4f, legScale));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Pata Trasera Izquierda
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.4f, -0.55f, -0.4f));
        model = glm::scale(model, glm::vec3(legScale, 0.4f, legScale));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Pata Trasera Derecha
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.4f, -0.55f, -0.4f));
        model = glm::scale(model, glm::vec3(legScale, 0.4f, legScale));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Base de las Patas 
        glUniform3f(colorLoc, 0.50f, 0.50f, 0.55f);
        model = glm::mat4(1.0f); model = glm::translate(model, glm::vec3(-0.4f, -0.725f, 0.4f)); model = glm::scale(model, glm::vec3(legScale, 0.05f, legScale)); glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f); model = glm::translate(model, glm::vec3(0.4f, -0.725f, 0.4f)); model = glm::scale(model, glm::vec3(legScale, 0.05f, legScale)); glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f); model = glm::translate(model, glm::vec3(-0.4f, -0.725f, -0.4f)); model = glm::scale(model, glm::vec3(legScale, 0.05f, legScale)); glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); glDrawArrays(GL_TRIANGLES, 0, 36);
        model = glm::mat4(1.0f); model = glm::translate(model, glm::vec3(0.4f, -0.725f, -0.4f)); model = glm::scale(model, glm::vec3(legScale, 0.05f, legScale)); glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); glDrawArrays(GL_TRIANGLES, 0, 36);

        // Lomito 
        glUniform3f(colorLoc, 0.72f, 0.75f, 0.80f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.425f, 0.501f));
        model = glm::scale(model, glm::vec3(0.6f, 0.15f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Bigotes 
        glUniform3f(colorLoc, 0.47f, 0.47f, 0.51f);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.55f, -0.15f, 0.2f));
        model = glm::scale(model, glm::vec3(0.1f, 0.05f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.55f, -0.3f, 0.2f));
        model = glm::scale(model, glm::vec3(0.1f, 0.05f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.55f, -0.15f, 0.2f));
        model = glm::scale(model, glm::vec3(0.1f, 0.05f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.55f, -0.3f, 0.2f));
        model = glm::scale(model, glm::vec3(0.1f, 0.05f, 0.05f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Cola 
        glUniform3f(colorLoc, 0.47f, 0.47f, 0.51f);

        // Base de la cola 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.35f, -0.575f));
        model = glm::scale(model, glm::vec3(0.15f, 0.5f, 0.15f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Punta de la cola 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.525f, -0.75f));
        model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.5f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        

        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return EXIT_SUCCESS;
}

void Inputs(GLFWwindow* window) {
    // Cerrar ventana
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // Movimiento en X 
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movX += 0.08f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movX -= 0.08f;
    // Movimiento en Y 
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        movY += 0.08f;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        movY -= 0.08f;
    // Movimiento en Z 
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movZ -= 0.08f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movZ += 0.08f;
    // Rotación en Y 
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rot += 0.8f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rot -= 0.8f;
    // Rotación en X 
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rotX += 0.8f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rotX -= 0.8f;
}