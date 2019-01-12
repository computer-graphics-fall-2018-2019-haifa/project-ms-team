#pragma once
#include <imgui/imgui.h>
#include "Scene.h"
#include "Renderer.h"
#include "PrimMeshModel.h"


void DrawImguiMenus(ImGuiIO& io, std::shared_ptr<Scene> scene, Renderer& renderer);
const glm::vec4& GetClearColor();
void HandleImguiInput(std::shared_ptr<Scene> scene, Renderer& renderer, ImGuiIO& imgui);
