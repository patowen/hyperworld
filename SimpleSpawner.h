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
			simpleRenderNodes.emplace_back(std::make_unique<SimpleRenderNode>(spawnCursor->getCameraPos(), ModelHandle::DODECAHEDRON, TextureHandle::PERLIN));
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