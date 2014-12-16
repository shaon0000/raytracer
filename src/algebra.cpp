//---------------------------------------------------------------------------
//
// CS488 -- Introduction to Computer Graphics
//
// algebra.hpp/algebra.cpp
//
// Classes and functions for manipulating points, vectors, matrices, 
// and colours.  You probably won't need to modify anything in these
// two files.
//
// University of Waterloo Computer Graphics Lab / 2003
//
//---------------------------------------------------------------------------

#include "algebra.hpp"

uint32_t rand_x, rand_y, rand_z, rand_w;

void init_rand() {
  rand_x = rand();
  rand_y = rand();
  rand_z = rand();
  rand_w = rand();
}

Vector3D refract(Vector3D normal, Vector3D incident, double n1, double n2) {
  double n = n1/n2;
  double cosI = -1 * normal.dot(incident);
  double sinT2 = n * n * (1.0 - cosI * cosI);
  if (sinT2 > 1.0) return Vector3D(0.0, 0.0, 0.0);
  double cosT = sqrt(1.0 - sinT2);
  return (n * incident) + ((n * cosI - cosT) * normal);
}

Vector3D projection(Vector3D b, Vector3D a) {
  return (a.dot(b)/b.length2()) * a;
}

uint32_t xorshift128(void) {
    uint32_t t = rand_x ^ (rand_x << 11);
    rand_x = rand_y; rand_y = rand_z; rand_z = rand_w;
    return rand_w = rand_w ^ (rand_w >> 19) ^ t ^ (t >> 8);
}

double dbl_rand(double HI, double LO) {
  return LO + (HI-LO) * zo_random();
}

double zo_random() {
    const float t24 = 1.0 / 16777216.0; /* 0.5**24 */
    return t24 * ( xorshift128() >> 8 );
}

double closest_point_to_line(Point3D b, Ray ray) {
  Vector3D pb = b - ray.p;
  Vector3D v = ray.d;
  double t = pb.dot(v) / v.dot(v);
  return t;
}

double plane_line_intersection(Point3D v0, Ray ray, Vector3D n, bool &parallel) {
  if (n.dot(ray.p - v0) < Ee && n.dot(ray.p - v0) > -Ee) {
    parallel = true;
    return 0.0;
  }
  parallel = false;
  return n.dot(v0 - ray.p) / n.dot(ray.d);
}

vector<Point3D> points_in_area(Point3D p, Vector3D v1, Vector3D v2, int n, bool random) {
  vector<Point3D> points;
  if (v1[0] == 0 && v1[1] == 0 && v1[2] == 0) {
    points.push_back(p);
    return points;
  }

  if (v2[0] == 0 && v2[1] == 0 && v2[2] == 0) {
    points.push_back(p);
    return points;
  }

  double increment = 1.0/(double)(n);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      Ray ray;
      Point3D plane_p;
      if (random) {
        plane_p = p + ((zo_random() + zo_random())/2.0) * v1 + ((zo_random() + zo_random())/2.0) * v2;
      } else {
        plane_p = p + (i*increment) * v1 + (j*increment) * v2;
      }
      plane_p = plane_p -  (0.5 * (v1 + v2));
      points.push_back(plane_p);
    }
  }

  return points;
}

double Vector3D::normalize()
{
  double denom = 1.0;
  double x = (v_[0] > 0.0) ? v_[0] : -v_[0];
  double y = (v_[1] > 0.0) ? v_[1] : -v_[1];
  double z = (v_[2] > 0.0) ? v_[2] : -v_[2];

  if(x > y) {
    if(x > z) {
      if(1.0 + x > 1.0) {
        y = y / x;
        z = z / x;
        denom = 1.0 / (x * sqrt(1.0 + y*y + z*z));
      }
    } else { /* z > x > y */ 
      if(1.0 + z > 1.0) {
        y = y / z;
        x = x / z;
        denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
      }
    }
  } else {
    if(y > z) {
      if(1.0 + y > 1.0) {
        z = z / y;
        x = x / y;
        denom = 1.0 / (y * sqrt(1.0 + z*z + x*x));
      }
    } else { /* x < y < z */
      if(1.0 + z > 1.0) {
        y = y / z;
        x = x / z;
        denom = 1.0 / (z * sqrt(1.0 + y*y + x*x));
      }
    }
  }

  if(1.0 + x + y + z > 1.0) {
    v_[0] *= denom;
    v_[1] *= denom;
    v_[2] *= denom;
    return 1.0 / denom;
  }

  return 0.0;
}

