#ifndef COLORS_H
#define COLORS_H
namespace MC {
    struct Color {
        unsigned char RED;
        unsigned char GREEN;
        unsigned char BLUE;
        float R() { return RED/256.0f; }
        float G() { return GREEN/256.0f; }
        float B() { return BLUE/256.0f; }
    };
}
#endif
