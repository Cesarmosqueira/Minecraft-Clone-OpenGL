#ifdef _WIN32
    #pragma comment(lib, "glfw3.lib")
#endif
#include "sandbox.hpp"
void m() {
    SandBox sb; 
    while (sb.open()) {
        sb.on_update();
        sb.on_gui_update();
	}
	return;
} 
int main(){
    m();
    std::cout << "Exiting\n";
}
