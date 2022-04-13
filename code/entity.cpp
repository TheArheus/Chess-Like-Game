#include "entity.h"

void
CreateEntity(world* World, v2 Position, v2 Velocity, u32 Width, u32 Height, entity_type Type, u32 Color)
{
    entity_storage* StorageToUpdate = World->EntityStorage;
    entity_storage* StorageToGetFrom = World->RemovedEntityStorage;

    if (StorageToUpdate->EntityCount == 0)
    {
        StorageToUpdate->Entities = (entity*)malloc(sizeof(entity));
    }

    // TODO: Fetching from the "StorageToGetFrom" 
    // should be better that this, I believe?
    entity NewEntity = {};
    u32 NewEntityID;
    if (StorageToGetFrom->EntityCount > 0)
    {
        NewEntity = StorageToGetFrom->Entities[--StorageToGetFrom->EntityCount];
        NewEntityID = NewEntity.ID;

        NewEntity.Component->P      = Position;
        NewEntity.Component->dP     = Velocity;
        NewEntity.Component->Width  = Width;
        NewEntity.Component->Height = Height;
        NewEntity.Component->Type   = Type;
        NewEntity.Component->Color  = Color;

        NewEntity.Component->StorageIndex = StorageToUpdate->EntityCount;
    }
    else
    {
        NewEntityID = StorageToUpdate->EntityCount;
        entity_component* NewEntityComponent = (entity_component*)calloc(1, sizeof(entity_component));

        NewEntityComponent->P      = Position;
        NewEntityComponent->dP     = Velocity;
        NewEntityComponent->Width  = Width;
        NewEntityComponent->Height = Height;
        NewEntityComponent->Type   = Type;
        NewEntityComponent->Color  = Color;

        NewEntity.Component = NewEntityComponent;

        NewEntity.ID = NewEntityID;

        NewEntity.Component->StorageIndex = StorageToUpdate->EntityCount;

        // NOTE: This is a bad thing here? 
        // I should think about a better solution here
        StorageToUpdate->Entities = (entity*)realloc(StorageToUpdate->Entities, sizeof(entity) * (StorageToUpdate->EntityCount + 1));
    }

    StorageToUpdate->Entities[StorageToUpdate->EntityCount++] = NewEntity;
}

// NOTE: Do I really want to remove Entities by their ID
// Maybe create another function to remove entities directly
void
RemoveEntityByID(world* World, u32 EntityID)
{
    entity_storage* StorageInUse = World->EntityStorage;
    entity_storage* StorageToStore = World->RemovedEntityStorage;

    if (StorageToStore->EntityCount == 0)
    {
        StorageToStore->Entities = (entity*)malloc(sizeof(entity));
    }

    entity EntityToRemove;
    entity EntityToMove = StorageInUse->Entities[--StorageInUse->EntityCount];

    if (EntityToMove.ID != EntityID) 
    {
        for (u32 EntityIndex = 0;
            EntityIndex < StorageInUse->EntityCount;
            ++EntityIndex)
        {
            entity Entity = StorageInUse->Entities[EntityIndex];
            if (Entity.ID == EntityID)
            {
                EntityToRemove = Entity;
                break;
            }
        }

        EntityToMove.Component->StorageIndex = EntityToRemove.Component->StorageIndex;
        EntityToRemove.Component->StorageIndex = StorageToStore->EntityCount;

        StorageInUse->Entities[EntityToMove.Component->StorageIndex] = EntityToMove;
    }
    else
    {
        EntityToRemove = EntityToMove;
    }

    // NOTE: This is a bad thing here? 
    // I should think about a better solution here
    StorageToStore->Entities = (entity*)realloc(StorageToStore->Entities, sizeof(entity) * (StorageToStore->EntityCount + 1));
    StorageToStore->Entities[StorageToStore->EntityCount++] = EntityToRemove;
}

entity*
GetEntityByType(world* World, entity_type Type)
{
    entity* Result = 0;

    entity_storage* StorageToUse = World->EntityStorage;
    for (u32 EntityIndex = 0;
        EntityIndex < StorageToUse->EntityCount;
        ++EntityIndex)
    {
        entity* EntityFound = StorageToUse->Entities + EntityIndex;
        if (EntityFound->Component->Type == Type)
        {
            Result = EntityFound;
        }
    }
    return Result;
}

