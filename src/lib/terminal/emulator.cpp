
#include <lib/terminal/emulator.hpp>

using namespace lib::terminal;

Emulator::Emulator(Display& display)
    : display(display)
    , width(display.width)
    , height(display.height)
{
    frame = new Char[width * height];
    display.Present(Point{0, 0}, Point{width - 1, height - 1}, frame);
}


