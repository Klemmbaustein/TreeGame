//! #include "shared.vert"

uniform float u_time;

out float Offset;

void main()
{
	vec3 pos = TranslatePosition(a_position);

	Offset = clamp((sin(u_time + pos.x) * sin(u_time + pos.z)) * 250 / length(pos.xz), -500, 500);

	ReturnPosition(pos + vec3(0, Offset, 0));
}