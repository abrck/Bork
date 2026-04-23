#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <cmath>
#include <cstdint>
#include <unistd.h>
#include <cfloat>
#include <algorithm>
#include <vector>
#include <cstring>

template <typename T>
struct TArray
{
    T *Data;
    int32_t Count;
    int32_t Max;

    TArray() : Data(nullptr), Count(0), Max(0) {}

    T &operator[](int32_t i) { return Data[i]; }
    const T &operator[](int32_t i) const { return Data[i]; }
};

struct FString
{
    TArray<char16_t> Data;

    int32_t Num() const { return Data.Count; }
    int32_t Len() const { return Data.Count > 0 ? Data.Count - 1 : 0; }

    bool IsValid() const
    {
        return Data.Data != nullptr;
    }

    static FString FromAnsi(const char* str)
    {
        FString result;

        if (!str)
            return result;

        const unsigned char* s = (const unsigned char*)str;

        std::vector<char16_t> utf16;
        utf16.reserve(strlen(str) + 1); // 预留空间

        while (*s)
        {
            uint32_t codepoint = 0;

            if (*s <= 0x7F)
            {
                // 1 byte
                codepoint = *s++;
            }
            else if ((*s >> 5) == 0x6)
            {
                // 2 bytes
                codepoint = ((*s & 0x1F) << 6) |
                            (s[1] & 0x3F);
                s += 2;
            }
            else if ((*s >> 4) == 0xE)
            {
                // 3 bytes
                codepoint = ((*s & 0x0F) << 12) |
                            ((s[1] & 0x3F) << 6) |
                            (s[2] & 0x3F);
                s += 3;
            }
            else if ((*s >> 3) == 0x1E)
            {
                // 4 bytes
                codepoint = ((*s & 0x07) << 18) |
                            ((s[1] & 0x3F) << 12) |
                            ((s[2] & 0x3F) << 6) |
                            (s[3] & 0x3F);
                s += 4;
            }
            else
            {
                // 非法字符，跳过
                ++s;
                continue;
            }

            if (codepoint <= 0xFFFF)
            {
                utf16.push_back((char16_t)codepoint);
            }
            else
            {
                // surrogate pair
                codepoint -= 0x10000;
                utf16.push_back((char16_t)((codepoint >> 10) + 0xD800));
                utf16.push_back((char16_t)((codepoint & 0x3FF) + 0xDC00));
            }
        }

        utf16.push_back(u'\0');

        // 填充到 TArray
        result.Data.Count = (int32_t)utf16.size();
        result.Data.Data = new char16_t[result.Data.Count];
        memcpy(result.Data.Data, utf16.data(),
            result.Data.Count * sizeof(char16_t));

        return result;
    }

