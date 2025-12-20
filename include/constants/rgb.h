#ifndef POKERUBY_CONSTANTS_RGB_H
#define POKERUBY_CONSTANTS_RGB_H

#define GET_R(color) ((color) & 0x1F)
#define GET_G(color) (((color) >> 5) & 0x1F)
#define GET_B(color) (((color) >> 10) & 0x1F)

#define RGB_ALPHA       (1 << 15)
#define IS_ALPHA(color) ((color) & RGB_ALPHA)

#define RGB2(r, g, b) (((b) << 10) | ((g) << 5) | (r))
#define RGB(r, g, b)  ((r) | ((g) << 5) | ((b) << 10))

#define RGB_BLACK      RGB(0, 0, 0)
#define RGB_WHITE      RGB(31, 31, 31)
#define RGB_RED        RGB(31, 0, 0)
#define RGB_GREEN      RGB(0, 31, 0)
#define RGB_BLUE       RGB(0, 0, 31)
#define RGB_YELLOW     RGB(31, 31, 0)
#define RGB_MAGENTA    RGB(31, 0, 31)
#define RGB_CYAN       RGB(0, 31, 31)
#define RGB_WHITEALPHA (RGB_WHITE | RGB_ALPHA)

#endif // POKERUBY_CONSTANTS_RGB_H
