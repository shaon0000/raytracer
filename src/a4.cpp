#include "a4.hpp"
#include "image.hpp"
#include <math.h>
#include <float.h>
#include <vector>
#include "mesh.hpp"
#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <pthread.h>
#include <mutex>

std::mutex mtx;
volatile int thread_safe_y = 0;

struct thread_data_def {
  PixelBuf** pix_buf;
  int x;
  int y;
  
  double start_x_ray;
  double start_y_ray;
  double x_increment;
  double y_increment;

  Point3D eye;

  SceneNode* root;

  int width;
  int height;

  // Lighting parameters
  Colour ambience;
  std::list<Light*> lights;
  double aperature;
  double dof;

};

typedef struct thread_data_def ThreadData; 


Ray rand_pnt_in_circle(Ray ray, double APERATURE_SIZE, double FOCAL_LENGTH) {
  Vector3D d2 = ray.d;
  bool par = false;
  double s = plane_line_intersection(Point3D(0, 0, -1 * FOCAL_LENGTH), ray, Vector3D(0, 0, 1), par);
  Point3D focal_point = ray.p + s * ray.d;
  Ray rand_ray;

  double t = zo_random() * (2 * M_PI);
  double u = zo_random();
  Vector3D unit_vector = u * Vector3D(cos(t), sin(t), 0.0);
  rand_ray.p = ray.p + APERATURE_SIZE * unit_vector;
  rand_ray.d = focal_point - rand_ray.p;

  return rand_ray;
}

/**

Ray rand_pnt_in_circle(Ray ray, double APERATURE_SIZE, double FOCAL_LENGTH) {
  Vector3D d2 = ray.d;
  bool par = false;
  double s = plane_line_intersection(Point3D(0, 0, -1 * FOCAL_LENGTH), ray, Vector3D(0, 0, 1), par);
  Point3D focal_point = ray.p + s * ray.d;
  Ray rand_ray;

  double t = 2*M_PI*zo_random();
  double u = zo_random() + zo_random();
  double rand_radius = u>1 ? 2-u : u;
  Vector3D unit_vector = Vector3D(rand_radius*cos(t), rand_radius*sin(t), 0);
  rand_ray.p = ray.p + APERATURE_SIZE * unit_vector;
  rand_ray.d = focal_point - rand_ray.p;

  return rand_ray;
}
**/

void add_px_buf(PixelBuf& org_px, PixelBuf& new_px, int count) {
  org_px.red = (org_px.red * (count - 1) + new_px.red) / count;
    org_px.green = (org_px.green * (count - 1) + new_px.green) / count;
      org_px.blue = (org_px.blue * (count - 1) + new_px.blue) / count;
}


void* render_section(void* threadarg) {
  ThreadData* data;
  data = (ThreadData *) threadarg;
  
  double x_ray;
  double y_ray;

  double start_x_ray = data->start_x_ray;
  double start_y_ray = data->start_y_ray;

  double x_increment = data->x_increment;
  double y_increment = data->y_increment;

  Point3D eye = data->eye;
  SceneNode* root = data->root;

  PixelBuf** pix_buf = data->pix_buf;
  int height = data->height;
  int width = data->width;
  int y = 0;
  while (y < height) {
    
    mtx.lock();
    y = thread_safe_y;
    if (y % 100 == 0) {
      std::cerr << "grabbed line: " << y << std::endl;
    }
    thread_safe_y++;
    mtx.unlock();
    if (y >= height) {
      break;
    }
    for (int x = data->x; x < width; x++) {
      x_ray = start_x_ray + (x_increment * x);
      y_ray = start_y_ray + (y_increment * y);
      Vector3D scene_ray = Vector3D(x_ray, y_ray, -1);
      Point3D camera_point = eye;
      Ray ray;
      ray.d = scene_ray;
      ray.p = camera_point;
      ray.t = 0.0;

      Intersection inter = root->shadow_ray(ray, DBL_MAX, false);
      if (inter.intersectedNode != NULL) {
        ((SceneNode*) (inter.intersectedNode))->ray_trace(ray, pix_buf[x][y], root, data->lights, data->ambience, 0, REF_DEPTH, inter);  
      }

      
      for (int i = 0; i < DOF_SAMPLES; i++) {
        PixelBuf focal_buf;
        Ray focal_ray = rand_pnt_in_circle(ray, data->aperature, data->dof);

        Intersection inter = root->shadow_ray(focal_ray, DBL_MAX, false);
        if (inter.intersectedNode != NULL) {
          ((SceneNode*) (inter.intersectedNode))->ray_trace(focal_ray, focal_buf, root, data->lights, data->ambience, 0, REF_DEPTH, inter);  
        }

        add_px_buf(pix_buf[x][y], focal_buf, i + 2);

      }

      //root->ray_trace(ray, pix_buf[x][y], root, lights, ambience, 0, 10); 
    }

  }
  //std::cerr << "finished lines: " << data->y << " to " << height << std::endl;
  pthread_exit(NULL);
}

