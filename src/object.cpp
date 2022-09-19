#include "object.hpp"

// TODO: move objects to separate .cpp files
namespace ren {
  
using vec2 = glm::vec2;

bool plane_hit(Object const &obj, ray const &r, float t_min, float t_max,
               hit_record &rec) {

  struct Coords {
    point3 a;
    point3 b;
  };
  auto get_coords = [](Object const &obj) -> Coords {
    // TODO: object rotation
    auto t = obj.translation();
    auto s = obj.scale();

    auto x = s.x + t.x;
    auto y = t.y;
    auto z = s.x + t.z;

    auto a = point3(-x, y, z);
    auto b = point3(x, y, -z);

    return {a, b};
  };

  auto coords = get_coords(obj);

  auto k = coords.a.y;
  auto t = (k - r.origin().y) / r.direction().y;
  if (t < t_min || t > t_max)
    return false;
  auto x = r.origin().x + t * r.direction().x;
  auto z = r.origin().z + t * r.direction().z;
  if (x < coords.a.x || x > coords.b.x || z > coords.a.z || z < coords.b.z) {
    return false;
  }
  // rec.u = (x - coords.a.x) / (coords.b.x - coords.a.x);
  // rec.v = (z - coords.a.z) / (coords.b.z - coords.a.z);
  rec.t = t;
  auto outward_normal = vec3(0, 1, 0);
  rec.set_face_normal(r, outward_normal);
  rec.mat_ptr = obj.material();
  rec.p = r.at(t);
  return true;
}

bool sphere_hit(Object const &obj, ray const &r, float t_min, float t_max,
                hit_record &rec) {
  auto center = obj.translation();
  auto radius = obj.scale().x;
  vec3 oc = r.origin() - center;
  auto a = glm::length2(r.direction());
  auto half_b = dot(oc, r.direction());
  auto c = glm::length2(oc) - radius * radius;
  auto discriminant = half_b * half_b - a * c;
  if (discriminant > 0) {
    auto root = sqrt(discriminant);
    auto temp = (-half_b - root) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.at(rec.t);
      vec3 outward_normal = (rec.p - center) / radius;
      rec.set_face_normal(r, outward_normal);
      rec.mat_ptr = obj.material();
      return true;
    }
    temp = (-half_b + root) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.at(rec.t);
      vec3 outward_normal = (rec.p - center) / radius;
      rec.set_face_normal(r, outward_normal);
      rec.mat_ptr = obj.material();
      return true;
    }
  }
  return false;
}

