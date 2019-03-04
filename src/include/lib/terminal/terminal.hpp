
#pragma once

#include <stdint.h>

namespace lib::terminal {

enum class Color : uint8_t {
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White
};

struct Char {
    Color fg = Color::White;
    Color bg = Color::Red;
    char character = ' ';
};

struct Display {
    int width;
    int height;

    Display(int width, int height)
        : width(width)
        , height(height)
    { }

    virtual void Fill(int x1, int y1, int x2, int y2, Char symbol) = 0;
    virtual void Present(int x1, int y1, int x2, int y2, const Char* symbols) = 0;
};

struct Emulator {
    Display* display;

    int x = 0;
    int y = 0;
    int width;
    int height;
    Char* symbols;

    Emulator(Display* display) {
        width   = display->width;
        height  = display->height;
        symbols = new Char[width * height];

        display->Present(0, 0, 78, 23, symbols);
    }
};

}
