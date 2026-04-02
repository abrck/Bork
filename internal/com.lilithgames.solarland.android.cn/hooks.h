#ifndef HOOKS_H
#define HOOKS_H

#include "engine.h"

static FLinearColor ColorWhite(1.0f, 1.0f, 1.0f, 1.0f);
static FLinearColor ColorBlack(0.0f, 0.0f, 0.0f, 1.0f);
static FLinearColor ColorRed(1.0f, 0.0f, 0.0f, 1.0f);
static FLinearColor ColorGreen(0.0f, 1.0f, 0.0f, 1.0f);
static FLinearColor ColorBlue(0.0f, 0.0f, 1.0f, 1.0f);
static FLinearColor ColorYellow(1.0f, 1.0f, 0.0f, 1.0f);
static FLinearColor ColorCyan(0.0f, 1.0f, 1.0f, 1.0f);
static FLinearColor ColorMagenta(1.0f, 0.0f, 1.0f, 1.0f);
static FLinearColor ColorGray(0.5f, 0.5f, 0.5f, 1.0f);
static FLinearColor ColorTransparent(0.0f, 0.0f, 0.0f, 0.0f);

// ======================================================================================================

void (*oDrawTransition)(void *GameViewport, UCanvas *Canvas) = nullptr;
void hkDrawTransition(void *GameViewport, UCanvas *Canvas)
{
    oDrawTransition(GameViewport, Canvas);

    GameData::World = *(UWorld **)((uintptr_t)GameData::GameViewport + Offsets::World);
    if (IsBadPtr(GameData::World))
        return;
    LOGI("World: %p", GameData::World);

    TArray<AActor *> Players;
    UGameplayStatics::GetAllActorsOfClass(GameData::World, ASolarCharacter::StaticClass(), Players);

    if (Players.Count <= 0)
        return;

    GameData::MyController = (ASolarPlayerController *)UGameplayStatics::GetPlayerController(GameData::World, 0);
    if (IsBadPtr(GameData::MyController))
        return;
    LOGI("MyController: %p", GameData::MyController);

    GameData::MyPawn = (ASolarCharacter *)GameData::MyController->K2_GetPawn();
    if (IsBadPtr(GameData::MyPawn))
        return;
    LOGI("MyPawn: %p", GameData::MyPawn);

    GameData::MyCameraManager = UGameplayStatics::GetPlayerCameraManager(GameData::World, 0);
    if (IsBadPtr(GameData::MyCameraManager))
        return;
    LOGI("MyCameraManager: %p", GameData::MyCameraManager);

    int PlayerCount = 0, RoBotCount = 0;

    for (int i = 0; i < Players.Count; i++)
    {
        ASolarCharacter *Player = (ASolarCharacter *)(Players[i]);
        if (IsBadPtr(Player))
            continue;

        if (!Player->K2_IsAlive())
            continue;

        if (Player == GameData::MyPawn)
            continue;

        ASolarPlayerState *PlayerState = Player->GetSolarPlayerState();
        if (IsBadPtr(PlayerState))
            continue;

        const char *PlayerName = "Bot";
        bool IsBot = PlayerState->IsAIPlayer();

        if (IsBot)
        {
            RoBotCount++;
        }
        else
        {
            PlayerCount++;
            PlayerName = PlayerState->GetPlayerName().ToString();
        }

        int Distance = (int)Player->GetDistanceTo((AActor *)GameData::MyPawn);

        USkeletalMeshComponent *Mesh = *(USkeletalMeshComponent **)((uintptr_t)Player + Offsets::Mesh);
        if (IsBadPtr(Mesh))
            continue;

        BoneIndex *BoneIdx = Mesh->GetBoneIndex();

        FVector2D BoxCoords[2];
        if (!Mesh->GetBoxCoords(GameData::MyController, BoxCoords))
            continue;

        float x = BoxCoords[0].X;
        float y = BoxCoords[0].Y;
        float w = BoxCoords[1].X - BoxCoords[0].X;
        float h = BoxCoords[1].Y - BoxCoords[0].Y;

        FVector2D RootScreenPos;
        FVector RootPos = Mesh->GetBoneLocation(BoneIdx->root);
        GameData::MyController->ProjectWorldLocationToScreen(RootPos, RootScreenPos);

        bool IsRootBlocked;
        IsRootBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, RootPos, false);

        FVector2D HeadScreenPos;
        FVector HeadPos = Mesh->GetBoneLocation(BoneIdx->head);
        GameData::MyController->ProjectWorldLocationToScreen(HeadPos, HeadScreenPos);

        bool IsHeadBlocked;
        IsHeadBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, HeadPos, false);

        FVector2D NeckScreenPos;
        FVector NeckPos = Mesh->GetBoneLocation(BoneIdx->neck_01);
        GameData::MyController->ProjectWorldLocationToScreen(NeckPos, NeckScreenPos);

        bool IsNeckBlocked;
        IsNeckBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, NeckPos, false);

        FVector2D Spine3ScreenPos;
        FVector Spine3Pos = Mesh->GetBoneLocation(BoneIdx->spine_03);
        GameData::MyController->ProjectWorldLocationToScreen(Spine3Pos, Spine3ScreenPos);

        bool IsSpine3Blocked;
        IsSpine3Blocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, Spine3Pos, false);

        FVector2D Spine2ScreenPos;
        FVector Spine2Pos = Mesh->GetBoneLocation(BoneIdx->spine_02);
        GameData::MyController->ProjectWorldLocationToScreen(Spine2Pos, Spine2ScreenPos);

        bool IsSpine2Blocked;
        IsSpine2Blocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, Spine2Pos, false);

        FVector2D Spine1ScreenPos;
        FVector Spine1Pos = Mesh->GetBoneLocation(BoneIdx->spine_01);
        GameData::MyController->ProjectWorldLocationToScreen(Spine1Pos, Spine1ScreenPos);

        bool IsSpine1Blocked;
        IsSpine1Blocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, Spine1Pos, false);

        FVector2D PelvisScreenPos;
        FVector PelvisPos = Mesh->GetBoneLocation(BoneIdx->pelvis);
        GameData::MyController->ProjectWorldLocationToScreen(PelvisPos, PelvisScreenPos);

        bool IsPelvisBlocked;
        IsPelvisBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, PelvisPos, false);

        // 左手
        FVector2D ClavicleLScreenPos;
        FVector ClavicleLPos = Mesh->GetBoneLocation(BoneIdx->clavicle_l);
        GameData::MyController->ProjectWorldLocationToScreen(ClavicleLPos, ClavicleLScreenPos);

        bool IsClavicleLBlocked;
        IsClavicleLBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, ClavicleLPos, false);

        FVector2D UpperArmLScreenPos;
        FVector UpperArmLPos = Mesh->GetBoneLocation(BoneIdx->upperarm_l);
        GameData::MyController->ProjectWorldLocationToScreen(UpperArmLPos, UpperArmLScreenPos);

        bool IsUpperArmLBlocked;
        IsUpperArmLBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, UpperArmLPos, false);

        FVector2D LowerArmLScreenPos;
        FVector LowerArmLPos = Mesh->GetBoneLocation(BoneIdx->lowerarm_l);
        GameData::MyController->ProjectWorldLocationToScreen(LowerArmLPos, LowerArmLScreenPos);

        bool IsLowerArmLBlocked;
        IsLowerArmLBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, LowerArmLPos, false);

        FVector2D HandLScreenPos;
        FVector HandLPos = Mesh->GetBoneLocation(BoneIdx->hand_l);
        GameData::MyController->ProjectWorldLocationToScreen(HandLPos, HandLScreenPos);

        bool IsHandLBlocked;
        IsHandLBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, HandLPos, false);

        // 右手
        FVector2D ClavicleRScreenPos;
        FVector ClavicleRPos = Mesh->GetBoneLocation(BoneIdx->clavicle_r);
        GameData::MyController->ProjectWorldLocationToScreen(ClavicleRPos, ClavicleRScreenPos);

        bool IsClavicleRBlocked;
        IsClavicleRBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, ClavicleRPos, false);

        FVector2D UpperArmRScreenPos;
        FVector UpperArmRPos = Mesh->GetBoneLocation(BoneIdx->upperarm_r);
        GameData::MyController->ProjectWorldLocationToScreen(UpperArmRPos, UpperArmRScreenPos);

        bool IsUpperArmRBlocked;
        IsUpperArmRBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, UpperArmRPos, false);

        FVector2D LowerArmRScreenPos;
        FVector LowerArmRPos = Mesh->GetBoneLocation(BoneIdx->lowerarm_r);
        GameData::MyController->ProjectWorldLocationToScreen(LowerArmRPos, LowerArmRScreenPos);

        bool IsLowerArmRBlocked;
        IsLowerArmRBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, LowerArmRPos, false);

        FVector2D HandRScreenPos;
        FVector HandRPos = Mesh->GetBoneLocation(BoneIdx->hand_r);
        GameData::MyController->ProjectWorldLocationToScreen(HandRPos, HandRScreenPos);

        bool IsHandRBlocked;
        IsHandRBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, HandRPos, false);

        // 左腿
        FVector2D ThighLScreenPos;
        FVector ThighLPos = Mesh->GetBoneLocation(BoneIdx->thigh_l);
        GameData::MyController->ProjectWorldLocationToScreen(ThighLPos, ThighLScreenPos);

        bool IsThighLBlocked;
        IsThighLBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, ThighLPos, false);

        FVector2D CalfLScreenPos;
        FVector CalfLPos = Mesh->GetBoneLocation(BoneIdx->calf_l);
        GameData::MyController->ProjectWorldLocationToScreen(CalfLPos, CalfLScreenPos);

        bool IsCalfLBlocked;
        IsCalfLBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, CalfLPos, false);

        FVector2D FootLScreenPos;
        FVector FootLPos = Mesh->GetBoneLocation(BoneIdx->foot_l);
        GameData::MyController->ProjectWorldLocationToScreen(FootLPos, FootLScreenPos);

        bool IsFootLBlocked;
        IsFootLBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, FootLPos, false);

        // 右腿
        FVector2D ThighRScreenPos;
        FVector ThighRPos = Mesh->GetBoneLocation(BoneIdx->thigh_r);
        GameData::MyController->ProjectWorldLocationToScreen(ThighRPos, ThighRScreenPos);

        bool IsThighRBlocked;
        IsThighRBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, ThighRPos, false);

        FVector2D CalfRScreenPos;
        FVector CalfRPos = Mesh->GetBoneLocation(BoneIdx->calf_r);
        GameData::MyController->ProjectWorldLocationToScreen(CalfRPos, CalfRScreenPos);

        bool IsCalfRBlocked;
        IsCalfRBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, CalfRPos, false);

        FVector2D FootRScreenPos;
        FVector FootRPos = Mesh->GetBoneLocation(BoneIdx->foot_r);
        GameData::MyController->ProjectWorldLocationToScreen(FootRPos, FootRScreenPos);

        bool IsFootRBlocked;
        IsFootRBlocked = !GameData::MyController->LineOfSightTo((AActor *)GameData::MyCameraManager, FootRPos, false);

        // =======================================================================================================

        // 绘制射线/方框
        {
            auto GetLineColor = [&](bool IsBlocked) -> FLinearColor
            {
                if (IsBlocked)
                    return IsBot ? ColorWhite : ColorRed;
                else
                    return IsBot ? ColorCyan : ColorGreen;
            };

            Canvas->K2_DrawLine(FVector2D(Canvas->GetSize().X / 2, 0), FVector2D((BoxCoords[0].X + BoxCoords[1].X) / 2.0f, BoxCoords[0].Y), 1.0f, GetLineColor(IsHeadBlocked));

            Canvas->K2_DrawLine(FVector2D(x, y), FVector2D(x + w, y), 1.0f, GetLineColor(IsHeadBlocked));         // Top
            Canvas->K2_DrawLine(FVector2D(x, y), FVector2D(x, y + h), 1.0f, GetLineColor(IsHeadBlocked));         // Left
            Canvas->K2_DrawLine(FVector2D(x + w, y), FVector2D(x + w, y + h), 1.0f, GetLineColor(IsHeadBlocked)); // Right
            Canvas->K2_DrawLine(FVector2D(x, y + h), FVector2D(x + w, y + h), 1.0f, GetLineColor(IsHeadBlocked)); // Bottom
        }

        // 绘制名字/距离
        {
            std::string NameText = std::string(PlayerName) + " [" + std::to_string(Distance / 100.0f) + "m]";
            Canvas->K2_DrawText(GameData::GEngine->GetTinyFont(), 13, FString::FromAnsi(NameText.c_str()), FVector2D((BoxCoords[0].X + BoxCoords[1].X) / 2, BoxCoords[0].Y + h + 15), ColorWhite, true, true, true, ColorBlack);
        }

        // 绘制骨骼
        {
            auto GetBoneColor = [&](bool IsBlocked) -> FLinearColor
            {
                if (IsBlocked)
                    return IsBot ? ColorWhite : ColorRed;
                else
                    return IsBot ? ColorCyan : ColorGreen;
            };

            Canvas->K2_DrawLine(HeadScreenPos, NeckScreenPos, 1.0f, GetBoneColor(IsHeadBlocked || IsNeckBlocked));
            Canvas->K2_DrawLine(NeckScreenPos, Spine3ScreenPos, 1.0f, GetBoneColor(IsNeckBlocked || IsSpine3Blocked));
            Canvas->K2_DrawLine(Spine3ScreenPos, Spine2ScreenPos, 1.0f, GetBoneColor(IsSpine3Blocked || IsSpine2Blocked));
            Canvas->K2_DrawLine(Spine2ScreenPos, Spine1ScreenPos, 1.0f, GetBoneColor(IsSpine2Blocked || IsSpine1Blocked));
            Canvas->K2_DrawLine(Spine1ScreenPos, PelvisScreenPos, 1.0f, GetBoneColor(IsSpine1Blocked || IsPelvisBlocked));
            Canvas->K2_DrawLine(NeckScreenPos, ClavicleLScreenPos, 1.0f, GetBoneColor(IsClavicleLBlocked || IsNeckBlocked));
            Canvas->K2_DrawLine(ClavicleLScreenPos, UpperArmLScreenPos, 1.0f, GetBoneColor(IsUpperArmLBlocked || IsClavicleLBlocked));
            Canvas->K2_DrawLine(UpperArmLScreenPos, LowerArmLScreenPos, 1.0f, GetBoneColor(IsLowerArmLBlocked || IsUpperArmLBlocked));
            Canvas->K2_DrawLine(LowerArmLScreenPos, HandLScreenPos, 1.0f, GetBoneColor(IsHandLBlocked || IsLowerArmLBlocked));
            Canvas->K2_DrawLine(NeckScreenPos, ClavicleRScreenPos, 1.0f, GetBoneColor(IsClavicleRBlocked || IsNeckBlocked));
            Canvas->K2_DrawLine(ClavicleRScreenPos, UpperArmRScreenPos, 1.0f, GetBoneColor(IsUpperArmRBlocked || IsClavicleRBlocked));
            Canvas->K2_DrawLine(UpperArmRScreenPos, LowerArmRScreenPos, 1.0f, GetBoneColor(IsLowerArmRBlocked || IsUpperArmRBlocked));
            Canvas->K2_DrawLine(LowerArmRScreenPos, HandRScreenPos, 1.0f, GetBoneColor(IsHandRBlocked || IsLowerArmRBlocked));
            Canvas->K2_DrawLine(PelvisScreenPos, ThighLScreenPos, 1.0f, GetBoneColor(IsThighLBlocked || IsPelvisBlocked));
            Canvas->K2_DrawLine(ThighLScreenPos, CalfLScreenPos, 1.0f, GetBoneColor(IsCalfLBlocked || IsThighLBlocked));
            Canvas->K2_DrawLine(CalfLScreenPos, FootLScreenPos, 1.0f, GetBoneColor(IsFootLBlocked || IsCalfLBlocked));
            Canvas->K2_DrawLine(PelvisScreenPos, ThighRScreenPos, 1.0f, GetBoneColor(IsThighRBlocked || IsPelvisBlocked));
            Canvas->K2_DrawLine(ThighRScreenPos, CalfRScreenPos, 1.0f, GetBoneColor(IsCalfRBlocked || IsThighRBlocked));
            Canvas->K2_DrawLine(CalfRScreenPos, FootRScreenPos, 1.0f, GetBoneColor(IsFootRBlocked || IsCalfRBlocked));
        }
    }

    std::string TotalCount = "P:" + std::to_string((int)PlayerCount) + " " + "R:" + std::to_string((int)RoBotCount);
    Canvas->K2_DrawText(GameData::GEngine->GetTinyFont(), 18, FString::FromAnsi(TotalCount.c_str()), FVector2D(Canvas->GetSize().X / 2, Canvas->GetSize().Y / 12), ColorWhite, true, true, true, ColorBlack);
}

#endif // HOOKS_H