Object create_sphere() {
  // clang-format off
  std::vector<Vertex> verts {
    Vertex{vec3(0.000000,-1.000000,0.000000),vec3(0.187600,-0.794700,0.577400),vec2(0.181819,1.000000)},
    Vertex{vec3(0.723600,-0.447215,0.525720),vec3(0.187600,-0.794700,0.577400),vec2(0.272728,0.842539)},
    Vertex{vec3(-0.276385,-0.447215,0.850640),vec3(0.187600,-0.794700,0.577400),vec2(0.090910,0.842539)},
    Vertex{vec3(0.723600,-0.447215,0.525720),vec3(0.607100,-0.794700,-0.000000),vec2(0.272728,0.842539)},
    Vertex{vec3(0.000000,-1.000000,0.000000),vec3(0.607100,-0.794700,-0.000000),vec2(0.363637,1.000000)},
    Vertex{vec3(0.723600,-0.447215,-0.525720),vec3(0.607100,-0.794700,-0.000000),vec2(0.454546,0.842539)},
    Vertex{vec3(0.000000,-1.000000,0.000000),vec3(-0.491100,-0.794700,0.356800),vec2(0.909091,1.000000)},
    Vertex{vec3(-0.276385,-0.447215,0.850640),vec3(-0.491100,-0.794700,0.356800),vec2(1.000000,0.842539)},
    Vertex{vec3(-0.894425,-0.447215,0.000000),vec3(-0.491100,-0.794700,0.356800),vec2(0.818182,0.842539)},
    Vertex{vec3(0.000000,-1.000000,0.000000),vec3(-0.491100,-0.794700,-0.356800),vec2(0.727273,1.000000)},
    Vertex{vec3(-0.894425,-0.447215,0.000000),vec3(-0.491100,-0.794700,-0.356800),vec2(0.818182,0.842539)},
    Vertex{vec3(-0.276385,-0.447215,-0.850640),vec3(-0.491100,-0.794700,-0.356800),vec2(0.636364,0.842539)},
    Vertex{vec3(0.000000,-1.000000,0.000000),vec3(0.187600,-0.794700,-0.577400),vec2(0.545455,1.000000)},
    Vertex{vec3(-0.276385,-0.447215,-0.850640),vec3(0.187600,-0.794700,-0.577400),vec2(0.636364,0.842539)},
    Vertex{vec3(0.723600,-0.447215,-0.525720),vec3(0.187600,-0.794700,-0.577400),vec2(0.454546,0.842539)},
    Vertex{vec3(0.723600,-0.447215,0.525720),vec3(0.982200,-0.187600,-0.000000),vec2(0.272728,0.842539)},
    Vertex{vec3(0.723600,-0.447215,-0.525720),vec3(0.982200,-0.187600,-0.000000),vec2(0.454546,0.842539)},
    Vertex{vec3(0.894425,0.447215,0.000000),vec3(0.982200,-0.187600,-0.000000),vec2(0.363637,0.685079)},
    Vertex{vec3(-0.276385,-0.447215,0.850640),vec3(0.303500,-0.187600,0.934200),vec2(0.090910,0.842539)},
    Vertex{vec3(0.723600,-0.447215,0.525720),vec3(0.303500,-0.187600,0.934200),vec2(0.272728,0.842539)},
    Vertex{vec3(0.276385,0.447215,0.850640),vec3(0.303500,-0.187600,0.934200),vec2(0.181819,0.685079)},
    Vertex{vec3(-0.894425,-0.447215,0.000000),vec3(-0.794600,-0.187600,0.577400),vec2(0.818182,0.842539)},
    Vertex{vec3(-0.276385,-0.447215,0.850640),vec3(-0.794600,-0.187600,0.577400),vec2(1.000000,0.842539)},
    Vertex{vec3(-0.723600,0.447215,0.525720),vec3(-0.794600,-0.187600,0.577400),vec2(0.909091,0.685079)},
    Vertex{vec3(-0.276385,-0.447215,-0.850640),vec3(-0.794600,-0.187600,-0.577400),vec2(0.636364,0.842539)},
    Vertex{vec3(-0.894425,-0.447215,0.000000),vec3(-0.794600,-0.187600,-0.577400),vec2(0.818182,0.842539)},
    Vertex{vec3(-0.723600,0.447215,-0.525720),vec3(-0.794600,-0.187600,-0.577400),vec2(0.727273,0.685079)},
    Vertex{vec3(0.723600,-0.447215,-0.525720),vec3(0.303500,-0.187600,-0.934200),vec2(0.454546,0.842539)},
    Vertex{vec3(-0.276385,-0.447215,-0.850640),vec3(0.303500,-0.187600,-0.934200),vec2(0.636364,0.842539)},
    Vertex{vec3(0.276385,0.447215,-0.850640),vec3(0.303500,-0.187600,-0.934200),vec2(0.545455,0.685079)},
    Vertex{vec3(0.723600,-0.447215,0.525720),vec3(0.794600,0.187600,0.577400),vec2(0.272728,0.842539)},
    Vertex{vec3(0.894425,0.447215,0.000000),vec3(0.794600,0.187600,0.577400),vec2(0.363637,0.685079)},
    Vertex{vec3(0.276385,0.447215,0.850640),vec3(0.794600,0.187600,0.577400),vec2(0.181819,0.685079)},
    Vertex{vec3(-0.276385,-0.447215,0.850640),vec3(-0.303500,0.187600,0.934200),vec2(0.090910,0.842539)},
    Vertex{vec3(0.276385,0.447215,0.850640),vec3(-0.303500,0.187600,0.934200),vec2(0.181819,0.685079)},
    Vertex{vec3(-0.723600,0.447215,0.525720),vec3(-0.303500,0.187600,0.934200),vec2(0.000000,0.685079)},
    Vertex{vec3(-0.894425,-0.447215,0.000000),vec3(-0.982200,0.187600,-0.000000),vec2(0.818182,0.842539)},
    Vertex{vec3(-0.723600,0.447215,0.525720),vec3(-0.982200,0.187600,-0.000000),vec2(0.909091,0.685079)},
    Vertex{vec3(-0.723600,0.447215,-0.525720),vec3(-0.982200,0.187600,-0.000000),vec2(0.727273,0.685079)},
    Vertex{vec3(-0.276385,-0.447215,-0.850640),vec3(-0.303500,0.187600,-0.934200),vec2(0.636364,0.842539)},
    Vertex{vec3(-0.723600,0.447215,-0.525720),vec3(-0.303500,0.187600,-0.934200),vec2(0.727273,0.685079)},
    Vertex{vec3(0.276385,0.447215,-0.850640),vec3(-0.303500,0.187600,-0.934200),vec2(0.545455,0.685079)},
    Vertex{vec3(0.723600,-0.447215,-0.525720),vec3(0.794600,0.187600,-0.577400),vec2(0.454546,0.842539)},
    Vertex{vec3(0.276385,0.447215,-0.850640),vec3(0.794600,0.187600,-0.577400),vec2(0.545455,0.685079)},
    Vertex{vec3(0.894425,0.447215,0.000000),vec3(0.794600,0.187600,-0.577400),vec2(0.363637,0.685079)},
    Vertex{vec3(0.276385,0.447215,0.850640),vec3(0.491100,0.794700,0.356800),vec2(0.181819,0.685079)},
    Vertex{vec3(0.894425,0.447215,0.000000),vec3(0.491100,0.794700,0.356800),vec2(0.363637,0.685079)},
    Vertex{vec3(0.000000,1.000000,0.000000),vec3(0.491100,0.794700,0.356800),vec2(0.272728,0.527618)},
    Vertex{vec3(-0.723600,0.447215,0.525720),vec3(-0.187600,0.794700,0.577400),vec2(0.000000,0.685079)},
    Vertex{vec3(0.276385,0.447215,0.850640),vec3(-0.187600,0.794700,0.577400),vec2(0.181819,0.685079)},
    Vertex{vec3(0.000000,1.000000,0.000000),vec3(-0.187600,0.794700,0.577400),vec2(0.090910,0.527618)},
    Vertex{vec3(-0.723600,0.447215,-0.525720),vec3(-0.607100,0.794700,-0.000000),vec2(0.727273,0.685079)},
    Vertex{vec3(-0.723600,0.447215,0.525720),vec3(-0.607100,0.794700,-0.000000),vec2(0.909091,0.685079)},
    Vertex{vec3(0.000000,1.000000,0.000000),vec3(-0.607100,0.794700,-0.000000),vec2(0.818182,0.527618)},
    Vertex{vec3(0.276385,0.447215,-0.850640),vec3(-0.187600,0.794700,-0.577400),vec2(0.545455,0.685079)},
    Vertex{vec3(-0.723600,0.447215,-0.525720),vec3(-0.187600,0.794700,-0.577400),vec2(0.727273,0.685079)},
    Vertex{vec3(0.000000,1.000000,0.000000),vec3(-0.187600,0.794700,-0.577400),vec2(0.636364,0.527618)},
    Vertex{vec3(0.894425,0.447215,0.000000),vec3(0.491100,0.794700,-0.356800),vec2(0.363637,0.685079)},
    Vertex{vec3(0.276385,0.447215,-0.850640),vec3(0.491100,0.794700,-0.356800),vec2(0.545455,0.685079)},
    Vertex{vec3(0.000000,1.000000,0.000000),vec3(0.491100,0.794700,-0.356800),vec2(0.454546,0.527618)},
};
std::vector<GLuint> indices {0,2,1,0,2,1,3,5,4,3,5,4,6,8,7,6,8,7,9,11,10,9,11,10,12,14,13,12,14,13,15,17,16,15,17,16,18,20,19,18,20,19,21,23,22,21,23,22,24,26,25,24,26,25,27,29,28,27,29,28,30,32,31,30,32,31,33,35,34,33,35,34,36,38,37,36,38,37,39,41,40,39,41,40,42,44,43,42,44,43,45,47,46,45,47,46,48,50,49,48,50,49,51,53,52,51,53,52,54,56,55,54,56,55,57,59,58,57,59,58,};
// clang-format off
  auto obj = Object(verts, indices, true);
  obj.set_type(Object::Type::sphere);
  return obj;
}

