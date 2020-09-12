#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <memory>

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
		fflush(stderr);
		char str[30];
		fgets(str, 20, stdin);
		return EXIT_FAILURE;
	}
}
