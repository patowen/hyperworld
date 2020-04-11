#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include "ContextWrapper.h"
#include "WindowWrapper.h"

void entry(void) {
	ContextWrapper outer;
	WindowWrapper windowWrapper(outer);
	windowWrapper.renderLoop();
}

int main(void) {
	try {
		entry();
		return EXIT_SUCCESS;
	} catch (const std::runtime_error &e) {
		fprintf(stderr, "Fatal error: %s\n", e.what());
		char str[30];
		gets_s(str, 20);
		return EXIT_FAILURE;
	}
}

// Code organization plan:
/*

Maybe: Use RenderNodes. Each RenderNode has a render method that takes a RenderContext which includes camera information and will make
the appropriate OpenGL calls. Some RenderNodes will render by having subnodes. The scene itself is a RenderNode.

Definitely: A scene has a bunch of objects that want to render themselves, but the way they are rendered is based on some kind of context.
For simplicity, they can be rendered individually in any order, and the context is given as a parameter, containing all information such
as the texture bank, model bank, shaders, and transformation matrices.

A Model is referenced with a ModelHandle, and a Texture is referenced with a TextureHandle. These handles will give functionality that
pointers would not necessarily be able to do efficiently.

*/