Object create_sphere(glm::vec3 cen, float r, std::shared_ptr<Material> mat) {
  auto obj = create_sphere();

  obj.set_translation(cen);
  obj.set_scale(vec3(r));
  obj.update_model();

  obj.set_material(mat);
  obj.set_hit_function(sphere_hit);

  return obj;
}

Object create_cube() {
// clang-format off
std::vector<Vertex> verts {
    Vertex{vec3(1.000000,-1.000000,-1.000000),vec3(0.666667,-0.666667,-0.333333),vec2(2.094305,-0.396205)},
    Vertex{vec3(1.000000,-1.000000,1.000000),vec3(0.408248,-0.408248,0.816497),vec2(1.396205,-1.094305)},
    Vertex{vec3(-1.000000,-1.000000,1.000000),vec3(-0.666667,-0.666667,0.333333),vec2(0.698100,-0.396205)},
    Vertex{vec3(-1.000000,-1.000000,-1.000000),vec3(-0.408248,-0.408248,-0.816497),vec2(1.396205,0.301900)},
    Vertex{vec3(1.000000,1.000000,-1.000000),vec3(0.333333,0.666667,-0.666667),vec2(5.000000,-4.000000)},
    Vertex{vec3(-1.000000,1.000000,-1.000000),vec3(-0.816497,0.408248,-0.408248),vec2(2.792410,-4.000000)},
    Vertex{vec3(-1.000000,1.000000,1.000000),vec3(-0.333334,0.666667,0.666667),vec2(2.792410,-1.792410)},
    Vertex{vec3(0.999999,1.000000,1.000001),vec3(0.816496,0.408248,0.408249),vec2(5.000000,-1.792410)},
    Vertex{vec3(1.000000,1.000000,-1.000000),vec3(0.333333,0.666667,-0.666667),vec2(2.792410,-1.094305)},
    Vertex{vec3(0.999999,1.000000,1.000001),vec3(0.816496,0.408248,0.408249),vec2(2.094305,-1.792410)},
    Vertex{vec3(0.999999,1.000000,1.000001),vec3(0.816496,0.408248,0.408249),vec2(0.698100,-1.792410)},
    Vertex{vec3(-1.000000,1.000000,1.000000),vec3(-0.333334,0.666667,0.666667),vec2(0.000000,-1.094305)},
    Vertex{vec3(-1.000000,1.000000,1.000000),vec3(-0.333334,0.666667,0.666667),vec2(0.000000,0.301900)},
    Vertex{vec3(-1.000000,1.000000,-1.000000),vec3(-0.816497,0.408248,-0.408248),vec2(0.698100,1.000000)},
    Vertex{vec3(1.000000,1.000000,-1.000000),vec3(0.333333,0.666667,-0.666667),vec2(2.792410,0.301900)},
    Vertex{vec3(-1.000000,1.000000,-1.000000),vec3(-0.816497,0.408248,-0.408248),vec2(2.094305,1.000000)},
};
std::vector<GLuint> indices_with_adj {0,7,1,6,2,3,0,1,2,5,3,4,4,3,5,2,6,7,4,5,6,1,7,0,0,3,4,6,7,1,0,4,7,6,1,2,1,0,7,4,6,2,1,7,6,5,2,0,2,1,6,4,5,3,2,6,5,4,3,0,4,7,0,2,3,5,4,0,3,2,5,6,};
// clang-format off
  return Object(verts, indices_with_adj, true);
}

