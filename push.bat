@echo off

adb push "build\internal\com.lilithgames.solarland.android.cn\libSolarland.so" /data/local/tmp/libSolarland.so
adb shell chmod 755 /data/local/tmp/libSolarland.so
