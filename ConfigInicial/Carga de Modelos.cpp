// Practica 06
// Marquez Martinez Fatima Ahtizi
// Fecha de entrega: 26 de septiembre del 2026
// No. de Cuenta: 319159596

// Std. Includes
#include <string>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"
#include "stb_image.h"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();


// Camera
Camera camera(glm::vec3(0.0f, 4.0f, 22.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


int main()
{
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 6. Marquez Martinez Fatima Athziri", nullptr, nullptr);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Set the required callback functions
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    // GLFW Options
    //glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Define the viewport dimensions
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Setup and compile our shaders
    Shader shader("Shader/modelLoading.vs", "Shader/modelLoading.frag");

    // Load models
    Model dog((char*)"Models/RedDog.obj");
    // Agregando el modelo de la cabaña
    Model cabana((char*)"Models/woodenhouse.obj");
    // Agregando el modelo del gato
    Model gato((char*)"Models/cat.obj");
    // Agregando el modelo del Audi
    Model audi((char*)"Models/r8.obj");
    // Agregando el modelo de las bancas y sillas (ODB)
    Model odb((char*)"Models/ODB-0002 Bench And Chair Cycles.obj");

   
    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 300.0f);

    
    const int NUM_ODB_INSTANCES = 1;
    glm::vec3 odbPositions[8] = {
        glm::vec3(-12.0f, 0.0f, -8.0f),
        glm::vec3(-9.0f,  0.0f, -8.0f),
        glm::vec3(-6.0f,  0.0f, -8.0f),
        glm::vec3(-3.0f,  0.0f, -8.0f),
        glm::vec3(-12.0f, 0.0f, -11.0f),
        glm::vec3(-9.0f,  0.0f, -11.0f),
        glm::vec3(-6.0f,  0.0f, -11.0f),
        glm::vec3(-3.0f,  0.0f, -11.0f)
    };

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check and call events
        glfwPollEvents();
        DoMovement();

        // Clear the colorbuffer
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Draw the loaded model
        glm::mat4 model(1);
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        dog.Draw(shader);

        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        dog.Draw(shader);

        // Dibujando el modelo de la cabaña de fondo
        glm::mat4 modelCabana(1.0f);
        modelCabana = glm::translate(modelCabana, glm::vec3(0.0f, 0.0f, -15.0f));
        modelCabana = glm::scale(modelCabana, glm::vec3(4.0f, 4.0f, 4.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelCabana));
        cabana.Draw(shader);

        // Dibujando el modelo del gato
        
        glm::mat4 modelGato(1.0f);
        modelGato = glm::translate(modelGato, glm::vec3(-4.0f, 0.0f, 0.0f));
        modelGato = glm::scale(modelGato, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelGato));
        gato.Draw(shader);

        // Dibujando el modelo del Audi
        
        glm::mat4 modelAudi(1.0f);
        modelAudi = glm::translate(modelAudi, glm::vec3(10.0f, 0.0f, -10.0f));
        modelAudi = glm::scale(modelAudi, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelAudi));
        audi.Draw(shader);

        // Dibujando las sillas/bancas ODB
        
        for (int i = 0; i < NUM_ODB_INSTANCES; i++)
        {
            glm::mat4 modelOdb(1.0f);
            modelOdb = glm::translate(modelOdb, odbPositions[i]);
            // Escala inicial en 1 (ajusta esto si salen muy gigantes o muy pequeñas)
            modelOdb = glm::scale(modelOdb, glm::vec3(1.0f, 1.0f, 1.0f));
            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(modelOdb));
            odb.Draw(shader);
        }

        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}


// Moves/alters the camera positions based on user input
void DoMovement()
{
    // Camera controls
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }


}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }

}

// MOVIMIENTO DE LA CÁMARA
void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}