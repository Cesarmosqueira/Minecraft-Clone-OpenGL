
#include "glutil.h"
#include "pncraft.hpp"

       
int SCR_WIDTH = 960;
int SCR_HEIGHT = 540;
bool lbutton_down = false;


glm::vec3 position = glm::vec3(0.0f, 0.0f, 4.0f);
glm::vec3 front    = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f; 
float pitch = 0.0f; 
float lastx = SCR_WIDTH / 2.0f;
float lasty = SCR_HEIGHT / 2.0f;
bool toggle_wireframe = false;
f32 deltaTime = 0.0f;
f32 lastFrame = 0.0f;

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	f32 speed = 4 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position -= speed * front;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position += glm::normalize(glm::cross(front, up)) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position += speed * front;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position -= glm::normalize(glm::cross(front, up)) * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		position[1] -= speed;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		position[1] += speed;
	}
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        toggle_wireframe = true;
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse) {
        lastx = xpos;
        lasty = ypos;
        firstMouse = false;
    }
    f32 xoffset = xpos - lastx;
    f32 yoffset = ypos - lasty;

    lastx = xpos;
    lasty = ypos;
    f32 sensitivity = 0.03f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch -= yoffset; 
    
    if (pitch > 89.0f) pitch = 89.0f;
    else if (pitch < -89.0f) pitch = -89.0f;
    
    glm::vec3 f; 
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(f);
}

int main() {
	GLFWwindow* window = glutilInit(3, 3, SCR_WIDTH, SCR_HEIGHT, "RAAAAAAAAAAAAAAA");
    glEnable(GL_DEPTH_TEST);
    BlockMesh mesh(1);
    
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

    while (!glfwWindowShouldClose(window)) {
        
        //polling events
        float currentFrame = glfwGetTime(); 
        deltaTime = lastFrame - currentFrame;
        lastFrame = currentFrame;
        if(toggle_wireframe){
            mesh.toggle_wireframe();
            toggle_wireframe = false;
        }

        glm::mat4 view       = glm::mat4(1.0f);
		view  = glm::lookAt(position, position + front, up);


		mesh.s()->setMat4("view", view);

		processInput(window); 
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetKeyCallback(window, key_callback);
		glfwPollEvents();
        

        //rendering
		glClearColor(0.1, 0.2, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mesh.on_update();

        glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
        mesh.update_width_height(SCR_WIDTH, SCR_HEIGHT);
		glfwSwapBuffers(window);
	}


    glfwDestroyWindow(window);
    glfwTerminate();

	return 0;
}
