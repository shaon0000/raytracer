#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }
  
  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
    m_invtrans_transpose = m_invtrans.transpose();
    m_trans_transpose = m_trans.transpose();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  void flatten(const Matrix4x4& mat) {
    m_trans = mat * m_translate * m_rotate;
    set_transform(m_trans);
    for (std::list<SceneNode*>::const_iterator I = m_children.begin(); 
      I != m_children.end(); I++) {
        (**I).flatten(m_trans);
    }
  }

  virtual Intersection shadow_ray(Ray ray, double t_max, bool aggressive);
  virtual void ray_trace(Ray scene_ray, 
    PixelBuf &pix_buf, 
    SceneNode *root, 
    const std::list<Light*>& lights,
    Colour ambient, double depth, double maxDepth, Intersection inter) {
      if (depth >= maxDepth) {
          return;
      }
    
    for (std::list<SceneNode*>::const_iterator I = m_children.begin(); 
      I != m_children.end(); I++) {
        (**I).ray_trace(scene_ray, pix_buf, root, lights, ambient, depth, maxDepth, inter);
    }
  }
  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;

protected:
  
  // Useful for picking
  int m_id;
  std::string m_name;

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;
  Matrix4x4 m_invtrans_transpose;
  Matrix4x4 m_trans_transpose;
  
  Matrix4x4 m_translate;
  Matrix4x4 m_rotate;
  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;

};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual bool is_joint() const;
  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

  
protected:

  JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  const Material* get_material() const;
  Material* get_material();
  virtual Intersection shadow_ray(Ray ray, double t_max, bool aggressive);
  void set_material(Material* material)
  {
    m_material = material;
  }

  void set_texture_scale(int x, int y) {
    m_primitive->textureScale(x, y);
  }
  
  void set_texture(const std::string& filename) {
    m_primitive->setTexture(filename);
  }

  void set_bump(const std::string& filename) {
    m_primitive->setBump(filename);
  }

  virtual void ray_trace(Ray scene_ray, 
    PixelBuf &pix_buf, 
    SceneNode *root, 
    const std::list<Light*>& lights,
    Colour ambient, double depth, double maxDepth, Intersection inter);
protected:
  Material* m_material;
  Primitive* m_primitive;
};

#endif
