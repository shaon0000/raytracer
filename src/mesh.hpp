#ifndef CS488_MESH_HPP
#define CS488_MESH_HPP

#include <vector>
#include <iosfwd>
#include "primitive.hpp"
#include "algebra.hpp"
#include "primitive.hpp"

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh(const std::vector<Point3D>& verts,
       const std::vector< std::vector<int> >& faces);

  typedef std::vector<int> Face;
	Vector3D getNormal(Face face);
	Intersection intersectTri(Ray ray, Face face);
	bool pointInTri(Point3D point, Face face);
	void vertsFromTri(Face face, Point3D &A, Point3D &B, Point3D &C);
	
	bool pointInFace(Point3D point, Face face);
	virtual Intersection intersect(Ray scene_ray, bool aggressive);
	void splitPolyFace(Face &polyFace, std::vector<Face> &tri_faces);
private:
  std::vector<Point3D> m_verts;
  std::vector<Face> m_faces;
  NonhierSphere* bound_sphere;

  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};

#endif
