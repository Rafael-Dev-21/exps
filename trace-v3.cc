#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <tuple>

static inline constexpr float ffmin(float a, float b) { return a < b ? a : b; }

class v3 {
public:
  float d[3];

  inline void set(float nx, float ny, float nz)
  {
    d[0] = nx;
    d[1] = ny;
    d[2] = nz;
  }

  inline void operator+=(const v3& v) { for (int i=0; i<3; i++) d[i] += v.d[i]; }
  inline void operator+=(float v) { for (int i=0; i<3; i++) d[i] += v; }
  inline v3 operator+(const v3& v) const { v3 r; for (int i=0; i<3; i++) r.d[i] = d[i] + v.d[i]; return r; }
  inline v3 operator+(float v) const { v3 r; for (int i=0; i<3; i++) r.d[i] = d[i] + v; return r; }
  inline void operator-=(const v3& v) { for (int i=0; i<3; i++) d[i] -= v.d[i]; }
  inline void operator-=(float v) { for (int i=0; i<3; i++) d[i] -= v; }
  inline v3 operator-(const v3& v) const { v3 r; for (int i=0; i<3; i++) r.d[i] = d[i] - v.d[i]; return r; }
  inline v3 operator-(float v) const { v3 r; for (int i=0; i<3; i++) r.d[i] = d[i] - v; return r; }
  inline void operator*=(const v3& v) { for (int i=0; i<3; i++) d[i] *= v.d[i]; }
  inline void operator*=(float v) { for (int i=0; i<3; i++) d[i] *= v; }
  inline v3 operator*(const v3& v) const { v3 r; for (int i=0; i<3; i++) r.d[i] = d[i] * v.d[i]; return r; }
  inline v3 operator*(float v) const { v3 r; for (int i=0; i<3; i++) r.d[i] = d[i] * v; return r; }

  inline v3 operator-() const { return {{ -d[0], -d[1], -d[2] }}; }

  inline float dot(const v3& v) const
  {
    float r=0;
    for(int i=0; i<3; i++)
      r += d[i] * v.d[i];
    return r;
  }

  inline void pow(float e)
  {
    for (int i=0; i<3; i++)
      d[i] = std::powf(d[i], e);
  }

  inline float squared() const { return dot(*this); }
  inline float len() const { return std::sqrtf(dot(*this)); }

  inline void norm() { return (*this) *= (1/len()); }

  void reflect(const v3& axis)
  {
    v3 N = axis; N.norm();
    float v = dot(N);
    *this = N * (v+v) - *this;
  }

  inline float luma() const { return .2126 * d[0] + .7152 * d[1] + .0722 * d[2]; }

  void clamp() {
    for (int i=0; i<3; i++)
      /**/ if (d[i] < 0.) d[i] = 0.;
      else if (d[i] > 1.) d[i] = 1.;
  }

  void clampWithDesat() {
    float l = luma(), sat = 1.;
    if (l > 1.) { d[0] = d[1] = d[2] = 1.; return; }
    if (l < 0.) { d[0] = d[1] = d[2] = 0.; return; }
    for (int i=0; i<3; i++)
      /**/ if (d[i] > 1.) sat = ffmin(sat, (l-1.) / (l-d[i]));
      else if (d[i] < 0.) sat = ffmin(sat,  l     / (l-d[i]));
    if (sat != 1.)
      { *this = (*this - l) * sat + l; clamp(); }
  }
};

class m3 {
public:
  v3 m[3];

  void initRotate(const v3& angle)
  {
    float Cx = std::cosf(angle.d[0]), Cy = std::cosf(angle.d[1]), Cz = std::cosf(angle.d[2]);
    float Sx = std::sinf(angle.d[0]), Sy = std::sinf(angle.d[1]), Sz = std::sinf(angle.d[2]);
    float sxsz = Sx * Sz, cxsz = Cx * Sz;
    float cxcz = Cx * Cz, sxcz = Sx * Cz;

    m3 result = {{ {{ Cy * Cz, Cy * Sz, -Sy }},
                   {{ sxcz*Sy - cxsz, sxsz*Sy + cxcz, Sx*Cy }},
                   {{ cxcz*Sy + sxsz, cxsz*Sy - sxcz, Cx*Cy }} }};
    *this = result;
  }

  void transform(v3& vec)
  {
    vec.set(m[0].dot(vec), m[1].dot(vec), m[2].dot(vec));
  }
};

struct Sphere {
  v3 center;
  v3 color;
  float radius;
  float specular;
  float reflective;

  float intersect(const v3& O, const v3& D) const
  {
    const v3 CO = O - center;
    const float A = D.dot(D);
    const float B = 2 * CO.dot(D);
    const float C = CO.dot(CO) - radius * radius;

    const float discriminant = B*B - 4*A*C;
    return (discriminant < 0) ? -1 : (-B - std::sqrtf(discriminant))/(2*A);
  }
};

struct Light {
  enum class Kind {
    AMBIENT,
    POINT,
    DIRECTION
  } kind;
  v3 color;
  union { v3 position; v3 direction; };
};

