#pragma once
#include <imgui/imgui.h>
#include "Scene.h"
#include "Renderer.h"

void DrawImguiMenus(ImGuiIO& io, Scene& scene);
const glm::vec4& GetClearColor();
void draw_line(Renderer& r);
