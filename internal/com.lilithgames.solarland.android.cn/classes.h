#ifndef CLASSES_H
#define CLASSES_H

#include "structs.h"
#include "offsets.h"
#include "globals.h"

class UObject
{
public:
    static UObject *StaticFindObject(const char16_t *Name)
    {
        using Fn = UObject *(*)(UObject *, UObject *, const char16_t *, bool);
        return reinterpret_cast<Fn>(Globals::libUE4 + Offsets::StaticFindObject)(nullptr, nullptr, Name, false);
    }

    void ProcessEvent(void *Function, void *Params)
    {
        using Fn = void (*)(UObject *, void *, void *);
        auto vtable = *reinterpret_cast<void ***>(this);
        reinterpret_cast<Fn>(vtable[Offsets::ProcessEventIdx])(this, Function, Params);
    }

    static std::string GetName(uint32_t Id)
    {
        uint32_t Block = Id >> 16;
        uint16_t Offset = Id & 0xFFFF;

        FNamePool* NamePool =
            (FNamePool*)(Globals::libUE4 + Offsets::GName);

        uint8_t* Entry =
            NamePool->Blocks[Block] + Offset * 2;

        uint16_t Header = *(uint16_t*)Entry;

        int Len = Header >> 6;

        if (Len <= 0 || Len > 250)
            return "None";

        return std::string((char*)(Entry + 2), Len);
    }

    static UObject *StaticClass()
    {
        static UObject *Clazz = nullptr;
        if (!Clazz)
            Clazz = StaticFindObject(u"/Script/CoreUObject.Object");
        return Clazz;
    }
};

class AActor : public UObject
{
public:
    // Object: Function Engine.Actor.GetDistanceTo
	// Flags: [Final|Native|Public|BlueprintCallable|BlueprintPure|Const]
	float GetDistanceTo(AActor* OtherActor)
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = StaticFindObject(u"/Script/Engine.Actor:GetDistanceTo");

        struct
        {
            AActor* OtherActor;
            float ReturnValue;
        } Params = {OtherActor};

        ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

};

class APlayerState : public AActor
{
public:
	// Object: Function Engine.PlayerState.GetPlayerName
	// Flags: [Final|Native|Public|BlueprintCallable|BlueprintPure|Const]
	FString GetPlayerName()
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.PlayerState:GetPlayerName");

        struct
        {
            FString ReturnValue;
        } Params;

        ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }
};

class AModularPlayerState : public APlayerState
{
public:
};

class ASolarPlayerState : public AModularPlayerState
{
public:
    // Object: Function Solarland.SolarPlayerState.IsAIPlayer
    // Flags: [Native|Public|BlueprintCallable|BlueprintPure|Const]
    bool IsAIPlayer()
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Solarland.SolarPlayerState:IsAIPlayer");

        struct
        {
            bool ReturnValue;
        } Params;

        ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }
};

class APawn : public AActor
{
public:
};

class ACharacter : public APawn
{
public:
};

class AModularCharacter : public ACharacter
{
public:
};

class ASolarCharacterBase : public AModularCharacter
{
public:
	// Object: Function Solarland.SolarCharacterBase.K2_IsAlive
	// Flags: [Final|Native|Public|BlueprintCallable|BlueprintPure|Const]
	bool K2_IsAlive()
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Solarland.SolarCharacterBase:K2_IsAlive");

        struct
        {
            bool ReturnValue;
        } Params;

        ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

};

class ASolarCharacter : public ASolarCharacterBase
{
public:
	// Object: Function Solarland.SolarCharacter.GetSolarPlayerState
	// Flags: [Final|RequiredAPI|Native|Public|BlueprintCallable|BlueprintPure|Const]
	ASolarPlayerState* GetSolarPlayerState()
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Solarland.SolarCharacter:GetSolarPlayerState");

        struct
        {
            ASolarPlayerState* ReturnValue;
        } Params;

        ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    static UObject *StaticClass()
    {
        static UObject *Clazz = nullptr;
        if (!Clazz)
            Clazz = StaticFindObject(u"/Script/Solarland.SolarCharacter");
        return Clazz;
    }
};

class AController : public AActor
{
public:
	// Object: Function Engine.Controller.LineOfSightTo
	// Flags: [Native|Public|HasDefaults|BlueprintCallable|BlueprintPure|Const]
	bool LineOfSightTo(AActor* Other, FVector ViewPoint, bool bAlternateChecks)
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.Controller:LineOfSightTo");

