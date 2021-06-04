#ifndef CALLBACKS_HPP
#define CALLBACKS_HPP

#include "util/info.hpp"


void framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height) {
	glViewport(0, 0, width, height);
    MC::resizing = true;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		//position -= speed * front;
        Cam::instance.processKeyboard(FORWARD, MC::deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//position += glm::normalize(glm::cross(front, up)) * speed;
        Cam::instance.processKeyboard(LEFT, MC::deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		//position += speed * front;
        Cam::instance.processKeyboard(BACKWARD, MC::deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//position -= glm::normalize(glm::cross(front, up)) * speed;
        Cam::instance.processKeyboard(RIGHT, MC::deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		//position[1] -= speed;
        Cam::instance.processKeyboard(DOWN, MC::deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		//position[1] += speed;
        Cam::instance.processKeyboard(UP, MC::deltaTime);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        MC::toggle_wireframe = true;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
        MC::shifting = true;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
        MC::shifting = false;
    }

    // for hidden/enable the cursor con the screen
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        MC::pressed_cursor = true;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(Cam::firstMouse) {
        Cam::lastx = xpos;
        Cam::lasty = ypos;
        Cam::firstMouse = false;
    }
    f32 xoffset = xpos - Cam::lastx;
    f32 yoffset = ypos - Cam::lasty;

    yoffset *= Cam::mouse_sensitivity;
    xoffset *= Cam::mouse_sensitivity;

    if (yoffset > 1800) yoffset = 1800;
    Cam::instance.processMouse(xoffset, yoffset);
}
#endif
