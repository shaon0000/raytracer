#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include "light.hpp"
#include "image.hpp"
#include <list>
#include <string>

class SceneNode;

class Primitive {
public:
  // Instead of one texture streteched into the whole primtive
  // we can control how many copies should be on the same (0, 1) space.

  virtual void textureScale(int x, int y) {
    texScaleX = x;
    texScaleY = y;
  }
  
  Colour shade(
    Point3D eye,
    Vector3D normal, 
    const std::list<Light*>& lights, 
    const Point3D& intersect_point, 
    const Colour& Kd, 
    const Colour& Ks, 
    double shine, const Colour& ambient, SceneNode* node, int depth, int maxDepth, double snell_N, double reflectivity, SceneNode* curNode);
  virtual ~Primitive();
  virtual Vector3D normal(Point3D point);
  virtual void get_uv(const Ray& intersection, double& u, double& v) {
    u = 0;
    v = 0;
  }
  virtual Intersection intersect(Ray scene_ray, bool aggressive);
  
  void setTexture(const std::string& filename) {
    textureScale(1, 1);
    img = Image();
    img.loadPng(filename);
    std::cerr << "setting texture map: " << filename << std::endl;
  }

  void setBump(const std::string& filename) {
    textureScale(1, 1);
    std::cerr << "setting bump map: " << filename << std::endl;
    bump_map = Image();
    bump_map.loadPng(filename);
  }

  virtual Colour getTexture(const Ray& inter) {
    return Colour(0.0, 0.0, 0.0);
  }

  virtual Vector3D getBumpNormal(const Ray& inter) {
    return Vector3D(0.0, 0.0, 0.0);
  }

  int texScaleX;
  int texScaleY;
  Image img;
  Image bump_map;
};



class Sphere : public Primitive {
public:
  virtual Vector3D normal(Point3D point);
  virtual ~Sphere();
  virtual Intersection intersect(Ray scene_ray, bool aggressive);
};

class Cube : public Primitive {
public:
  virtual Vector3D normal(Point3D point);
  virtual ~Cube();
    virtual Intersection intersect(Ray scene_ray, bool aggressive);
};

class NonhierSphere : public Primitive {
public:
      virtual Vector3D normal(Point3D point);
  NonhierSphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
    virtual Intersection intersect(Ray scene_ray, bool aggressive);
  virtual Colour getTexture(const Ray& inter);
  virtual Vector3D getBumpNormal(const Ray& inter);
private:
  Point3D m_pos;
  double m_radius;
};

class Cylinder : public Primitive {
public:
  Cylinder(const Ray& ray, double radius)
  : m_ray(ray), m_radius(radius)
  {
    m_t_max = m_ray.d.length();
    m_ray.d.normalize();

    caps[0].p = m_ray.p + m_t_max * m_ray.d;
    caps[0].d = m_ray.d;

    caps[1].p = m_ray.p;
    caps[1].d = -1 * m_ray.d;
  }
  virtual ~Cylinder();
  Colour getTexture(const Ray& intersection);
  Vector3D getBumpNormal(const Ray& intersection);
  virtual Intersection intersect(Ray scene_ray, bool aggressive);

private:
  Intersection check_face(Ray scene_ray, Ray face);
  Intersection intersect_face(Ray scene_ray);
  Ray m_ray;
  double m_radius;
  double m_t_max;
  Ray caps[2];
};

class Cone : public Primitive {
public:
  Cone(const Ray& ray) 
  : m_ray(ray) {
    m_t_max = m_ray.d.length();
    m_ray.d.normalize();

    cap.p = m_ray.p + m_t_max * m_ray.d;
    cap.d = m_ray.d;
  }
  virtual ~Cone();
  virtual Intersection intersect(Ray scene_ray, bool aggressive);
  virtual void get_uv(const Ray& intersection, double& u, double& v);
  Colour getTexture(const Ray& intersection);
  Vector3D getBumpNormal(const Ray& intersection);
private:
  Intersection intersect_face(Ray scene_ray);
  Intersection check_face(Ray scene_ray, Ray face);
  Ray m_ray;
  double m_t_max;
  Ray cap;
  double m_theta;
};

class Wall : public Primitive {
public:
  Wall() {
    m_face.p = Point3D(0, 0, 0);
    m_face.d = Vector3D(0, 0, 1);
    m_top = 0.5;
    m_bottom = -0.5;
    m_left = -0.5;
    m_right = 0.5;
  }
  virtual ~Wall();
  virtual Intersection intersect(Ray scene_ray, bool aggressive);
  virtual void get_uv(const Ray& intersection, double& u, double& v);
  virtual Colour getTexture(const Ray& intersection);
  virtual Vector3D getBumpNormal(const Ray& intersection);
  
private:
  Ray m_face;
  double m_top;
  double m_bottom;
  double m_right;
  double m_left;
};
class NonhierBox : public Primitive {
public:
  NonhierBox(const Point3D& pos, double size)
    : m_pos(pos), m_size(size)
  {
  for (int i = 0; i < 6; i++) {
    normals[i].p = m_pos;
  }

  normals[0].p[2] = m_pos[2] + m_size; // front
  normals[0].d = Vector3D(0, 0, 1);
  uv_range[0][0] = 0.25;
  uv_range[0][1] = 0.0;

  normals[1].p[1] = m_pos[1] + m_size; // top
  normals[1].d = Vector3D(0, 1, 0);
  uv_range[1][0] = 0.0;
  uv_range[1][1] = 1.0/3.0;

  normals[2].p[0] = m_pos[0] + m_size; // right
  normals[2].d = Vector3D(1, 0, 0);
  uv_range[2][0] = 0.25;
  uv_range[2][1] = 1.0/3.0;

  normals[3].p[2] = m_pos[2]; // back
  normals[3].d = Vector3D(0, 0, -1);
  uv_range[3][0] = 0.5;
  uv_range[3][1] = 1.0/3.0;

  normals[4].p[1] = m_pos[1]; // bottom
  normals[4].d = Vector3D(0, -1, 0);
  uv_range[4][0] = 0.75;
  uv_range[4][1] = 1.0/3.0;

  normals[5].p[0] = m_pos[0]; // left
  normals[5].d = Vector3D(-1, 0, 0);
  uv_range[5][0] = 0.25;
  uv_range[5][1] = 2.0/3.0;


  top = m_pos[1] + m_size;
  right = m_pos[0] + m_size;
  bottom = m_pos[1];
  left = m_pos[0];
  front = m_pos[2] + m_size;
  back = m_pos[2];
  }
    virtual Intersection intersect(Ray scene_ray, bool aggressive);
    virtual bool in_box(Point3D p);
  virtual ~NonhierBox();
    virtual Colour getTexture(const Ray& inter);
    Vector3D getBumpNormal(const Ray& inter);

private:
  double top;
  double right;
  double bottom;
  double left;
  double front;
  double back;
  Ray normals[6];
  Point3D m_pos;
  double m_size;
  double uv_range[6][2];
};

#endif
