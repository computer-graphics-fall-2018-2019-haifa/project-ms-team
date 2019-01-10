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
bool showFogWindow = false;

std::shared_ptr<MeshModel> cameraModel = nullptr;
glm::vec4 clearColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);
glm::vec4 ambient = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);

int shadingType = 0;
int fogType = 0;	//no fog

const glm::vec4& GetClearColor() {
	return clearColor;
}

void DrawImguiMenus(ImGuiIO& io, std::shared_ptr<Scene> scene, Renderer& renderer)
{

	std::vector<std::string> models;
	std::vector<std::string> cameras;
	std::vector<std::string> lights;

	for (int i = 0; i < scene->GetModelCount(); ++i) {
		models.push_back(scene->getModel(i)->GetModelName());
	}
	for (int i = 0; i < scene->GetCameraCount(); ++i) {
		models.push_back(scene->getCamera(i)->GetModelName());
	}
	for (int i = 0; i < scene->GetLightCount(); ++i) {
		models.push_back(scene->getLight(i)->getLightName());
	}

	int activeModel = scene->GetActiveModelIndex();
	int cameraIndex = scene->GetActiveCameraIndex();
	int lightIndex = scene->GetActiveLightIndex();

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
				auto cam = scene->getCamera(cameraIndex);
				auto model = scene->getModel(activeModel);
				glm::vec3 camPos = cam->getPosition();
				glm::vec3 modelPos = model->getPosition();
				cam->SetCameraLookAt(camPos, modelPos, glm::vec3(0, 1, 0));
			}
		}

		ImGui::RadioButton("Flat", &shadingType, 0); ImGui::SameLine();
		ImGui::RadioButton("Gouraud", &shadingType, 1); ImGui::SameLine();
		ImGui::RadioButton("Phong", &shadingType, 2);
		scene->setShadingType(shadingType);
		
		if (ImGui::ColorEdit3("Ambient Color", (float*)&ambient)) {
			scene->setAmbientColor(ambient);
		}

		if (ImGui::Button("Rainbow mode!")) {
			scene->toggleRainbow();
		}
		if (ImGui::Button("Circles mode!")) {
			scene->toggleCircles();
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
						scene->AddModel(std::make_shared<MeshModel>(Utils::LoadMeshModel(outPath)));
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

	if (showCameraWindow) {
		ImGui::Begin("Camera Control Window", &showCameraWindow);
		
		if (cameraModel != nullptr) {
			static float eye[3] = { 0.0f, 0.0f, 0.0f };
			static float at[3] = { 0.0f, 0.0f, 0.0f };
			static float up[3] = { 0.0f, 1.0f, 0.0f };
			ImGui::InputFloat3("Camera Eye", eye, 2);
			ImGui::InputFloat3("Camera At", at, 2);
			ImGui::InputFloat3("Camera Up", up, 2);
			if (ImGui::Button("Add Camera")) {
				scene->AddCamera(std::make_shared<Camera>(Camera(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(at[0], at[1], at[2]), glm::vec3(up[0], up[1], up[2]), *cameraModel)));
			}

			if (cameraIndex != -1) {
				ImGui::SameLine();
				if (ImGui::Button("Set Camera Look At")) {
					auto m = scene->getActiveCamera();
					m->SetCameraLookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(at[0], at[1], at[2]), glm::vec3(up[0], up[1], up[2]));
				}
			}
			if (ImGui::ListBox("Cameras", &cameraIndex, Utils::convertStringVectorToCharArray(cameras), (int)cameras.size())) {
				scene->SetActiveCameraIndex(cameraIndex);
			}
		}
		else {
			ImGui::Text("Load a camera model before adding a camera");
		}

		if (cameraIndex != -1) {
			auto m = scene->getCamera(cameraIndex);
			//perspective stuff
			{
				float zNear = m->getZNear();
				float zFar = m->getZFar();
				float param = m->getParam();
				float aspect = m->getAspect();
				bool perspectiveChanged = false;

				perspectiveChanged |= ImGui::SliderFloat("Z Near", &zNear, -100.0f, 100.f);
				perspectiveChanged |= ImGui::SliderFloat("Z Far", &zFar, -100.0f, 100.f);
				perspectiveChanged |= ImGui::SliderFloat("Aspect Ratio", &aspect, 1.0f, 5.0f);

				if (m->isPerspective()) {
					perspectiveChanged |= ImGui::InputFloat("Field of View", &param, -45.0f, 45.0f);
					if (perspectiveChanged) {
						m->SetPerspectiveProjection(param, 16.0f / 9.0f, zNear, zFar);
					}
				}
				else {
					perspectiveChanged |= ImGui::InputFloat("Height", &param, 0.0f, 100.0f);
					if (perspectiveChanged) {
						m->SetOrthographicProjection(param, 16.0f / 9.0f, zNear, zFar);
					}
				}

				if (ImGui::Button("Set prespective")) {
					m->SetPerspectiveProjection(param, aspect, zNear, zFar);
				}
				ImGui::SameLine();
				if (ImGui::Button("Set orthographic")) {
					m->SetOrthographicProjection(param, aspect, zNear, zFar);
				}

				if (ImGui::ArrowButton("Zoom In", 0)) {
					m->SetZoom(1.1f);
				}
				if (ImGui::ArrowButton("Zoom Out", 1)) {
					m->SetZoom(0.9f);
				}
			}

			static int cameraTrasformType = 0;
			ImGui::RadioButton("Object", &cameraTrasformType, 0); ImGui::SameLine();
			ImGui::RadioButton("World", &cameraTrasformType, 1);

			//scaling stuff
			{
				static float scaleXYZ[3] = { 1.0f, 1.0f, 1.0f };
				if (ImGui::SliderFloat("X Scale", &scaleXYZ[0], -10.0f, 10.f)) {
					if (scaleXYZ[0] != 0.0f) {
						scaleXYZ[1] = 1.0f;
						scaleXYZ[2] = 1.0f;
						if (cameraTrasformType) {
							m->scaleWorld(scaleXYZ);
						}
						else {
							m->scaleObject(scaleXYZ);
						}
					}
				}
				if (ImGui::SliderFloat("Y Scale", &scaleXYZ[1], -10.0f, 10.f)) {
					if (scaleXYZ[1] != 0.0f) {
						scaleXYZ[0] = 1.0f;
						scaleXYZ[2] = 1.0f;
						if (cameraTrasformType) {
							m->scaleWorld(scaleXYZ);
						}
						else {
							m->scaleObject(scaleXYZ);
						}
					}
				}
				if (ImGui::SliderFloat("Z Scale", &scaleXYZ[2], -10.0f, 10.f)) {
					if (scaleXYZ[2] != 0.0f) {
						scaleXYZ[0] = 1.0f;
						scaleXYZ[1] = 1.0f;
						if (cameraTrasformType) {
							m->scaleWorld(scaleXYZ);
						}
						else {
							m->scaleObject(scaleXYZ);
						}
					}
				}
			}

			//translation stuff
			{
				static float cameraTranslation[3] = {0.0f, 0.0f, 0.0f};
				if (ImGui::SliderFloat("X translation", &cameraTranslation[0], -10.0f, 10.f)) {
					cameraTranslation[1] = 0.0f;
					cameraTranslation[2] = 0.0f;
					if (cameraTrasformType) {
						m->translateWorld(cameraTranslation);
					}
					else {
						m->translateObject(cameraTranslation);
					}
				}
				if (ImGui::SliderFloat("Y translation", &cameraTranslation[1], -10.0f, 10.f)) {
					cameraTranslation[0] = 0.0f;
					cameraTranslation[2] = 0.0f;
					if (cameraTrasformType) {
						m->translateWorld(cameraTranslation);
					}
					else {
						m->translateObject(cameraTranslation);
					}
				}
				if (ImGui::SliderFloat("Z translation", &cameraTranslation[2], -10.0f, 10.f)) {
					cameraTranslation[0] = 0.0f;
					cameraTranslation[1] = 0.0f;
					if (cameraTrasformType) {
						m->translateWorld(cameraTranslation);
					}
					else {
						m->translateObject(cameraTranslation);
					}
				}
			}

			//rotation stuff
			{
				static float cameraRotation[3] = {0.0f, 0.0f, 0.0f};
				if (ImGui::SliderFloat("X rotation", &cameraRotation[0], -270.0f, 270.0f)) {
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

		if (ImGui::ListBox("Models", &activeModel, Utils::convertStringVectorToCharArray(models), (int)models.size())) {
			scene->SetActiveModelIndex(activeModel);
		}

		if (activeModel != -1) {

			auto m = scene->getActiveModel();

			static int modelTransformType = 0;
			ImGui::RadioButton("Object", &modelTransformType, 0); ImGui::SameLine();
			ImGui::RadioButton("World", &modelTransformType, 1);

			//scaling stuff
			{
				static float scaleXYZ[3] = { 1.0f, 1.0f, 1.0f };
				if (ImGui::SliderFloat("X Scale", &scaleXYZ[0], -10.0f, 10.f)) {
					if (scaleXYZ[0] != 0.0f) {
						scaleXYZ[1] = 1.0f;
						scaleXYZ[2] = 1.0f;
						if (modelTransformType) {
							m->scaleWorld(scaleXYZ);
						}
						else {
							m->scaleObject(scaleXYZ);
						}
					}
				}
				if (ImGui::SliderFloat("Y Scale", &scaleXYZ[1], -10.0f, 10.f)) {
					if (scaleXYZ[1] != 0.0f) {
						scaleXYZ[0] = 1.0f;
						scaleXYZ[2] = 1.0f;
						if (modelTransformType) {
							m->scaleWorld(scaleXYZ);
						}
						else {
							m->scaleObject(scaleXYZ);
						}
					}
				}
				if (ImGui::SliderFloat("Z Scale", &scaleXYZ[2], -10.0f, 10.f)) {
					if (scaleXYZ[2] != 0.0f) {
						scaleXYZ[0] = 1.0f;
						scaleXYZ[1] = 1.0f;
						if (modelTransformType) {
							m->scaleWorld(scaleXYZ);
						}
						else {
						m->scaleObject(scaleXYZ);
						}
					}
				}
			}

			//translation stuff
			{
			static float modelTranslation[3] = { 0.0f, 0.0f, 0.0f };
			if (ImGui::SliderFloat("X translation", &modelTranslation[0], -10.0f, 10.f)) {
				modelTranslation[1] = 0.0f;
				modelTranslation[2] = 0.0f;
				if (modelTransformType) {
					m->translateWorld(modelTranslation);
				}
				else {
					m->translateObject(modelTranslation);
				}
			}
			if (ImGui::SliderFloat("Y translation", &modelTranslation[1], -10.0f, 10.f)) {
				modelTranslation[0] = 0.0f;
				modelTranslation[2] = 0.0f;
				if (modelTransformType) {
					m->translateWorld(modelTranslation);
				}
				else {
					m->translateObject(modelTranslation);
				}
			}
			if (ImGui::SliderFloat("Z translation", &modelTranslation[2], -10.0f, 10.f)) {
				modelTranslation[0] = 0.0f;
				modelTranslation[1] = 0.0f;
				if (modelTransformType) {
					m->translateWorld(modelTranslation);
				}
				else {
					m->translateObject(modelTranslation);
				}
			}
			}

			//rotation stuff
			{
				static float modelRotation[3] = { 0.0f, 0.0f, 0.0f };
				if (ImGui::SliderFloat("X rotation", &modelRotation[0], -270.0f, 270.0f)) {
					if (modelTransformType) {
						m->xRotateWorld(modelRotation[0]);
					}
					else {
						m->xRotateObject(modelRotation[0]);
					}
				}
				if (ImGui::SliderFloat("Y rotation", &modelRotation[1], -180.0f, 180.0f)) {
					if (modelTransformType) {
						m->yRotateWorld(modelRotation[1]);
					}
					else {
						m->yRotateObject(modelRotation[1]);
					}
				}
				if (ImGui::SliderFloat("Z rotation", &modelRotation[2], -180.0f, 180.0f)) {
					if (modelTransformType) {
						m->zRotateWorld(modelRotation[2]);
					}
					else {
						m->zRotateObject(modelRotation[2]);
					}
				}
			}

			//color stuff
			{
				glm::vec4 ambient(0.0f);
				glm::vec4 diffuse(0.0f);
				glm::vec4 specular(0.0f);
				glm::vec4 line(0.0f);
				static float k1 = 1.0f;
				static float k2 = 1.0f;
				static float k3 = 1.0f;
				static int k4 = 1;

				if (ImGui::ColorEdit3("Line Color", (float*)&line)) {
					m->SetLineColor(line);
				}
				if (ImGui::ColorEdit3("Ambient Color", (float*)&ambient)) {
					m->SetAmbientColor(ambient);
				}
				if (ImGui::ColorEdit3("Diffuse Color", (float*)&diffuse)) {
					m->SetDiffuseColor(diffuse);
				}
				if (ImGui::ColorEdit3("Specular Color", (float*)&specular)) {
					m->SetSpecularColor(specular);
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
				if (ImGui::SliderInt("Specualr Exponent", &k4, 1, 100)) {
					m->setSpecularExp((float)k4);
				}

			}

			//others
			{
				if (ImGui::Button("Toggle Bounding Box")) {
					m->toggleBounding();
				}
				if (ImGui::Button("Toggle Vertex Normals")) {
					m->toggleNormals();
				}
				if (ImGui::Button("Flip Vertex Normals")) {
					m->toggleFlipNormals();
				}
				if (ImGui::Button("Toggle Face Normals")) {
					m->toggleFaceNormals();
				}
				if (ImGui::Button("Flip Face Normals")) {
					m->toggleFlipFaceNormals();
				}
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

		if (ImGui::ListBox("Lights", &lightIndex, Utils::convertStringVectorToCharArray(lights), (int)lights.size())) {
			scene->SetActiveLightIndex(lightIndex);
		}

		if (ImGui::Button("Add Light")) {
			scene->AddLight(std::make_shared<Light>(Light(0, "Light")));
		}

		if (lightIndex != -1) {
			auto l = scene->getActiveLight();
			int lightType = l->getType();

			{
				bool typeChange = false;
				
				typeChange |= ImGui::RadioButton("Parallel", &lightType, 0); ImGui::SameLine();
				typeChange |= ImGui::RadioButton("Point", &lightType, 1);
				if (typeChange) {
					l->setType(lightType);
				}
			}

			// light moving
			{
				static glm::vec4 lightColor(0.0f);
				if (ImGui::ColorEdit3("Intensity", (float*)&lightColor)) {
					l->setIntensity(lightColor);
				}

				if (lightType == 1) {
					static float translation[3] = {0.0f, 0.0f, 0.0f};
					if (ImGui::SliderFloat("X translation", &translation[0], -10.0f, 10.0f)) {
						translation[1] = 0.0f;
						translation[2] = 0.0f;
						l->setXYZ(translation);
					}
					if (ImGui::SliderFloat("Y translation", &translation[1], -10.0f, 10.0f)) {
						translation[0] = 0.0f;
						translation[2] = 0.0f;
						l->setXYZ(translation);
					}
					if (ImGui::SliderFloat("Z translation", &translation[2], -10.0f, 10.0f)) {
						translation[0] = 0.0f;
						translation[1] = 0.0f;
						l->setXYZ(translation);
					}
				}
				if (lightType == 0) {
					static float direction[3] = { 0.0f, 0.0f, 0.0f };
					if (ImGui::InputFloat3("Light Direction", direction, 2)) {
						l->setDirection(glm::vec3(direction[0], direction[1], direction[2]));
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
			scene->setFogColor(fogColor);
		}

		ImGui::RadioButton("No fog", &fogType, 0); ImGui::SameLine();
		ImGui::RadioButton("Linear", &fogType, 1); ImGui::SameLine();
		ImGui::RadioButton("Exp fog", &fogType, 2); ImGui::SameLine();
		ImGui::RadioButton("Exp squared", &fogType, 3);

		scene->setFogType(fogType);
		if (fogType) {
			if (ImGui::SliderFloat("Fog Start", &begin, -10.0f, 100.0f)) {
				scene->setFogBegin(begin);
			}
			if (ImGui::SliderFloat("Fog End", &end, -5.0f, 200.0f)) {
				scene->setFogEnd(end);
			}
			if (ImGui::SliderFloat("Fog Density", &density, -20.0f, 20.0f)) {
				scene->setDensity(density);
			}
		}
		if (ImGui::Button("Close")) {
			showFogWindow = false;
		}
		ImGui::End();
	}
}