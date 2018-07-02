#include "../common/common.sh"

#define MAX_POINT_LIGHT	6
#define MAX_SPOT_LIGHT	2

uniform vec4 u_materialKa;
uniform vec4 u_materialKd;
uniform vec4 u_materialKs;

uniform vec4 u_lightAmbColor;
uniform vec4 u_lightDirColor;
uniform vec4 u_lightDirDir;
uniform vec4 u_lightColor[MAX_POINT_LIGHT + MAX_SPOT_LIGHT];
uniform vec4 u_lightPos[MAX_POINT_LIGHT + MAX_SPOT_LIGHT];
uniform vec4 u_lightAttnOuter[MAX_POINT_LIGHT + MAX_SPOT_LIGHT];
uniform vec4 u_lightSpotDirInner[MAX_SPOT_LIGHT];


float attenuation(float _dist, vec3 _attn)
{
	return 1.0 / ( _attn.x                  //const
				 + _attn.y * _dist          //linear
				 + _attn.z * _dist * _dist  //quadrantic
				 );
}


float spot(vec3 _ld, vec4 _dirInner, float _outer)
{
	float ldotd = max(0.0, dot(_ld, _dirInner.xyz));
	float spotV = (ldotd - _dirInner.w) / (_outer - _dirInner.w);
	return clamp(spotV, 0.0, 1.0);
}


struct Light
{
	vec3 dir;
	float attn;
};

Light evalPointLight(vec3 _viewPos, vec3 _pos, vec3 _attn)
{
	Light light;

	vec3 ld  = _pos - _viewPos;
	light.dir = normalize(ld);

	light.attn = attenuation(length(ld), _attn);

	return light;
}

Light evalSpotLight(vec3 _viewPos, vec3 _pos, vec4 _attnOuter, vec4 _dirInner)
{
	Light light;

	vec3 ld  = _pos - _viewPos;
	light.dir = normalize(ld);

	light.attn = attenuation(length(ld), _attnOuter.xyz);

	float falloff = spot(light.dir, _dirInner, _attnOuter.w);
	falloff = mix(falloff, 1.0, step(90, _attnOuter.w));
	light.attn *= falloff;

	return light;
}


vec2 lit(vec3 _ld, vec3 _n, vec3 _vd, float _exp)
{
	//diff
	float ndotl = dot(_n, _ld);

	//spec
	vec3 r = 2.0*ndotl*_n - _ld; // reflect(_ld, _n);
	float rdotv = dot(r, _vd);
	float spec = step(0.0, ndotl) * pow(max(0.0, rdotv), _exp) * (2.0 + _exp)/8.0;

	return max(vec2(ndotl, spec), 0.0);
}


float DirectionalLighting(vec3 light_dir, vec3 normal)
{
	return dot(-light_dir, normal);
}

float PointLighting(vec3 light_pos, vec3 pos, vec3 normal)
{
	return dot(normalize(light_pos - pos), normal);
}

float SpotLighting(vec3 light_pos, vec3 light_dir, vec2 cos_cone, vec3 pos)
{
	// cos_cone is (cos_outer_cone, cos_inner_cone)
	
	vec3 v = normalize(pos - light_pos);
	float cos_direction = dot(v, light_dir);

	return smoothstep(cos_cone.x, cos_cone.y, cos_direction);
}


vec3 FresnelTerm(vec3 light_vec, vec3 halfway_vec, vec3 c_spec)
{
	float e_n = saturate(dot(light_vec, halfway_vec));
	return c_spec > 0 ? c_spec + (1 - c_spec) * exp2(-(5.55473f * e_n + 6.98316f) * e_n) : 0;
}

float SpecularNormalizeFactor(float shininess)
{
	return (shininess + 2) / 8;
}

vec3 DistributionTerm(vec3 halfway_vec, vec3 normal, float shininess)
{
	return exp((shininess + 0.775f) * (max(dot(halfway_vec, normal), 0.0f) - 1));
}