/*
 * Define some helper functions for matrix inversion.
 */

static void swaprows(Matrix4x4& a, size_t r1, size_t r2)
{
  std::swap(a[r1][0], a[r2][0]);
  std::swap(a[r1][1], a[r2][1]);
  std::swap(a[r1][2], a[r2][2]);
  std::swap(a[r1][3], a[r2][3]);
}

static void dividerow(Matrix4x4& a, size_t r, double fac)
{
  a[r][0] /= fac;
  a[r][1] /= fac;
  a[r][2] /= fac;
  a[r][3] /= fac;
}

static void submultrow(Matrix4x4& a, size_t dest, size_t src, double fac)
{
  a[dest][0] -= fac * a[src][0];
  a[dest][1] -= fac * a[src][1];
  a[dest][2] -= fac * a[src][2];
  a[dest][3] -= fac * a[src][3];
}

Matrix4x4 getRotateMat(float theta, float x, float y, float z) {
  
  if (theta > 2*M_PI) {
    std:cerr << "stop using angles please, you're breaking stuff" << std::endl;
  }

  float sine_val = sin(theta);
  float cos_val = cos(theta);
  
  Vector4D row1;
  Vector4D row2;
  Vector4D row3;
  Vector4D row4;

  if (x) {
    row1 = Vector4D(1, 0, 0, 0);
    row2 = Vector4D(0, cos_val, -1*sine_val, 0);
    row3 = Vector4D(0, sine_val, cos_val, 0);
    row4 = Vector4D(0, 0, 0, 1);
  } else if (y) {
    row1 = Vector4D(cos_val, 0, sine_val, 0);
    row2 = Vector4D(0, 1, 0, 0);
    row3 = Vector4D(-1*sine_val, 0, cos_val, 0);
    row4 = Vector4D(0, 0, 0, 1);
  } else if (z) {
    row1 = Vector4D(cos_val, -1*sine_val, 0, 0);
    row2 = Vector4D(sine_val, cos_val, 0, 0);
    row3 = Vector4D(0, 0, 1, 0);
    row4 = Vector4D(0, 0, 0, 1);
  }

  return Matrix4x4(row1, row2, row3, row4);
}

Matrix4x4 getTranslateMat(float x, float y, float z) {
  Vector4D row1;
  Vector4D row2;
  Vector4D row3;
  Vector4D row4;

  row1 = Vector4D(1, 0, 0, x);
  row2 = Vector4D(0, 1, 0, y);
  row3 = Vector4D(0, 0, 1, z);
  row4 = Vector4D(0, 0, 0, 1);

  return Matrix4x4(row1, row2, row3, row4);
}

Matrix4x4 getScaleMat(float x, float y, float z) {
  Vector4D row1(x, 0, 0, 0);
  Vector4D row2(0, y, 0, 0);
  Vector4D row3(0, 0, z, 0);
  Vector4D row4(0, 0, 0, 1);
  return Matrix4x4(row1, row2, row3, row4);
}


/*
 * invertMatrix
 *
 * I lifted this code from the skeleton code of a raytracer assignment
 * from a different school.  I taught that course too, so I figured it
 * would be okay.
 */
Matrix4x4 Matrix4x4::invert() const
{
  /* The algorithm is plain old Gauss-Jordan elimination 
     with partial pivoting. */

  Matrix4x4 a(*this);
  Matrix4x4 ret;

  /* Loop over cols of a from left to right, 
     eliminating above and below diag */

  /* Find largest pivot in column j among rows j..3 */
  for(size_t j = 0; j < 4; ++j) { 
    size_t i1 = j; /* Row with largest pivot candidate */
    for(size_t i = j + 1; i < 4; ++i) {
      if(fabs(a[i][j]) > fabs(a[i1][j])) {
        i1 = i;
      }
    }

    /* Swap rows i1 and j in a and ret to put pivot on diagonal */
    swaprows(a, i1, j);
    swaprows(ret, i1, j);

    /* Scale row j to have a unit diagonal */
    if(a[j][j] == 0.0) {
      // Theoretically throw an exception.
      return ret;
    }

    dividerow(ret, j, a[j][j]);
    dividerow(a, j, a[j][j]);

    /* Eliminate off-diagonal elems in col j of a, doing identical 
       ops to b */
    for(size_t i = 0; i < 4; ++i) {
      if(i != j) {
        submultrow(ret, i, j, a[i][j]);
        submultrow(a, i, j, a[i][j]);
      }
    }
  }

  return ret;
}