std::vector<v2>
GetEntityVertices(entity* Entity)
{
    v2 EntityP = Entity->Component->P;
    r32 Width  = Entity->Component->Width;
    r32 Height = Entity->Component->Height;

    std::vector<v2> VerticesResult;
    VerticesResult.push_back(EntityP);
    VerticesResult.push_back(EntityP + V2(Width, 0));
    VerticesResult.push_back(EntityP + V2(Width, Height));
    VerticesResult.push_back(EntityP + V2(0, Height));

    return VerticesResult;
}

struct collision_result
{
    entity* CollidedEntity;
    b32 AreCollided;
};

collision_result
CheckForCollision(entity_storage* Storage, entity* A)
{
    collision_result Result;
    Result.AreCollided = false;
    Result.CollidedEntity = nullptr;

    for (u32 EntityIndex = 0;
        EntityIndex < Storage->EntityCount;
        ++EntityIndex)
    {
        entity* B = Storage->Entities + EntityIndex;
        if (A != B)
        {
            v2 PositionA = A->Component->P;
            i32 WidthA = A->Component->Width;
            i32 HeightA = A->Component->Height;

            v2 PositionB = B->Component->P;
            i32 WidthB = B->Component->Width;
            i32 HeightB = B->Component->Height;
            if (((PositionA.x) < (PositionB.x + WidthB)) &&
                ((PositionA.y) < (PositionB.y + HeightB)) &&
                ((PositionA.x + WidthA) > (PositionB.x)) &&
                ((PositionA.y + HeightA) > (PositionB.y)))
            {
                Result.CollidedEntity = B;
                Result.AreCollided = true;
            }
        }
    }
    return Result;
}

struct collision_resolution_result
{
    v2 Normal;
};

collision_resolution_result
ResolveCollisionBoxBox(entity* A, entity* B)
{
    collision_resolution_result Result = {};

    v2 VerticesOfA[] =
    {
         A->Component->P,
         A->Component->P + V2(A->Component->Width, 0),
         A->Component->P + V2(A->Component->Width, A->Component->Height),
        (A->Component->P + V2(0, A->Component->Height))
    };

    v2 VerticesOfB[] =
    {
         B->Component->P,
         B->Component->P + V2(B->Component->Width, 0),
         B->Component->P + V2(B->Component->Width, B->Component->Height),
        (B->Component->P + V2(0, B->Component->Height))
    };

    i32 SizeOfA = ArraySize(VerticesOfA);
    i32 SizeOfB = ArraySize(VerticesOfB);

    r32 Separation = -FLT_MAX;
    for (u32 IndexA = 0;
        IndexA < SizeOfA;
        ++IndexA)
    {
        v2 SideOfA_A = VerticesOfA[IndexA];
        v2 SideOfA_B = VerticesOfA[(IndexA + 1) % SizeOfA];

        v2 NormalOfA = Normal(SideOfA_B - SideOfA_A);

        r32 MinimumSeparation = FLT_MAX;
        for (u32 IndexB = 0;
            IndexB < SizeOfB;
            ++IndexB)
        {
            v2 SideOfB_A = VerticesOfB[IndexB];

            r32 Projection = Inner(NormalOfA, SideOfB_A - SideOfA_A);
            if (Projection < MinimumSeparation)
            {
                MinimumSeparation = Projection;
            }
        }
        if (MinimumSeparation > Separation)
        {
            Separation = MinimumSeparation;
            Result.Normal = NormalOfA;
        }
    }

    return Result;
}

void
UpdateEntities(world* World, r32 DeltaTime, bool* GameOver, i32* BallCount)
{
    entity_storage* StorageToUpdate = World->EntityStorage;
    for (u32 EntityIndex = 0;
        EntityIndex < StorageToUpdate->EntityCount;
        ++EntityIndex)
    {
        entity* Entity = StorageToUpdate->Entities + EntityIndex;

        Entity->Component->P += Entity->Component->dP * DeltaTime;
    }
}
