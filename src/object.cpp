#include "object.hpp"

// TODO: move objects to separate .cpp files
namespace ren {

bool sphere_hit(Object const& obj, ray const &r, float t_min, float t_max,
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
  std::vector<Vertex> verts;

  auto radius = 1.f;

  float x, y, z, xy;                           // vertex position
  float nx, ny, nz, lengthInv = 1.0f / radius; // vertex normal
  float s, t;                                  // vertex texCoord

  auto PI = 3.14159265359;
  auto sector_count{64};
  auto stack_count{64};

  float sectorStep = 2 * PI / sector_count;
  float stackStep = PI / stack_count;
  float sectorAngle, stackAngle;

  for (int i = 0; i <= stack_count; ++i) {
    stackAngle = PI / 2 - i * stackStep; // starting from pi/2 to -pi/2
    xy = radius * cosf(stackAngle);      // r * cos(u)
    z = radius * sinf(stackAngle);       // r * sin(u)

    // add (sectorCount+1) vertices per stack
    // the first and last vertices have same position and normal, but
    // different tex coords
    for (int j = 0; j <= sector_count; ++j) {
      sectorAngle = j * sectorStep; // starting from 0 to 2pi

      // vertex position (x, y, z)
      x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
      y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
      Vertex v;
      v.pos = glm::vec3(x, y, z);

      // normalized vertex normal (nx, ny, nz)
      nx = x * lengthInv;
      ny = y * lengthInv;
      nz = z * lengthInv;
      v.norm = glm::vec3(nx, ny, nz);

      // vertex tex coord (s, t) range between [0, 1]
      s = (float)j / sector_count;
      t = (float)i / stack_count;
      v.tex = glm::vec2(s, t);
      verts.push_back(v);
    }
  }
  std::vector<GLuint> indices;
  int k1, k2;
  for (int i = 0; i < stack_count; ++i) {
    k1 = i * (sector_count + 1); // beginning of current stack
    k2 = k1 + sector_count + 1;  // beginning of next stack

    for (int j = 0; j < sector_count; ++j, ++k1, ++k2) {
      // 2 triangles per sector excluding first and last stacks
      // k1 => k2 => k1+1
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      // k1+1 => k2 => k2+1
      if (i != (stack_count - 1)) {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }
    }
  }
	auto obj = Object(verts, indices);
	obj.set_type(Object::Type::sphere);
  return obj;
}

Object create_sphere(glm::vec3 cen, float r, std::shared_ptr<Material> mat) {
	auto obj = create_sphere();
	
	obj.set_translation(cen);
	obj.set_scale(vec3(r));
	// obj.set_model(mod);
	
	obj.set_material(mat);
	obj.set_hit_function(sphere_hit);
	
	return obj;	
}

Object create_cube() {
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
  // clang-format on

  std::vector<Vertex> verts;
  for (size_t i = 0; i < vertices.size(); i += 3) {
    Vertex v;
    v.pos = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
    v.norm = glm::vec3(normals[i], normals[i + 1], normals[i + 2]);
    verts.push_back(v);
  }
  return Object(verts);
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
	constexpr std::array<float, 18> vertices {
		 1.0f, -0.5f,  1.0f,
		-1.0f, -0.5f,  1.0f,
		-1.0f, -0.5f, -1.0f,

		 1.0f, -0.5f,  1.0f,
		-1.0f, -0.5f, -1.0f,
		 1.0f, -0.5f, -1.0f
	};
	constexpr std::array<float, 18> normals {
		0.0f, 1.0f, 0,
		0.0f, 1.0f, 0,
		0.0f, 1.0f, 0,
		0.0f, 1.0f, 0,
		0.0f, 1.0f, 0,
		0.0f, 1.0f, 0
	};
	// clang-format off

	std::vector<Vertex> verts;
	for (size_t i = 0; i < vertices.size(); i += 3) {
		Vertex v;
		v.pos = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
		v.norm = glm::vec3(normals[i], normals[i + 1], normals[i + 2]);
		verts.push_back(v);
	}
	return Object(verts);
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
