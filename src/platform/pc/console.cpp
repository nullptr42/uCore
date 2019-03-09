
#include <stdint.h>
#include <arch/x86_64/io-port.hpp>
#include <platform/print.hpp>
#include <terminal/emulator.hpp>

struct Console : public lib::terminal::Display {

    static const int kVgaPortRegister = 0x3D4;
    static const int kVgaPortData = 0x3D5;

    static const int kVgaCursorReg = 14;

    static const uint8_t kColorMap[16];

    Console() : Display(80, 25) { }

    auto ToCode(lib::terminal::Char const& symbol) -> uint16_t {
        int offset = symbol.brightness * 8;

        return symbol.character |
               (kColorMap[int(symbol.fg) + offset] <<  8) |
               (kColorMap[int(symbol.bg)] << 12);
    }

    void SetCursor(lib::terminal::Point const& p) final override {
        if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height) {
            // TODO: Hide/show cursor.
            return;
        }

        uint16_t address = p.y * width + p.x;

        /* high byte */
        outb(kVgaPortRegister, kVgaCursorReg+0);
        outb(kVgaPortData, address >> 8);
            
        /* low byte */
        outb(kVgaPortRegister, kVgaCursorReg+1);
        outb(kVgaPortData, address & 0xFF);
    }

    void Present(lib::terminal::Point const& p1, 
                 lib::terminal::Point const& p2,
                 const lib::terminal::Char* frame) final override
    {
        for (int y = p1.y; y <= p2.y; y++) {
            auto dst = &((uint16_t*)0xFFFFFFFF800B8000)[y * width];
            auto src = &frame[y * width];
            for (int x = p1.x; x <= p2.x; x++) {
                dst[x] = ToCode(src[x]);
            }
        }
    }
};

const uint8_t Console::kColorMap[] = {
    0,  4,  2,  6, 1,  5,  3,  7,
    8, 12, 10, 14, 9, 13, 11, 15
};


static Console console;
static lib::terminal::Emulator emulator(console);

void platform::print(const char* string) {
    emulator.Write(string);
}
