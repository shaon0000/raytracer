#include "primitive.hpp"
#include "polyroots.hpp"
#include <math.h>
#include <algorithm>
#include <float.h>
#include "algebra.hpp"
#include "scene.hpp"
#define DOT_EPSILON 0.001

Primitive::~Primitive()
{
}

Vector3D Primitive::normal(Point3D point) {
	return Vector3D(0.0, 0.0, 0.0);
}


Intersection Primitive::intersect(Ray scene_ray, bool aggressive) {
	Intersection a;
	return a;
}

Sphere::~Sphere() {
}

Intersection Sphere::intersect(Ray scene_ray, bool aggressive) {
	Intersection a;
	return a;
}

Vector3D Sphere::normal(Point3D point) {
	return Vector3D(0.0, 0.0, 0.0);
}


Colour NonhierSphere::getTexture(const Ray& inter) {
	double x = inter.p[0] - m_pos[0];
	double y = inter.p[1] - m_pos[1];
	double z = inter.p[2] - m_pos[2];

	double s_angle = atan2(x, z);
	double t_angle = asin(y/m_radius);

	if (s_angle > M_PI) {
		s_angle = s_angle - 2*M_PI;
	}

	if (s_angle < -M_PI) {
		s_angle = s_angle + 2*M_PI;
	}
	double s = (s_angle - (-M_PI)) / (2 * M_PI);
	double t = (t_angle - (-M_PI / 2)) / M_PI;

	int x_img = (int)(s * img.width());
	int y_img = (int)(t * img.height());

	double r = img(x_img, y_img, 0);
	double g = img(x_img, y_img, 1);
	double b = img(x_img, y_img, 2);
	return Colour(r, g, b);
}

Matrix4x4 rotateFromAtoB(Vector3D a, Vector3D b) {
	a.normalize();
	b.normalize();
	Vector3D v = a.cross(b);
	double s = v.length();
	double c = a.dot(b);
	Matrix4x4 vx = Matrix4x4(
		Vector4D(0   ,    -1 * v[2], v[1]     , 0),
		Vector4D(v[2],     0       , -1 * v[0], 0),
		Vector4D(-1 * v[1], v[0],            0, 0),
		Vector4D(0, 0, 0, 0));
	Matrix4x4 R = Matrix4x4() + vx +  ((1 - c)/ (s*s)) * (vx * vx);
	return R;

}

Vector3D NonhierSphere::getBumpNormal(const Ray& inter) {
	double x = inter.p[0] - m_pos[0];
	double y = inter.p[1] - m_pos[1];
	double z = inter.p[2] - m_pos[2];

	double s_angle = atan2(x, z);
	double t_angle = asin(y/m_radius);


	if (s_angle > M_PI) {
		s_angle = s_angle - 2*M_PI;
	}

	if (s_angle < -M_PI) {
		s_angle = s_angle + 2*M_PI;
	}
	double s = (s_angle - (-M_PI)) / (2 * M_PI);
	double t = (t_angle - (-M_PI / 2)) / M_PI;	

	//Matrix4x4 mat_y_axis_rotation = getRotateMat(s_angle, 0.0, 1.0, 0.0) * getRotateMat(M_PI/2, 0.0, 1.0, 0.0);
	//Matrix4x4 mat_z_axis_rotation = getRotateMat(t_angle, 0.0, 0.0, 1.0);
	// first we rotate on the z axis (lattitude), then on the y axis (longitude)
	// assuming the vector is (1, 0, 0), we need to find how much we rotate it up
	// or down on the xy-plane, then we need to rotate that point along the xz-plane.
	//Matrix4x4 rotation_matrix = mat_y_axis_rotation * mat_z_axis_rotation;

	int x_img = (int)(s * img.width());
	int y_img = (int)(t * img.height());

	double nx = 2*bump_map(x_img, y_img, 0) - 1;
	double ny = 2*bump_map(x_img, y_img, 1) - 1;
	double nz = 2*bump_map(x_img, y_img, 2) - 1;
	Vector3D bump_normal = Vector3D(nx, ny, nz);
	bump_normal.normalize();
	Vector3D n_test = inter.d;
	n_test.normalize();
	if (n_test[2] > 0.999) {
		//std::cerr << "bump normal: " << bump_normal << std::endl;
	}
	Matrix4x4 rotation_matrix = rotateFromAtoB(Vector3D(0, 0, 1), inter.d);


	bump_normal = rotation_matrix * bump_normal;
	if (n_test[2] > 0.999) {
		//std::cerr << "bump normal rotated into position: " << bump_normal << std::endl;
	}
	return bump_normal;
}


