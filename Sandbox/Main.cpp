// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

#include "Graphics/GraphicsEngine.hpp"

int main()
{
	std::cout << "Welcome to the Flint sandbox!\n";
	try
	{
		auto pGraphicsEngine = std::make_unique<Flint::GraphicsEngine>(true);
		auto pGraphicsReactor = pGraphicsEngine->CreateReactor();
	}
	catch (...)
	{
		
	}
	return 0;
}

/*

class GraphicsObject { ... }; 

...

auto pGraphicsEngine = new Flint::GraphicsEngine(enableValidation);
auto pGraphicsDisplay = pGraphicsEngine->CreateDisplay(extent, title);
auto pGraphicsReactor = pGraphicsEngine->CreateDevice(deviceSpecs);

GraphicsReactor();

auto pCamera = pGraphicsReactor->CreateCamera<MonoCamera>(startingPos);

pCamera->MoveUp();
pCamera->MoveDown();
...

auto pTerrain = pGraphicsReactor->CreateObject<Terrain>("terrain", ...);
pTerrain->BindResource(CameraSetIndex, cameraLocation, pCamera);
pTerrain->BindResource(textureSet, textureLocation, pGraphicsEngine->LoadTexture("tree_texture.png"));
pTerrain->Show();
pTerrain->Hide();

auto pTree = pGraphicsReactor->CreateObject<Tree>("tree", ...);
auto pSecondTree = pTree->Instance(location);

auto pTreeCopy = pGraphicsReactor->CreateObjectFrom(pTree, "copy", ...);
pTreeCopy->Show(pSecondTree);	// This will hide the pSecondTree and show the pTreeCopy in that location (where the pSecondTree was).

auto pShadowNode = pGraphicsReactor->CreateProcessingNode(extent, bufferCount);
auto pDrawNode = pGraphicsReactor->CreateProcessingNode(extent, bufferCount);

auto pTarrainInstance = pShadowNode->ToDrawQueue(pTerrain);
auto pTreeInstance = pShadowNode->ToDrawQueue(pTree);

pDrawNode->Copy(pShadowNode);

pGraphicsDisplay->Present(pDrawNode);

auto pFinalNode = pGraphicsDisplay->GetFinalNode();

Note: Every single function call from pGraphicsEngine and pGraphicsDisplay are asynchronous; they will send a command to the reactor.
*/