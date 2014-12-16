#include "mesh.hpp"
#include <iostream>
#include <float.h>
#include <algorithm>

Mesh::Mesh(const std::vector<Point3D>& verts,
           const std::vector< std::vector<int> >& faces)
  : m_verts(verts),
    m_faces(faces)
{

  
  Point3D min_vert = Point3D(DBL_MAX, DBL_MAX, DBL_MAX);
  Point3D max_vert = Point3D(-DBL_MAX, -DBL_MAX, -DBL_MAX);
  for (unsigned int i = 0; i < verts.size(); i++) {
    Point3D vert = verts[i];
    for (int j = 0; j < 3; j++) {
      min_vert[j] = std::min(min_vert[j], vert[j]);
      max_vert[j] = std::max(max_vert[j], vert[j]);
    }
  }

  Vector3D bound = max_vert - min_vert;
  Point3D bound_center = min_vert + 0.5 * bound;
  double bound_rad = bound.length()/2.0;

  bound_sphere = new NonhierSphere(bound_center, bound_rad);
}

void Mesh::vertsFromTri(Face face, Point3D &A, Point3D &B, Point3D &C) {
  A = m_verts[face[0]];
  B = m_verts[face[1]];
  C = m_verts[face[2]];
}

Vector3D Mesh::getNormal(Face face) {
  Point3D A, B, C;
  vertsFromTri(face, A, B, C);
  return (B - A).cross(C - A);
}

Intersection Mesh::intersectTri(Ray ray, Face face) {
  Point3D A, B, C;
  vertsFromTri(face, A, B, C);
  Vector3D norm = getNormal(face);
  bool parallel;
  double t = plane_line_intersection(A, ray, norm, parallel);
  Point3D point = ray.p + t * ray.d;
  Intersection result;
  result.total_intersections = 0;
  if (parallel) {
    return result;
  }

  if (pointInTri(point, face)) {
    result.t1 = t;
    result.p1 = point;
    result.total_intersections = 1;
    result.normal = norm;
  }

  return result;
}
void Mesh::splitPolyFace(Face& polyFace, std::vector<Face>& tri_faces) {
  if (polyFace.size() == 3) {
    Point3D A, B, C;
      Face face;
      face.push_back(polyFace[0]);
      face.push_back(polyFace[1]);
      face.push_back(polyFace[2]);
      tri_faces.push_back(face);
  } else {
    for (unsigned int i = 0; i < polyFace.size() - 2; i++) {
      Face face;
      face.push_back(polyFace[0]);
      face.push_back(polyFace[i + 1]);
      face.push_back(polyFace[i + 2]);
      tri_faces.push_back(face);
    }
  }
}

Intersection Mesh::intersect(Ray scene_ray, bool aggressive) {
  Intersection result;
  result.total_intersections = 0;
  if (bound_sphere->intersect(scene_ray, aggressive).total_intersections != 1) {
    return result;
  }
  if (SHOW_BOUND_VOLUME) {
    return bound_sphere->intersect(scene_ray, aggressive);
  }
  for (unsigned int i = 0; i < m_faces.size(); i++) {
    std::vector<Face> tri_faces;
    splitPolyFace(m_faces[i], tri_faces);
    for (unsigned int j = 0; j < tri_faces.size(); j++) {
      Face face = tri_faces[j];
      Intersection inter = intersectTri(scene_ray, face);
      if (inter.total_intersections != 0 && inter.t1 > 0.0) {
        // we found an intersection
        if (result.total_intersections == 0 || inter.t1 < result.t1) {
          result.total_intersections = inter.total_intersections;
          result.p1 = inter.p1;
          result.t1 = inter.t1;
          result.normal = inter.normal;
        } 
      }
    }
  }
  return result;
}

bool Mesh::pointInTri(Point3D p, Face face) {
  Vector3D normal = getNormal(face);
  Point3D A, B, C;
  vertsFromTri(face, A, B, C);

  // first check if it's even on the plan
  double planar_check = normal.dot(p - A);
  if (planar_check < -Ee || planar_check > Ee) {
    return false;
  }

  // If it is sufficiently on the plane, check the area.
  double t_area = 0.5 * normal.length();
  
  // A -> B
  Vector3D AB = B - A;
  // B -> C
  Vector3D BC = C - B;
  // C -> A
  Vector3D CA = A - C;

  // A -> p
  Vector3D Ap = p - A;

  // B -> p 
  Vector3D Bp = p - B;

  // C -> p
  Vector3D Cp = p - C;
  double new_area = 0.5 * (
    AB.cross(Ap).length() 
    + BC.cross(Bp).length()
    + CA.cross(Cp).length());
  // if the point is outside, the triangle area should be
  // larger. 
  if (new_area == t_area) {
    return true;
  }

  if (new_area > t_area && new_area - t_area > 3*Ee) 
  {
    return false;
  }

  if (new_area < t_area) {
    return true;
  }

  return true;
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  std::cerr << "mesh({";
  for (std::vector<Point3D>::const_iterator I = mesh.m_verts.begin(); I != mesh.m_verts.end(); ++I) {
    if (I != mesh.m_verts.begin()) std::cerr << ",\n      ";
    std::cerr << *I;
  }
  std::cerr << "},\n\n     {";
  
  for (std::vector<Mesh::Face>::const_iterator I = mesh.m_faces.begin(); I != mesh.m_faces.end(); ++I) {
    if (I != mesh.m_faces.begin()) std::cerr << ",\n      ";
    std::cerr << "[";
    for (Mesh::Face::const_iterator J = I->begin(); J != I->end(); ++J) {
      if (J != I->begin()) std::cerr << ", ";
      std::cerr << *J;
    }
    std::cerr << "]";
  }
  std::cerr << "});" << std::endl;
  return out;
}
