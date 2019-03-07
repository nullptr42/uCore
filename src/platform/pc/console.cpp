
#include <stdint.h>
#include <lib/terminal/emulator.hpp>
#include <platform/print.hpp>

struct Console : public lib::terminal::Display {
    Console() : Display(80, 25) { }

    auto ToCode(lib::terminal::Char const& symbol) -> uint16_t {
        // TODO(fleroviux): convert colors to VGA colors.
        return symbol.character |
               (static_cast<int>(symbol.fg) <<  8) |
               (static_cast<int>(symbol.bg) << 12);
    }

    void Present(lib::terminal::Point const& p1, lib::terminal::Point const& p2, const lib::terminal::Char* frame) {
        for (int y = p1.y; y <= p2.y; y++) {
            auto dst = &((uint16_t*)0xFFFFFFFF800B8000)[y * width];
            auto src = &frame[y * width];
            for (int x = p1.x; x <= p2.x; x++) {
                dst[x] = ToCode(src[x]);
            }
        }
    }
};

static Console console;
static lib::terminal::Emulator emulator(console);

void platform::print(const char* string) {
    emulator.Write(string);
}
