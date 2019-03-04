
#pragma once

namespace lib::terminal {

enum class Color : int {
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
    Color fg;
    Color bg;
    char character;
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

}
