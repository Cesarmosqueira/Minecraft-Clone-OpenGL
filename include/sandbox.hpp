#ifndef SANDBOX_H
#define SANDBOX_H

#include "util/callbacks.hpp"
#include "util/colors.hpp"

#include "resources/Sun.hpp"
#include "pncraft.hpp"

class SandBox {
    void set_callbacks() {
        //callbacks defined in util/callbacks.hpp
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetKeyCallback(window, key_callback);
    }
    void int_player_pos() {
        Cam::instance.get_position()[1] += WATER_LEVEL + 10;
        Cam::instance.get_position()[0] += CHUNK_SIDE * 30;
        Cam::instance.get_position()[2] += CHUNK_SIDE * 30;
    }
    void FPS_handler(){
        // Measure speed
        CT = glfwGetTime();
        FC++;
        // If a second has passed.
        if ( CT - PT >= 1.0 ) {
            // Display the frame count here any way you want.
            FPS = FC;
            FC = 0;
            PT = CT;
        }
    }
    void poll_events(){
		if (MC::pressed_cursor == true) {
            MC::CURSOR_ON ^= true;
			glfwSetInputMode(window, GLFW_CURSOR, MC::CURSOR_ON ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            MC::pressed_cursor = false;
		}

        if ( MC::shifting ) {
            if ( Cam::instance.getSpeed() < 50)  
                Cam::instance.getSpeed() +=  3;
        }
        else {
            if ( 8 < Cam::instance.getSpeed() )
                Cam::instance.getSpeed() -= 3;
            else Cam::instance.getSpeed() = 8;
        }
        //float CF = glfwGetTime();
        MC::deltaTime = MC::lastFrame- CT;
        MC::lastFrame = CT;

        if(MC::toggle_wireframe) {
           world->toggle_wireframe();
           MC::toggle_wireframe = false;
        }

        if(MC::resizing){

            world->update_width_height(Screen::W, Screen::H);
            MC::resizing = false;
        }
    }
public:
    SandBox() {
        window = glutilInit(3, 3, Screen::W, Screen::H, "Minecraft clone!!");
        glEnable(GL_DEPTH_TEST);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        bg = {32,38,51};
        world = new World();
        Cam::instance.set_speed(Cam::movement_speed);
        Cam::instance.get_position()[1] = WATER_LEVEL + 20;
        this->set_callbacks();
        glm::vec3 sunpos = Cam::instance.get_position();
        sunpos[1] = CHUNK_HEIGHT;
        sun = new Sun(sunpos);
    }
    ~SandBox(){
        delete sun;
        delete world;
        std::cout << "﫟 World deleted succesfully.\n";
        glfwDestroyWindow(window);
        std::cout << "Window destroyed\n";
        glfwTerminate();
        std::cout << "GLFW terminate\n";
    }
    void on_update(){
        FPS_handler();
        poll_events();

        world->send_view_mat(Cam::instance.getViewM4());
		processInput(window); 

		glfwPollEvents();
        
        //rendering
		glClearColor(bg.R(),bg.G(),bg.B(), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        world->on_update(Cam::instance.get_position(), sun);

        glfwGetFramebufferSize(window, &Screen::W, &Screen::H);
		glfwSwapBuffers(window);
    }
    void on_gui_update(){
        std::cout << " " << FPS << "\n";
    }

private:
    World* world;
    Sun* sun;
    GLFWwindow* window;
    MC::Color bg;

    //FPS handling
    double PT = glfwGetTime();
    double CT;
    int FC = 0;
    int FPS;
public:
    bool open(){
        return !glfwWindowShouldClose(window);
    }
};

#endif
