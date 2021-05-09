#include "pncraft.hpp"
#include "camera.hpp"
       
int SCR_WIDTH = 960;
int SCR_HEIGHT = 540;
bool lbutton_down = false;
f32 sensitivity = 0.5f;
f32 speed = 8.0f;

bool firstMouse = true;
float yaw = -90.0f; 
float pitch = 0.0f; 
float lastx = SCR_WIDTH / 2.0f;
float lasty = SCR_HEIGHT / 2.0f;
bool toggle_wireframe = false;
f32 deltaTime = 0.0f;
f32 lastFrame = 0.0f;

//Pos: [46.9726,174.369,-12.9438]
//LookAt: [-0.700691,-0.632705,0.32972]
//Up: [-0.572489,0.774393,0.269393]
Cam camera;

void showvec3(const glm::vec3& v){
    std::cout << "[" << v[0] << "," << v[1] << "," << v[2] << "]";
}
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		//position -= speed * front;
        camera.processKeyboard(FORWARD, deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//position += glm::normalize(glm::cross(front, up)) * speed;
        camera.processKeyboard(LEFT, deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		//position += speed * front;
        camera.processKeyboard(BACKWARD, deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//position -= glm::normalize(glm::cross(front, up)) * speed;
        camera.processKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		//position[1] -= speed;
        camera.processKeyboard(DOWN, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		//position[1] += speed;
        camera.processKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		//position[1] += speed;
        C::Info inf = camera.get_info();
        std::cout << "Pos: "; showvec3(inf.pos); std::cout << "\n";  
        std::cout << "LookAt: "; showvec3(inf.lookat); std::cout << "\n";  
        std::cout << "Up: "; showvec3(inf.up); std::cout << "\n";  
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
    yoffset *= sensitivity;
    xoffset *= sensitivity;
    camera.processMouse(xoffset, yoffset);
}
int main() {
    camera.set_speed(speed);
	GLFWwindow* window = glutilInit(3, 3, SCR_WIDTH, SCR_HEIGHT, "RAAAAAAAAAAAAAAA");
    glEnable(GL_DEPTH_TEST);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    World world(1);
    while (!glfwWindowShouldClose(window)) {
        //polling events
        float currentFrame = glfwGetTime(); 
        deltaTime = lastFrame - currentFrame;
        lastFrame = currentFrame;
        if(toggle_wireframe){
            world.toggle_wireframe();
            toggle_wireframe = false;
        }
		world.s()->setMat4("view",camera.getViewM4());
		processInput(window); 
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetKeyCallback(window, key_callback);
		glfwPollEvents();
        
        //rendering
		glClearColor(0.1, 0.2, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        world.on_update();

        glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
        world.update_width_height(SCR_WIDTH, SCR_HEIGHT);
		glfwSwapBuffers(window);
	}

    glfwDestroyWindow(window);

    glfwTerminate();

	return 0;
}