Cube::~Cube()
{
}

Intersection Cube::intersect(Ray scene_ray, bool aggressive) {
	Intersection a;
	return a;
}

Vector3D Cube::normal(Point3D point) {
	return Vector3D(0.0, 0.0, 0.0);
}

NonhierSphere::~NonhierSphere()
{
}

Vector3D reflect(Vector3D I, Vector3D N) {
  return (2.0 * N.dot(I)) * N - I;
}

double r_percent(Vector3D normal, Vector3D incident, double n1, double n2) {
	double r0 = (n1 - n2) / (n1 * n2);
	r0 *= r0;

	double cosX = -1 * normal.dot(incident);
	if (n1 > n2) {
		double n = n1 / n2;
		double sinT2 = n * n * (1.0 - cosX * cosX);
		if (sinT2 > 1.0) return 1.0;
		cosX = sqrt(1.0 - sinT2);
	}

	double x = 1.0 - cosX;
	return r0 + (1.0 - r0) * x * x * x * x * x;
}

Vector3D NonhierSphere::normal(Point3D point) {
	return point - m_pos;
}

Colour Primitive::shade(
	Point3D eye,
	Vector3D normal, 
	const std::list<Light*>& lights, 
	const Point3D& intersect_point, const Colour& Kd, const Colour& Ks, double shine, const Colour& ambient, SceneNode* root, int depth, int maxDepth, double snell_N, double reflectivity, SceneNode* curNode) {
	Colour finalColour(0.0, 0.0, 0.0);
	normal.normalize();
	Vector3D v = eye - intersect_point;
	v.normalize();

	// Quick check to see if this node is an associate node for a light.
	for (std::list<Light*>::const_iterator I = lights.begin(); 
        I != lights.end(); I++) {
		Light* light = *I;

		if (light->associate == curNode) {
			return light->colour;
		}
	}

	for (std::list<Light*>::const_iterator I = lights.begin(); 
        I != lights.end(); I++) {
		Light* light = *I;

		if (light->associate == curNode) {
			return light->colour;
		}
		vector<Point3D> light_positions = points_in_area(light->position, light->v1,light->v2, SHADOW_SAMPLES, true);
		
		Colour avgColour(0.0, 0.0, 0.0);

		for(unsigned int index = 0; index < light_positions.size(); index++) {
			Point3D light_point = light_positions[index];
			Vector3D s = light_point - intersect_point;
			Vector3D s_project_norm = projection(s, light->n);
			if (light->cos_theta_limit_squared > (1 - Ee)) {
				// nothing
			} if (s_project_norm.length2() < Ee && s_project_norm.length2() > -Ee) {
				// perpendicular to light
			} else if ((s_project_norm.dot(s) * s_project_norm.dot(s))
				 / (s.dot(s) * s_project_norm.dot(s_project_norm)) < light->cos_theta_limit_squared) {
				continue;
			}
		    
			Point3D shadow_point = intersect_point + Ee * normal;
			Ray shadow_ray;
			shadow_ray.d = light_point - intersect_point;
			shadow_ray.p = shadow_point;
			shadow_ray.isShadow = true;
			
			Intersection shadow = root->shadow_ray(shadow_ray, 1.0, false);
			
			if (shadow.total_intersections == 1) {
				if (shadow.t1 < 1.0) {
					continue;
				} else {
					//std::cerr << "intersection is past the light point" << std::endl;
				}
			}
			s.normalize();
		    


		    Vector3D r = reflect(s, normal);
		    double  sDotN = std::max( s.dot(normal), 0.0 );
		    Colour diffuse = sDotN * Kd * light->colour;
		    Colour spec = Colour(0.0, 0.0, 0.0);


		    spec = pow(std::max( r.dot(v), 0.0), shine) * Ks * light->colour;
	  		avgColour = avgColour + (diffuse + spec);

  		}
  		avgColour = (1.0/(light_positions.size())) * avgColour;
  		finalColour = finalColour + avgColour;
  	}
  	
  	Colour mixed;
  	Colour refraction_colour;
  	Colour reflection_colour;
  	
  	if (reflectivity > 0.0) {
		Ray reflection_ray;
		PixelBuf ref_buf;
		Intersection ref_inter;

		if ((depth + 1) != REF_DEPTH) {
		 	reflection_ray.p = intersect_point + Ee * normal;
			reflection_ray.d = reflect(eye - intersect_point, normal);

			ref_buf.t_buf = DBL_MAX;
			ref_inter = root->shadow_ray(reflection_ray, DBL_MAX, false);
			if (ref_inter.total_intersections != 0) {
				((SceneNode*)(ref_inter.intersectedNode))->ray_trace(reflection_ray, 
			    	ref_buf, 
			    	root, 
			    	lights,
			    	ambient, depth + 1, maxDepth, ref_inter);
			}

			reflection_colour = Colour(ref_buf.red, ref_buf.green, ref_buf.blue);
		}

		if (ref_buf.t_buf == DBL_MAX) {
			reflection_colour = 0 * reflection_colour;
		}

		if (snell_N > 0.0) {
			Ray refraction_ray;
			PixelBuf refract_buf;
			Intersection refract_inter;

			if ((depth + 1) != REFRACT_DEPTH) {
				refraction_ray.p = intersect_point - Ee * normal;
				refraction_ray.d = refract(normal, -1 * v, 1.0, snell_N);
				if (refraction_ray.d.length2() != 0.0) {
					refract_inter = curNode->shadow_ray(refraction_ray, DBL_MAX, false);
				}
				if (refract_inter.total_intersections != 0) {
					refraction_ray.p = refract_inter.p1 - Ee * refract_inter.normal;
					refraction_ray.d = refract(refract_inter.normal, refraction_ray.d, snell_N, 1.0);
					if (refraction_ray.d.length2() != 0.0) {
						refract_inter = root->shadow_ray(refraction_ray, DBL_MAX, false);
					}
					if (refract_inter.total_intersections != 0) {
					((SceneNode*)(refract_inter.intersectedNode))->ray_trace(refraction_ray, 
				    	refract_buf, 
				    	root, 
				    	lights,
				    	ambient, depth + 1, maxDepth, refract_inter);
					}
				}
			}
			refraction_colour = Colour(refract_buf.red, refract_buf.green, refract_buf.blue);
			if (refract_buf.t_buf == DBL_MAX) {
				refraction_colour = 0 * refraction_colour;
			}

			double reflect_percent = r_percent(normal, -1 * v, 1.0, snell_N);
			mixed = reflect_percent * reflection_colour + (1 - reflect_percent) * refraction_colour;

			return  (1 - reflectivity) * (finalColour + (ambient * Kd)) +  (reflectivity * mixed);
		} else {
			return (finalColour + (ambient * Kd)) +  (reflectivity * reflection_colour);
		}
	}
	return finalColour + (ambient * Kd);
	
}


