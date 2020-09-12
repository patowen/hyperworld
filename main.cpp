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
