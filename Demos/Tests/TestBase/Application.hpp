// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Instance.hpp"
#include "GraphicsCore/Device.hpp"

#include <imgui.h>

namespace Flint
{
	class GraphicsScene;

	class Application
	{
	public:
		Application();
		~Application();

		void PrepareNewFrame();
		void UpdateGraphicsScenes();
		void DrawGraphicsScenes();

		GraphicsScene* CreateGraphicsScene(const std::string& name, const FBox2D extent);
		GraphicsScene* GetGraphicsScene(const std::string& name) { return &mGraphicsScenes.at(name); }

		std::shared_ptr<GeometryStore> CreateGeometryStore(const std::string& name, const std::vector<ShaderAttribute>& vertexAttributes, UI64 indexSize, const BufferMemoryProfile profile = BufferMemoryProfile::Automatic);
		std::shared_ptr<GeometryStore> GetGeometryStore(const std::string& name) const { return pGeometryStores.at(name); }

		std::shared_ptr<Instance> GetInstance() const { return pInstance; }
		std::shared_ptr<Device> GetDevice() const { return pDevice; }

		std::shared_ptr<Instance> pInstance = nullptr;
		std::shared_ptr<Device> pDevice = nullptr;

		std::unordered_map<std::string, GraphicsScene> mGraphicsScenes;
		std::unordered_map<std::string, std::shared_ptr<GeometryStore>> pGeometryStores;

		ImGuiDockNodeFlags mDockspaceFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	};
}
