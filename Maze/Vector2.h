#pragma once
class Vector2
{
public:

    static const Vector2 UP;
    static const Vector2 RIGHT;
    static const Vector2 DOWN;
    static const Vector2 LEFT;

    int x;
    int y;

    Vector2();

    Vector2(int x, int y);

    Vector2(Vector2* other);

    Vector2 operator*(int v);

    Vector2 operator+(Vector2 other);

    Vector2 operator-(Vector2 other);

    void reverse();

    void operator+=(Vector2 other);

    void operator-=(Vector2 other);

    bool operator==(Vector2 other);
};