        struct
        {
            AActor* Other;
            FVector ViewPoint;
            bool bAlternateChecks;
            bool ReturnValue;
        } Params = {Other, ViewPoint, bAlternateChecks};

        ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    // Object: Function Engine.Controller.K2_GetPawn
    // Flags: [Final|Native|Public|BlueprintCallable|BlueprintPure|Const]
    APawn *K2_GetPawn()
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.Controller:K2_GetPawn");

        struct
        {
            APawn *ReturnValue;
        } Params = {};

        ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }
};

class APlayerController : public AController
{
public:
	// Object: Function Engine.PlayerController.ProjectWorldLocationToScreen
	// Flags: [Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable|BlueprintPure|Const]
    bool ProjectWorldLocationToScreen(FVector WorldLocation, FVector2D& ScreenLocation)
    {
        static UObject* Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.PlayerController:ProjectWorldLocationToScreen");

        struct
        {
            FVector WorldLocation;
            FVector2D ScreenLocation;
            bool bPlayerViewportRelative;
            bool ReturnValue;
        } Params = {WorldLocation, {}, false};

        ProcessEvent(Func, &Params);

        ScreenLocation.X = Params.ScreenLocation.X;
        ScreenLocation.Y = Params.ScreenLocation.Y;

        return Params.ReturnValue;
    }
};

class APlayerCameraManager : public AActor
{
public:
};

class AModularPlayerController : public APlayerController
{
public:
};

class ASolarPlayerControllerBase : public AModularPlayerController
{
public:
};

class ASolarPlayerController : public ASolarPlayerControllerBase
{
public:
};

class UActorComponent : public UObject
{
public:
};

class USceneComponent : public UActorComponent
{
public:

};

class UPrimitiveComponent : public USceneComponent
{
public:
};

class UMeshComponent : public UPrimitiveComponent
{
public:
};

