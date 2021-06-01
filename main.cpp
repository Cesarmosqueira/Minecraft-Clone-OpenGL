#ifdef _WIN32
    #pragma comment(lib, "glfw3.lib")
#endif
#include "util/info.hpp"       

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		//position -= speed * front;
        Cam::instance.processKeyboard(FORWARD, deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//position += glm::normalize(glm::cross(front, up)) * speed;
        Cam::instance.processKeyboard(LEFT, deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		//position += speed * front;
        Cam::instance.processKeyboard(BACKWARD, deltaTime);
	}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//position -= glm::normalize(glm::cross(front, up)) * speed;
        Cam::instance.processKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		//position[1] -= speed;
        Cam::instance.processKeyboard(DOWN, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		//position[1] += speed;
        Cam::instance.processKeyboard(UP, deltaTime);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        toggle_wireframe = true;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
        shifting = true;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
        shifting = false;
    }

    // for hidden/enable the cursor con the screen
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        pressed_cursor = true;
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

int main() {
    std::srand(time(NULL));
    Cam::instance.set_speed(movement_speed);
	GLFWwindow* window = glutilInit(3, 3, Screen::W, Screen::H, "Minecraft clone!!");

    glEnable(GL_DEPTH_TEST);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    
    World world(1); /* 0 solid; 1 dirt; 2 not a thing*/
    world.update_width_height(Screen::W, Screen::H);


    double previousTime = glfwGetTime();
    int frameCount = 0;
    float r = 32/256.0f, g = 38/256.0f, b = 51/256.0f;
    Cam::instance.get_position()[1] += WATER_LEVEL + 10;
    Cam::instance.get_position()[0] += CHUNK_SIDE * 30;
    Cam::instance.get_position()[2] += CHUNK_SIDE * 30;
    while (!glfwWindowShouldClose(window)) {

        
		if (pressed_cursor == true) {
            CURSOR_ON ^= true;
			glfwSetInputMode(window, GLFW_CURSOR, CURSOR_ON ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
			pressed_cursor = false;
		}
        // Measure speed
        double currentTime = glfwGetTime();
        frameCount++;

        // If a second has passed.
        if ( currentTime - previousTime >= 1.0 ) {
            // Display the frame count here any way you want.
            std::cout << "FPS: " << frameCount << "\n";

            frameCount = 0;
            previousTime = currentTime;
        }

        world.update_width_height(Screen::W, Screen::H);

        //polling events
        float currentFrame = glfwGetTime(); 

        if ( shifting ) {
            if ( Cam::instance.getSpeed() < 50)  Cam::instance.getSpeed() +=  3;
        }

        else {
            if ( 8 < Cam::instance.getSpeed() ) Cam::instance.getSpeed() -= 3;

            else Cam::instance.getSpeed() = 8;
       }

        deltaTime = lastFrame - currentFrame;
        lastFrame = currentFrame;

        if(toggle_wireframe) {
            world.toggle_wireframe();
            toggle_wireframe = false;
        }

        world.send_view_mat(Cam::instance.getViewM4());
		processInput(window); 

		glfwPollEvents();
        
        //rendering
		glClearColor(r,g,b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        world.on_update(Cam::instance.get_position());

        glfwGetFramebufferSize(window, &Screen::W, &Screen::H);
		glfwSwapBuffers(window);
	}

    glfwDestroyWindow(window);
    std::cout << "Window destroyed\n";
    glfwTerminate();
    std::cout << "GLFW terminate\n";

	return 0;
} 
