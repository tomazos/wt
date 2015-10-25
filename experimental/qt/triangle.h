#pragma once

struct Point {
  float32 x, y, z;
};

static_assert(sizeof(Point) == 12);

struct Triangle {
  Point position, color;
};

static_assert(sizeof(Triangle) == 24);
