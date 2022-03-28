// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include <iostream>

int main()
{
	std::cout << "Welcome to the Flint sandbox!\n";
	return 0;
}

/*

class GraphicsObject { ... };

...

auto pGraphicsEngine = new Flint::GraphicsEngine(enableValidation);
auto pGraphicsDisplay = pGraphicsEngine->CreateDisplay(extent, title);
auto pGraphicsDevice = pGraphicsEngine->CreateDevice(deviceSpecs);

auto pCamera = pGraphicsDevice->CreateCamera<MonoCamera>(startingPos);

pCamera->MoveUp();
pCamera->MoveDown();
...

auto pTerrain = pGraphicsDevice->CreateObject<Terrain>("terrain", ...);
pTerrain->BindResource(CameraSetIndex, cameraLocation, pCamera);
pTerrain->BindResource(textureSet, textureLocation, pGraphicsEngine->LoadTexture("tree_texture.png"));
pTerrain->Show();
pTerrain->Hide();

auto pTree = pGraphicsDevice->CreateObject<Tree>("tree", ...);
auto pSecondTree = pTree->Instance(location);

auto pTreeCopy = pGraphicsDevice->CreateObjectFrom(pTree, "copy", ...);
pTreeCopy->Show(pSecondTree);	// This will hide the pSecondTree and show the pTreeCopy in that location (where the pSecondTree was).

auto pShadowNode = pGraphicsDevice->CreateProcessingNode(extent, bufferCount);
auto pDrawNode = pGraphcisDevice->CreateProcessingNode(extent, bufferCount);

auto pTarrainInstance = pShadowNode->ToDrawQueue(pTerrain);
auto pTreeInstance = pShadowNode->ToDrawQueue(pTree);

pDrawNode->Copy(pShadowNode);

pGraphicsDisplay->Present(pDrawNode);

auto pFinalNode = pGraphicsDisplay->GetFinalNode();

Note: Every single function call from pGraphicsEngine and pGraphicsDisplay are asynchronous; they will send a command to the reactor.
*/