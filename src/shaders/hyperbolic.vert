#version 150

/*
	Copyright 2021 Patrick Owen

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

uniform mat4 projection;
uniform mat4 modelView;
in vec4 vPos;
in vec4 vNormal;
in vec2 vTexCoord;
out vec4 pos;
out vec4 normal;
out vec2 texCoord;
out vec4 pos_global;

void main()
{
	pos_global = modelView * vPos;
	gl_Position = projection * pos_global;
	pos = vPos;
	normal = vNormal;
	texCoord = vTexCoord;
}
