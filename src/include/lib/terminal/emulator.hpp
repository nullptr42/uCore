
#pragma once

#include "display.hpp"

namespace lib::terminal {

class Emulator {

public:
    Emulator(Display& display);

    void Write(const char* string);

private:
    Display& display;

    Point cursor;
    int width;
    int height;

    Char* frame;

    bool linewrap = true;

    Color foreground = Color::White;
    Color background = Color::Black;

    static const int kMaxParams = 10;
    struct {
        int params[kMaxParams];
        int count = 0;
    } csi;

    enum class State {
        Initial,
        Escape,
        ControlSequence
    };

    State state = State::Initial;

    auto StateInitial(const char* string) -> const char*;
};

}