Cylinder::~Cylinder() {

}

Intersection Cylinder::check_face(Ray scene_ray, Ray face) {
	bool parallel;
	double t = plane_line_intersection(face.p, scene_ray, face.d, parallel);
	Intersection inter;
	if (parallel || t < 0.0) {
		inter.total_intersections = 0;
		return inter;
	}

	Point3D p = scene_ray.p + t * scene_ray.d;
	Point3D center = face.p;
	if ((p - center).length2() > m_radius * m_radius) {
		inter.total_intersections = 0;
		return inter;
	}

	inter.total_intersections = 1;
	inter.p1 = p;
	inter.t1 = t;
	inter.normal = projection(-1 * scene_ray.d, face.d);

	return inter;
}

Intersection Cylinder::intersect_face(Ray scene_ray) {
	Intersection result;
	result.total_intersections = 0;

	for (int i = 0; i < 2; i++) {
		Intersection inter = check_face(scene_ray, caps[i]);
		if (inter.total_intersections == 1) {
			if (result.total_intersections == 0) {
				result = inter;
			} else if (inter.t1 < result.t1) {
				result = inter;
			}
		}
	}

	return result;
}

Intersection Cylinder::intersect(Ray scene_ray, bool aggressive) {
	Vector3D u = scene_ray.d;
	Vector3D v = m_ray.d;

	Vector3D dp = scene_ray.p - m_ray.p;


	double A = (u - (u.dot(v) * v)).length2();
	double B = 2 * ((u - (u.dot(v) * v)).dot(dp - (dp.dot(v) * v)));
	double C = (dp - (dp.dot(v) * v)).length2() - (m_radius * m_radius);
	double roots[2];
	roots[0] = 0.0;
	roots[1] = 0.0;
	quadraticRoots(A, B, C, roots);

	Intersection inter;
	inter.total_intersections = 0;
	if (roots[0] < 0.0 && roots[1] > 0.0) {
		roots[0] = roots[1];
	} else if (roots[0] > 0.0 && roots[1] < 0.0) {
		roots[1] = roots[0];
	}
	double t = std::min(roots[0], roots[1]);
	Intersection face_inter = intersect_face(scene_ray);
	
	if (face_inter.total_intersections == 1 && face_inter.t1 < t) {
		return face_inter;
	}

	if (t > 0.0) {
		inter.total_intersections = 1;
		inter.t1 = t;
		inter.p1 = scene_ray.p + t * scene_ray.d;
		double s = closest_point_to_line(inter.p1, m_ray);
		if (s > m_t_max || s < 0) {
			return intersect_face(scene_ray);
		}

		Point3D center = m_ray.p + (s * m_ray.d);
		inter.normal = projection(-1 * scene_ray.d, (inter.p1 - center));
	} 

	return inter;


}
Cone::~Cone() {

}

