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

float hypdot(vec4 v1, vec4 v2)
{
	return dot(v1.xyz, v2.xyz) - v1.w * v2.w;
}

void main()
{
	float hypdot_pos_pos = hypdot(pos, pos);
	float hypdot_pos_light = hypdot(pos, light);
	float hypdot_pos_normal = hypdot(pos, normal);
	float numerator = hypdot(light, normal) * hypdot_pos_pos - hypdot_pos_light * hypdot_pos_normal;
	float denominator_light = hypdot(light, light) * hypdot_pos_pos - hypdot_pos_light * hypdot_pos_light;
	float denominator_normal = hypdot(normal, normal) * hypdot_pos_pos - hypdot_pos_normal * hypdot_pos_normal;
	
	float directness = max(0.0, -(gl_FrontFacing ? 1.0 : -1.0) * numerator / sqrt(denominator_light * denominator_normal));
	float depth_factor = 1.0 / sqrt(max(1e-3, -hypdot_pos_pos));
	gl_FragDepth = -pos_global.z / ((pos_global.w * depth_factor) + 1.0) * depth_factor;
	fragColor = vec4(texture(texture_sampler, texCoord).rgb * directness, 1.0);
}
