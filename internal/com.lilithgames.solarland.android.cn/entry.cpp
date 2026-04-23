#include "hooks.h"
#include "And64InlineHook.hpp"

void __attribute__((constructor)) init()
{
    std::thread([]()
    {
        while (Globals::libUE4 <= 0)
        {
            Globals::libUE4 = (uintptr_t)FindModuleBase("libUE4.so");
            if (IsBadPtr(Globals::libUE4))
                break;
            usleep(500);
        }
        LOGI("libUE4.so 基址: 0x%lx", Globals::libUE4);

        while ((uintptr_t)GameData::GEngine <= 0)
        {
            GameData::GEngine = *(UEngine **)(Globals::libUE4 + Offsets::GEngine);
            if (IsBadPtr(GameData::GEngine))
                break;
            usleep(500);
        }
        LOGI("GEngine: 0x%lx", GameData::GEngine);

        while (GameData::GameViewport <= 0)
        {
            GameData::GameViewport = *(uintptr_t*)((uintptr_t)GameData::GEngine + Offsets::GameViewport);
            if (IsBadPtr(GameData::GameViewport))
                break;
            usleep(500);
        }
        LOGI("GameViewport: 0x%lx", GameData::GameViewport);

        JumpVirtualHook(*(void***)GameData::GameViewport, Offsets::DrawTransitionIdx, (void*)hkDrawTransition, (void**)&oDrawTransition);


        // 寻找方法
        // void GetAimStartEnd(struct UAmmoConfig* InAmmo, struct FVector& OutStart, struct FVector& OutEnd, struct AActor*& OutTargetActor, bool bNeedSpread);
        // sdk找到他的虚函数地址，ida 过去，找到他的实地址
        // 进入 GetAimStartEnd 函数，看参数 a3(OutStart),a4(OutEnd), 找到同时包含这两个参数的函数
        // 进入找到的这个函数, 看他的 return 函数, 交叉引用一下，第一个就是

        A64HookFunction((void*)(Globals::libUE4 + 0x74D98C4), (void*)hkSub74D98C4, (void**)&oSub74D98C4);

        LOGI("注射成功, 当前版本: %s", UKismetSystemLibrary::GetEngineVersion().ToString());
    }).detach();
}