Intersection Cone::intersect(Ray scene_ray, bool aggressive) {
	
	double xd = scene_ray.d[0];
	double yd = scene_ray.d[1];
	double zd = scene_ray.d[2];
	
	double xe = scene_ray.p[0];
	double ye = scene_ray.p[1];
	double ze = scene_ray.p[2];
	double A = xd * xd + yd * yd - zd * zd;
	double B = 2 * (xe * xd + ye * yd - ze * zd);
	double C = xe * xe + ye * ye - ze * ze;

	double roots[2];
	roots[0] = 0.0;
	roots[1] = 0.0;

	quadraticRoots(A, B, C, roots);

	Intersection inter;
	inter.total_intersections = 0;
	
	if (roots[0] < 0.0 && roots[1] > 0.0) {
		roots[0] = roots[1];
	} else if (roots[0] > 0.0 && roots[1] < 0.0) {
		roots[1] = roots[0];
	}

	double t = std::min(roots[0], roots[1]);
	Intersection face_inter = intersect_face(scene_ray);

	if (face_inter.total_intersections == 1 && face_inter.t1 < t) {
		return face_inter;
	}

	if (t > 0.0) {
		inter.total_intersections = 1;
		inter.t1 = t;
		inter.p1 = scene_ray.p + t * scene_ray.d;
		double s = closest_point_to_line(inter.p1, m_ray);
		
		if (s > m_t_max) {
			return intersect_face(scene_ray);
		}
		
		if (s < 0.0) {
			inter.total_intersections = 0;
			return inter;
		}

		Vector3D bottom_to_inter = inter.p1 - m_ray.p;
		Point3D center = m_ray.p + s * m_ray.d;
		Vector3D bottom_to_center = center - m_ray.p;
		Vector3D bitangent = bottom_to_inter.cross(bottom_to_center);
		Vector3D norm = bottom_to_inter.cross(bitangent);

		inter.normal = projection(-1 * scene_ray.d, norm);
	}

	return inter; 	
}