class USkinnedMeshComponent : public UMeshComponent
{
public:
    // Object: Function Engine.SkinnedMeshComponent.GetBoneName
	// Flags: [Final|Native|Public|BlueprintCallable|BlueprintPure|Const]
	FName GetBoneName(int32_t BoneIndex)
    {
        static UObject* Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.SkinnedMeshComponent:GetBoneName");

        struct
        {
            int32_t BoneIndex;
            FName ReturnValue;
        } Params = {BoneIndex};

        ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

	// Object: Function Engine.SkinnedMeshComponent.GetNumBones
	// Flags: [Final|Native|Public|BlueprintCallable|BlueprintPure|Const]
	int32_t GetNumBones()
    {
        static UObject* Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.SkinnedMeshComponent:GetNumBones");

        struct
        {
            int32_t ReturnValue;
        } Params;

        ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

	// Object: Function Engine.SkinnedMeshComponent.TransformFromBoneSpace
	// Flags: [Final|Native|Public|HasOutParms|HasDefaults|BlueprintCallable]
    void TransformFromBoneSpace(FName BoneName, const FVector& InPosition, const FRotator& InRotation, FVector& OutPosition, FRotator& OutRotation)
    {
        static UObject* Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.SkinnedMeshComponent:TransformFromBoneSpace");
        
        struct
        {
            FName BoneName;
            FVector InPosition;
            FRotator InRotation;
            FVector OutPosition;
            FRotator OutRotation;
        } Params = {BoneName, InPosition, InRotation, {}, {}};
        
        ProcessEvent(Func, &Params);

        OutPosition = Params.OutPosition;
        OutRotation = Params.OutRotation;
    }

    FVector GetBoneLocation(FName BoneName)
    {
        FVector OutPos; FRotator OutRot;
        TransformFromBoneSpace(BoneName, FVector(), FRotator(), OutPos, OutRot);
        return OutPos;
    }

    BoneIndex* GetBoneIndex()
    {
        static std::unordered_map<int, std::unique_ptr<BoneIndex>> BoneIdxCache;
        int Num = GetNumBones();
        
        auto it = BoneIdxCache.find(Num);
        if (it != BoneIdxCache.end())
            return it->second.get();

        auto NewBoneIdx = std::make_unique<BoneIndex>();
        NewBoneIdx->NumBones = Num;

        for (int i = 0; i < Num; ++i)
        {
            int NameIdx = GetBoneName(i).ComparisonIndex;
            std::string BoneName = UObject::GetName(NameIdx);

            if (BoneName == "head") NewBoneIdx->head = NameIdx;
            else if (BoneName == "neck_01") NewBoneIdx->neck_01 = NameIdx;
            else if (BoneName == "spine_03") NewBoneIdx->spine_03 = NameIdx;
            else if (BoneName == "spine_02") NewBoneIdx->spine_02 = NameIdx;
            else if (BoneName == "spine_01") NewBoneIdx->spine_01 = NameIdx;
            else if (BoneName == "pelvis") NewBoneIdx->pelvis = NameIdx;
            else if (BoneName == "Root") NewBoneIdx->root = NameIdx;
            else if (BoneName == "hand_l") NewBoneIdx->hand_l = NameIdx;
            else if (BoneName == "lowerarm_l") NewBoneIdx->lowerarm_l = NameIdx;
            else if (BoneName == "upperarm_l") NewBoneIdx->upperarm_l = NameIdx;
            else if (BoneName == "clavicle_l") NewBoneIdx->clavicle_l = NameIdx;
            else if (BoneName == "hand_r") NewBoneIdx->hand_r = NameIdx;
            else if (BoneName == "lowerarm_r") NewBoneIdx->lowerarm_r = NameIdx;
            else if (BoneName == "upperarm_r") NewBoneIdx->upperarm_r = NameIdx;
            else if (BoneName == "clavicle_r") NewBoneIdx->clavicle_r = NameIdx;
            else if (BoneName == "ball_l") NewBoneIdx->ball_l = NameIdx;
            else if (BoneName == "foot_l") NewBoneIdx->foot_l = NameIdx;
            else if (BoneName == "calf_l") NewBoneIdx->calf_l = NameIdx;
            else if (BoneName == "thigh_l") NewBoneIdx->thigh_l = NameIdx;
            else if (BoneName == "ball_r") NewBoneIdx->ball_r = NameIdx;
            else if (BoneName == "foot_r") NewBoneIdx->foot_r = NameIdx;
            else if (BoneName == "calf_r") NewBoneIdx->calf_r = NameIdx;
            else if (BoneName == "thigh_r") NewBoneIdx->thigh_r = NameIdx;
        }

        BoneIndex* result = NewBoneIdx.get();
        BoneIdxCache.emplace(Num, std::move(NewBoneIdx));
        return result;
    }

    bool GetBoxCoords(ASolarPlayerController *MyController, FVector* Coords)
    {
        BoneIndex* BoneIdx = GetBoneIndex();

        FVector BoneList[] = {
            GetBoneLocation(BoneIdx->head),
            GetBoneLocation(BoneIdx->neck_01),
            GetBoneLocation(BoneIdx->spine_03),
            GetBoneLocation(BoneIdx->spine_02),
            GetBoneLocation(BoneIdx->spine_01),
            GetBoneLocation(BoneIdx->pelvis),
            GetBoneLocation(BoneIdx->hand_l),
            GetBoneLocation(BoneIdx->lowerarm_l),
            GetBoneLocation(BoneIdx->upperarm_l),
            GetBoneLocation(BoneIdx->clavicle_l),
            GetBoneLocation(BoneIdx->hand_r),
            GetBoneLocation(BoneIdx->lowerarm_r),
            GetBoneLocation(BoneIdx->upperarm_r),
            GetBoneLocation(BoneIdx->clavicle_r),
            GetBoneLocation(BoneIdx->ball_l),
            GetBoneLocation(BoneIdx->foot_l),
            GetBoneLocation(BoneIdx->calf_l),
            GetBoneLocation(BoneIdx->thigh_l),
            GetBoneLocation(BoneIdx->ball_r),
            GetBoneLocation(BoneIdx->foot_r),
            GetBoneLocation(BoneIdx->calf_r),
            GetBoneLocation(BoneIdx->thigh_r)
        };

        float MinX = FLT_MAX, MinY = FLT_MAX;
        float MaxX = -FLT_MAX, MaxY = -FLT_MAX;

        bool Found = false;

        for (int i = 0; i < sizeof(BoneList) / sizeof(FVector); i++)
        {
            FVector Screen;
            if (!MyController->ProjectWorldLocationToScreen(BoneList[i], Screen))
                continue;

            Found = true;

            if (Screen.X < MinX) MinX = Screen.X;
            if (Screen.X > MaxX) MaxX = Screen.X;
            if (Screen.Y < MinY) MinY = Screen.Y;
            if (Screen.Y > MaxY) MaxY = Screen.Y;
        }

        if (!Found)
            return false;

        float PaddingX = (MaxX - MinX) * 0.1f;
        float PaddingY = (MaxY - MinY) * 0.1f;

        Coords[0] = FVector(MinX - PaddingX, MinY - PaddingY, 0);  // 左上角
        Coords[1] = FVector(MaxX + PaddingX, MaxY + PaddingY, 0);  // 右下角

        return true;
    }
};

class USkeletalMeshComponent : public USkinnedMeshComponent
{
public:
};

class UWorld : public UObject
{
public:
};

class UFont : public UObject
{
public:
    void SetFontSize(int32_t Size)
    {
        *reinterpret_cast<int32_t *>((uintptr_t)this + Offsets::LegacyFontSize) = Size;
    }

    int32_t GetFontSize()
    {
        return *reinterpret_cast<int32_t *>((uintptr_t)this + Offsets::LegacyFontSize);
    }
};

class UEngine : public UObject
{
public:
    UFont* GetTinyFont()
    {
        return *reinterpret_cast<UFont **>((uintptr_t)this + Offsets::TinyFont);
    }
};

class UCanvas : public UObject
{
public:
    FVector2D GetSize()
    {
        return FVector2D(*(int*)((uintptr_t)this + Offsets::SizeX), *(int*)((uintptr_t)this + Offsets::SizeY));
    }

	// Object: Function Engine.Canvas.K2_DrawText
	// Flags: [Final|Native|Public|HasDefaults|BlueprintCallable]
	void K2_DrawText(UFont* RenderFont, FString RenderText, FVector2D ScreenPosition, FVector2D Scale, FLinearColor RenderColor, float Kerning, FLinearColor ShadowColor, FVector2D ShadowOffset, bool bCentreX, bool bCentreY, bool bOutlined, FLinearColor OutlineColor)
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.Canvas:K2_DrawText");

        struct
        {
            UFont* RenderFont;
            FString RenderText;
            FVector2D ScreenPosition;
            FVector2D Scale;
            FLinearColor RenderColor;
            float Kerning;
            FLinearColor ShadowColor;
            FVector2D ShadowOffset;
            bool bCentreX;
            bool bCentreY;
            bool bOutlined;
            FLinearColor OutlineColor;
        } Params = {RenderFont, RenderText, ScreenPosition, Scale, RenderColor, Kerning, ShadowColor, ShadowOffset, bCentreX, bCentreY, bOutlined, OutlineColor};

        ProcessEvent(Func, &Params);
    }

	// Object: Function Engine.Canvas.K2_DrawLine
	// Flags: [Final|Native|Public|HasDefaults|BlueprintCallable]
	void K2_DrawLine(FVector2D ScreenPositionA, FVector2D ScreenPositionB, float Thickness, FLinearColor RenderColor)
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.Canvas:K2_DrawLine");

        struct
        {
            FVector2D ScreenPositionA;
            FVector2D ScreenPositionB;
            float Thickness;
            FLinearColor RenderColor;
        } Params = {ScreenPositionA, ScreenPositionB, Thickness, RenderColor};

        ProcessEvent(Func, &Params);
    }

