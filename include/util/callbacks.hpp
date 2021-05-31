#ifndef CALLBACKS_HPP
#define CALLBACKS_HPP
#include "sandbox.hpp"
Sandbox world;
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		//position -= speed * front;
        E::camera.processKeyboard(FORWARD, world.deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//position += glm::normalize(glm::cross(front, up)) * speed;
        E::camera.processKeyboard(LEFT, world.deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		//position += speed * front;
        E::camera.processKeyboard(BACKWARD, world.deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//position -= glm::normalize(glm::cross(front, up)) * speed;
        E::camera.processKeyboard(RIGHT, world.deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		//position[1] -= speed;
        E::camera.processKeyboard(DOWN, world.deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		//position[1] += speed;
        E::camera.processKeyboard(UP, world.deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
		//position[1] += speed;
        C::Info inf = E::camera.get_info();
        std::cout << "Pos: "; Sandbox::showvec3(inf.pos); std::cout << "\n";  
        std::cout << "LookAt: "; Sandbox::showvec3(inf.lookat); std::cout << "\n";  
        std::cout << "Up: "; Sandbox::showvec3(inf.up); std::cout << "\n";  
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        world.toggle_wireframe = true;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
        world.shifting = true;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
        world.shifting = false;
    }

    // for hidden/enable the cursor con the screen
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        world.pressed_cursor = true;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(world.firstMouse) {
        world.lastx = xpos;
        world.lasty = ypos;
        world.firstMouse = false;
    }
    f32 xoffset = xpos - world.lastx;
    f32 yoffset = ypos - world.lasty;

    yoffset *= world.sensitivity;
    xoffset *= world.sensitivity;

    if (yoffset > 1800) yoffset = 1800;
    E::camera.processMouse(xoffset, yoffset);
}
#endif