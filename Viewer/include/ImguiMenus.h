#pragma once
#include <imgui/imgui.h>
#include "Scene.h"
#include "Renderer.h"


void DrawImguiMenus(ImGuiIO& io, Scene& scene, Renderer& renderer);
const glm::vec4& GetClearColor();
