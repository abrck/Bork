#ifndef HOOKS_H
#define HOOKS_H

#include "engine.h"

void (*oDrawTransition)(void *GameViewport, UCanvas *Canvas) = nullptr;
void hkDrawTransition(void *GameViewport, UCanvas *Canvas)
{
    oDrawTransition(GameViewport, Canvas);

    UWorld* World = *(UWorld**)((uintptr_t)GameViewport + Offsets::World);
    if (IsBadPtr(World))
        return;

    TArray<AActor *> Players;
    UGameplayStatics::GetAllActorsOfClass(World, ASolarCharacter::StaticClass(), Players);

    if (Players.Count <= 0)
        return;

    LOGI("玩家数量: %d", Players.Count);


    

}

#endif // HOOKS_H