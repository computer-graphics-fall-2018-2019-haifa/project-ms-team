#pragma once
#define _USE_MATH_DEFINES

#include "ImguiMenus.h"
#include "MeshModel.h"
#include "Utils.h"
#include "PrimMeshModel.h"
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

int shadingType = 2;
int fogType = 0;	//no fog
int numColors = 255;
int sides = 5;
int topDiff = 2;

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
		cameras.push_back(scene->getCamera(i)->GetModelName());
	}
	for (int i = 0; i < scene->GetLightCount(); ++i) {
		lights.push_back(scene->getLight(i)->getLightName());
	}

	int activeModel = scene->GetActiveModelIndex();
	int cameraIndex = scene->GetActiveCameraIndex();
	int lightIndex = scene->GetActiveLightIndex();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	// scene controls
	{
		ImGui::Begin("Graphics");
		ImGui::Checkbox("Demo Window", &showDemoWindow);
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

		if (ImGui::Button("Add Cube")) {
			auto m = PrimMeshModel::genCube();
			scene->AddModel(std::make_shared<MeshModel>(m));
		}

		if (ImGui::Button("Add Pyramid")) {
			auto m = PrimMeshModel::genPyramid();
			scene->AddModel(std::make_shared<MeshModel>(m));
		}

		if (ImGui::Button("Add Prismatic Uniform Polyhedron")) {
			auto m = PrimMeshModel::genUniform(sides, topDiff);
			scene->AddModel(std::make_shared<MeshModel>(m));
		}

		ImGui::SliderInt("Prism Vertex", &sides, 5, 20);
		ImGui::SliderInt("Prism Vertex Top", &topDiff, 2, sides-2);

		if ((activeModel != -1) && (cameraIndex != -1)) {
			if (ImGui::Button("Active Camera Look At Active Model")) {
				auto cam = scene->getCamera(cameraIndex);
				auto model = scene->getModel(activeModel);
				glm::vec3 camPos = cam->getPosition();
				glm::vec3 modelPos = model->getPosition();
				cam->SetCameraLookAt(camPos, modelPos, glm::vec3(0, 1, 0));
			}
		}

		bool shadingChange = false;
		shadingChange |= ImGui::RadioButton("Gouraud", &shadingType, 1); ImGui::SameLine();
		shadingChange |= ImGui::RadioButton("Phong", &shadingType, 2);

		if (shadingChange) {
			if (shadingType == 1) {
				renderer.LoadGouradShaders();
			}
			if (shadingType == 2) {
				renderer.LoadPhongShaders();
			}
		}
		
		if (ImGui::ColorEdit3("Ambient Color", (float*)&ambient)) {
			scene->setAmbientColor(ambient);
		}

		if (ImGui::SliderInt("Number of Colors", &numColors, 1, 255)) {
			renderer.setNumColors(256-numColors);
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

			if (ImGui::BeginMenu("Textures"))
			{
				if (ImGui::MenuItem("Texture", "CTRL+T")) {
					nfdchar_t *outPath = NULL;
					nfdresult_t result = NFD_OpenDialog("jpg", NULL, &outPath);
					if (result == NFD_OKAY) {
						auto model = scene->getActiveModel();
						if (model != nullptr) {
							model->LoadTextures(outPath);
						}
						free(outPath);
					}
					else if (result == NFD_CANCEL) {
					}
					else {
					}

				}
				if (ImGui::MenuItem("Circles", "CTRL+P")) {
					auto model = scene->getActiveModel();
					if (model != nullptr) {
						model->LoadBombTexture();
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
			static float eye[3] = { 0.0f, 0.0f, 5.0f };
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
				static float zNear = 0.1f;
				static float zFar = 200.0f;
				static float param = 10.0f;
				static float aspect = 16.0f / 9.0f;
				bool perspectiveChanged = false;

				perspectiveChanged |= ImGui::SliderFloat("Z Near", &zNear, -100.0f, 100.0f);
				perspectiveChanged |= ImGui::SliderFloat("Z Far", &zFar, -100.0f, 200.0f);
				perspectiveChanged |= ImGui::SliderFloat("Aspect Ratio", &aspect, 0.1f, 5.0f);

				if (m->isPerspective()) {
					perspectiveChanged |= ImGui::SliderFloat("Field of View", &param, -3.1415f, 3.1415f);
					if (perspectiveChanged) {
						m->SetPerspectiveProjection(param, aspect, zNear, zFar);
					}
				}
				else {
					perspectiveChanged |= ImGui::SliderFloat("Height", &param, -45.0f, 45.0f);
					if (perspectiveChanged) {
						m->SetOrthographicProjection(param, aspect, zNear, zFar);
					}
				}

				if (ImGui::Button("Set prespective")) {
					m->SetPerspectiveProjection(param, aspect, zNear, zFar);
				}
				ImGui::SameLine();
				if (ImGui::Button("Set orthographic")) {
					m->SetOrthographicProjection(param, aspect, zNear, zFar);
				}

				if (ImGui::ArrowButton("Zoom In", 2)) {
					m->SetZoom(1.1f);
				}
				ImGui::SameLine();
				if (ImGui::ArrowButton("Zoom Out", 3)) {
					m->SetZoom(0.9f);
				}
			}

			static int cameraTrasformType = 0;
			ImGui::RadioButton("Object", &cameraTrasformType, 0); ImGui::SameLine();
			ImGui::RadioButton("World", &cameraTrasformType, 1);

			//scaling stuff
			{
				static float scaleXYZ[3] = { 1.0f, 1.0f, 1.0f };
				if (ImGui::SliderFloat("X Scale", &scaleXYZ[0], 0.0f, 10.f)) {
					if (scaleXYZ[0] != 0.0f) {
						if (cameraTrasformType) {
							m->scaleWorld(scaleXYZ);
						}
						else {
							m->scaleObject(scaleXYZ);
						}
					}
				}
				if (ImGui::SliderFloat("Y Scale", &scaleXYZ[1], 0.0f, 10.f)) {
					if (scaleXYZ[1] != 0.0f) {
						if (cameraTrasformType) {
							m->scaleWorld(scaleXYZ);
						}
						else {
							m->scaleObject(scaleXYZ);
						}
					}
				}
				if (ImGui::SliderFloat("Z Scale", &scaleXYZ[2], 0.0f, 10.f)) {
					if (scaleXYZ[2] != 0.0f) {
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
				}
				if (ImGui::SliderFloat("Z translation", &cameraTranslation[2], -10.0f, 10.f)) {
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
				if (ImGui::SliderFloat("X rotation", &cameraRotation[0], -360.0f, 360.0f)) {
					if (cameraTrasformType) {
						m->xRotateWorld(cameraRotation[0]);
					}
					else {
						m->xRotateObject(cameraRotation[0]);
					}
				}
				if (ImGui::SliderFloat("Y rotation", &cameraRotation[1], -360.0f, 360.0f)) {
					if (cameraTrasformType) {
						m->yRotateWorld(cameraRotation[1]);
					}
					else {
						m->yRotateObject(cameraRotation[1]);
					}
				}
				if (ImGui::SliderFloat("Z rotation", &cameraRotation[2], -360.0f, 360.0f)) {
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
				if (ImGui::SliderFloat("X Scale", &scaleXYZ[0], 0.0f, 10.f)) {
					if (scaleXYZ[0] != 0.0f) {
						if (modelTransformType) {
							m->scaleWorld(scaleXYZ);
						}
						else {
							m->scaleObject(scaleXYZ);
						}
					}
				}
				if (ImGui::SliderFloat("Y Scale", &scaleXYZ[1], 0.0f, 10.f)) {
					if (scaleXYZ[1] != 0.0f) {
						if (modelTransformType) {
							m->scaleWorld(scaleXYZ);
						}
						else {
							m->scaleObject(scaleXYZ);
						}
					}
				}
				if (ImGui::SliderFloat("Z Scale", &scaleXYZ[2], 0.0f, 10.f)) {
					if (scaleXYZ[2] != 0.0f) {
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
					if (modelTransformType) {
						m->translateWorld(modelTranslation);
					}
					else {
						m->translateObject(modelTranslation);
					}
				}
				if (ImGui::SliderFloat("Y translation", &modelTranslation[1], -10.0f, 10.f)) {
					if (modelTransformType) {
						m->translateWorld(modelTranslation);
					}
					else {
						m->translateObject(modelTranslation);
					}
				}
				if (ImGui::SliderFloat("Z translation", &modelTranslation[2], -10.0f, 10.f)) {
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
				if (ImGui::SliderFloat("X rotation", &modelRotation[0], -360.0f, 360.0f)) {
					if (modelTransformType) {
						m->xRotateWorld(modelRotation[0]);
					}
					else {
						m->xRotateObject(modelRotation[0]);
					}
				}
				if (ImGui::SliderFloat("Y rotation", &modelRotation[1], -360.0f, 360.0f)) {
					if (modelTransformType) {
						m->yRotateWorld(modelRotation[1]);
					}
					else {
						m->yRotateObject(modelRotation[1]);
					}
				}
				if (ImGui::SliderFloat("Z rotation", &modelRotation[2], -360.0f, 360.0f)) {
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
				static glm::vec4 ambient(0.0f);
				static glm::vec4 diffuse(0.0f);
				static glm::vec4 specular(0.0f);
				static glm::vec4 line(0.0f);
				static float k1 = 1.0f;
				static float k2 = 1.0f;
				static float k3 = 1.0f;
				static int k4 = 50;

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
				if (ImGui::Button("Toggle Texture")) {
					m->toggleTexture();
				}

				if (ImGui::Button("Toggle Wireframe Only")) {
					m->toggleWireFrameOnly();
				}
				ImGui::SameLine();
				if (ImGui::Button("Toggle Bounding Box")) {
					m->toggleBounding();
				}

				if (ImGui::Button("Use Planar Mapping")) {
					m->usePlanarMap();
				}
				ImGui::SameLine();
				if (ImGui::Button("Use Cylindrical Mapping")) {
					m->useCylindricalMap();
				}
				ImGui::SameLine();
				if (ImGui::Button("Use Spherical Mapping")) {
					m->useSphericalMap();
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
			// light type
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
						l->setXYZ(translation);
					}
					if (ImGui::SliderFloat("Y translation", &translation[1], -10.0f, 10.0f)) {
						l->setXYZ(translation);
					}
					if (ImGui::SliderFloat("Z translation", &translation[2], -10.0f, 10.0f)) {
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
		static glm::vec4 fogColor(0.0f, 0.0f, 0.0f, 1.0f);
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
			if (ImGui::SliderFloat("Fog Start", &begin, 0.0f, 10.0f)) {
				scene->setFogBegin(begin);
			}
			if (ImGui::SliderFloat("Fog End", &end, 0.0f, 20.0f)) {
				scene->setFogEnd(end);
			}
			if (ImGui::SliderFloat("Fog Density", &density, 0.0f, 20.0f)) {
				scene->setDensity(density);
			}
		}
		if (ImGui::Button("Close")) {
			showFogWindow = false;
		}
		ImGui::End();
	}

	HandleImguiInput(scene, renderer, io);
}


void HandleImguiInput(std::shared_ptr<Scene> scene, Renderer & renderer, ImGuiIO& imgui)
{
	auto model = scene->getActiveModel();
	if ((!imgui.WantCaptureKeyboard) && model != nullptr)
	{
		if (imgui.KeysDown[81]) // q
		{
			model->yRotateObject(3, true);
		}

		if (imgui.KeysDown[69]) // e
		{
			model->yRotateObject(-3, true);
		}

		if (imgui.KeysDown[GLFW_KEY_UP]) // up
		{
			model->xRotateObject(3, true);
		}

		if (imgui.KeysDown[GLFW_KEY_DOWN]) // down
		{
			model->xRotateObject(-3, true);
		}

		if (imgui.KeysDown[GLFW_KEY_LEFT]) // left
		{
			model->zRotateObject(-3, true);
		}

		if (imgui.KeysDown[GLFW_KEY_RIGHT]) // right
		{
			model->zRotateObject(3, true);
		}

		if (imgui.KeysDown[45]) // -
		{
			float mov[3] = { 0.9f, 0.9f, 0.9f };
			model->scaleObject(mov, true);
		}

		if (imgui.KeysDown[61]) // +
		{
			float mov[3] = { 1.1f, 1.1f, 1.1f };
			model->scaleObject(mov, true);
		}

		if (imgui.KeysDown[65]) // a
		{
			float mov[3] = { -0.1f, 0.0f, 0.0f };
			model->translateObject(mov, true);
		}

		if (imgui.KeysDown[68]) // d
		{
			float mov[3] = { 0.1f, 0.0f, 0.0f };
			model->translateObject(mov, true);
		}

		if (imgui.KeysDown[83]) // s
		{
			float mov[3] = { 0.0f, -0.1f, 0.0f };
			model->translateObject(mov, true);
		}

		if (imgui.KeysDown[87]) // w
		{
			float mov[3] = { 0.0f, 0.1f, 0.0f };
			model->translateObject(mov, true);
		}
	}
}