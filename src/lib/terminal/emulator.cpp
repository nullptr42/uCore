
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

void Emulator::Write(const char* string) {
    while (*string) {
        switch (state) {
        case State::Initial:
            string = StateInitial(string);
            break;
        case State::Escape:
            break;
        case State::ControlSequence:
            break;
        }
    }

    display.SetCursor(cursor);
}

auto Emulator::StateInitial(const char* string) -> const char* {
    auto cursor_next = Point();

    while (*string) {
        auto line = cursor.y * width;
        auto stop = false;
        auto cursor_x = cursor.x;

        while (!stop && *string) {
            char character = *string++;

            switch (character) {
            case '\n': {
                cursor_next = { 0, cursor.y + 1 };
                stop = true;
                break;
            }
            case '\r': {
                cursor_next = { 0, cursor.y };
                stop = true;
                break;
            }
            case '\t': {
                // TODO: Tab width should be configurable.
                cursor_next = {
                    cursor.x + 4 - (cursor.x % 4),
                    cursor.y
                };
                stop = true;
                break;
            }
            default: {
                // TODO: Must implement linewrap support here.
                if (cursor.x < width) {
                    auto& chr = frame[line + cursor.x++];

                    chr.character = character;
                    chr.bg = background;
                    chr.fg = foreground;
                }
                break;
            }
            }
        }

        display.Present({ cursor_x, cursor.y }, cursor, frame);
        if (stop) {
            cursor = cursor_next;
        }

        // TODO: Evaluate if it would suffice to update the cursor outside of the loop.
        display.SetCursor(cursor);
    }

    return string;
}