    void K2_DrawText(UFont* RenderFont, int RenderFontSize, FString RenderText, FVector2D ScreenPosition, FLinearColor RenderColor, bool bCentreX, bool bCentreY, bool bOutlined, FLinearColor OutlineColor)
    {
        int OrigFontSize = RenderFont->GetFontSize();
        RenderFont->SetFontSize(RenderFontSize);
        K2_DrawText(RenderFont, RenderText, ScreenPosition, FVector2D(1.0f, 1.0f), RenderColor, 1.0f, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), FVector2D(0.0f, 0.0f), bCentreX, bCentreY, bOutlined, OutlineColor);
        RenderFont->SetFontSize(OrigFontSize);
    }
};

class UBlueprintFunctionLibrary : public UObject
{
public:
};

class UKismetStringLibrary : public UBlueprintFunctionLibrary
{
public:
	// Object: Function Engine.KismetStringLibrary.Conv_StringToName
	// Flags: [Final|Native|Static|Public|BlueprintCallable|BlueprintPure]
	static FName Conv_StringToName(FString InString)
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.KismetStringLibrary:Conv_StringToName");

        struct
        {
            FString InString;
            FName ReturnValue;
        } Params = {InString};

        GetDefaultObj()->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

	// Object: Function Engine.KismetStringLibrary.Conv_ObjectToString
	// Flags: [Final|Native|Static|Public|BlueprintCallable|BlueprintPure]
	static FString Conv_ObjectToString(UObject* InObj)
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.KismetStringLibrary:Conv_ObjectToString");

