#ifndef SANDBOX_H
#define SANDBOX_H

/*imgui*/
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include "util/callbacks.hpp"

#include "resources/Sun.hpp"
#include "pncraft.hpp"

class SandBox {
public:
    SandBox() {
        srand(time(NULL));
        window = glutilInit(3, 3, Screen::W, Screen::H, "Minecraft clone!!");
        if(!window){
            std::cerr << "Couldn't create window\n";
            glfwTerminate();
        }

        std::cout << "<----------------------------------------------------------------->\n";
        std::cout << " OPENGL VERSION: " << glGetString(GL_VERSION) << "\n";
        std::cout << " GLFW VERSION: " << glfwGetVersionString() << "\n";
        std::cout << "<----------------------------------------------------------------->\n";
        glEnable(GL_DEPTH_TEST);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        world = new World();
        Cam::instance.set_speed(Cam::movement_speed);
        Cam::instance.get_position()[1] = WATER_LEVEL + 20;
        this->set_callbacks();
        glm::vec3 sunpos = Cam::instance.get_position();
        sunpos[1] = CHUNK_HEIGHT;
        sun = new Sun(sunpos);

        clear_color = {0.45f, 0.55f, 0.60f, 1.00f};

        init_imgui();
    }
    ~SandBox(){
        delete sun;
        delete world;
        std::cout << "﫟 World deleted succesfully.\n";

        // Cleanup

        glfwDestroyWindow(window);
        std::cout << "Window destroyed\n";
        glfwTerminate();
        std::cout << "GLFW terminate\n";

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        std::cout << "Imgui context destroyed\n";
    }
    void on_update(bool gui=true){
        poll_events();

        world->send_view_mat(Cam::instance.getViewM4());
		processInput(window); 

		glfwPollEvents();
        
        //rendering
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(MC::GUI_ON) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }
        world->on_update(Cam::instance.get_position(), Cam::instance.getViewM4());

        glfwGetFramebufferSize(window, &Screen::W, &Screen::H);

        if(MC::GUI_ON) {
            on_GUI_update();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
		glfwSwapBuffers(window);
    }

private:
    void on_GUI_update(){
        ImGui::Begin("Main tweaks");
        
        ImGui::SliderInt("Chunking: ", &world->get_chunking(), 3, 15);

        ImGui::SliderFloat("Specular", (float*)&world->u_starting_ambient, 0.0, 100.0f);
        ImGui::SliderFloat("Ambient", (float*)&world->u_starting_specular, 0.0, 100.0f);
        ImGui::SliderFloat("Min Ambient", (float*)&world->u_minimum_ambient, 1.0f, 5.0f);

        ImGui::SliderFloat("MonitoScale", (float*)&world->monito_scale(), 0.0f, 5.0f);
        ImGui::SliderFloat("Monito Ambient", (float*)&world->monito_ambient(), 0.0f, 400.0f);
        ImGui::SliderFloat("Monito Specular", (float*)&world->monito_specular(), 0.0f, 400.0f);
        ImGui::SliderFloat("Monito Min Ambient", (float*)&world->monito_min_ambient(), 0.0f, 400.0f);
        ImGui::SliderFloat3("Monito Pos", (float*)&world->get_monito_pos().x, CHUNK_HEIGHT*-2, CHUNK_HEIGHT*2);


        ImGui::SliderFloat3("Sun Pos", (float*)&world->get_lsrc_pos().x, CHUNK_HEIGHT*-2, CHUNK_HEIGHT*2);
        ImGui::SliderFloat("Sun Size", (float*)&world->sun_scale(), 0.0f, 100.0f);
        ImGui::SliderFloat("Sun Wide", (float*)&world->SunWide, -200.0f, 200.0f);
        ImGui::SliderFloat("Sun speed", (float*)&world->SunSpeed, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color.r); // Edit 3 floats representing a color

        ImGui::Text("Current bg: %lf, %lf, %lf, %lf", clear_color.r, clear_color.g, clear_color.b, clear_color.w); 
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

    }
    void init_imgui() {
        //Setup IMGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
    }
    
    void set_callbacks() {
        //callbacks defined in util/callbacks.hpp
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetKeyCallback(window, key_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
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
        MC::deltaTime = MC::lastFrame- glfwGetTime();
        MC::lastFrame = glfwGetTime();

        if(MC::toggle_wireframe) {
           world->toggle_wireframe();
           MC::toggle_wireframe = false;
        }

        if(MC::resizing){

            world->update_width_height(Screen::W, Screen::H);
            MC::resizing = false;
        }
    }

private:
    World* world;
    Sun* sun;
    GLFWwindow* window;
    glm::vec4 clear_color;

public:
    bool open(){
        return !glfwWindowShouldClose(window);
    }
};

#endif