void print_progress(double progress) {
    int barWidth = 70;
    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

void a4_render(// What to render
               SceneNode* root,
               // Where to output the image
               const std::string& filename,
               // Image size
               int width, int height,
               // Viewing parameters
               const Point3D& eye, const Vector3D& view,
               const Vector3D& up, double fov,
               // Lighting parameters
               const Colour& ambient,
               const std::list<Light*>& lights,
               double aperature,
               double dof
               ) {

  std::cerr << "Stub: a4_render(" << root << ",\n     "
            << filename << ", " << width << ", " << height << ",\n     "
            << eye << ", " << view << ", " << up << ", " << fov << ",\n     "
            << ambient << ",\n     {";


  srand(time(0));
  init_rand();
  root->flatten(Matrix4x4());
  for (std::list<Light*>::const_iterator I = lights.begin(); I != lights.end(); ++I) {
    if (I != lights.begin()) std::cerr << ", ";
    std::cerr << **I;
  }
  int final_width = width;
  int final_height = height;
  width = width * MSAA;
  height = height * MSAA;
  Colour ambience = ambient;
  std::cerr << "});" << std::endl;
  std::cerr << "super-sampling AA: " << MSAA << std::endl;
  std::cerr << "shadow samples: " << SHADOW_SAMPLES << std::endl;
  std::cerr << "reflection depth: " << REF_DEPTH << std::endl;
  std::cerr << "dof samples: " << DOF_SAMPLES << std::endl;
  std::cerr << "parallel threads: " << THREAD_NUM << std::endl;
  if (SHOW_BOUND_VOLUME) {
    std::cerr << "showing bounding volumes for meshes" << std::endl;
  }
  // test_simple_mesh();
  // For now, just make a sample image.
  Image img(final_width, final_height, 3);
  
  fov = M_PI * fov / 180;

  double aspect = ((double) width) / ((double) height);
  
  double start_x_ray = -aspect * tan(fov / 2.0);
  double start_y_ray = tan(fov / 2.0);
  
  double finish_x_ray = aspect * tan(fov / 2.0);
  double finish_y_ray = -tan(fov / 2.0);

  double x_increment = (finish_x_ray - start_x_ray) / width;
  double y_increment = (finish_y_ray - start_y_ray) / height;

  double x_ray = start_x_ray;
  double y_ray = start_y_ray;
  double z_ray = -1.0;
  PixelBuf** pix_buf = new PixelBuf*[width];
  for(int i = 0; i < width; i++)
    pix_buf[i] = new PixelBuf[height];

  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      
    }
  }
  int total = width * height;
  int counter = 0;
  int mod_push = total/10;

  Colour org_col = Colour(
    0.3, 
    0.3, 
    0.3);
  
  pthread_t threads[THREAD_NUM];
  ThreadData td[THREAD_NUM];
  int rc;
  pthread_attr_t attr;
  void *status;

  // Initialize and set thread joinable
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for (int i = 0; i < THREAD_NUM; i++) {
    td[i].pix_buf = pix_buf;
    td[i].x = 0;
    td[i].y = 0;
  
    td[i].start_x_ray = start_x_ray;
    td[i].start_y_ray = start_y_ray;
    td[i].x_increment = x_increment;
    td[i].y_increment = y_increment;

    td[i].eye = eye;

    td[i].root = root;

    td[i].width = width;
    td[i].height = height;

    td[i].aperature = aperature;
    td[i].dof = dof;
    td[i].ambience = ambience;
    td[i].lights = lights;
  }

  for (int i = 0; i < THREAD_NUM; i++) {
    rc = pthread_create(&threads[i], NULL, render_section, (void *)(td + i));
      if (rc){
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
  } 

// free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
   for(int i=0; i < THREAD_NUM; i++ ){
      rc = pthread_join(threads[i], &status);
      if (rc){
         cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }
      cout << "Main: completed thread id :" << i ;
      cout << "  exiting with status :" << status << endl;
   }


     cout << "Moving to final stich stage" << endl;

  std::cerr << std::endl;
  for (int y = 0; y < final_height; y++) {
    for (int x = 0; x < final_width; x++) {
      Colour final_colour = Colour(0.0, 0.0, 0.0);
      for (int i = 0; i < MSAA; i++) {
        for (int j = 0; j < MSAA; j++) {
          int pix_x = (MSAA * x) + i;
          int pix_y = (MSAA * y) + j;
          
          
          PixelBuf pix = pix_buf[pix_x][pix_y];

          Colour new_col = Colour(pix.red, pix.green, pix.blue);
          final_colour = final_colour + new_col;
        }
      }
      final_colour = (1.0/(MSAA*MSAA)) * final_colour;
      
      img(x, y, 0) = final_colour.R();
      img(x, y, 1) = final_colour.G();
      img(x, y, 2) = final_colour.B();
    }
  }
  img.savePng(filename);
  pthread_exit(NULL);
}
