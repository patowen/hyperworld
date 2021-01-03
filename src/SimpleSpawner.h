/*
	Copyright 2020 Patrick Owen

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
 */

#include <vector>
#include <memory>
#include "Entity.h"
#include "SimpleRenderNode.h"
#include "UserInput.h"
#include "GhostCamera.h"
#include "Scene.h"
#include "ModelBank.h"
#include "TextureBank.h"

class SimpleSpawner : public Entity {
public:
	SimpleSpawner(Scene& scene, GhostCamera& spawnCursor): scene(&scene), spawnCursor(&spawnCursor) {}

	void step(double dt, const UserInput& userInput) override {
		if (userInput.pressedThisStep(inputs.spawnDodecahedron)) {
			simpleRenderNodes.emplace_back(std::make_unique<SimpleRenderNode>(spawnCursor->getPos(), ModelHandle::DODECAHEDRON, TextureHandle::PERLIN));
			scene->addRenderNode(*simpleRenderNodes.back());
		}

		if (userInput.pressedThisStep(inputs.spawnHorosphere)) {
			simpleRenderNodes.emplace_back(std::make_unique<SimpleRenderNode>(spawnCursor->getPos(), ModelHandle::HOROSPHERE, TextureHandle::TILE));
			scene->addRenderNode(*simpleRenderNodes.back());
		}

		if (userInput.pressedThisStep(inputs.spawnPlane)) {
			simpleRenderNodes.emplace_back(std::make_unique<SimpleRenderNode>(spawnCursor->getPos(), ModelHandle::PLANE, TextureHandle::PERLIN));
			scene->addRenderNode(*simpleRenderNodes.back());
		}

		if (userInput.pressedThisStep(inputs.spawnPrism)) {
			simpleRenderNodes.emplace_back(std::make_unique<SimpleRenderNode>(spawnCursor->getPos(), ModelHandle::PRISM, TextureHandle::BLANK));
			scene->addRenderNode(*simpleRenderNodes.back());
		}

		if (userInput.pressedThisStep(inputs.spawnTree)) {
			simpleRenderNodes.emplace_back(std::make_unique<SimpleRenderNode>(spawnCursor->getPos(), ModelHandle::TREE, TextureHandle::BLANK));
			scene->addRenderNode(*simpleRenderNodes.back());
		}
	}

	class Inputs {
	public:
		InputHandle spawnDodecahedron = KeyboardButton(GLFW_KEY_1);
		InputHandle spawnHorosphere = KeyboardButton(GLFW_KEY_2);
		InputHandle spawnPlane = KeyboardButton(GLFW_KEY_3);
		InputHandle spawnPrism = KeyboardButton(GLFW_KEY_4);
		InputHandle spawnTree = KeyboardButton(GLFW_KEY_5);
	};

	Inputs inputs;

private:
	Scene* scene;
	GhostCamera* spawnCursor;
	std::vector<std::unique_ptr<SimpleRenderNode>> simpleRenderNodes;
};