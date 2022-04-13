
#if !defined(INTRINSICS_H)

#include <vector>
#include <unordered_map>
#include <string>
#include <stdint.h>

//#include "stb_truetype.h"
//#include "stb_image.h"

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

typedef size_t      memory_index;

#define internal        static
#define local_persist   static
#define global_variable static

#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS)
#define TILE_SIZE 32

#define Min(a, b) ((a < b) ? a : b)
#define Max(a, b) ((a > b) ? a : b)

#define ArraySize(Arr) (sizeof(Arr) / (sizeof(Arr[0])))
#define Assert(Expression) if(!(Expression)) { *(int*)0 = 0; }

struct memory_block
{
    u8* Base;

    memory_index Size;
    memory_index Used;

    u32 TempCount;
};

internal void
AllocateMemoryBlock(memory_block* Block, u8* Base, memory_index Size)
{
    Block->Base = Base;
    Block->Size = Size;
    Block->Used = 0;
    Block->TempCount = 0;
}

#define PushStruct(Block, Type) (Type*)PushSize_(Block, sizeof(Type)); 
#define PushArray(Block, Type, Ammount) (Type*)PushSize_(Block, sizeof(Type) * Ammount);
#define PushSize(Block, Size) PushSize_(Block, Size);
inline void*
PushSize_(memory_block* Block, memory_index Size)
{
    Assert((Block->Used + Size) <= Block->Size);
    void* Result = 0;

    if(((Block->Used + Size) <= Block->Size))
    {
        Result = Block->Base;

        Block->Base += Size;
        Block->Used += Size;
    }

    return Result;
}

inline void
PushData(memory_block* Block, void* SrcData, size_t Size)
{
    void* DstData = PushSize(Block, Size);
    memcpy(DstData, SrcData, Size);
}

inline void
SubMemoryBlock(memory_block* MainBlock, memory_block* SubBlock, memory_index Size)
{
    SubBlock->Base = (u8*)PushSize(MainBlock, Size);
    SubBlock->Size = Size;
    SubBlock->Used = 0;
    SubBlock->TempCount = 0;
}

inline size_t
GetOffsetFromMainBase(memory_block* MainBlock, memory_block* SubBlock)
{
    size_t Result = (SubBlock->Base - SubBlock->Used) - (MainBlock->Base - MainBlock->Used);
    return Result;
}

#define INTRINSICS_H
#endif
