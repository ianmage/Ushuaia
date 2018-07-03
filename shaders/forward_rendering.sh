#include "pbr_lighting.sh"

#define MAX_POINT_LIGHT	6
#define MAX_SPOT_LIGHT	2

uniform vec4 u_mtlAlbedoMetal;
uniform vec4 u_mtlNormalGloss;

uniform vec4 u_lightsCnt;
uniform vec4 u_lightAmbColor;
uniform vec4 u_lightDirColor;
uniform vec4 u_lightDirDir;
uniform vec4 u_lightColor[MAX_POINT_LIGHT + MAX_SPOT_LIGHT];
uniform vec4 u_lightPos[MAX_POINT_LIGHT + MAX_SPOT_LIGHT];	// w for spot Outer
uniform vec4 u_lightAttnRange[MAX_POINT_LIGHT + MAX_SPOT_LIGHT];
uniform vec4 u_lightSpotDirInner[MAX_SPOT_LIGHT];


vec4 CalcColor(float n_dot_l, float spec, float atten, vec3 shadow, vec4 light_color)
{
	vec2 ds = n_dot_l * atten * float2(1, spec);
	ds = max(ds, 0);
	return ds.xxxy * light_color * float4(shadow, dot(shadow, 1) / 3);
}


vec4 CalcAmbient(vec3 normal, vec4 light_color)
{
	vec3 dir = u_lightDirDir.xyz;
	float n_dot_l = dot(normal, dir);
	return CalcColor(0.5f + 0.5f * n_dot_l, 0, 1, 1, light_color);
}

vec4 CalcDirectional(vec3 normal, vec4 light_color, vec3 view_dir, float shininess)
{
	vec4 lighting = 0;
	vec3 dir = -u_lightDirDir.xyz;
	float n_dot_l = dot(normal, dir);
	if (n_dot_l > 0)
	{
		float spec = DistributionTerm(normalize(dir - view_dir), normal, shininess).x;
		lighting = CalcColor(n_dot_l, spec, 1, 1, light_color);
	}
	return lighting;
}


vec4 CalcPoint(vec3 normal, vec4 light_color, vec3 view_dir, float shininess, vec3 pos_es, vec4 light_pos, vec4 falloff_range)
{
	vec4 lighting = 0;
	vec3 dir = light_pos.xyz - pos_es;
	float dist = length(dir);
	if (dist < falloff_range.w)
	{
		dir /= dist;
		float n_dot_l = dot(normal, dir);
		float spec = DistributionTerm(normalize(dir - view_dir), normal, shininess).x;
		float attn = AttenuationTerm(light_pos.xyz, pos_es, falloff_range.xyz);
		lighting = CalcColor(n_dot_l, spec, attn, 1, light_color);
	}
	return lighting;
}


vec4 CalcSpot(vec3 normal, vec4 light_color, vec3 view_dir, float shininess, vec3 pos_es, vec4 light_pos, vec4 falloff_range, vec4 light_dir)
{
	vec4 lighting = 0;
	vec3 dir = light_pos.xyz - pos_es;
	float dist = length(dir);
	if (dist < falloff_range.w)
	{
		dir /= dist;
		float n_dot_l = dot(normal, dir);

		float spec = DistributionTerm(normalize(dir - view_dir), normal, shininess).x;

		float cos_dir = dot(-dir, light_dir.xyz);
		float cos_cone_inner = light_dir.w;
		float cos_cone_outer = light_pos.w;
		float spot = smoothstep(cos_cone_inner, cos_cone_outer, cos_dir);

		float attn = spot * AttenuationTerm(light_pos.xyz, pos_es, falloff_range.xyz);
		lighting = CalcColor(n_dot_l, spec, attn, 1, light_color);
	}
	return lighting;
}


vec4 CalcLighting(vec3 normal, vec3 view_dir, float shininess, vec3 pos_es)
{
	vec4 lighting = 0;

	lighting += CalcAmbient(normal, u_lightAmbColor) * u_lightsCnt.x;

	lighting += CalcDirectional(normal, u_lightDirColor, view_dir, shininess);
	lighting *= u_lightsCnt.y;

	for (int i = 0; i < u_lightsCnt.z; ++i)
	{
		lighting += CalcPoint(normal, u_lightColor[i], view_dir, shininess, 
			pos_es, u_lightPos[i], u_lightAttnRange[i]);
	}

	for (int i = 0; i < u_lightsCnt.w; ++i)
	{
		lighting += CalcSpot(normal, u_lightColor[MAX_POINT_LIGHT + i], view_dir, shininess, 
			pos_es, u_lightPos[MAX_POINT_LIGHT + i], u_lightAttnRange[MAX_POINT_LIGHT + i], u_lightSpotDirInner[i]);
	}

	return lighting;
}


vec3 ShadingFS(vec3 normal, vec3 pos_es)
{
	vec3 view_dir = normalize(pos_es);
	float shininess = Glossiness2Shininess(u_mtlNormalGloss.z);

	vec4 lighting = CalcLighting(normal, view_dir, shininess, pos_es);

	vec3 c_diff = DiffuseColor(u_mtlAlbedoMetal.xyz, u_mtlAlbedoMetal.w);
	vec3 c_spec = SpecularColor(u_mtlAlbedoMetal.xyz, u_mtlAlbedoMetal.w);

	vec3 shading = CalcShading(lighting, shininess, c_diff, c_spec, view_dir, normal);
	return shading;
}
