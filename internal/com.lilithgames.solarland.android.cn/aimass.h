#pragma once

#include "classes.h"

namespace AimAss
{
    void PushTarget(FVector2D ScreenCenter, ASolarCharacter* Player, USkeletalMeshComponent* Mesh, BoneIndex* Bones);
    void PopTarget();
    void Reset();

    bool CanAim();
    ASolarCharacter* GetTarget();
    FVector GetAimPos();
    FVector2D GetAimScreenPos();

    void MemoryAimbot(float Smooth);
}
