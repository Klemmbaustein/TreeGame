//! #include "shared.vert"

uniform float u_time;

void main()
{
	vec3 pos = TranslatePosition(a_position);
	ReturnPosition(pos + vec3(0, sin(u_time + pos.x) * sin(u_time + pos.z), 0));
}