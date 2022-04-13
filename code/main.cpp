#include <memory>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vulkan_renderer.h"
#include "entity.h"
#include "entity.cpp"
#undef main

bool IsDebug = false;
bool StartGame = false;

i32 PreviousFrameTime = 0;
r32 DeltaTime = 0;
r32 TimeForFrame = 0;
r32 dtForFrame = 0;

struct vertex_data
{
    v2 Vertex;
    v3 Color;
};

vertex_data CreateVertex(v2 Vert, v3 Col)
{
    vertex_data Result = {};
    Result.Vertex = Vert;
    Result.Color = Col;
    return Result;
}

class game
{
private:
    world* World;
    bool IsRunning;

    void Setup();
    void ProcessInput();
    void Update();
    void Render();

    vulkan_renderer* Renderer;

    image RenderEntry;
    buffer RenderBuffer;
    
    buffer TransientBuffer;
    buffer VertexBuffer;
    buffer IndexBuffer;

    memory_block MainBlock;
    memory_block VertexBlock;
    memory_block IndexBlock;

public:
    game();
    ~game();

    void Run();
    void CreateLevel(u32, u32);
};

game::
game()
{
    IsRunning = InitWindow();

    Renderer = new vulkan_renderer(window, ColorBuffer->Width, ColorBuffer->Height);
    Renderer->InitVulkanRenderer();
    
    Renderer->UploadShader("../shaders/mesh.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    Renderer->UploadShader("../shaders/mesh.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    Renderer->InitGraphicsPipeline();

    World = (world*)malloc(sizeof(world));
    World->EntityStorage = (entity_storage*)calloc(1, sizeof(entity_storage));
    World->RemovedEntityStorage = (entity_storage*)calloc(1, sizeof(entity_storage));

    Setup();
}

void game::
CreateLevel(u32 NumOfCols, u32 NumOfRows)
{
    v2 Start = V2(0, 0);

    i32 LevelWidth  = ColorBuffer->Width;
    i32 LevelHeight = ColorBuffer->Height;

    i32 EntityWidth  = LevelWidth  / NumOfRows;
    i32 EntityHeight = LevelHeight / NumOfCols;

    for(u32 Y = 0;
        Y < NumOfRows;
        ++Y)
    {
        for(u32 X = 0;
            X < NumOfCols;
            ++X)
        {
            v2 Position = Start + V2(X * EntityWidth, Y * EntityHeight);
            v4 Color;
            if((Y % 2) == 0)
            {
                if((X % 2) == 0)
                {
                    Color = V4(255, 255, 255, 255);
                }
                else
                {
                    Color = V4(0, 0, 0, 255);
                }
            }
            else
            {
                if((X % 2) == 0)
                {
                    Color = V4(0, 0, 0, 255);
                }
                else
                {
                    Color = V4(255, 255, 255, 255);
                }
            }
            //CreateEntity(World, Position, V2(0, 0), EntityWidth, EntityHeight, EntityType_Structure, PackBGRA(Color));
            DrawRect(ColorBuffer, Position, Position + V2(EntityWidth, EntityHeight), PackRGBA(Color));

            if((X < 3) && (Y < 3))
            {
                CreateEntity(World, Position + 2, V2(0, 0), EntityWidth - 4, EntityHeight - 4, EntityType_PlayerChess, 0xFFFFFF00);
            }
            else if((X > (NumOfRows - 4)) && (Y > (NumOfCols - 4)))
            {
                CreateEntity(World, Position + 2, V2(0, 0), EntityWidth - 4, EntityHeight - 4, EntityType_EnemyChess, 0xFF00FFFF);
            }
        }
    }
}

void game::
Setup()
{
    RenderEntry  = Renderer->CreateImage(ColorBuffer->Width, ColorBuffer->Height, 
                                         VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, 
                                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    RenderBuffer = Renderer->AllocateBuffer(ColorBuffer->Width*ColorBuffer->Height*sizeof(u32), 
                                            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    TransientBuffer = Renderer->AllocateBuffer(1024, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    VertexBuffer = Renderer->AllocateBuffer(1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    IndexBuffer = Renderer->AllocateBuffer(1024, VK_BUFFER_USAGE_INDEX_BUFFER_BIT|VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    ColorBuffer->Memory = (u32*)RenderBuffer.Data;
    //ColorBuffer->Memory = (u32*)malloc(sizeof(u32)*ColorBuffer->Width*ColorBuffer->Height);

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, 
                                SDL_TEXTUREACCESS_STREAMING, 
                                ColorBuffer->Width, ColorBuffer->Height);

    CreateLevel(8, 8);
}

void game::
ProcessInput()
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type)
    {
        case SDL_QUIT:
            IsRunning = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE) IsRunning = false;
            if(event.key.keysym.sym == SDLK_r) IsDebug = !IsDebug;
            if(event.key.keysym.sym == SDLK_SPACE)
            break;
        case SDL_KEYUP:
            break;
    }
}

void game::
Update()
{
    dtForFrame += TimeForFrame;
    int TimeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() + PreviousFrameTime);

    if(TimeToWait > 0 && (TimeToWait <= FRAME_TARGET_TIME))
    {
        SDL_Delay(TimeToWait);
    }

    DeltaTime = (SDL_GetTicks() - PreviousFrameTime) / 1000.0f;
    //if (DeltaTime > TimeForFrame) DeltaTime = TimeForFrame;
    PreviousFrameTime = SDL_GetTicks();

    UpdateEntities(World, DeltaTime);
}

void game::
Render()
{
    Renderer->UpdateTexture(RenderEntry, RenderBuffer);

#if 0
    entity_storage* StorageToUpdate = World->EntityStorage;
    for(u32 EntityIndex = 0;
        EntityIndex < StorageToUpdate->EntityCount;
        ++EntityIndex)
    {
        // Use here stl to store entities
        entity* Entity = StorageToUpdate->Entities + EntityIndex;

        v2 Start  = Entity->Component->P;
        v2 Width  = Entity->Component->Width  * V2(1, 0);
        v2 Height = Entity->Component->Height * V2(0, 1);

        u32 Color = Entity->Component->Color;

        switch(Entity->Component->Type)
        {
            case EntityType_PlayerChess:
            {
                DrawRotRect(ColorBuffer, Start, Width, Height, Color, nullptr);
            } break;

            case EntityType_EnemyChess:
            {
                DrawRotRect(ColorBuffer, Start, Width, Height, Color, nullptr);
            } break;

            case EntityType_Structure:
            {
                DrawRotRect(ColorBuffer, Start, Width, Height, Color, nullptr);
            } break;
        }
    }
#endif
    //Renderer->DrawImage(RenderEntry);
    //Renderer->BindBuffer(VertexBuffer, 0);
    //Renderer->BindImage(RenderEntry, 1);
    Renderer->DrawMeshes(VertexBuffer, IndexBuffer, RenderEntry);
}

void game::
Run()
{
    while(IsRunning)
    {
        AllocateMemoryBlock(&MainBlock, (u8*)TransientBuffer.Data, (memory_index)TransientBuffer.Size);

#if 1
        std::vector<v2> MainWindow;
        MainWindow.push_back(V2(-1, -1));
        MainWindow.push_back(V2( 1, -1));
        MainWindow.push_back(V2( 1,  1));
        MainWindow.push_back(V2(-1,  1));
#else
        std::vector<vertex_data> Vertices;
        Vertices.push_back(CreateVertex(V2( 0.0, -0.5), V3(1, 0, 0)));
        Vertices.push_back(CreateVertex(V2( 0.5,  0.5), V3(0, 1, 0)));
        Vertices.push_back(CreateVertex(V2(-0.5,  0.5), V3(0, 0, 1)));
#endif

        std::vector<u32> MainWindowIndices = {0, 1, 2, 2, 3, 0};
        //std::vector<u32> MainWindowIndices = {0, 1, 2};

#if 0
        SubMemoryBlock(&MainBlock, &VertexBlock, Vertices.size()*sizeof(vertex_data));
        SubMemoryBlock(&MainBlock, &IndexBlock, MainWindowIndices.size()*sizeof(u32));

        PushData(&VertexBlock, Vertices.data(), VertexBlock.Size);
        PushData(&IndexBlock, MainWindowIndices.data(), IndexBlock.Size);

        Renderer->UpdateBuffer(VertexBuffer, TransientBuffer, VertexBlock.Size, GetOffsetFromMainBase(&MainBlock, &VertexBlock));
        Renderer->UpdateBuffer(IndexBuffer, TransientBuffer, IndexBlock.Size, GetOffsetFromMainBase(&MainBlock, &IndexBlock));
#else
        Renderer->UpdateBuffer(VertexBuffer, TransientBuffer, MainWindow.data(), MainWindow.size()*sizeof(v2));
        Renderer->UpdateBuffer(IndexBuffer, TransientBuffer, MainWindowIndices.data(), MainWindowIndices.size()*sizeof(u32));
#endif

        ProcessInput();
        Update();
        Render();
    }
}

game::
~game()
{
    DestroyWindow();
}

int 
main(int argc, char** argv)
{
    game* NewGame = new game();

    NewGame->Run();

    return 0;
}
