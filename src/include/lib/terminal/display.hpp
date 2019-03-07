
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

    virtual void Present(Point const& p1, Point const& p2, const Char* frame) = 0;
};

}