        struct
        {
            UObject* InObj;
            FString ReturnValue;
        } Params = {InObj};

        GetDefaultObj()->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

	// Object: Function Engine.KismetStringLibrary.Conv_NameToString
	// Flags: [Final|Native|Static|Public|BlueprintCallable|BlueprintPure]
	static FString Conv_NameToString(FName InName)
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.KismetStringLibrary:Conv_NameToString");

        struct
        {
            FName InName;
            FString ReturnValue;
        } Params = {InName};

        GetDefaultObj()->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    static UKismetStringLibrary *GetDefaultObj()
    {
        static UObject *Obj = nullptr;
        if (!Obj)
            Obj = UObject::StaticFindObject(u"/Script/Engine.Default__KismetStringLibrary");
        return reinterpret_cast<UKismetStringLibrary *>(Obj);
    }
};

class UKismetSystemLibrary : public UBlueprintFunctionLibrary
{
public:
	// Object: Function Engine.KismetSystemLibrary.GetFrameCount
	// Flags: [Final|Native|Static|Public|BlueprintCallable|BlueprintPure]
	static int64_t GetFrameCount()
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.KismetSystemLibrary:GetFrameCount");

        struct
        {
            int64_t ReturnValue;
        } Params;

        GetDefaultObj()->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

	// Object: Function Engine.KismetSystemLibrary.GetEngineVersion
	// Flags: [Final|Native|Static|Public|BlueprintCallable|BlueprintPure]
	static FString GetEngineVersion()
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.KismetSystemLibrary:GetEngineVersion");

        struct
        {
            FString ReturnValue;
        } Params;

        GetDefaultObj()->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    // Function  /Script/Engine.KismetSystemLibrary.GetDisplayName
	// Flags: [Final|Native|Static|Public|BlueprintCallable|BlueprintPure]
    static FString GetDisplayName(UObject *Object)
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.KismetSystemLibrary:GetDisplayName");

        struct
        {
            UObject *Object;
            FString ReturnValue;
        } Params = {Object};

        GetDefaultObj()->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    static UKismetSystemLibrary *GetDefaultObj()
    {
        static UObject *Obj = nullptr;
        if (!Obj)
            Obj = UObject::StaticFindObject(u"/Script/Engine.Default__KismetSystemLibrary");
        return reinterpret_cast<UKismetSystemLibrary *>(Obj);
    }
};

class UGameplayStatics : public UBlueprintFunctionLibrary
{
public:
    static APlayerController *GetPlayerController(UObject *WorldContextObject, int32_t PlayerIndex)
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.GameplayStatics:GetPlayerController");

        struct
        {
            UObject *WorldContextObject;
            int32_t PlayerIndex;
            APlayerController *ReturnValue;
        } Params = {WorldContextObject, PlayerIndex};

        GetDefaultObj()->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

	// Object: Function Engine.GameplayStatics.GetPlayerCameraManager
	// Flags: [Final|Native|Static|Public|BlueprintCallable|BlueprintPure]
	static APlayerCameraManager* GetPlayerCameraManager(UObject* WorldContextObject, int32_t PlayerIndex)
    {
        static UObject* Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.GameplayStatics:GetPlayerCameraManager");

        struct
        {
            UObject* WorldContextObject;
            int32_t PlayerIndex;
            APlayerCameraManager* ReturnValue;
        } Params = {WorldContextObject, PlayerIndex, nullptr};

        GetDefaultObj()->ProcessEvent(Func, &Params);

        return Params.ReturnValue;
    }

    static void GetAllActorsOfClass(UObject *WorldContextObject, UObject *ActorClass, TArray<AActor *> &OutActors)
    {
        static UObject *Func = nullptr;
        if (!Func)
            Func = UObject::StaticFindObject(u"/Script/Engine.GameplayStatics:GetAllActorsOfClass");

        struct
        {
            UObject *WorldContextObject;
            UObject *ActorClass;
            TArray<AActor *> OutActors;
        } Params = {WorldContextObject, ActorClass, {}};

        GetDefaultObj()->ProcessEvent(Func, &Params);

        OutActors = Params.OutActors;
    }

    static UGameplayStatics *GetDefaultObj()
    {
        static UObject *Obj = nullptr;
        if (!Obj)
            Obj = UObject::StaticFindObject(u"/Script/Engine.Default__GameplayStatics");
        return reinterpret_cast<UGameplayStatics *>(Obj);
    }
};


#endif // CLASSES_H