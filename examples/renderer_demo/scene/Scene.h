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

static std::string string_TextureType(const TextureType type) {
  if (type == TextureType_DIFFUSE) {
    return "TextureType_DIFFUSE";
  }
  if (type == TextureType_NONE) {
    return "TextureType_NONE";
  }
  if (type == TextureType_SPECULAR) {
    return "TextureType_SPECULAR";
  }
  if (type == TextureType_AMBIENT) {
    return "TextureType_AMBIENT";
  }
  if (type == TextureType_EMISSIVE) {
    return "TextureType_EMISSIVE";
  }
  if (type == TextureType_HEIGHT) {
    return "TextureType_HEIGHT";
  }
  if (type == TextureType_NORMALS) {
    return "TextureType_NORMALS";
  }
  if (type == TextureType_SHININESS) {
    return "TextureType_SHININESS";
  }
  if (type == TextureType_OPACITY) {
    return "TextureType_OPACITY";
  }
  if (type == TextureType_DISPLACEMENT) {
    return "TextureType_DISPLACEMENT";
  }
  if (type == TextureType_LIGHTMAP) {
    return "TextureType_LIGHTMAP";
  }
  if (type == TextureType_REFLECTION) {
    return "TextureType_REFLECTION";
  }
  if (type == TextureType_BASE_COLOR) {
    return "TextureType_BASE_COLOR";
  }
  if (type == TextureType_NORMAL_CAMERA) {
    return "TextureType_NORMAL_CAMERA";
  }
  if (type == TextureType_EMISSION_COLOR) {
    return "TextureType_EMISSION_COLOR";
  }
  if (type == TextureType_METALNESS) {
    return "TextureType_METALNESS";
  }
  if (type == TextureType_DIFFUSE_ROUGHNESS) {
    return "TextureType_DIFFUSE_ROUGHNESS";
  }
  if (type == TextureType_AMBIENT_OCCLUSION) {
    return "TextureType_AMBIENT_OCCLUSION";
  }
  if (type == TextureType_UNKNOWN) {
    return "TextureType_UNKNOWN";
  }
  if (type == TextureType_SHEEN) {
    return "TextureType_SHEEN";
  }
  if (type == TextureType_CLEARCOAT) {
    return "TextureType_CLEARCOAT";
  }
  if (type == TextureType_TRANSMISSION) {
    return "TextureType_TRANSMISSION";
  }
  if (type == TextureType_MAYA_BASE) {
    return "TextureType_MAYA_BASE";
  }
  if (type == TextureType_MAYA_SPECULAR) {
    return "TextureType_MAYA_SPECULAR";
  }
  if (type == TextureType_MAYA_SPECULAR_COLOR) {
    return "TextureType_MAYA_SPECULAR_COLOR";
  }
  if (type == TextureType_MAYA_SPECULAR_ROUGHNESS) {
    return "TextureType_MAYA_SPECULAR_ROUGHNESS";
  }
  if (type == TextureType_ANISOTROPY) {
    return "TextureType_ANISOTROPY";
  }
  if (type == TextureType_GLTF_METALLIC_ROUGHNESS) {
    return "TextureType_GLTF_METALLIC_ROUGHNESS";
  }
  return "TextureType_UNKNOWN";
}

struct Texture {
  TextureType type;
  std::string path;
  uint32_t width;
  uint32_t height;
  uint32_t channels;
};

struct Mesh {
  std::string path;
  std::string name;
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
