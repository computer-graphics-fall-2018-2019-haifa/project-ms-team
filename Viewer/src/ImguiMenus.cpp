#pragma once
#define _USE_MATH_DEFINES

#include "ImguiMenus.h"
#include "MeshModel.h"
#include "Utils.h"
#include <cmath>
#include <memory>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <nfd.h>
#include <random>

bool showDemoWindow = false;
bool showModelWindow = false;
bool showCameraWindow = false;
bool showLightWindow = false;
bool showScaleError = false;
bool showFogWindow = false;

std::shared_ptr<MeshModel> cameraModel = nullptr;
glm::vec4 clearColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);

int activeModel = -1;
int cameraIndex = -1;
int lightIndex = -1;
int shadingType = 0;
int fogType = 0;	//no fog
std::vector<std::string> models;
std::vector<std::string> cameras;
std::vector<std::string> lights;

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
		ImGui::Begin("Graphics");								// Create a window called "Hello, world!" and append into it.
		ImGui::Checkbox("Demo Window", &showDemoWindow);        // Edit bools storing our window open/close state
		ImGui::ColorEdit3("background color", (float*)&clearColor);
		
		if (ImGui::Button("Show Camera Controls")) {
			showCameraWindow = true;
		}

		if (ImGui::Button("Show Model Controls")) {
			showModelWindow = true;
		}
		
		if (ImGui::Button("Show Light Controls")) {
			showLightWindow = true;
		}

		if (ImGui::Button("Show Fog Controls")) {
			showFogWindow = true;
		}

		if ((activeModel != -1) && (cameraIndex != -1)) {
			if (ImGui::Button("Active Camera Look At Active Model")) {
				auto cam = scene.getCamera(cameraIndex);
				auto model = scene.getModel(activeModel);

				glm::vec3 camPos = cam->getPosition();
				glm::vec4 homCamPos(camPos.x, camPos.y, camPos.z, 1);
				homCamPos = cam->GetWorldTransformation() * (cam->GetObjectTransformation() * homCamPos);
				camPos = glm::vec3(homCamPos.x, homCamPos.y, homCamPos.z) / homCamPos.z;

				glm::vec3 modelPos = model->getPosition();
				glm::vec4 homModelPos(modelPos.x, modelPos.y, modelPos.z, 1);
				homModelPos = model->GetWorldTransformation() * (model->GetObjectTransformation() * homModelPos);
				modelPos = glm::vec3(homModelPos.x, homModelPos.y, homModelPos.z) / homModelPos.z;

				cam->SetCameraLookAt(camPos, modelPos, glm::vec3(0, 1, 0));
			}
		}

		ImGui::RadioButton("Flat", &shadingType, 0); ImGui::SameLine();
		ImGui::RadioButton("Gouraud", &shadingType, 1); ImGui::SameLine();
		ImGui::RadioButton("Phong", &shadingType, 2); ImGui::SameLine();
		ImGui::RadioButton("Something", &shadingType, 3);
		scene.setShadingType(shadingType);


		if (ImGui::Button("4X Aliasing")) {
			scene.toggleAliasing();
		}

		if (ImGui::Button("Rainbow mode!")) {
			scene.toggleRainbow();
		}
		if (ImGui::Button("Circles mode!")) {
			scene.toggleCircles();
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// Demonstrate creating a fullscreen menu bar and populating it.
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load Model...", "CTRL+O")) {
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
					if (result == NFD_OKAY) {
						scene.AddModel(std::make_shared<MeshModel>(Utils::LoadMeshModel(outPath)));
						activeModel = scene.GetModelCount() - 1;
						scene.SetActiveModelIndex(activeModel);
						models.push_back(scene.getModel(scene.GetActiveModelIndex())->GetModelName());
						free(outPath);
					}
					else if (result == NFD_CANCEL) {
					}
					else {
					}

				}
				if (ImGui::MenuItem("Load Camera Model...", "CTRL+1")) {
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
					if (result == NFD_OKAY) {
						cameraModel = std::make_shared<MeshModel>(Utils::LoadMeshModel(outPath));
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

	// Display error when trying to set the scale to 0
	if (showScaleError) {
		ImGui::Begin("Scale Use Error", &showScaleError);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Scale value cannot be set to 0!");
		if (ImGui::Button("Close")) {
			showScaleError = false;
		}
		ImGui::End();
	}

	if (showCameraWindow) {
		ImGui::Begin("Camera Control Window", &showCameraWindow);
		static float eye[3] = { 0.0f, 0.0f, 0.0f };
		static float at[3] = { 0.0f, 0.0f, 0.0f };
		static float up[3] = { 0.0f, 1.0f, 0.0f };
		static float z[2] = { 1.0f, 10.0f };
		static float fovy[2] = { 45.0f, 1.0f };
		static float zoom = 1.0f;
		static int cameraTrasformType = 0;
		static float cameraScale[3] = { 1.0f, 1.0f, 1.0f };
		static float cameraTranslation[3] = { 0.0f, 0.0f, 0.0f };
		static float cameraRotation[3] = { 0.0f, 0.0f, 0.0f };

		if (cameraModel != nullptr) {
			ImGui::InputFloat3("Camera Eye", eye, 2);
			ImGui::InputFloat3("Camera At", at, 2);
			ImGui::InputFloat3("Camera Up", up, 2);
			if (ImGui::Button("Add Camera")) {
				scene.AddCamera(std::make_shared<Camera>(Camera(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(at[0], at[1], at[2]), glm::vec3(up[0], up[1], up[2]), *cameraModel)));
				cameraIndex = scene.GetCameraCount() - 1;
				scene.SetActiveCameraIndex(cameraIndex);
				std::string s = "Camera ";
				cameras.push_back(s.append(std::to_string(scene.GetCameraCount())));
			}

			cameraIndex = scene.GetActiveCameraIndex();
			if (cameraIndex != -1) {
				ImGui::SameLine();
				if (ImGui::Button("Set Camera Look At")) {
					auto m = scene.getCamera(cameraIndex);
					m->SetCameraLookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(at[0], at[1], at[2]), glm::vec3(up[0], up[1], up[2]));
				}
			}
			if (ImGui::ListBox("Cameras", &cameraIndex, Utils::convertStringVectorToCharArray(cameras), (int)cameras.size())) {
				scene.SetActiveCameraIndex(cameraIndex);
			}
		}
		else {
			ImGui::Text("Load a camera model before adding a camera");
		}
		if (cameraIndex != -1) {
			auto m = scene.getCamera(cameraIndex);
			static glm::vec4 lineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.00f);
			ImGui::RadioButton("Object", &cameraTrasformType, 0); ImGui::SameLine();
			ImGui::RadioButton("World", &cameraTrasformType, 1);
			if (ImGui::ColorEdit3("line color", (float*)&lineColor)) {
				m->SetColor(lineColor);
			}
			ImGui::InputFloat2("Z near/Z far", z, 2);
			ImGui::InputFloat2("fovy/height", fovy, 2);
			if (ImGui::Button("Set prespective")) {
				m->SetPerspectiveProjection(fovy[0], 16 / 9, z[0], z[1]);
			}
			ImGui::SameLine();
			if (ImGui::Button("Set orthographic")) {
				m->SetOrthographicProjection(fovy[1], 16 / 9, z[0], z[1]);
			}
			
			if (ImGui::InputFloat("Camera Zoom", &zoom, 0.1f, 1, 2)) {
				m->SetZoom(1/zoom);
			}

			ImGui::InputFloat3("XYZ scale", cameraScale, 2);
			if (ImGui::Button("Set scale")) {
				if ((cameraScale[0] == 0.0f) || (cameraScale[1] == 0.0f) || (cameraScale[1] == 0.0f)) {
					cameraScale[0] = 1.0f;
					cameraScale[1] = 1.0f;
					cameraScale[2] = 1.0f;
					showScaleError = true;
				}
				else {
					if (cameraTrasformType) {
						m->scaleWorld(cameraScale);
					}
					else {
						m->scaleObject(cameraScale);
					}
				}
			}

			if (ImGui::SliderFloat("X translation", &cameraTranslation[0], -10.0f, 10.f)) {
				if (cameraTrasformType) {
					m->translateWorld(cameraTranslation);
				}
				else {
					m->translateObject(cameraTranslation);
				}
			}
			if (ImGui::SliderFloat("Y translation", &cameraTranslation[1], -10.0f, 10.f)) {
				if (cameraTrasformType) {
					m->translateWorld(cameraTranslation);
				}
				else {
					m->translateObject(cameraTranslation);
				}
			}if (ImGui::SliderFloat("Z translation", &cameraTranslation[2], -10.0f, 10.f)) {
				if (cameraTrasformType) {
					m->translateWorld(cameraTranslation);
				}
				else {
					m->translateObject(cameraTranslation);
				}
			}
			if (ImGui::SliderFloat("X rotation", &cameraRotation[0], -180.0f, 180.0f)) {
				if (cameraTrasformType) {
					m->xRotateWorld(cameraRotation[0]);
				}
				else {
					m->xRotateObject(cameraRotation[0]);
				}
			}
			if (ImGui::SliderFloat("Y rotation", &cameraRotation[1], -180.0f, 180.0f)) {
				if (cameraTrasformType) {
					m->yRotateWorld(cameraRotation[1]);
				}
				else {
					m->yRotateObject(cameraRotation[1]);
				}
			}
			if (ImGui::SliderFloat("Z rotation", &cameraRotation[2], -180.0f, 180.0f)) {
				if (cameraTrasformType) {
					m->zRotateWorld(cameraRotation[2]);
				}
				else {
					m->zRotateObject(cameraRotation[2]);
				}
			}
		}
		else {
			ImGui::Text("Add a camera to show camera transformations");
		}

		if (ImGui::Button("Close")) {
			showCameraWindow = false;
		}
		ImGui::End();
	}

	if (showModelWindow) {
		ImGui::Begin("Model Control Window", &showModelWindow);
		static int trasformType = 0;
		static float scale[3] = { 1.0f, 1.0f, 1.0f };
		static float translation[3] = { 0.0f, 0.0f, 0.0f };
		static float rotation[3] = { 0.0f, 0.0f, 0.0f };
		static float k1 = 1.0f;
		static float k2 = 1.0f;
		static float k3 = 1.0f;
		static float k4 = 1.0f;
		static glm::vec4 lineColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.00f);

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

			if (ImGui::Button("Toggle Bounding Box")) {
				m->toggleBounding();
			}

			if (ImGui::Button("Toggle Vertex Normals")) {
				m->toggleNormals();
			}
			ImGui::SameLine();
			if (ImGui::Button("Flip Vertex Normals")) {
				m->toggleFlipNormals();
			}

			if (ImGui::Button("Toggle Face Normals")) {
				m->toggleFaceNormals();
			}
			ImGui::SameLine();
			if (ImGui::Button("Flip Face Normals")) {
				m->toggleFlipFaceNormals();
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
			if (ImGui::SliderFloat("X translation", &translation[0], -10.0f, 10.f)) {
				if (trasformType) {
					m->translateWorld(translation);
				}
				else {
					m->translateObject(translation);
				}
			}
			if (ImGui::SliderFloat("Y translation", &translation[1], -10.0f, 10.f)) {
				if (trasformType) {
					m->translateWorld(translation);
				}
				else {
					m->translateObject(translation);
				}
			}if (ImGui::SliderFloat("Z translation", &translation[2], -10.0f, 10.f)) {
				if (trasformType) {
					m->translateWorld(translation);
				}
				else {
					m->translateObject(translation);
				}
			}
			if (ImGui::SliderFloat("X rotation", &rotation[0], -180.0f, 180.0f)) {
				if (trasformType) {
					m->xRotateWorld(rotation[0]);
				}
				else {
					m->xRotateObject(rotation[0]);
				}
			}
			if (ImGui::SliderFloat("Y rotation", &rotation[1], -180.0f, 180.0f)) {
				if (trasformType) {
					m->yRotateWorld(rotation[1]);
				}
				else {
					m->yRotateObject(rotation[1]);
				}
			}
			if (ImGui::SliderFloat("Z rotation", &rotation[2], -180.0f, 180.0f)) {
				if (trasformType) {
					m->zRotateWorld(rotation[2]);
				}
				else {
					m->zRotateObject(rotation[2]);
				}
			}
			if (ImGui::SliderFloat("Ambient K", &k1, 0.0f, 1.0f)) {
				m->setKAmbient(k1);
			}
			if (ImGui::SliderFloat("Diffuse K", &k2, 0.0f, 1.0f)) {
				m->setKDiffuse(k2);
			}
			if (ImGui::SliderFloat("Specualr K", &k3, 0.0f, 1.0f)) {
				m->setKSpecular(k3);
			}
			if (ImGui::InputFloat("Specualr Exponent", &k4, 0.5f)) {
				m->setSpecularExp(k4);
			}

		}
		else {
			ImGui::Text("Add a model to show model transformations");
		}
		if (ImGui::Button("Close")) {
			showModelWindow = false;
		}
		ImGui::End();
	}

	if (showLightWindow) {
		ImGui::Begin("Light Control Window", &showLightWindow);
		static int lightType = 0;
		static int trasformType = 0;
		static float translation[3] = { 0.0f, 0.0f, 0.0f };
		static float rotation[3] = { 0.0f, 0.0f, 0.0f };
		static float direction[3] = { 0.0f, 0.0f, 0.0f };
		static glm::vec4 lineColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.00f);

		lightIndex = scene.GetActiveLightIndex();
		if (ImGui::ListBox("Lights", &lightIndex, Utils::convertStringVectorToCharArray(lights), (int)lights.size())) {
			scene.SetActiveLightIndex(lightIndex);
		}

		if (ImGui::Button("Add light")) {
			scene.AddLight(std::make_shared<Light>(Light(lightType, "light")));
			lightIndex = scene.GetLightCount() - 1;
			scene.SetActiveLightIndex(lightIndex);
			std::string s = "Light ";
			lights.push_back(s.append(std::to_string(lightIndex)));
		}

		if (lightIndex != -1) {
			auto l = scene.getLight(lightIndex);
			bool typeChange = false;
			typeChange = ImGui::RadioButton("Ambient", &lightType, 0); ImGui::SameLine();
			typeChange |= ImGui::RadioButton("Parallel", &lightType, 1); ImGui::SameLine();
			typeChange |= ImGui::RadioButton("Point", &lightType, 2);
			if (typeChange) {
				l->setType(lightType);
			}
			lightType = l->getType();

			ImGui::RadioButton("Object", &trasformType, 0); ImGui::SameLine();
			ImGui::RadioButton("World", &trasformType, 1);

			if (ImGui::ColorEdit3("Color", (float*)&lineColor)) {
				l->SetColor(lineColor);
			}

			if (lightType == 2) {
				if (ImGui::InputFloat3("XYZ translation", translation, 2)) {
					if (trasformType) {
						l->translateWorld(translation);
					}
					else {
						l->translateObject(translation);
					}
				}
			}
			if (lightType == 1) {
				if (ImGui::InputFloat3("Light Direction", direction, 2)) {
					l->setDirection(glm::vec3(direction[0], direction[1], direction[2]));
				}
			}
			if ((lightType == 2) && (trasformType)) {
				if (ImGui::SliderFloat("X rotation", &rotation[0], -180.0f, 180.0f)) {
					if (trasformType) {
						l->xRotateWorld(rotation[0]);
					}
					else {
						l->xRotateObject(rotation[0]);
					}
				}
				if (ImGui::SliderFloat("Y rotation", &rotation[1], -180.0f, 180.0f)) {
					if (trasformType) {
						l->yRotateWorld(rotation[1]);
					}
					else {
						l->yRotateObject(rotation[1]);
					}
				}
				if (ImGui::SliderFloat("Z rotation", &rotation[2], -180.0f, 180.0f)) {
					if (trasformType) {
						l->zRotateWorld(rotation[2]);
					}
					else {
						l->zRotateObject(rotation[2]);
					}
				}
			}
		}
		else {
			ImGui::Text("Add a light to show light controls");
		}
		if (ImGui::Button("Close")) {
			showLightWindow = false;
		}
		ImGui::End();
	}
	if (showFogWindow) {
		static float end = 2.0f;
		static float begin = 0.0f;
		static float density = 1.0f;
		static glm::vec4 fogColor(0.0f, 0.0f, 0.0f, 1.00f);
		ImGui::Begin("Fog Control Window", &showFogWindow);
		
		if (ImGui::ColorEdit3("Fog color", (float*)&fogColor)) {
			scene.setFogColor(fogColor);
		}

		ImGui::RadioButton("No fog", &fogType, 0); ImGui::SameLine();
		ImGui::RadioButton("Linear", &fogType, 1); ImGui::SameLine();
		ImGui::RadioButton("Exp fog", &fogType, 2); ImGui::SameLine();
		ImGui::RadioButton("Exp squared", &fogType, 3);
		
		scene.setFogType(fogType);

		if (ImGui::InputFloat("Fog Start", &begin, 1)) {
			scene.setFogBegin(begin);
		}
		if (ImGui::InputFloat("Fog End", &end, 1)) {
			scene.setFogEnd(end);
		}


		if (ImGui::InputFloat("Fog Density", &density, 0.5)) {
			scene.setDensity(density);
		}

		if (ImGui::Button("Close")) {
			showFogWindow = false;
		}
		ImGui::End();
	}
}