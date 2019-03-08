
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
                /* TODO: tab width must be configurable. */
                cursor_next = {
                    cursor.x + 4 - (cursor.x % 4),
                    cursor.y
                };
                stop = true;
                break;
            }
            default: {
                if (cursor.y == height) {
                    /* TODO: This is very slow and needs optimization. */
                    auto src = &frame[width];
                    auto dst = &frame[0];
                    for (int y = 1; y < height; y++) {
                        for (int x = 0; x < width; x++) {
                            dst[x] = src[x];
                        }
                        src += width;
                        dst += width;
                    }
                    for (int x = 0; x < width; x++) {
                        dst[x].character = ' ';
                    }
                    display.Present({0, 0}, {width-1, height-1}, frame);
                    cursor.y = height - 1;
                    line -= width;
                }

                if (cursor.x >= width) {
                    if (linewrap) {
                        string--;
                        cursor_next = { 0, cursor.y + 1 };
                        stop = true;
                    }
                } else {
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

        /* TODO: Would it suffice to update the cursor outside the loop? */
        display.SetCursor(cursor);
    }

    return string;
}