class Scene {
public:
  std::vector<Sphere> spheres;
  std::vector<Light> lights;
  v3 background_color{{0,0,0}};

  std::tuple<float, const Sphere*> closest_hit(const v3 O, const v3 D, float t_min, float t_max)
  {
    float closest_t = 1e30;
    const Sphere *hit = nullptr;
    for (const auto& sphere : spheres) {
      float t = sphere.intersect(O, D);
      if (t >= t_min && t < t_max && t < closest_t) {
        closest_t = t;
        hit = &sphere;
      }
    }
    return { closest_t, hit };
  }

  v3 compute_light(const v3 P, const v3 N, const v3 V, float s)
  {
    v3 color{};
    for (const auto& light : lights) {
      if (light.kind == Light::Kind::AMBIENT) {
        color += light.color;
      } else {
        v3 L{};
        float t_max{};
        if (light.kind == Light::Kind::POINT) {
          L = light.position - P;
          t_max = 1.;
        } else {
          L = light.direction;
          t_max = 1e30;
        }
        float shadow_t;
        const Sphere *shadow_hit;
        std::tie(shadow_t, shadow_hit) = closest_hit(P, L, 0.001, t_max);
        if (shadow_hit) {
          continue;
        }
        float n_dot_l = N.dot(L);
        if (n_dot_l > 0.) {
          color += light.color * (n_dot_l/(L.len()*N.len()));
        }
        if (s != -1.) {
          v3 R = L; R.reflect(N);
          const float r_dot_v = R.dot(V);
          if (r_dot_v > 0.) {
            color += light.color * std::powf(r_dot_v/(R.len()*V.len()), s);
          }
        }
      }
    }
    return color;
  }

  v3 trace_ray(const v3& O, const v3& D, float t_min, float t_max, float recursion_depth=3)
  {
    float closest_t;
    const Sphere *hit;
    std::tie(closest_t, hit) = closest_hit(O, D, t_min, t_max);
    if (!hit) return background_color;
    v3 P = O + D * closest_t;
    v3 N = P - hit->center;
    N.norm();
    const v3 local_color = hit->color * compute_light(P, N, -D, hit->specular);
    v3 R = -D; R.reflect(N);
    if (recursion_depth >= 0)
      return local_color * (1.-hit->reflective) + trace_ray(P, R, 0.001, t_max, recursion_depth - 1) * hit->reflective;
    else
      return local_color;
  }
};

int main()
{
  const int W = 2048, H = 1152;

  std::vector<v3> fb(W*H);

  Scene scene = {{
    { {{ 0, -1, 3 }}, {{ 1, 0, 0 }}, 1, 500, .2 },
    { {{ 2, 0, 4 }}, {{ 0, 0, 1 }}, 1 , 500, .3 },
    { {{ -2, 0, 4 }}, {{ 0, 1, 0 }}, 1, 10, .4 },
    { {{ 0, -5001, 0 }}, {{ 1, 1, 0 }}, 5000, 1000, .5 }
  },{
    { Light::Kind::AMBIENT, {{.5*.2, 1.*.2, .5*.2 }}, {} },
    { Light::Kind::POINT, {{1.*.6, .5*.6, .5*.6}}, {{{ 2, 1, 0 }}} },
    { Light::Kind::DIRECTION, {{.5*.2, .5*.2, 1.*.2 }}, {{{ 1, 4, 4 }}} }
  }};

  v3 O{{0,0,0}};

  float gamma = .45;
  float ungamma = 1./gamma;

#pragma omp parallel for
  for (int x = 0; x < W; x++) {
    for (int y = 0; y < H; y++) {
      float u = (2. * x / W - 1.) * W / H;
      float v = -(2. * y / H - 1.);
      float s = 1. / H;

      v3 off1{0.,0.}, off2{s, 0.}, off3{0., s}, off4{s,1.};
      v3 D{u, v, 1};
      v3 D1 = D + off1, D2 = D + off2, D3 = D+off3, D4 = D + off4; D1.norm(); D2.norm(); D3.norm(); D4.norm();
      v3 color00 = scene.trace_ray(O, D1, 1., 1e30), color01 = scene.trace_ray(O, D2, 1., 1e30), color10 = scene.trace_ray(O, D3, 1., 1e30), color11 = scene.trace_ray(O, D4, 1., 1e30);
      v3 color0 = color00 * .75 + color01 * .25, color1 = color10 * .75 + color11 * .25; v3 color = color0 * .75 + color1 * .25;
      fb[y * W + x] = color;
    }
  }

  for (auto& px : fb) {
    px.clampWithDesat();
    px.pow(gamma);
  }

  std::cout << "P3\n" << W << " " << H << "\n255\n";
  for (const auto& px : fb) {
    std::cout << static_cast<int>(255 * px.d[0]) << " "
              << static_cast<int>(255 * px.d[1]) << " "
              << static_cast<int>(255 * px.d[2]) << " ";
  }
}