    inline const char* ToString() const
    {
        if (Data.Data == nullptr || Data.Count == 0)
            return {};

        int len = Data.Count;
        if (len > 0 && Data.Data[len - 1] == u'\0')
            --len;

        static std::string out;
        out.clear();
        out.reserve(len);

        for (int i = 0; i < len; ++i)
        {
            uint32_t c = Data.Data[i];
            if (c == 0)
                break;

            if (c >= 0xD800 && c <= 0xDBFF && i + 1 < len)
            {
                uint32_t low = Data.Data[i + 1];
                if (low >= 0xDC00 && low <= 0xDFFF)
                {
                    c = ((c - 0xD800) << 10) + (low - 0xDC00) + 0x10000;
                    ++i;
                }
            }

            if (c <= 0x7F)
            {
                out.push_back(static_cast<char>(c));
            }
            else if (c <= 0x7FF)
            {
                out.push_back(static_cast<char>(0xC0 | (c >> 6)));
                out.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
            else if (c <= 0xFFFF)
            {
                out.push_back(static_cast<char>(0xE0 | (c >> 12)));
                out.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
            else
            {
                out.push_back(static_cast<char>(0xF0 | (c >> 18)));
                out.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                out.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
        }

        return out.c_str();
    }
};

struct FVector2D
{
    float X, Y;

    FVector2D() : X(0), Y(0) {}
    FVector2D(float x, float y) : X(x), Y(y) {}

    FVector2D operator+(const FVector2D& v) const { return FVector2D(X + v.X, Y + v.Y); }
    FVector2D operator-(const FVector2D& v) const { return FVector2D(X - v.X, Y - v.Y); }
    FVector2D operator*(float s) const { return FVector2D(X * s, Y * s); }
    FVector2D operator/(float s) const { return FVector2D(X / s, Y / s); }

    float Size() const { return sqrtf(X * X + Y * Y); }
    float Distance(const FVector2D& v) const { return (*this - v).Size(); }
};

struct FVector
{
    float X, Y, Z;

    FVector() : X(0), Y(0), Z(0) {}
    FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}

    FVector operator+(const FVector& v) const { return FVector(X + v.X, Y + v.Y, Z + v.Z); }
    FVector operator-(const FVector& v) const { return FVector(X - v.X, Y - v.Y, Z - v.Z); }
    FVector operator*(float s) const { return FVector(X * s, Y * s, Z * s); }
    FVector operator/(float s) const { return FVector(X / s, Y / s, Z / s); }
    float Dot(const FVector& v) const { return X * v.X + Y * v.Y + Z * v.Z; }
    float Size() const { return sqrtf(X * X + Y * Y + Z * Z); }
    float SizeSquared() const { return X * X + Y * Y + Z * Z; }
    float Distance(const FVector& v) const { return (*this - v).Size(); }
    static float DistSquared(const FVector& V1, const FVector& V2) 
    {
        return (V2.X - V1.X) * (V2.X - V1.X) + 
               (V2.Y - V1.Y) * (V2.Y - V1.Y) + 
               (V2.Z - V1.Z) * (V2.Z - V1.Z);
    }

    FVector Normalize() const
    {
        float s = Size();
        return s > 0 ? *this / s : FVector();
    }
};

struct FRotator
{
    float Pitch, Yaw, Roll;

    FRotator() : Pitch(0), Yaw(0), Roll(0) {}
    FRotator(float p, float y, float r) : Pitch(p), Yaw(y), Roll(r) {}

    FRotator operator+(const FRotator &r) const { return FRotator(Pitch + r.Pitch, Yaw + r.Yaw, Roll + r.Roll); }
    FRotator operator-(const FRotator &r) const { return FRotator(Pitch - r.Pitch, Yaw - r.Yaw, Roll - r.Roll); }
    FRotator operator/(float s) const { return FRotator(Pitch / s, Yaw / s, Roll / s); }
    FRotator operator*(float s) const { return FRotator(Pitch * s, Yaw * s, Roll * s); }

    static float ClampAxis(float Angle)
    {
        Angle = fmodf(Angle, 360.0f);
        if (Angle < 0.0f)
            Angle += 360.0f;
        return Angle;
    }

    static float NormalizeAxis(float Angle)
    {
        Angle = ClampAxis(Angle);
        if (Angle > 180.0f)
            Angle -= 360.0f;
        return Angle;
    }

    FRotator Clamp() const
    {
        return FRotator(NormalizeAxis(Pitch), NormalizeAxis(Yaw), NormalizeAxis(Roll));
    }

    FVector ToVector() const
    {
        float cp = cosf(Pitch * 0.0174533f);
        float sp = sinf(Pitch * 0.0174533f);
        float cy = cosf(Yaw * 0.0174533f);
        float sy = sinf(Yaw * 0.0174533f);
        return FVector(cp * cy, cp * sy, -sp);
    }
};

struct FNamePool
{
    char pad[0x40];
    uint8_t* Blocks[8192];
};

struct FName
{
    int32_t ComparisonIndex;
    int32_t Number;

    FName() : ComparisonIndex(0), Number(0) {}
    FName(int32_t idx, int32_t num = 0) : ComparisonIndex(idx), Number(num) {}

    bool operator==(const FName &other) const { return ComparisonIndex == other.ComparisonIndex && Number == other.Number; }
    bool operator!=(const FName &other) const { return !(*this == other); }
};

struct FLinearColor
{
    float R, G, B, A;
    
    FLinearColor() : R(0), G(0), B(0), A(0) {}
    FLinearColor(float r, float g, float b, float a) : R(r), G(g), B(b), A(a) {}
};

struct BoneIndex
{
    int32_t NumBones = 0;
    int32_t head, neck_01, spine_03, spine_02, spine_01, pelvis, root;
    int32_t hand_l, lowerarm_l, upperarm_l, clavicle_l;
    int32_t hand_r, lowerarm_r, upperarm_r, clavicle_r;
    int32_t ball_l, foot_l, calf_l, thigh_l;
    int32_t ball_r, foot_r, calf_r, thigh_r;
};

struct FPlayerVirtualBulletSpawnParameter  // Size: 0xB4 (180 bytes)
{
    uint32_t FireUniqueID;              // 0x0
    uint8_t ShotGunFireUniqueID;        // 0x4
    uint8_t Pad_0x5[0x3];               // 0x5
    float FireTimeStamp;                // 0x8
    FVector AimStart;                   // 0xC
    FVector AimDir;                     // 0x18
    FVector CharacterLoc;               // 0x24
    FVector StartLoc;                   // 0x30
    FVector TargetLoc;                  // 0x3C
    uint8_t Pad_0x48[0xC];              // 0x48
    void* Target;                       // 0x54 (TWeakObjectPtr)
    uint8_t Pad_0x5C[0x4];              // 0x5C
    FVector HitTargetRelativeLocation;  // 0x5C
    uint8_t bHasForceAimIgnore;         // 0x68
    uint8_t bInputAdjustRecoil;         // 0x69
    bool bAutoLockTarget;               // 0x6A
    uint8_t Pad_0x6B[0x1];              // 0x6B
    FName AutoLockSocketName;           // 0x6C (FName = 8 bytes)
    uint8_t CurveGroupIndex;            // 0x74
    uint8_t CurveIndex;                 // 0x75
    int16_t CurveDegree;                // 0x76
    uint8_t bEnableCharge;              // 0x78
    uint8_t Pad_0x79[0x3];              // 0x79
    float ChargeTime;                   // 0x7C
    int32_t ChargeStage;                // 0x80
    uint8_t PreFireChargeLevel;         // 0x84
    uint8_t bSubShootPattern;           // 0x85
    uint8_t Pad_0x86[0x2];              // 0x86
    uint32_t FireFrame;                 // 0x88
    uint32_t FireInternalID;            // 0x8C
    uint8_t IsScopeOpen;                // 0x90
    uint8_t Pad_0x91[0x3];              // 0x91
    float HorizontalVar;                // 0x94
    float VerticalVar;                  // 0x98
    uint8_t Pad_0x9C[0x18];             // 0x9C
};

#endif // STRUCTS_H