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

	void addEntity(Entity& entity) {
		entities.insert(&entity);
	}

	void setCamera(GhostCamera& camera) {
		this->camera = &camera;
	}

	void step(double dt, const UserInput& userInput) {
		for (Entity* entity : entities) {
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
		context.addModelView(camera->getCameraTransform());

		for (RenderNode* renderNode : renderNodes) {
			renderNode->render(context);
		}
	}

private:
	std::unordered_set<Entity*> entities;
	std::unordered_set<RenderNode*> renderNodes;
	GhostCamera* camera = nullptr;
};
