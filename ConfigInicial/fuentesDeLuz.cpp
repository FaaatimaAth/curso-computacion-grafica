//Practica 9
//Marquez Martinez Fatima Athziri
//Fecha de entrega: 17 de octubre de 2026
//Numero de cuenta: 319159596

#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Utilidades para medir un .obj y escalarlo a una altura deseada
struct Bounds
{
	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

	glm::vec3 Size() const { return max - min; }
	glm::vec3 Center() const { return (min + max) * 0.5f; }
	float MinY() const { return min.y; }
	float MaxY() const { return max.y; }
};

static Bounds LoadBounds(const std::string& path)
{
	Bounds b;
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "LoadBounds: no se pudo abrir " << path << std::endl;
		b.min = glm::vec3(0.0f);
		b.max = glm::vec3(1.0f);
		return b;
	}

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string tag;
		iss >> tag;
		if (tag == "v")
		{
			float x, y, z;
			if (iss >> x >> y >> z)
			{
				b.min = glm::min(b.min, glm::vec3(x, y, z));
				b.max = glm::max(b.max, glm::vec3(x, y, z));
			}
		}
	}

	if (b.min.x > b.max.x)
	{
		b.min = glm::vec3(0.0f);
		b.max = glm::vec3(1.0f);
	}
	return b;
}

static glm::mat4 FitToHeight(const Bounds& b, const glm::vec3& pos, float targetHeight)
{
	float h = b.Size().y;
	float s = (h > 0.0f) ? (targetHeight / h) : 1.0f;

	glm::mat4 m(1.0f);
	m = glm::translate(m, pos);
	m = glm::scale(m, glm::vec3(s));
	m = glm::translate(m, glm::vec3(-b.Center().x, -b.MinY(), -b.Center().z));
	return m;
}

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 3.0f, 20.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active = true;

// Indice para seleccionar cual de las 5 luces mover (0 a 4)
int selectedLight = 0;

// Direccion de la luz direccional (Sol - Luz 5)
glm::vec3 dirLightDir(-0.7f, -0.8f, -0.3f);

// Positions of the point lights (0 y 1: Puntuales, 2 y 3: Reflectores)
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),   // [0] Point light 1 (Fuego sobre el perro)
	glm::vec3(0.0f, 0.0f, 0.0f),   // [1] Point light 2 (Purpura pulsante en cabana)
	glm::vec3(0.0f, 15.0f, 0.0f),  // [2] SpotLight 1 (Reflector cian desde el cielo)
	glm::vec3(-15.0f, 2.0f, -5.0f) // [3] SpotLight 2 (Reflector rojo lateral)
};

float vertices[] = {
	 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};



