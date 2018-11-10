#pragma once
#define _USE_MATH_DEFINES

#include "ImguiMenus.h"
#include "MeshModel.h"
#include "Utils.h"
#include <cmath>
#include <memory>
#include <stdio.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <nfd.h>
#include <random>

bool showDemoWindow = false;
bool showScaleError = false;

glm::vec4 clearColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);
glm::vec4 lineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.00f);

int activeModel = 0;
int cameraIndex = 0;
static int trasformType = 0;
std::vector<std::string> models;
std::vector<std::string> cameras;

const glm::vec4& GetClearColor() {
	return clearColor;
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene)
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static float scale[3] = { 1.0f, 1.0f, 1.0f };
		static float translation[3] = { 0.0f, 0.0f, 0.0f };
		static float rotation[3] = { 0.0f, 0.0f, 0.0f };
		static float eye[3] = { 0.0f, 0.0f, 0.0f };
		static float at[3] = { 0.0f, 0.0f, 0.0f };
		static float up[3] = { 0.0f, 1.0f, 0.0f };
		static int counter = 0;
		static int cameraNum = 1;

		ImGui::Begin("Graphics");								// Create a window called "Hello, world!" and append into it.

		ImGui::Checkbox("Demo Window", &showDemoWindow);        // Edit bools storing our window open/close state

		ImGui::ColorEdit3("clear color", (float*)&clearColor);  // Edit 3 floats representing a color
		
		ImGui::InputFloat3("Camera Eye", eye, 2);
		ImGui::InputFloat3("Camera At", at, 2);
		ImGui::InputFloat3("Camera Up", up, 2);
		if (ImGui::Button("Add Camera")) {
			scene.AddCamera(std::make_shared<Camera>(Camera(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(at[0], at[1], at[2]), glm::vec3(up[0], up[1], up[2]), Utils::LoadMeshModel("C:\\Users\\makol\\Documents\\GitHub\\project-ms-team\\Data\\camera.obj"))));
			scene.SetActiveCameraIndex(scene.GetCameraCount() - 1);
			scene.getCamera(scene.GetActiveCameraIndex())->SetCameraLookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(at[0], at[1], at[2]), glm::vec3(up[0], up[1], up[2]));
			std::string s = "Camera ";
			cameras.push_back(s.append(std::to_string(scene.GetCameraCount())));
		}
		
		cameraIndex = scene.GetActiveCameraIndex();
		if (ImGui::ListBox("Cameras", &cameraIndex, Utils::convertStringVectorToCharArray(cameras), (int)cameras.size())) {
			scene.SetActiveCameraIndex(cameraIndex);
		}

		activeModel = scene.GetActiveModelIndex();
		if (ImGui::ListBox("Models", &activeModel, Utils::convertStringVectorToCharArray(models), (int)models.size())) {
			scene.SetActiveModelIndex(activeModel);
		}
		if (activeModel != -1) {
			auto m = scene.getModel(activeModel);
			ImGui::RadioButton("Object", &trasformType, 0); ImGui::SameLine();
			ImGui::RadioButton("World", &trasformType, 1);
			if (ImGui::ColorEdit3("line color", (float*)&lineColor)) {
				m->SetColor(lineColor);
			}
			ImGui::InputFloat3("XYZ scale", scale, 2);
			if (ImGui::Button("Set scale")) {
				if ((scale[0] == 0.0f) || (scale[1] == 0.0f) || (scale[1] == 0.0f)) {
					scale[0] = 1.0f;
					scale[1] = 1.0f;
					scale[2] = 1.0f;
					showScaleError = true;
				}
				else {
					if (trasformType) {
						m->scaleWorld(scale);
					}
					else {
						m->scaleObject(scale);
					}
				}
			}
			ImGui::InputFloat3("XYZ translation", translation, 2);
			if (ImGui::Button("Set translation")) {
				if (trasformType) {
					m->translateWorld(translation);
				}
				else {
					m->translateObject(translation);
				}
			}
			ImGui::InputFloat("X rotation", &rotation[0], 2);
			if (ImGui::Button("Set X rotation")) {
				if (trasformType) {
					m->xRotateWorld(rotation[0]);
				}
				else {
					m->xRotateObject(rotation[0]);
				}
			}
			ImGui::InputFloat("Y rotation", &rotation[1], 2);
			if (ImGui::Button("Set Y rotation")) {
				if (trasformType) {
					m->yRotateWorld(rotation[1]);
				}
				else {
					m->yRotateObject(rotation[1]);
				}
			}
			ImGui::InputFloat("Z rotation", &rotation[2], 2);
			if (ImGui::Button("Set Z rotation")) {
				if (trasformType) {
					m->zRotateWorld(rotation[2]);
				}
				else {
					m->zRotateObject(rotation[2]);
				}
			}
		}
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 4. Demonstrate creating a fullscreen menu bar and populating it.
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Model...", "CTRL+O"))
				{
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
					if (result == NFD_OKAY) {
						scene.AddModel(std::make_shared<MeshModel>(Utils::LoadMeshModel(outPath)));
						scene.SetActiveModelIndex(scene.GetModelCount() - 1);
						models.push_back(scene.getModel(scene.GetActiveModelIndex())->GetModelName());
						free(outPath);
					}
					else if (result == NFD_CANCEL) {
					}
					else {
					}

				}
				ImGui::EndMenu();
			}
		ImGui::EndMainMenuBar();
		}
	}

	// 5. Display error when trying to set the scale to 0
	if (showScaleError) {
		ImGui::Begin("Scale Use Error", &showScaleError);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Scale value cannot be set to 0!");
		if (ImGui::Button("Close")) {
			showScaleError = false;
		}
		ImGui::End();
	}
}