
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
            string = StateEscape(string);
            break;
        case State::ControlSequence:
            string = StateControlSequence(string);
            break;
        }
    }

    display.SetCursor(cursor);
}

auto Emulator::StateInitial(const char* string) -> const char* {
    auto cursor_next = Point();

    while (*string && state == State::Initial) {
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
            case '\e': {
                /* TODO: Setting cursor_next = cursor is a bit ugly.
                 * We should not need to pipeline the cursor for all stopping characters.
                 */
                cursor_next = cursor;
                state = State::Escape;
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

                if (cursor.x < width) {
                    auto& chr = frame[line + cursor.x++];

                    chr.character = character;
                    chr.brightness = brightness;
                    chr.bg = background;
                    chr.fg = foreground;
                } else if (linewrap) {
                    string--;
                    cursor_next = { 0, cursor.y + 1 };
                    stop = true;
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

auto Emulator::StateEscape(const char* string) -> const char* {
    char character = *string++;

    switch (character) {
    case '[': {
        /* CSI - Control Sequence Introducer */
        csi.count = 0;
        for (int i = 0; i < kMaxParams; i++) {
            csi.params[i] = 0;
        }
        state = State::ControlSequence;
        break;
    }
    case 'c': {
        /* RIS - Reset to Initial */
        state = State::Initial;
        // TODO
        break;
    }
    default: {
        /* Unknown escape sequence, ignore. */
        state = State::Initial;
        break;
    }
    }

    return string;
}

auto Emulator::StateControlSequence(const char* string) -> const char* {
    while (*string) {
        char character = *string++;

        switch (character) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            if (csi.count < kMaxParams) {
                csi.params[csi.count] *= 10;
                csi.params[csi.count] += character - '0';
            }
            break;
        }

        case ';': {
            /* Read next CSI parameter or code. */
            csi.count++;
            break;
        }

        case 'H':
        case 'f': {
            /* Set cursor position. */
            cursor.x = csi.params[1];
            cursor.y = csi.params[0];
            display.SetCursor(cursor);
            state = State::Initial;
            break;
        }

        case 'A': {
            /* Move cursor up. */
            cursor.y -= csi.params[0];
            if (cursor.y < 0) {
                cursor.y = 0;
            }
            display.SetCursor(cursor);
            state = State::Initial;
            break;
        }

        case 'B': {
            /* Move cursor down. */
            cursor.y += csi.params[0];
            if (cursor.y >= height) {
                cursor.y = height - 1;
            }
            display.SetCursor(cursor);
            state = State::Initial;
            break;
        }

        case 'C': {
            /* Move cursor right. */
            cursor.x += csi.params[0];
            if (cursor.x >= width) {
                cursor.x = width - 1;
            }
            display.SetCursor(cursor);
            state = State::Initial;
            break;
        }

        case 'D': {
            /* Move cursor left. */
            cursor.x -= csi.params[0];
            if (cursor.x < 0 ) {
                cursor.x = 0;
            }
            display.SetCursor(cursor);
            state = State::Initial;
            break;
        }

        case 's': {
            /* Save cursor position. */
            cursor_saved = cursor; 
            state = State::Initial;
            break;
        }
        case 'u': {
            /* Restore cursor position. */
            cursor = cursor_saved;
            display.SetCursor(cursor);
            state = State::Initial;
            break;
        }

        case 'm': {
            /* Set display attributes such as color and brightness. */
            for (int i = 0; i <= csi.count; i++) {
                SetDisplayAttribute(csi.params[i]);
            }
            state = State::Initial;
            return string;
        }

        default: {
            /* Unexpected character, stop parsing. */
            state = State::Initial;
            return string - 1;
        }
        }
    }

    return string;
}

void Emulator::SetDisplayAttribute(int code) {
    switch (code) {
    case 0: {
        /* All attributes off */
        brightness = 0;
        background = Color::Black;
        foreground = Color::White;
        break;
    }

    case 1: brightness = 0; break;
    case 2: brightness = 1; break;

    case 30: case 31:
    case 32: case 33:
    case 34: case 35:
    case 36: case 37: {
        /* Foreground color */
        foreground = static_cast<Color>(code - 30);
        break;
    }

    case 40: case 41:
    case 42: case 43:
    case 44: case 45:
    case 46: case 47: {
        /* Background color */
        background = static_cast<Color>(code - 30);
        break;
    }
    }
}
