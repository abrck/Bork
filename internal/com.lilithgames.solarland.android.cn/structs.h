#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <cmath>
#include <cstdint>
#include <unistd.h>

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

    static FString FromAnsi(const char *str)
    {
        FString result;
        if (!str)
            return result;

        int len = 0;
        while (str[len])
            len++;

        result.Data.Data = new char16_t[len + 1];
        result.Data.Count = len + 1;
        result.Data.Max = len + 1;

        for (int i = 0; i < len; i++)
        {
            result.Data.Data[i] = static_cast<char16_t>(str[i]);
        }
        result.Data.Data[len] = u'\0';

        return result;
    }

    inline const char* ToString() const
    {
        if (Data.Data == nullptr || Data.Count == 0)
            return {};

        int len = Data.Count;
        if (len > 0 && Data.Data[len - 1] == u'\0')
            --len;

        std::string out;
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

#endif // STRUCTS_H