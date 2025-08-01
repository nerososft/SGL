//
// Created by neo on 25-5-3.
//

#ifndef SCENE_H
#define SCENE_H
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

struct Material {
  glm::vec4 ambientColor;
  glm::vec4 diffuseColor;
  glm::vec4 specularColor;
  glm::vec4 transparentColor;
  glm::vec4 emissiveColor;
  glm::vec4 reflectiveColor;
  glm::vec4 shininess;
};

enum TextureType {
  TextureType_NONE = 0,
  TextureType_DIFFUSE = 1,
  TextureType_SPECULAR = 2,
  TextureType_AMBIENT = 3,
  TextureType_EMISSIVE = 4,
  TextureType_HEIGHT = 5,
  TextureType_NORMALS = 6,
  TextureType_SHININESS = 7,
  TextureType_OPACITY = 8,
  TextureType_DISPLACEMENT = 9,
  TextureType_LIGHTMAP = 10,
  TextureType_REFLECTION = 11,
  TextureType_BASE_COLOR = 12,
  TextureType_NORMAL_CAMERA = 13,
  TextureType_EMISSION_COLOR = 14,
  TextureType_METALNESS = 15,
  TextureType_DIFFUSE_ROUGHNESS = 16,
  TextureType_AMBIENT_OCCLUSION = 17,
  TextureType_UNKNOWN = 18,
  TextureType_SHEEN = 19,
  TextureType_CLEARCOAT = 20,
  TextureType_TRANSMISSION = 21,
  TextureType_MAYA_BASE = 22,
  TextureType_MAYA_SPECULAR = 23,
  TextureType_MAYA_SPECULAR_COLOR = 24,
  TextureType_MAYA_SPECULAR_ROUGHNESS = 25,
  TextureType_ANISOTROPY = 26,
  TextureType_GLTF_METALLIC_ROUGHNESS = 27,
};

struct Texture {
  TextureType type;
  std::string path;
};

struct Mesh {
  std::vector<Vertex> vertexData;
  std::vector<uint32_t> indicesData;
  std::vector<Texture> textures;
  Material material{};
  glm::mat4 transform{};
};

struct Light {
  glm::vec4 position;
  glm::vec4 color;
  glm::vec4 direction;
};

#endif // SCENE_H