void Cone::get_uv(const Ray& intersection, double& u, double& v) {
	Point3D center = Point3D(0, 0, intersection.p[2]);
	
	//std ::cerr << "center: " << center << std::endl;
	Vector3D segment = intersection.p - center;
	u = segment[0];
	v = segment[1];
	bool isFace = center[2] > (1 - Ee);
	if (isFace) {
		// we're on a face plane.
		u = (u/2.0) + 0.5;
		v = (v/2.0) + 0.5;
	} else {
		double theta = atan2(segment[1], segment[0]);
		if (theta < 0) {
			theta += 2 * M_PI;
		}

		u = theta / (2 * M_PI);
		v = center[2];
		v = (1 - v);
	}
	if (u >= 1.0 && u < (1.0 + Ee)) {
		u -= Ee;
	}

	if (u < 0.0 && u > -Ee) {
		u += Ee;
	}

	if (v >= 1.0 && v < (1.0 + Ee)) {
		v -= Ee;
	}

	if (v < 0.0 && v > -Ee) {
		v += Ee;
	}
}

Colour Cone::getTexture(const Ray& intersection) {
	double u = 0;
	double v = 0;
	
	get_uv(intersection, u, v);

	int img_x = (int)(u * img.width());
	int img_y = (int)(v * img.height());
	double red = img(img_x, img_y, 0);
	double green = img(img_x, img_y, 1);
	double blue = img(img_x, img_y, 2);
	return Colour(red, green, blue);
}

Vector3D Cone::getBumpNormal(const Ray& intersection) {
	double u = 0;
	double v = 0;
	
	get_uv(intersection, u, v);

	int img_x = (int)(u * bump_map.width());
	int img_y = (int)(v * bump_map.height());
	double nx = 2 * bump_map(img_x, img_y, 0) - 1;
	double ny = 2 * bump_map(img_x, img_y, 1) - 1;
	double nz = 2 * bump_map(img_x, img_y, 2) - 1;

	if (intersection.p[2] > (1 - Ee)) {
		// face
		return Vector3D(nx, ny, nz);
	} else {
		Vector3D n_test = intersection.d;
		n_test.normalize();
		Matrix4x4 rotation_matrix = rotateFromAtoB(Vector3D(0, 0, 1), n_test);
		return rotation_matrix * Vector3D(nx, ny, nz);
	}
}


Colour Cylinder::getTexture(const Ray& intersection) {
	Point3D center = Point3D(0, 0, intersection.p[2]);
	
	//std ::cerr << "center: " << center << std::endl;
	Vector3D segment = intersection.p - center;
	double u = segment[0];
	double v = segment[1];
	if (segment.length2() < 1 && (center[2] <= 0 || center[2] >=1)) {
		// we're on a face plane.
		u = (u/2.0) + 0.5;
		v = (v/2.0) + 0.5;
	} else {
		//std::cerr << "segment: " << segment << std::endl;
		double theta = atan2(segment[1], segment[0]);
		if (theta < 0) {
			theta += 2 * M_PI;
		}

		u = theta / (2 * M_PI);
		v = center[2];
		v = (1 - v);
	}
	if (u >= 1.0 && u < (1.0 + Ee)) {
		u -= Ee;
	}

	if (u < 0.0 && u > -Ee) {
		u += Ee;
	}

	if (v >= 1.0 && v < (1.0 + Ee)) {
		v -= Ee;
	}

	if (v < 0.0 && v > -Ee) {
		v += Ee;
	}
	int img_x = (int)(u * img.width());
	int img_y = (int)(v * img.height());
	double red = img(img_x, img_y, 0);
	double green = img(img_x, img_y, 1);
	double blue = img(img_x, img_y, 2);
	return Colour(red, green, blue);
}

