#ifndef __COMMON_H__
#define __COMMON_H__

#include <memory>
#include <string>
#include <optional>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>

#define CLASS_PTR(_ClassName) \
class _ClassName; \
using _ClassName ## UPtr = std::unique_ptr<_ClassName>; \
using _ClassName ## Ptr = std::shared_ptr<_ClassName>; \
using _ClassName ## WPtr = std::weak_ptr<_ClassName>; 

std::optional<std::string> LoadTextFile(const std::string& filename);

#endif // __COMMON_H__