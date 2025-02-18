#pragma once

#include <cmath>
#include <iostream>
#include <optional>

// -------- 2D Vektor Klasse --------
class Vec2 {
public:
    float x, y;

    Vec2(float _x = 0, float _y = 0);
    Vec2 operator+(const Vec2& v) const;
    Vec2 operator-(const Vec2& v) const;
    Vec2 operator*(float s) const;
    float length() const;
    Vec2 normalized() const;
};

// -------- Linie (zwischen zwei Punkten) --------
class Line {
public:
    Vec2 start, end;

    Line(const Vec2& s, const Vec2& e);
    float length() const;
    Vec2 pointAt(float t) const; // Berechnet Punkt auf der Linie (0 ≤ t ≤ 1)
    std::optional<Vec2> intersect(const Line& other) const; // Berechnet Schnittpunkt
};

// -------- Kreis --------
class Circle {
public:
    Vec2 center;
    float radius;

    Circle(const Vec2& c, float r);
    bool contains(const Vec2& point) const;
    Vec2 pointAt(float angle) const; // Berechnet Punkt auf dem Kreis mit Winkel (Grad)
};

// -------- Geschwindigkeit (2D mit Richtung) --------
class Velocity {
public:
    Vec2 direction;
    float speed;

    Velocity(const Vec2& dir, float spd);
    Vec2 apply(float deltaTime) const;
};
