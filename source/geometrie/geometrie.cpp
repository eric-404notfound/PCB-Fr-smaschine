#include "geometry.h"

// -------- Vec2 Implementierung --------
Vec2::Vec2(float _x, float _y) : x(_x), y(_y) {}

Vec2 Vec2::operator+(const Vec2& v) const {
    return Vec2(x + v.x, y + v.y);
}

Vec2 Vec2::operator-(const Vec2& v) const {
    return Vec2(x - v.x, y - v.y);
}

Vec2 Vec2::operator*(float s) const {
    return Vec2(x * s, y * s);
}

float Vec2::length() const {
    return std::sqrt(x*x + y*y);
}

Vec2 Vec2::normalized() const {
    float len = length();
    return len > 0 ? *this * (1.0f / len) : Vec2();
}

// -------- 2D Line Implementierung --------
Line2D::Line2D(const Vec2& s, const Vec2& e) : start(s), end(e) {}

float Line2D::length() const {
    return (end - start).length();
}

// Berechnet den Punkt auf der Linie basierend auf `t` (0 ≤ t ≤ 1)
Vec2 Line2D::pointAt(float t) const {
    return start + (end - start) * t;
}

// Berechnet den Schnittpunkt zweier Linien
std::optional<Vec2> Line2D::intersect(const Line& other) const {
    float x1 = start.x, y1 = start.y;
    float x2 = end.x, y2 = end.y;
    float x3 = other.start.x, y3 = other.start.y;
    float x4 = other.end.x, y4 = other.end.y;

    float denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (denom == 0) return std::nullopt; // Parallel oder identisch

    float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
    float u = ((x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2)) / denom;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        return Vec2(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
    }
    return std::nullopt; // Kein Schnittpunkt innerhalb der Liniensegmente
}

// -------- Circle Implementierung --------
Circle::Circle(const Vec2& c, float r) : center(c), radius(r) {}

bool Circle::contains(const Vec2& point) const {
    return (point - center).length() <= radius;
}

// Berechnet den Punkt auf dem Kreis für den gegebenen Winkel in Grad
Vec2 Circle::pointAt(float angle) const {
    float rad = angle * (M_PI / 180.0);
    return Vec2(center.x + radius * cos(rad), center.y + radius * sin(rad));
}

// -------- Velocity Implementierung --------
Velocity::Velocity(const Vec2& dir, float spd) : direction(dir.normalized()), speed(spd) {}

Vec2 Velocity::apply(float deltaTime) const {
    return direction * (speed * deltaTime);
}

// -------- Vec3 Implementierung --------
Vec3::Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

Vec3 Vec3::operator+(const Vec3& v) const {
    return Vec3(x + v.x, y + v.y, z + v.z);
}

Vec3 Vec3::operator-(const Vec3& v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
}

Vec3 Vec3::operator*(float s) const {
    return Vec3(x * s, y * s, z * s);
}

float Vec3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::normalized() const {
    float len = length();
    return (len > 0) ? Vec3(x / len, y / len, z / len) : Vec3();
}

float Vec3::dot(const Vec3& v) const {
    return x * v.x + y * v.y + z * v.z;
}

Vec3 Vec3::cross(const Vec3& v) const {
    return Vec3(
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x
    );
}

// -------- Line3D Implementierung --------
Line::Line(const Vec3& s, const Vec3& e) : start(s), end(e) {}

float Line::length() const {
    return (end - start).length();
}

Vec3 Line::pointAt(float t) const {
    return start + (end - start) * t;
}

// Schnittpunkt zweier Linien im 3D-Raum (nur für ko-planare Linien exakt bestimmbar)
std::optional<Vec3> Line::intersect(const Line& other) const {
    Vec3 d1 = end - start;
    Vec3 d2 = other.end - other.start;
    Vec3 r = other.start - start;

    Vec3 d1xd2 = d1.cross(d2);
    float denominator = d1xd2.length();

    if (denominator < 1e-6) {
        return std::nullopt; // Linien sind parallel oder identisch
    }

    float t = r.cross(d2).dot(d1xd2) / denominator;
    float u = r.cross(d1).dot(d1xd2) / denominator;

    if (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f) {
        return start + d1 * t;
    }

    return std::nullopt;
}


