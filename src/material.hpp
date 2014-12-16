#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "algebra.hpp"

class Material {
public:
  virtual ~Material();
  virtual void apply_gl() const = 0;
  virtual Colour getColour() {
    return Colour(0.0, 0.0, 0.0);
  }

  virtual Colour getKs() {
        return Colour(0.0, 0.0, 0.0);
  }
  virtual Colour getKd() {
        return Colour(0.0, 0.0, 0.0);
  }

  virtual double get_n() {
    return 0.0;
  }

  virtual double get_shine() {
    return 0.0;
  }

  virtual double get_trans() {
    return 0.0;
  }
protected:
  Material()
  {
  }
};

class PhongMaterial : public Material {
public:
  PhongMaterial(const Colour& kd, const Colour& ks, double shininess, double snell_N, double trans);
  virtual ~PhongMaterial();

  virtual void apply_gl() const;
  Colour m_kd;
  Colour m_ks;
  double m_shininess;
  double m_snell_N;
  double m_trans;
  virtual Colour getColour() {
    return m_kd;
  }
    virtual double get_n() {
    return m_snell_N;
  }
    virtual Colour getKs() {
        return m_ks;
  }
  virtual Colour getKd() {
        return m_kd;
  }

  virtual double get_shine() {
    return m_shininess;
  }

  virtual double get_trans() {
    return m_trans;
  }
private:

};


#endif
