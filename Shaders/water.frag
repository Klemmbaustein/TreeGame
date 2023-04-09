//! #include "shared.frag"

uniform float u_time;

in float Offset;

float rand(vec2 co){
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	float randval = rand(floor(v_position.xz
		+ floor(vec2(u_time * 6, sin(u_time * 2) * 3))));

	//vec3 normal = v_normal;
	vec3 normal = vec3(sin(u_time) * 0.1, 1, 0);

	RETURN_COLOR_OPACITY(
		GetLightingNormal(
			vec3(0.5, 0.8, 1) + (Offset / 15) * (1 + randval * 0.1),
			randval * 0.5 + 0.5,
			0.25 + sin(u_time) / 8,
			vec3(0, 1, 0)),
		0.9);
}