#include "Vector2.h"

const Vector2 Vector2::UP = { 0, -1 };
const Vector2 Vector2::RIGHT = { 1, 0 };
const Vector2 Vector2::DOWN = { 0, 1 };
const Vector2 Vector2::LEFT = { -1, 0 };

Vector2::Vector2() : Vector2(0, 0) { }

Vector2::Vector2(int x, int y)
{
    this->x = x;
    this->y = y;
}

Vector2::Vector2(Vector2* other) : Vector2(other->x, other->y)
{
}

Vector2 Vector2::operator*(int v)
{
    return Vector2(x * v, y * v);
}

Vector2 Vector2::operator+(Vector2 other)
{
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(Vector2 other)
{
    return Vector2(x - other.x, y - other.y);
}

void Vector2::reverse()
{
    x *= -1;
    y *= -1;
}

void Vector2::operator+=(Vector2 other)
{
    x += other.x;
    y += other.y;
}

void Vector2::operator-=(Vector2 other)
{
    x -= other.x;
    y -= other.y;
}

bool Vector2::operator==(Vector2 other)
{
    return x == other.x && y == other.y;
}