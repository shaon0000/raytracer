#ifndef CS488_LIGHT_HPP
#define CS488_LIGHT_HPP

#include "algebra.hpp"
#include <iosfwd>

// Represents a simple point light.
struct Light {
  Light();
  
  Colour colour;
  Point3D position;
  double falloff[3];
  Vector3D v1;
  Vector3D v2;
  Vector3D n;
  void* associate;
  double cos_theta_limit_squared;
};

std::ostream& operator<<(std::ostream& out, const Light& l);

#endif
