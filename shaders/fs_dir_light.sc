$input v_tc0

#include "../common/common.sh"
#include "pbr_lighting.sh"

uniform vec4 PV_lightDirColor;
uniform vec4 PV_lightDirDir;

SAMPLER2D(s_tex0, 0);
SAMPLER2D(s_tex1, 1);


vec4 CalcColor(float n_dot_l, float spec, float atten, vec3 shadow, vec4 light_color)
{
	vec2 ds = n_dot_l * atten * vec2(1, spec);
	ds = max(ds, 0);
	return ds.xxxy * light_color * vec4(shadow, dot(shadow, vec3_splat(1)) / 3);
}


void main()
{
	vec4 normG = texture2D(s_tex0, v_tc0.xy);
	vec3 normal = decodeNormalSphereMap(normG.xy);
	float shininess = Glossiness2Shininess(normG.w);

	float depth = texture2D(s_tex1, v_tc0.xy).x;
	vec3 vPos = vec3(depth * v_tc0.zw, depth);

	vec3 viewDir = normalize(vPos);

	float spec = 0;
	vec4 lighting = vec4_splat(0);
	vec3 dir = -PV_lightDirDir.xyz;
	float n_dot_l = dot(normal, dir);
	if (n_dot_l > 0)
	{
		spec = DistributionTerm(normalize(dir - viewDir), normal, shininess);
		lighting = CalcColor(n_dot_l, spec, 1, vec3_splat(1), PV_lightDirColor);
	}

	gl_FragColor = lighting;
}
