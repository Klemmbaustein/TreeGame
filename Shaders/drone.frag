//! #include "shared.frag"
uniform sampler2D u_texture;
uniform sampler2D u_texture2;
uniform vec3 u_diffuse = vec3(1);
uniform vec3 u_emissive = vec3(0);
uniform float u_specularsize = 0;
uniform float u_specularstrength = 0;
uniform float u_time;

void main()
{
	bool transparent = false;
	vec4 TexResult = ((int(u_time * 16.f) % 2) == 0) ? texture(u_texture, v_texcoord) : texture(u_texture2, v_texcoord);
	vec3 texColor;
	texColor = u_diffuse * TexResult.xyz;;
	transparent = TexResult.w < 0.33f ? true : false;
	if(transparent)
		discard;
	vec3 normal = normalize(v_modelnormal);
	vec3 color = GetLighting(texColor, u_specularstrength, u_specularsize) + u_emissive;
	RETURN_COLOR(color);
}