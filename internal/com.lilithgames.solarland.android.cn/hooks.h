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

    GameData::World = *(UWorld**)((uintptr_t)GameData::GameViewport + Offsets::World);
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

        if (PlayerState->IsAIPlayer())
        {
            RoBotCount++;
        }
        else
        {
            PlayerCount++;
            PlayerName = PlayerState->GetPlayerName().ToString();
        }

        USkeletalMeshComponent *Mesh = *(USkeletalMeshComponent **)((uintptr_t)Player + Offsets::Mesh);
        if (IsBadPtr(Mesh))
            continue;

        TArray<FName> SocketNames = Mesh->GetAllSocketNames();

        for (int j = 0; j < SocketNames.Count; j++)
        {
            FName BoneName = SocketNames[j];
            LOGI("%d | %s (%.2f, %.2f, %.2f)", j, UKismetStringLibrary::Conv_NameToString(BoneName).ToString(), Mesh->GetBoneLocation(BoneName).X, Mesh->GetBoneLocation(BoneName).Y, Mesh->GetBoneLocation(BoneName).Z);
        }

    }

    std::string TotalCount = "P:" + std::to_string((int)PlayerCount) + " " + "R:" + std::to_string((int)RoBotCount);
    Canvas->K2_DrawText(GameData::GEngine->GetTinyFont(), 18, FString::FromAnsi(TotalCount.c_str()), FVector2D(Canvas->GetSize().X / 2, Canvas->GetSize().Y / 12), ColorWhite, true, true, true, ColorBlack);
}

#endif // HOOKS_H