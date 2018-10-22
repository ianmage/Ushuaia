$input v_tc0

#include "../common/common.sh"
#include "pbr_lighting.sh"

uniform vec4 PV_viewVec;
uniform vec4 PV_rtSize;

uniform vec4 PV_lightColor;	// w is attn
uniform vec4 PV_lightPos;	// w is range
//uniform vec4 PV_lightAttnOuter;
//uniform vec4 PV_lightSpotDirInner;

SAMPLER2D(s_tex0, 0);
SAMPLER2D(s_tex1, 1);


vec4 CalcColor(float n_dot_l, float spec, float atten, vec3 shadow, vec3 light_color)
{
	vec2 ds = n_dot_l * atten * vec2(1, spec);
	ds = max(ds, 0);
	return ds.xxxy * vec4(light_color,1) * vec4(shadow, dot(shadow, vec3_splat(1)) / 3);
}


vec4 CalcPoint(vec3 normal, vec4 light_color, vec3 view_dir, float shininess, vec3 pos_es, vec4 light_pos)
{
	vec4 lighting = vec4_splat(0);
	vec3 dir = light_pos.xyz - pos_es;
	float dist = length(dir);

	dir /= dist;
	float n_dot_l = dot(normal, dir);
	float spec = DistributionTerm(normalize(dir - view_dir), normal, shininess);
	float attn = AttenuationTerm(light_pos.xyz, pos_es, light_pos.w, light_color.w);
	lighting = CalcColor(n_dot_l, spec, attn, vec3_splat(1), light_color.xyz);
lighting = vec4(pos_es - light_pos.xyz, 0);
	return lighting;
}


vec4 CalcUvView(vec4 v_tc0)
{
	vec2 homoCoord = v_tc0.xy / v_tc0.w;
	vec2 viewVec = homoCoord * PV_viewVec.xy;
	vec2 uv = vec2(homoCoord.x + 1.f, 1.f - homoCoord.y) * 0.5f;
	uv += PV_viewVec.zw;

	return vec4(uv, viewVec);
}


void main()
{
	vec2 uv = gl_FragCoord.xy / PV_rtSize.xy;

	vec4 normG = texture2D(s_tex0, uv);
	vec3 normal = decodeNormalSphereMap(normG.xy);
	float shininess = Glossiness2Shininess(normG.w);

	float depth = texture2D(s_tex1, uv).x;
	vec3 vPos = depth * PV_viewVec;

	vec3 viewDir = normalize(vPos);

	vec4 lighting = CalcPoint(normal, PV_lightColor, viewDir, shininess, vPos, PV_lightPos);
lighting = vec4_splat(depth / 300);
	gl_FragColor = lighting;
}
