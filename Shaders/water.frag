//! #include "shared.frag"

uniform float u_time;

float rand(vec2 co){
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	float randval = rand(floor(v_position.xz));

	vec3 normal = vec3(sin(u_time) * 0.1, 1, 0);

	RETURN_COLOR_OPACITY(
		GetLightingNormal(
			vec3(0.5, 0.8, 1) * (1 + randval * 0.1),
			randval * 0.5 + 0.5,
			0.5,
			normal),
		0.9);
}