Vector3D Cylinder::getBumpNormal(const Ray& intersection) {
	Point3D center = Point3D(0, 0, intersection.p[2]);
	
	//std ::cerr << "center: " << center << std::endl;
	Vector3D segment = intersection.p - center;
	double u = segment[0];
	double v = segment[1];
	bool isFace = segment.length() < (1 - Ee) && (center[2] < Ee || center[2] > (1 - Ee));
	
	if (isFace) {
		// we're on a face plane.
		u = (u/2.0) + 0.5;
		v = (v/2.0) + 0.5;
	} else {
		//std::cerr << "segment: " << segment << std::endl;
		double theta = atan2(segment[1], segment[0]);
		if (theta < 0) {
			theta += 2 * M_PI;
		}

		u = theta / (2 * M_PI);
		v = center[2];
		v = (1 - v);
	}
	if (u >= 1.0 && u < (1.0 + Ee)) {
		u -= Ee;
	}

	if (u < 0.0 && u > -Ee) {
		u += Ee;
	}

	if (v >= 1.0 && v < (1.0 + Ee)) {
		v -= Ee;
	}

	if (v < 0.0 && v > -Ee) {
		v += Ee;
	}
	int img_x = (int)(u * bump_map.width());
	int img_y = (int)(v * bump_map.height());
	double nx = 2 * bump_map(img_x, img_y, 0) - 1;
	double ny = 2 * bump_map(img_x, img_y, 1) - 1;
	double nz = 2 * bump_map(img_x, img_y, 2) - 1;
	Vector3D n_test = intersection.d;
	


	if (isFace) {
		if (n_test[2] > 0) {
			return Vector3D(nx, ny, nz);
		} else {
			return -1 * Vector3D(nx, ny, nz);
		}
	}

	Matrix4x4 rotation_matrix = rotateFromAtoB(Vector3D(0, 0, 1), n_test);
	return rotation_matrix * Vector3D(nx, ny, nz);
	
}


Intersection Cone::check_face(Ray scene_ray, Ray face) {
	bool parallel;
	double t = plane_line_intersection(face.p, scene_ray, face.d, parallel);
	Intersection inter;
	if (parallel || t < 0.0) {
		inter.total_intersections = 0;
		return inter;
	}

	Point3D p = scene_ray.p + t * scene_ray.d;
	Point3D center = face.p;
	if ((p - center).length2() > 1) {
		inter.total_intersections = 0;
		return inter;
	}

	inter.total_intersections = 1;
	inter.p1 = p;
	inter.t1 = t;
	inter.normal = projection(-1 * scene_ray.d, face.d);

	return inter;
}


Intersection Cone::intersect_face(Ray scene_ray) {
	Intersection result;
	result.total_intersections = 0;

	for (int i = 0; i < 1; i++) {
		Intersection inter = check_face(scene_ray, cap);
		if (inter.total_intersections == 1) {
			if (result.total_intersections == 0) {
				result = inter;
			} else if (inter.t1 < result.t1) {
				result = inter;
			}
		}
	}

	return result;
}

Intersection NonhierSphere::intersect(Ray scene_ray, bool aggressive) {
	Vector3D d = scene_ray.d;
	Point3D c = m_pos;
	Point3D p = scene_ray.p;
	double r = m_radius;

	double A = d.dot(d);
	double B = (2 * d).dot(p - c);
	double C = (p - c).dot(p - c) - (r * r);
	double roots[2];
	roots[0] = 0.0;
	roots[1] = 0.0;
	quadraticRoots(A, B, C, roots);

	Intersection inter;
	inter.total_intersections = 0;
	if (roots[0] < 0.0 && roots[1] > 0.0) {
		roots[0] = roots[1];
	} else if (roots[0] > 0.0 && roots[1] < 0.0) {
		roots[1] = roots[0];
	}
	double t = std::min(roots[0], roots[1]);
	if (t > 0.0) {
		inter.total_intersections = 1;
		inter.t1 = t;
		inter.p1 = p + t * d;
		inter.normal = projection(-1 * scene_ray.d, inter.p1 - m_pos);
		
	} 

	return inter;
}


