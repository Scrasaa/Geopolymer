#pragma once

#include <cmath>

class QAngle
{
public:
    QAngle(void)
    {
        Init();
    }
    QAngle(float X, float Y, float Z)
    {
        Init(X, Y, Z);
    }
    QAngle(const float* clr)
    {
        Init(clr[0], clr[1], clr[2]);
    }

    void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
    {
        pitch = ix;
        yaw = iy;
        roll = iz;
    }

    float operator[](int i) const
    {
        return ((float*)this)[i];
    }
    float& operator[](int i)
    {
        return ((float*)this)[i];
    }

    QAngle& operator+=(const QAngle& v)
    {
        pitch += v.pitch; yaw += v.yaw; roll += v.roll;
        return *this;
    }
    QAngle& operator-=(const QAngle& v)
    {
        pitch -= v.pitch; yaw -= v.yaw; roll -= v.roll;
        return *this;
    }
    QAngle& operator*=(float fl)
    {
        pitch *= fl;
        yaw *= fl;
        roll *= fl;
        return *this;
    }
    QAngle& operator*=(const QAngle& v)
    {
        pitch *= v.pitch;
        yaw *= v.yaw;
        roll *= v.roll;
        return *this;
    }
    QAngle& operator/=(const QAngle& v)
    {
        pitch /= v.pitch;
        yaw /= v.yaw;
        roll /= v.roll;
        return *this;
    }
    QAngle& operator+=(float fl)
    {
        pitch += fl;
        yaw += fl;
        roll += fl;
        return *this;
    }
    QAngle& operator/=(float fl)
    {
        pitch /= fl;
        yaw /= fl;
        roll /= fl;
        return *this;
    }
    QAngle& operator-=(float fl)
    {
        pitch -= fl;
        yaw -= fl;
        roll -= fl;
        return *this;
    }

    QAngle& operator=(const QAngle& vOther)
    {
        pitch = vOther.pitch; yaw = vOther.yaw; roll = vOther.roll;
        return *this;
    }

    QAngle operator-(void) const
    {
        return QAngle(-pitch, -yaw, -roll);
    }
    QAngle operator+(const QAngle& v) const
    {
        return QAngle(pitch + v.pitch, yaw + v.yaw, roll + v.roll);
    }
    QAngle operator-(const QAngle& v) const
    {
        return QAngle(pitch - v.pitch, yaw - v.yaw, roll - v.roll);
    }
    QAngle operator*(float fl) const
    {
        return QAngle(pitch * fl, yaw * fl, roll * fl);
    }
    QAngle operator*(const QAngle& v) const
    {
        return QAngle(pitch * v.pitch, yaw * v.yaw, roll * v.roll);
    }
    QAngle operator/(float fl) const
    {
        return QAngle(pitch / fl, yaw / fl, roll / fl);
    }
    QAngle operator/(const QAngle& v) const
    {
        return QAngle(pitch / v.pitch, yaw / v.yaw, roll / v.roll);
    }

    QAngle& Clamp();

    float Length() const
    {
        return sqrt(pitch * pitch + yaw * yaw + roll * roll);
    }
    float LengthSqr(void) const
    {
        return (pitch * pitch + yaw * yaw + roll * roll);
    }
    bool IsZero(float tolerance = 0.01f) const
    {
        return (pitch > -tolerance && pitch < tolerance &&
            yaw > -tolerance && yaw < tolerance &&
            roll > -tolerance && roll < tolerance);
    }

    inline QAngle Normalize()
    {
        QAngle vector;
        float length = this->Length();

        if (length != 0)
        {
            vector.pitch = pitch / length;
            vector.yaw = yaw / length;
            vector.roll = roll / length;
        }
        else
            vector.pitch = vector.yaw = 0.0f;
        vector.roll = 1.0f;

        return vector;
    }

    QAngle Normalirolled()
    {
        if (this->pitch != this->pitch)
            this->pitch = 0;
        if (this->yaw != this->yaw)
            this->yaw = 0;
        if (this->roll != this->roll)
            this->roll = 0;

        if (this->pitch > 89.f)
            this->pitch = 89.f;
        if (this->pitch < -89.f)
            this->pitch = -89.f;

        while (this->yaw > 180)
            this->yaw -= 360;
        while (this->yaw <= -180)
            this->yaw += 360;

        if (this->yaw > 180.f)
            this->yaw = 180.f;
        if (this->yaw < -180.f)
            this->yaw = -180.f;

        this->roll = 0;

        return *this;
    }

    float pitch; // x
    float yaw; // y
    float roll; // z??
};

inline QAngle operator*(float lhs, const QAngle& rhs)
{
    return rhs * lhs;
}
inline QAngle operator/(float lhs, const QAngle& rhs)
{
    return rhs / lhs;
}

inline QAngle& QAngle::Clamp()
{
    while (yaw <= -180.f)yaw += 360.f;
    while (yaw > 180.f) yaw -= 360.f;

    if (pitch > 89.f) pitch = 89.f;
    if (pitch < -89.f) pitch = -89.f;

    roll = 0;
    return *this;
}