vec3 SpecularTerm(vec3 c_spec, vec3 light_vec, vec3 halfway_vec, vec3 normal, float shininess)
{
	return SpecularNormalizeFactor(shininess) * DistributionTerm(halfway_vec, normal, shininess)
		* FresnelTerm(light_vec, halfway_vec, c_spec);
}


// diffuse only
vec3 CalcBRDFShading(vec3 c_diff, vec3 l, vec3 n)
{
	return max(c_diff * dot(n, l), 0);
}
// diffuse + specular
vec3 CalcBRDFShading(vec3 c_diff, vec3 c_spec, float shininess, vec3 l, vec3 h, vec3 n)
{
	return max((c_diff + SpecularTerm(c_spec, l, h, n, shininess)) * dot(n, l), 0);
}


float AttenuationTerm(vec3 light_pos, vec3 pos, vec3 atten)
{
	vec3 v = light_pos - pos;
	float d2 = dot(v, v);
	float d = sqrt(d2);
	return 1 / dot(atten, vec3(1, d, d2));
}

vec3 CalcShading(vec3 diff_lighting, vec3 spec_lighting, float shininess,
				vec3 diffuse, vec3 specular, vec3 view_dir, vec3 normal)
{
	return vec3(max(diff_lighting * diffuse
		+ SpecularNormalizeFactor(shininess) * spec_lighting
			* FresnelTerm(normalize(view_dir), normal, specular), 0));
}

vec3 CalcShading(float4 lighting, float shininess, vec3 diffuse, vec3 specular,
				vec3 view_dir, vec3 normal)
{
	const vec3 RGB_TO_LUM = vec3(0.2126f, 0.7152f, 0.0722f);
	vec3 diff_lighting = lighting.rgb;
	vec3 spec_lighting = lighting.a / (dot(lighting.rgb, RGB_TO_LUM) + 1e-6f) * lighting.rgb;
	return CalcShading(diff_lighting, spec_lighting, shininess, diffuse, specular,
		view_dir, normal);
}


// Environment BRDF

float CalcPrefilteredEnvMip(float glossiness, int num_mip)
{
	return num_mip - glossiness * num_mip;
}

vec3 CalcPrefilteredEnvVec(vec3 normal, vec3 view)
{
	return -reflect(view, normal);
}

vec3 CalcEnvDiffuse(vec3 prefiltered_env, vec3 c_diff)
{
	return prefiltered_env * c_diff;
}

vec3 CalcEnvSpecular(vec3 prefiltered_env, vec3 c_spec, float glossiness, vec3 normal, vec3 view)
{
	float n_dot_v = saturate(dot(normal, view));
	glossiness = max(0.5f / 16, glossiness);
	vec2 env_brdf;
	float4 tmp = ((float4(-4.996914762f, 7.253111161f, -1.963867075f, -0.170416225f) * glossiness
		+ float4(8.993475061f, -15.42473953f, 4.712593001f, 0.448102365f)) * glossiness
		+ float4(-4.928234727f, 10.95286522f, -3.852980973f, -0.389332014f)) * glossiness
		+ float4(0.846124834f, -1.596815751f, 1.005729748f, 0.113484128f);
	env_brdf.x = (((tmp.x * n_dot_v + tmp.y) * n_dot_v + tmp.z) * n_dot_v) + tmp.w;
	tmp = ((float4(-0.684077741f, 1.316163916f, -0.70869252f, 0.073264505f) * glossiness
		+ float4(0.719315865f, -1.384932814f, 0.775880046f, -0.105818706f)) * glossiness
		+ float4(-0.101034049f, 0.18680998f, -0.117919199f, 0.030727381f)) * glossiness
		+ float4(0.001922126f, 0.006697305f, -0.015741592f, 0.007229544f);
	env_brdf.y = (((tmp.x * n_dot_v + tmp.y) * n_dot_v + tmp.z) * n_dot_v) + tmp.w;
	env_brdf = saturate(env_brdf);
	return prefiltered_env * (c_spec * env_brdf.x + env_brdf.y);
}
