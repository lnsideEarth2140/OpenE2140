//
// Created by Ion Agorria on 25/03/18
//
#include <iostream>
#include "vector2.h"

Vector2::Vector2(int x, int y) {
    this->x = x;
    this->y = y;
}

Vector2::Vector2(int v) {
    this->x = v;
    this->y = v;
}

Vector2::Vector2(const Vector2& vector2) {
    this->x = vector2.x;
    this->y = vector2.y;
}

Vector2::Vector2() {
    this->x = 0;
    this->y = 0;
}

bool Vector2::operator==(const Vector2& vector2) const {
    return x == vector2.x
        && y == vector2.y;
}

bool Vector2::operator!=(const Vector2& vector2) const {
    return x != vector2.x
        || y != vector2.y;
}

void Vector2::operator+=(Vector2 const &vector2) {
    this->x += vector2.x;
    this->y += vector2.y;
}

void Vector2::operator-=(Vector2 const &vector2) {
    this->x -= vector2.x;
    this->y -= vector2.y;
}

void Vector2::operator*=(Vector2 const &vector2) {
    this->x *= vector2.x;
    this->y *= vector2.y;
}

void Vector2::operator/=(Vector2 const &vector2) {
    this->x /= vector2.x;
    this->y /= vector2.y;
}

Vector2 Vector2::operator+(Vector2 const &vector2) const {
    return Vector2(
            this->x + vector2.x,
            this->y + vector2.y
    );
}

Vector2 Vector2::operator-(Vector2 const &vector2) const {
    return Vector2(
            this->x - vector2.x,
            this->y - vector2.y
    );
}

Vector2 Vector2::operator*(Vector2 const &vector2) const {
    return Vector2(
            this->x * vector2.x,
            this->y * vector2.y
    );
}

Vector2 Vector2::operator/(Vector2 const &vector2) const {
    return Vector2(
            this->x / vector2.x,
            this->y / vector2.y
    );
}

void Vector2::operator+=(int v) {
    this->x += v;
    this->y += v;
}

void Vector2::operator-=(int v) {
    this->x -= v;
    this->y -= v;
}

void Vector2::operator*=(int v) {
    this->x *= v;
    this->y *= v;
}

void Vector2::operator/=(int v) {
    this->x /= v;
    this->y /= v;
}

Vector2 Vector2::operator+(int v) const {
    return Vector2(
            this->x + v,
            this->y + v
    );
}

Vector2 Vector2::operator-(int v) const {
    return Vector2(
            this->x - v,
            this->y - v
    );
}

Vector2 Vector2::operator*(int v) const {
    return Vector2(
            this->x * v,
            this->y * v
    );
}

Vector2 Vector2::operator/(int v) const {
    return Vector2(
            this->x / v,
            this->y / v
    );
}

Vector2::operator bool() const {
    return 0 != x && 0 != y;
}

bool Vector2::zero() const {
    return 0 == x && 0 == y;
}

void Vector2::set(int v) {
    this->x = v;
    this->y = v;
}

void Vector2::set(int x, int y) {
    this->x = x;
    this->y = y;
}

void Vector2::set(const Vector2& vector2) {
    this->x = vector2.x;
    this->y = vector2.y;
}

std::string Vector2::toString() const {
    return "Vector2(" + toStringContent() + ")";
}

std::string Vector2::toStringContent() const {
    return std::to_string(x) + ", " + std::to_string(y);
}
