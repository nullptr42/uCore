
#include <stdint.h>
#include <lib/terminal/terminal.hpp>

struct Console : public lib::terminal::Display {
    Console() : Display(80, 25) { }

    auto ToCode(lib::terminal::Char const& symbol) -> uint16_t {
        // TODO(fleroviux): convert colors to VGA colors.
        return symbol.character |
               (static_cast<int>(symbol.fg) <<  8) |
               (static_cast<int>(symbol.bg) << 12);
    }

    void Fill(int x1, int y1, int x2, int y2, lib::terminal::Char symbol) {

    }

    void Present(int x1, int y1, int x2, int y2, const lib::terminal::Char* symbols) {
        for (int y = y1; y <= y2; y++) {
            auto dst = (uint16_t*)0xFFFFFFFF800B8000 + y * width;
            auto src = symbols + y * width;

            for (int x = x1; x <= x2; x++) {
                dst[x] = ToCode(src[x]);
            }
        }
    }
};

lib::terminal::Emulator emulator(new Console());