
#pragma once

#include "display.hpp"

namespace lib::terminal {

class Emulator {

public:
    Emulator(Display& display);

private:
    Display& display;

    Point cursor;
    int width;
    int height;

    Char* frame;
};

}