glm::vec3 Light1 = glm::vec3(1.0f, 1.0f, 1.0f);


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica9. Marquez Martinez Fatima Athziri", nullptr, nullptr);

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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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



	Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
	Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");

	Model Dog((char*)"Models/RedDog.obj");
	Model Piso((char*)"Models/piso.obj");
	Model Cabana((char*)"Models/woodenhouse.obj");
	Model Ball((char*)"Models/ball.obj");
	Model Odb((char*)"Models/ODB-0002 Bench And Chair Cycles.obj");

	// Calculo de cajas envolventes y matrices de modelo
	Bounds bDog = LoadBounds("Models/RedDog.obj");
	Bounds bPiso = LoadBounds("Models/piso.obj");
	Bounds bCabin = LoadBounds("Models/woodenhouse.obj");
	Bounds bBall = LoadBounds("Models/ball.obj");
	Bounds bOdb = LoadBounds("Models/ODB-0002 Bench And Chair Cycles.obj");

	const float dogScale = 3.0f;
	float dogH = bDog.Size().y * dogScale;
	float cabH = dogH * 5.0f;
	float odbH = dogH * 0.9f;
	float ballH = dogH * 0.5f;

	glm::mat4 modelDog(1.0f);
	modelDog = glm::scale(modelDog, glm::vec3(dogScale));
	modelDog = glm::translate(modelDog, glm::vec3(0.0f, -bDog.MinY(), 0.0f));

	glm::mat4 modelPiso(1.0f);
	modelPiso = glm::translate(modelPiso, glm::vec3(0.0f, -bPiso.MaxY() - 0.01f, 0.0f));
	modelPiso = glm::scale(modelPiso, glm::vec3(10.0f, 1.0f, 10.0f));

	glm::mat4 modelCabana = FitToHeight(bCabin, glm::vec3(0.0f, 0.0f, -15.0f), cabH);
	glm::mat4 modelOdb = FitToHeight(bOdb, glm::vec3(-5.0f, 0.0f, -5.0f), odbH);
	glm::mat4 modelBall = FitToHeight(bBall, glm::vec3(3.0f, 0.0f, 2.0f), ballH);

	// Centros de los modelos y posiciones iniciales de las luces puntuales
	glm::vec3 dogCenter(0.0f, dogH * 0.5f, 0.0f);
	glm::vec3 odbCenter(-5.0f, odbH * 0.5f, -5.0f);
	float cabDepth = bCabin.Size().z * (cabH / bCabin.Size().y);

	pointLightPositions[0] = glm::vec3(0.0f, dogH + 1.0f, 0.0f);
	pointLightPositions[1] = glm::vec3(0.0f, cabH * 0.3f, -15.0f + cabDepth * 0.5f + 2.0f);



	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();

		// Clear the colorbuffer
		glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// OpenGL options
		glEnable(GL_DEPTH_TEST);



		//Load Model


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


		// Directional light (Luz 5: Sol de las 5 PM)
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), dirLightDir.x, dirLightDir.y, dirLightDir.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.15f, 0.10f, 0.15f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.8f, 0.4f, 0.2f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);


		// Point light 1 (Luz 1: Fuego encima del perro)
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.2f * Light1.x, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), 1.0f * Light1.x, 0.3f * Light1.y, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f * Light1.x, 0.5f * Light1.y, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.14f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.07f);


		// Point light 2 (Luz 2: Purpura pulsante delante de la cabana)
		float pulsacion = 0.2f + 0.8f * ((float)sin(glfwGetTime() * 2.0) * 0.5f + 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.1f * pulsacion, 0.0f, 0.2f * pulsacion);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.6f * pulsacion, 0.0f, 1.0f * pulsacion);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.8f * pulsacion, 0.0f, 1.0f * pulsacion);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.022f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.0019f);


		// SpotLight 1 (Luz 3: Reflector cian desde el cielo, apuntando al perro)
		glm::vec3 spot1Dir = glm::normalize(dogCenter - pointLightPositions[2]);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight1.position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight1.direction"), spot1Dir.x, spot1Dir.y, spot1Dir.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight1.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight1.diffuse"), 0.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight1.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight1.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight1.linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight1.quadratic"), 0.0075f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight1.cutOff"), glm::cos(glm::radians(25.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight1.outerCutOff"), glm::cos(glm::radians(35.0f)));


		// SpotLight 2 (Luz 4: Reflector rojo lateral, apuntando a la banca)
		glm::vec3 spot2Dir = glm::normalize(odbCenter - pointLightPositions[3]);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight2.position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight2.direction"), spot2Dir.x, spot2Dir.y, spot2Dir.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight2.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight2.diffuse"), 1.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight2.specular"), 1.0f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight2.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight2.linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight2.quadratic"), 0.0075f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight2.cutOff"), glm::cos(glm::radians(10.0f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight2.outerCutOff"), glm::cos(glm::radians(15.0f)));


		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glm::mat4 model(1);



		//Carga de modelo 
		view = camera.GetViewMatrix();
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

		model = modelPiso;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Piso.Draw(lightingShader);

		model = modelCabana;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Cabana.Draw(lightingShader);

		model = modelOdb;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Odb.Draw(lightingShader);

		model = modelBall;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Ball.Draw(lightingShader);


		model = modelDog;
		glEnable(GL_BLEND);//Avtiva la funcionalidad para trabajar el canal alfa
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
		Dog.Draw(lightingShader);
		glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
		glDisable(GL_BLEND);  //Desactiva el canal alfa 
		glBindVertexArray(0);


		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);



		// Swap the screen buffers
		glfwSwapBuffers(window);
	}


	// Terminate GLFW, clearing any resources allocated by GLFW.
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

	// Velocidad de movimiento de las luces
	float speed = 5.0f * deltaTime;

	// Mover luces con posicion fisica (0: Puntual 1, 1: Puntual 2, 2: Reflector 1, 3: Reflector 2)
	if (selectedLight >= 0 && selectedLight <= 3)
	{
		if (keys[GLFW_KEY_T])
		{
			pointLightPositions[selectedLight].x += speed;
		}
		if (keys[GLFW_KEY_G])
		{
			pointLightPositions[selectedLight].x -= speed;
		}

		if (keys[GLFW_KEY_Y])
		{
			pointLightPositions[selectedLight].y += speed;
		}

		if (keys[GLFW_KEY_H])
		{
			pointLightPositions[selectedLight].y -= speed;
		}
		if (keys[GLFW_KEY_U])
		{
			pointLightPositions[selectedLight].z -= speed;
		}
		if (keys[GLFW_KEY_J])
		{
			pointLightPositions[selectedLight].z += speed;
		}
	}
	// Mover la direccion de la luz direccional (4: Sol)
	else if (selectedLight == 4)
	{
		if (keys[GLFW_KEY_T])
		{
			dirLightDir.x += speed * 0.2f;
		}
		if (keys[GLFW_KEY_G])
		{
			dirLightDir.x -= speed * 0.2f;
		}

		if (keys[GLFW_KEY_Y])
		{
			dirLightDir.y += speed * 0.2f;
		}

		if (keys[GLFW_KEY_H])
		{
			dirLightDir.y -= speed * 0.2f;
		}
		if (keys[GLFW_KEY_U])
		{
			dirLightDir.z -= speed * 0.2f;
		}
		if (keys[GLFW_KEY_J])
		{
			dirLightDir.z += speed * 0.2f;
		}
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

	// Seleccionar que luz mover con las teclas numericas 1 al 5
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_1) selectedLight = 0; // Luz puntual 1 (Fuego sobre el perro)
		if (key == GLFW_KEY_2) selectedLight = 1; // Luz puntual 2 (Purpura en cabana)
		if (key == GLFW_KEY_3) selectedLight = 2; // Reflector 1 (Cian desde el cielo)
		if (key == GLFW_KEY_4) selectedLight = 3; // Reflector 2 (Rojo hacia la banca)
		if (key == GLFW_KEY_5) selectedLight = 4; // Luz direccional (Sol)
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
		{
			Light1 = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		else
		{
			Light1 = glm::vec3(0);//Cuado es solo un valor en los 3 vectores pueden dejar solo una componente
		}
	}
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}