NonhierBox::~NonhierBox()
{

}



Vector3D convert3Dto2D(Vector3D source) {
	Vector3D result(0.0, 0.0, 0.0);
	int index_result = 0;
	int index_source = 0;
	while (index_result < 2 && index_source < 3) {
		if (source[index_source] >= Ee || source[index_source] <= -Ee) {
			result[index_result] = source[index_source];
			index_result++;
		}
		index_source++;
	}

	return result;
}

Colour NonhierBox::getTexture(const Ray& intersection) {

	for (int i = 0; i < 6; i++) {
		bool parallel = false;
		double t = plane_line_intersection(normals[i].p, intersection, normals[i].d, parallel);

		if (t > -Ee && t < Ee) {
			// we found the normal;
			Vector3D pos_to_point = intersection.p - normals[i].p;
			pos_to_point = (1.0/m_size) * pos_to_point;
			Vector3D result = convert3Dto2D(pos_to_point);
			//std::cerr << result << std::endl;
			if (result[0] < 0) {
				result[0] = 1 - result[0];
			}

			if (result[1] < 0) {
				result[1] = 1 - result[1];
			}
			double u = result[0] * img.width();
			double v = result[1] * img.height();

			double red = img((int)(u), (int)(v), 0);
			double green = img((int)(u), (int)(v), 1);
			double blue = img((int)(u), (int)(v), 2);
			return Colour(red, green, blue);
		}
	}

	return Colour(0.0, 0.0, 0.0);
}

Vector3D NonhierBox::getBumpNormal(const Ray& intersection) {
	//std::cerr << "intersection (p, d): " << intersection.p << ", "  << intersection.d << std::endl;
	for (int i = 0; i < 6; i++) {
		bool parallel = false;
		double t = plane_line_intersection(normals[i].p, intersection, normals[i].d, parallel);

		//std:: cerr << "t value: " << t << " parallel: " << parallel << std::endl;
		if (t > -Ee && t < Ee) {
			// we found the normal;
			//std::cerr << "found a result" << std::endl;
			Vector3D pos_to_point = intersection.p - normals[i].p;
			pos_to_point = (1.0/m_size) * pos_to_point;
			Vector3D result = convert3Dto2D(pos_to_point);
			//std::cerr << result << std::endl;
			if (result[0] < 0) {
				result[0] = 1 - result[0];
			}

			if (result[1] < 0) {
				result[1] = 1 - result[1];
			}
			
			double u = result[0] * bump_map.width();
			double v = result[1] * bump_map.height();
			
			//std::cerr << "(u, v): " << u << " "  << v << std::endl;

			int x_img = (int) (u);
			int y_img = (int) (v);

			double nx = 2 * bump_map(x_img, y_img, 0) - 1;
			double ny = 2 * bump_map(x_img, y_img, 1) - 1;
			double nz = 2 * bump_map(x_img, y_img, 2) - 1;
			
			Vector3D bump_normal = Vector3D(nx, ny, nz);
			bump_normal.normalize();

			Vector3D n_test = normals[i].d;
			n_test.normalize();
			Matrix4x4 rotation_matrix;
			if (n_test[2] > 0.999) {
				//std::cerr << "bump normal: " << bump_normal << std::endl;
			}
			
			if (n_test[0] == 0.0 && n_test[1] == 0.0 && n_test[2] == 1.0) {
				rotation_matrix = Matrix4x4();
			} else if (n_test[0] == 0.0 && n_test[1] == 0.0 && n_test[2] == -1.0) {
								rotation_matrix = Matrix4x4();
								bump_normal = -1 * bump_normal;
			} 
			else {
				rotation_matrix = rotateFromAtoB(Vector3D(0, 0, 1), n_test);
			}
			bump_normal = rotation_matrix * bump_normal;
			
			if (n_test[2] > 0.999) {
				//std::cerr << "bump normal rotated into position: " << bump_normal << std::endl;
			}

			return bump_normal;
		}
	}

	std::cerr << "we fucked up, there was no bump normal for this point?? gdb plox" << std::endl;
	return intersection.d;
}

