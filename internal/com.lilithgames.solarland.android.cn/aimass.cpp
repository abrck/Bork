#include <cmath>
#include "aimass.h"
#include "globals.h"
#include "engine.h"

namespace AimAss
{
    static ASolarCharacter* Enemy = nullptr;
    static FVector AimPos;
    static FVector2D AimPosSc;

    static ASolarCharacter* CandidateEnemy = nullptr;
    static FVector CandidatePos;
    static FVector2D CandidatePosSc;
    static float BestDistance = 3.4e38f;

    void Reset()
    {
        CandidateEnemy = nullptr;
        BestDistance = 3.4e38f;
    }

    void PushTarget(FVector2D ScreenCenter, ASolarCharacter* Player, USkeletalMeshComponent* Mesh, BoneIndex* BoneIdx)
    {
        FVector TempAimPos = Mesh->GetBoneLocation(BoneIdx->head);
        FVector2D TempAimPosSc;

        if (!GameData::MyController->ProjectWorldLocationToScreen(TempAimPos, TempAimPosSc))
            return;

        float dx = TempAimPosSc.X - ScreenCenter.X;
        float dy = TempAimPosSc.Y - ScreenCenter.Y;
        float distance = sqrtf(dx * dx + dy * dy);

        if (distance < 300 && distance < BestDistance)
        {
            CandidateEnemy = Player;
            CandidatePos = TempAimPos;
            CandidatePosSc = TempAimPosSc;
            BestDistance = distance;
        }
    }

    void PopTarget()
    {
        Enemy = nullptr;
        AimPos = FVector();
        AimPosSc = FVector2D();

        if (CandidateEnemy)
        {
            Enemy = CandidateEnemy;
            AimPos = CandidatePos;
            AimPosSc = CandidatePosSc;
        }

        CandidateEnemy = nullptr;
        BestDistance = 3.4e38f;
    }

    bool CanAim()
    {
        return Enemy != nullptr;
    }

    ASolarCharacter* GetTarget()
    {
        return Enemy;
    }

    FVector GetAimPos()
    {
        return AimPos;
    }

    FVector2D GetAimScreenPos()
    {
        return AimPosSc;
    }

    void MemoryAimbot(float Smooth)
    {
        FVector CameraLocation = GameData::MyCameraManager->GetCameraLocation();
        FRotator CameraRotation = GameData::MyCameraManager->GetCameraRotation();

        FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, AimPos);

        FRotator DeltaRot = NewRotation - CameraRotation;

        GameData::MyController->AddPitchInput(-DeltaRot.Pitch * Smooth);
        GameData::MyController->AddYawInput(DeltaRot.Yaw * Smooth);
    }
}
