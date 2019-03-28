
#pragma once

#include "display.hpp"

namespace lib::terminal {

class Emulator {

public:
  Emulator(Display &display);

  void Write(const char *string);

private:
  Display &display;

  Point cursor;
  Point cursor_saved;
  int width;
  int height;

  Char *frame;

  bool linewrap = true;

  int brightness = 0;
  Color foreground = Color::White;
  Color background = Color::Black;

  static const int kMaxParams = 10;
  struct {
    int params[kMaxParams];
    int count = 0;
  } csi;

  enum class State { Initial, Escape, ControlSequence };

  State state = State::Initial;

  auto StateInitial(const char *string) -> const char *;
  auto StateEscape(const char *string) -> const char *;
  auto StateControlSequence(const char *string) -> const char *;

  void SetDisplayAttribute(int code);
};

} // namespace lib::terminal