Object create_cube(glm::vec3 cen, float r, std::shared_ptr<Material> mat) {
  auto cube = create_cube();

  cube.set_scale(vec3(r));
  cube.set_translation(cen);
  cube.update_model();

  cube.set_material(mat);
  cube.set_hit_function(sphere_hit);
  
  return cube;
}

Object create_textured_cube() {
  // clang-format off
	constexpr std::array<float, 108> vertices {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};
	constexpr std::array<float, 108> normals {
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,

		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,
		0.0f,  0.0f,  1.0f,

	   -1.0f,  0.0f,  0.0f,
	   -1.0f,  0.0f,  0.0f,
	   -1.0f,  0.0f,  0.0f,
	   -1.0f,  0.0f,  0.0f,
	   -1.0f,  0.0f,  0.0f,
	   -1.0f,  0.0f,  0.0f,

		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,

		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,

		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f
	};
	constexpr std::array<float, 72> texture_coords {
		0.0f,  0.0f,
		1.0f,  0.0f,
		1.0f,  1.0f,
		1.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  0.0f,

		0.0f,  0.0f,
		1.0f,  0.0f,
		1.0f,  1.0f,
		1.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  0.0f,

		1.0f,  0.0f,
		1.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  0.0f,
		1.0f,  0.0f,

		1.0f,  0.0f,
		1.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  1.0f,
		0.0f,  0.0f,
		1.0f,  0.0f,

		0.0f,  1.0f,
		1.0f,  1.0f,
		1.0f,  0.0f,
		1.0f,  0.0f,
		0.0f,  0.0f,
		0.0f,  1.0f,

		0.0f,  1.0f,
		1.0f,  1.0f,
		1.0f,  0.0f,
		1.0f,  0.0f,
		0.0f,  0.0f,
		0.0f,  1.0f
	};
  // clang-format on
  std::vector<Vertex> verts;
  for (size_t i = 0; i < vertices.size(); i += 3) {
    Vertex v;
    v.pos = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
    v.norm = glm::vec3(normals[i], normals[i + 1], normals[i + 2]);
    verts.push_back(v);
  }

  size_t i = 0;
  for (auto &vert : verts) {
    vert.tex = glm::vec2(texture_coords[i], texture_coords[i + 1]);
    i += 2;
  }

  return Object(verts);
}
Object create_plane() {
  // clang-format off
std::vector<Vertex> verts {
    Vertex{vec3(-1.000000,0.000000,1.000000),vec3(-0.000000,1.000000,-0.000000),vec2(0.000000,1.000000)},
    Vertex{vec3(1.000000,0.000000,1.000000),vec3(-0.000000,1.000000,-0.000000),vec2(1.000000,1.000000)},
    Vertex{vec3(1.000000,0.000000,-1.000000),vec3(-0.000000,1.000000,-0.000000),vec2(1.000000,0.000000)},
    Vertex{vec3(-1.000000,0.000000,-1.000000),vec3(-0.000000,1.000000,-0.000000),vec2(0.000000,0.000000)},
};
std::vector<GLuint> indices_with_adj {0,2,1,0,2,3,0,1,2,0,3,2,};
// clang-format off
	return Object(verts,indices_with_adj, true);
}

Object create_plane(glm::vec3 cen, vec3 scale, std::shared_ptr<Material> mat) {
  auto obj = create_plane();

  obj.set_translation(cen);
  obj.set_scale(vec3(scale));
  obj.update_model();

  obj.set_material(mat);
  obj.set_hit_function(plane_hit);

  return obj;
}
Object create_skybox()
{
	// clang-format off
	constexpr std::array<float, 108> vertices {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
  // clang-format on

  std::vector<Vertex> verts;
  for (size_t i = 0; i < vertices.size(); i += 3) {
    Vertex v;
    v.pos = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
    verts.push_back(v);
  }
  return Object(verts);
}
} // namespace ren
