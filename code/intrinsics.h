#include <vector>
#include <string>
#include <stdint.h>

//#include "stb_truetype.h"

#if 0
// NOTE: I am doing it because that seems
// newer versions of SDL2 does not
// have SDL2main.lib hence 
// that seems they are not defining main function.
// 100% I could be mistaken here
#undef main
#endif

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef uint64_t    i64;

typedef float       r32;
typedef double      r64;
typedef uint32_t    b32;

#define internal static
#define global_persist static

#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS)
#define TILE_SIZE 32

#define Min(a, b) ((a < b) ? a : b)
#define Max(a, b) ((a > b) ? a : b)

#define ArraySize(Arr) (sizeof(Arr) / (sizeof(Arr[0])))

