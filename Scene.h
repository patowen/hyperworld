#pragma once
#include <unordered_set>
#include <memory>
#include "RenderNode.h"
#include "GhostCamera.h"
#include "RenderContext.h"

class Scene {
public:
	void addRenderNode(RenderNode& renderNode) {
		renderNodes.insert(&renderNode);
	}

	void addEntity(GhostCamera& camera) {
		entities.insert(&camera);
	}

	void setCamera(GhostCamera& camera) {
		this->camera = &camera;
	}

	void step(double dt, const UserInput& userInput) {
		for (GhostCamera* entity : entities) {
			entity->step(dt, userInput);
		}
	}

	void render(RenderContext& context) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (camera == nullptr) {
			return; // No camera, no picture
		}

		double ratio = (double)context.getWidth() / (double)context.getHeight();
		double zoom = camera->getCameraZoom();
		context.setProjection(VectorMath::perspective(ratio * zoom, zoom, 0.01, 10));

		context.resetModelView();
		context.addModelView(VectorMath::isometricInverse(camera->getCameraPos()));

		context.useShader();
		for (RenderNode* renderNode : renderNodes) {
			renderNode->render(context);
		}
	}

private:
	std::unordered_set<GhostCamera*> entities;
	std::unordered_set<RenderNode*> renderNodes;
	GhostCamera* camera = nullptr;
};
