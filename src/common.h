//
// Created by User on 26. 4. 29
//

#ifndef __COMMON_H__
#define __COMMON_H__

#include <memory>
#include <optional>
// clang-format off
#include <glad/glad.h>
#include <glfw/glfw3.h>
// clang-format on
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>
#include <string>


#define CLASS_PTR( klassName )                                                                                         \
    class klassName;                                                                                                   \
    using klassName##UPtr = std::unique_ptr< klassName >;                                                              \
    using klassName##Ptr = std::shared_ptr< klassName >;                                                               \
    using klassName##WPtr = std::weak_ptr< klassName >;

std::optional<std::string> LoadTextFile(const std::string& filename);

glm::vec3 GetAttenuationCoeff(float distance);

#endif // __COMMON_H__
