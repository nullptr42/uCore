
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
    Color bg = Color::Black;
    char character = ' ';
};

struct Point {
    int x = 0;
    int y = 0;

    Point() { }
    Point(int x, int y) : x(x), y(y) { }
};

struct Display {
    int width;
    int height;

    Display(int width, int height)
        : width(width)
        , height(height)
    { }

    virtual void Present(Point const& p1, Point const& p2, const Char* symbols) = 0;
};

class Emulator {

public:
    Emulator(Display& display)
        : display(display)
        , width(display.width)
        , height(display.height)
    {
        symbols = new Char[width * height];
        display.Present(Point{0, 0}, Point{width - 1, height - 1}, symbols);
    }

private:
    Display& display;

    Point cursor;
    int width;
    int height;

    Char* symbols;
};

}
