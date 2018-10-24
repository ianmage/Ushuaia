$input v_tc0

#include "../common/common.sh"
#include "pbr_lighting.sh"
#include "util.sh"

uniform vec4 PV_lightDirColor;
uniform vec4 PV_lightDirDir;
uniform vec4 PV_lightAmbColor;
uniform vec4 PV_lightAmbViewUp;

SAMPLER2D(s_tex0, 0);


vec4 CalcColor(float n_dot_l, float spec, float atten, vec3 shadow, vec4 light_color)
{
	vec2 ds = n_dot_l * atten * vec2(1, spec);
	ds = max(ds, 0);
	return ds.xxxy * light_color * vec4(shadow, dot(shadow, vec3_splat(1)) / 3);
}


vec4 AmbLighting(vec3 normal)
{
	float n_dot_l = 0.5f + 0.5f * dot(PV_lightAmbViewUp.xyz, normal);
	return CalcColor(n_dot_l, 0, 1, vec3_splat(1), PV_lightAmbColor);
}


vec4 DirLighting(vec3 normal, vec2 uv, float gloss)
{
	vec4 ret = vec4_splat(0);

	vec3 dir = -PV_lightDirDir.xyz;
	float n_dot_l = dot(normal, dir);
	if (n_dot_l > 0) {
		vec3 viewDir = normalize(vec3(v_tc0.zw, 1));

		float shininess = Glossiness2Shininess(gloss);
		float spec = DistributionTerm(normalize(dir - viewDir), normal, shininess);
		ret = CalcColor(n_dot_l, spec, 1, vec3_splat(1), PV_lightDirColor);
	}

	return ret;
}


void main()
{
	vec2 uv = v_tc0.xy;

	vec4 normG = texture2D(s_tex0, uv);
	vec3 normal = decodeNormalSphereMap(normG.xy);

	vec4 lighting = vec4_splat(0);
	lighting += AmbLighting(normal);
	lighting += DirLighting(normal, uv, normG.w);

	gl_FragColor = lighting;
}
