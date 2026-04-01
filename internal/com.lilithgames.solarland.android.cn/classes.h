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
};

class AController : public AActor
{
public:
};

class APlayerController : public AController
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
};

class ASolarCharacter : public ASolarCharacterBase
{
public:
    static UObject *StaticClass()
    {
        static UObject *Clazz = nullptr;
        if (!Clazz)
            Clazz = StaticFindObject(u"/Script/Solarland.SolarCharacter");
        return Clazz;
    }
};

class UWorld : public UObject
{
public:
};

class UFont : public UObject
{
public:
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
        return FVector2D(*(float*)((uintptr_t)this + Offsets::SizeX), *(float*)((uintptr_t)this + Offsets::SizeY));
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

};

class UBlueprintFunctionLibrary : public UObject
{
public:
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