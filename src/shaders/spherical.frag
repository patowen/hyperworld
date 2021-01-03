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

uniform sampler2D texture_sampler;
uniform vec4 light;
in vec4 pos;
in vec4 normal;
in vec2 texCoord;
in vec4 pos_global;
out vec4 fragColor;

void main()
{
	/*float dot_pos_pos = dot(pos, pos);
	float dot_pos_light = dot(pos, light);
	float dot_pos_normal = dot(pos, normal);
	float numerator = dot(light, normal) * dot_pos_pos - dot_pos_light * dot_pos_normal;
	float denominator_light = dot(light, light) * dot_pos_pos - dot_pos_light * dot_pos_light;
	float denominator_normal = dot(normal, normal) * dot_pos_pos - dot_pos_normal * dot_pos_normal;
	
	float directness = max(0.0, -(gl_FrontFacing ? 1.0 : -1.0) * numerator / sqrt(denominator_light * denominator_normal));*/
	// float depth_factor = 1.0 / sqrt(max(1e-3, dot_pos_pos));
	// gl_FragDepth = -pos_global.z / ((pos_global.w * depth_factor) + 1.0) * depth_factor;
	float directness = 1.0f;
	fragColor = vec4(texture(texture_sampler, texCoord).rgb * directness, 1.0);
}
