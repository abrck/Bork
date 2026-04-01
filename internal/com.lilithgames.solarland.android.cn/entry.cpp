#include "hooks.h"

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

        LOGI("注射成功, 当前版本: %s", UKismetSystemLibrary::GetEngineVersion().ToString());
    }).detach();
}