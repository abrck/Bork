#ifndef ENGINE_H
#define ENGINE_H

#include "classes.h"

namespace GameData
{
    inline UEngine *GEngine = 0;
    inline uintptr_t GameViewport = 0;

    inline UWorld* World = nullptr;

    inline ASolarPlayerController *MyController = nullptr;
    inline ASolarCharacter *MyPawn = nullptr;
    inline APlayerCameraManager *MyCameraManager = nullptr;

} // namespace GameData

#endif // ENGINE_H