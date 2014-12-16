#include "scene.hpp"
#include <iostream>

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{

}

SceneNode::~SceneNode()
{

}


void SceneNode::rotate(char axis, double angle)
{
  angle = M_PI * angle / 180.0;
  if (axis == 'x') {
    m_rotate = getRotateMat(angle, 1.0, 0.0, 0.0) * m_rotate;
  }

  if (axis == 'y') {
    m_rotate = getRotateMat(angle, 0.0, 1.0, 0.0) * m_rotate;
  }

  if (axis == 'z') {
    m_rotate = getRotateMat(angle, 0.0, 0.0, 1.0) * m_rotate;
  }
}

void SceneNode::scale(const Vector3D& amount)
{
  m_rotate = m_rotate * getScaleMat(amount[0], amount[1], amount[2]);
}

void SceneNode::translate(const Vector3D& amount)
{
  m_translate =  getTranslateMat(amount[0], amount[1], amount[2]) * m_translate;
}

bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
}

JointNode::~JointNode()
{
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

Intersection SceneNode::shadow_ray(Ray ray, double t_max, bool aggressive) {
    Intersection result;
    result.total_intersections = 0;
    result.t1 = DBL_MAX;
    aggressive = false;

    for (std::list<SceneNode*>::const_iterator I = m_children.begin(); 
      I != m_children.end(); I++) {
        Intersection inter = (**I).shadow_ray(ray, t_max, aggressive);
        if (inter.total_intersections == 1 && inter.t1 > 0.0) {
            if (aggressive) {
              return inter;
            }
            if (result.total_intersections == 0 || inter.t1 < result.t1) {
              result = inter;
            }
        }
    }

    return result;
}

Intersection GeometryNode::shadow_ray(Ray ray, double t_max, bool aggressive) {
  aggressive = false;
  Ray transformed_ray;
  transformed_ray.d = m_invtrans * ray.d;
  transformed_ray.p = m_invtrans * ray.p; 
  Intersection inter = m_primitive->intersect(transformed_ray, aggressive);
  if (inter.total_intersections == 1) {
    inter.intersectedNode = this;
    inter.p1 = m_trans * inter.p1;
    inter.normal = m_invtrans_transpose * inter.normal;
  }
  return inter;
}

void GeometryNode::ray_trace(Ray scene_ray, 
  PixelBuf &pix_buf, 
  SceneNode *root, 
  const std::list<Light*>& lights, 
  Colour ambient, double depth, double maxDepth, Intersection inter) {
  if (depth >= maxDepth) {
      return;
  }
  // this will force the method to redo the intersection check
  if (inter.total_intersections > 1) {
    std::cerr << "we fucked up so hard" << std::endl;
    inter = m_primitive->intersect(scene_ray, false);
  }
  if (inter.total_intersections > 0) {
    if (inter.t1 < pix_buf.t_buf) {
      if (pix_buf.t_buf != DBL_MAX) {
        std::cerr << "double hitting a pixel buffer" << std::endl;
      }
      pix_buf.t_buf = inter.t1;
      Colour shadedCol(0.0, 0.0, 0.0);
        //if (m_primitive->bump_map.width() != 0) {
        //  m_primitive->getBumpNormal(inter);
        //}
        Ray model_space;
        model_space.p = inter.p1;
        model_space.d = inter.normal;

        if (m_primitive->img.width() != 0 || m_primitive->bump_map.width() != 0) {
          model_space.p = m_invtrans * model_space.p;
          model_space.d = m_trans_transpose * model_space.d;
        }

        Vector3D final_normal = (m_primitive->bump_map.width() == 0) ? inter.normal : m_invtrans_transpose * m_primitive->getBumpNormal(model_space);
        Colour final_kd = (m_primitive->img.width() == 0) ? m_material->getKd() : m_primitive->getTexture(model_space);
        if (ENABLE_NORMAL_VIEW) {
          final_normal.normalize();
          Vector3D tn = (0.5) * (final_normal + Vector3D(1, 1, 1));
          final_kd = Colour(tn[0], tn[1], tn[2]);
        }

        shadedCol = shadedCol + m_primitive->shade(
          scene_ray.p,
          final_normal,
          lights,
          inter.p1,
          final_kd, 
          m_material->getKs(), 
          m_material->get_shine(),
          ambient,
          root,
          depth,
          maxDepth,
          m_material->get_n(),
          m_material->get_trans(),
          this
          ); 

      pix_buf.red = shadedCol.R();
      pix_buf.green = shadedCol.G();
      pix_buf.blue = shadedCol.B();
    } 
  }
}
 