bool NonhierBox::in_box(Point3D p) {
	if (p[0] < left - DOT_EPSILON || p[0] > right + DOT_EPSILON) {
		return false;	
	}

	if (p[1] < bottom - DOT_EPSILON || p[1] > top + DOT_EPSILON) {
		return false;
	}

	if (p[2] < back - DOT_EPSILON || p[2] > front + DOT_EPSILON) {
		return false;
	}

	return true;
}

Intersection NonhierBox::intersect(Ray scene_ray, bool aggressive) {
	
	Intersection result;
	result.total_intersections = 0;
	double min_t = DBL_MAX;
	for(int i = 0; i < 6; i++) {
			bool parallel = false;
			double t = plane_line_intersection(normals[i].p, scene_ray, normals[i].d, parallel);
			if (parallel) {
				continue;
			}

			Point3D intersect_point = scene_ray.p + t * scene_ray.d;
			if (in_box(intersect_point)) {
				
				if (t < min_t && t > 0.0) {
					min_t = t;
					result.total_intersections = 1;
					result.t1 = t;
					result.p1 = scene_ray.p + t * scene_ray.d;
					result.normal = projection(-1 * scene_ray.d, normals[i].d);
					if (aggressive) {
						return result;
					}
				}	
			}	
	}

	return result;
}

Intersection Wall::intersect(Ray scene_ray, bool aggressive) {
	bool parallel;
	double t = plane_line_intersection(m_face.p, scene_ray, m_face.d, parallel);
	Intersection inter;
	//std::cerr << "yea, we're checking to see intersection" << std::endl;
	if (parallel) {
		//std::cerr << "line is parallel" << std::endl;
		inter.total_intersections = 0;
		return inter;
	}

	inter.t1 = t;
	inter.p1 = scene_ray.p + t * scene_ray.d;
	//std:: cerr << inter.p1 << std::endl;
	Point3D p = inter.p1;
	if ((p[0] < m_right) && (p[0] > m_left) && (p[1] < m_top) && (p[1] > m_bottom)) {
		//std::cerr << "somthing hit the face!" << std::endl;
		inter.total_intersections = 1;
		inter.normal = projection(-1 * scene_ray.d, m_face.d);

		return inter;
	} else {
		inter.total_intersections = 0;
		return inter;
	}
}

Wall::~Wall() {

}

void Wall::get_uv(const Ray& intersection, double& u, double& v) {
	u = intersection.p[0] + 0.5;
	v = intersection.p[1] + 0.5;
	
	double ratioX = 1.0/texScaleX;
	double ratioY = 1.0/texScaleY;

	double a = (int)(u/ratioX) * ratioX;
	double b = (int)(v/ratioY) * ratioY;

	u = (u - a) / ratioX;
	v = (v - b) / ratioY;
}

Colour Wall::getTexture(const Ray& intersection) {
	double u;
	double v;
	get_uv(intersection, u, v);
	u = img.width() * u;
	v = img.height() * v;
	double red = img((int)(u), (int)(v), 0);
	double green = img((int)(u), (int)(v), 1);
	double blue = img((int)(u), (int)(v), 2);
	return Colour(red, green, blue);
}

Vector3D Wall::getBumpNormal(const Ray& intersection) {
	double u;
	double v;
	get_uv(intersection, u, v);
	u = bump_map.width() * u;
	v = bump_map.height() * v;
	//std::cerr << "u, v" << u << " " << v << std::endl;

	double nx = 2 * bump_map((int)(u), (int)(v), 0) - 1 ;
	double ny = 2 * bump_map((int)(u), (int)(v), 1) - 1 ;
	double nz = 2 * bump_map((int)(u), (int)(v), 2) - 1 ; 

	Vector3D bump_normal = Vector3D(nx, ny, nz);
	double intersection_z = intersection.d[2];
	
	if (intersection_z < 0) {
		return -1 * bump_normal;
	} else {
		return bump_normal;
	}
}