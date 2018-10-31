#include "pbr_lighting.sh"

#define MAX_POINT_LIGHT	6
#define MAX_SPOT_LIGHT	2

uniform vec4 PV_lightsCnt;
uniform vec4 PV_lightAmbColor;
uniform vec4 PV_lightDirColor;
uniform vec4 PV_lightDirDir;
uniform vec4 PV_lightColor[MAX_POINT_LIGHT + MAX_SPOT_LIGHT];	// w for attn
uniform vec4 PV_lightPos[MAX_POINT_LIGHT + MAX_SPOT_LIGHT];	// w for spot Outer
uniform vec4 PV_lightAttnOuter[MAX_POINT_LIGHT + MAX_SPOT_LIGHT];
uniform vec4 PV_lightSpotDirInner[MAX_SPOT_LIGHT];


vec4 CalcColor(float n_dot_l, float spec, float atten, vec3 shadow, vec4 light_color)
{
	vec2 ds = n_dot_l * atten * vec2(1, spec);
	ds = max(ds, 0);
	return ds.xxxy * light_color * vec4(shadow, dot(shadow, vec3_splat(1)) / 3);
}


vec4 CalcAmbient(vec3 normal, vec4 light_color, vec3 light_dir)
{
	vec3 dir = light_dir;
	float n_dot_l = dot(normal, dir);
#if 0
	return CalcColor(0.5f + 0.5f * n_dot_l, 0, 1, vec3_splat(1), light_color);
#else
	return CalcColor(1, 0, 1, vec3_splat(1), light_color);
#endif
}

vec4 CalcDirectional(vec3 normal, vec4 light_color, vec3 light_dir, vec3 view_dir, float shininess)
{
	vec4 lighting = vec4_splat(0);
	vec3 dir = -light_dir;
	float n_dot_l = dot(normal, dir);
	if (n_dot_l > 0)
	{
		float spec = DistributionTerm(normalize(dir - view_dir), normal, shininess);
		lighting = CalcColor(n_dot_l, spec, 1, vec3_splat(1), light_color);
	}
	return lighting;
}


vec4 CalcPoint(vec3 normal, vec4 light_color, vec3 view_dir, float shininess, vec3 pos_es, vec4 light_pos, vec4 falloff_range)
{
	vec4 lighting = vec4_splat(0);
	vec3 dir = light_pos.xyz - pos_es;
	float dist = length(dir);
	if (dist < falloff_range.w)
	{
		dir /= dist;
		float n_dot_l = dot(normal, dir);
		float spec = DistributionTerm(normalize(dir - view_dir), normal, shininess);
		float attn = AttenuationTerm(light_pos.xyz, pos_es, falloff_range.xyz);
		lighting = CalcColor(n_dot_l, spec, attn, vec3_splat(1), light_color);
	}
	return lighting;
}


vec4 CalcSpot(vec3 normal, vec4 light_color, vec3 view_dir, float shininess, vec3 pos_es, vec4 light_pos, vec4 falloff_range, vec4 light_dir)
{
	vec4 lighting = vec4_splat(0);
	vec3 dir = light_pos.xyz - pos_es;
	float dist = length(dir);
	if (dist < falloff_range.w)
	{
		dir /= dist;
		float n_dot_l = dot(normal, dir);

		float spec = DistributionTerm(normalize(dir - view_dir), normal, shininess);

		float cos_dir = dot(-dir, light_dir.xyz);
		float cos_cone_inner = light_dir.w;
		float cos_cone_outer = light_pos.w;
		float spot = smoothstep(cos_cone_inner, cos_cone_outer, cos_dir);

		float attn = spot * AttenuationTerm(light_pos.xyz, pos_es, falloff_range.xyz);
		lighting = CalcColor(n_dot_l, spec, attn, vec3_splat(1), light_color);
	}
	return lighting;
}


vec4 CalcLighting(vec3 normal, vec3 view_dir, float shininess, vec3 pos_es)
{
	vec4 lighting = vec4_splat(0);

	lighting += CalcAmbient(normal, PV_lightAmbColor, PV_lightDirDir.xyz) * PV_lightsCnt.x;

	lighting += CalcDirectional(normal, PV_lightDirColor, PV_lightDirDir.xyz, view_dir, shininess);
	lighting *= PV_lightsCnt.y;

	//for (int i = 0; i < PV_lightsCnt.z; ++i)
	//{
	//	lighting += CalcPoint(normal, PV_lightColor[i], view_dir, shininess, 
	//		pos_es, PV_lightPos[i], PV_lightAttnOuter[i]);
	//}

	//for (int i = 0; i < PV_lightsCnt.w; ++i)
	//{
	//	lighting += CalcSpot(normal, PV_lightColor[MAX_POINT_LIGHT + i], view_dir, shininess, 
	//		pos_es, PV_lightPos[MAX_POINT_LIGHT + i], PV_lightAttnOuter[MAX_POINT_LIGHT + i], PV_lightSpotDirInner[i]);
	//}

	return lighting;
}


vec3 ShadingFS(vec3 normal, vec3 pos_es, vec4 mtlAlbedoMetal, vec4 mtlNormalGloss)
{
	vec3 view_dir = normalize(pos_es);
	float shininess = Glossiness2Shininess(mtlNormalGloss.w);

	vec4 lighting = CalcLighting(normal, view_dir, shininess, pos_es);

	vec3 c_diff = DiffuseColor(mtlAlbedoMetal.xyz, mtlAlbedoMetal.w);
	vec3 c_spec = SpecularColor(mtlAlbedoMetal.xyz, mtlAlbedoMetal.w);

	vec3 shading = CalcShading(lighting, shininess, c_diff, c_spec, view_dir, normal);
	return shading;
}
