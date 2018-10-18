// PBR lighting utils

#define MAX_SHININESS	8192.0f

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
	float e_n = clamp(dot(light_vec, halfway_vec), 0.f, 1.f);
	//return c_spec > 0 ? c_spec + (1 - c_spec) * exp2(-(5.55473f * e_n + 6.98316f) * e_n) : 0;
	vec3 ret = c_spec + (1 - c_spec) * exp2(-(5.55473f * e_n + 6.98316f) * e_n);
	return ret * sign(c_spec);
}

float SpecularNormalizeFactor(float shininess)
{
	return (shininess + 2) / 8;
}

float DistributionTerm(vec3 halfway_vec, vec3 normal, float shininess)
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


#if 0
float AttenuationTerm(vec3 light_pos, vec3 pos, vec3 atten)
{
	vec3 v = light_pos - pos;
	float d2 = dot(v, v);
	float d = sqrt(d2);
	return 1 / dot(atten, vec3(1, d, d2));
}
#else
float AttenuationTerm(vec3 light_pos, vec3 pos, float range, float attn)
{
	vec3 v = light_pos - pos;
	float d2 = dot(v, v);
	//float inv_d = inversesqrt(d2);
	float dr2 = d2 / (range * range);
	float num = clamp(1 - dr2*dr2, 0, 1);
	float ret = num * num / (d2 + 1);
	return ret;
}
#endif

vec3 CalcShading(vec3 diff_lighting, vec3 spec_lighting, float shininess,
				vec3 diffuse, vec3 specular, vec3 view_dir, vec3 normal)
{
	return vec3(max(diff_lighting * diffuse
		+ SpecularNormalizeFactor(shininess) * spec_lighting
			* FresnelTerm(view_dir, normal, specular), 0));
}

vec3 CalcShading(vec4 lighting, float shininess, vec3 diffuse, vec3 specular,
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
	float n_dot_v = clamp(dot(normal, view), 0.f, 1.f);
	glossiness = max(0.5f / 16, glossiness);
	vec2 env_brdf;
	vec4 tmp = ((vec4(-4.996914762f, 7.253111161f, -1.963867075f, -0.170416225f) * glossiness
		+ vec4(8.993475061f, -15.42473953f, 4.712593001f, 0.448102365f)) * glossiness
		+ vec4(-4.928234727f, 10.95286522f, -3.852980973f, -0.389332014f)) * glossiness
		+ vec4(0.846124834f, -1.596815751f, 1.005729748f, 0.113484128f);
	env_brdf.x = (((tmp.x * n_dot_v + tmp.y) * n_dot_v + tmp.z) * n_dot_v) + tmp.w;
	tmp = ((vec4(-0.684077741f, 1.316163916f, -0.70869252f, 0.073264505f) * glossiness
		+ vec4(0.719315865f, -1.384932814f, 0.775880046f, -0.105818706f)) * glossiness
		+ vec4(-0.101034049f, 0.18680998f, -0.117919199f, 0.030727381f)) * glossiness
		+ vec4(0.001922126f, 0.006697305f, -0.015741592f, 0.007229544f);
	env_brdf.y = (((tmp.x * n_dot_v + tmp.y) * n_dot_v + tmp.z) * n_dot_v) + tmp.w;
	env_brdf = clamp(env_brdf, 0.f, 1.f);
	return prefiltered_env * (c_spec * env_brdf.x + env_brdf.y);
}

// Metalness

vec3 DiffuseColor(vec3 albedo, float metalness)
{
	return albedo * (1 - metalness);
}

vec3 SpecularColor(vec3 albedo, float metalness)
{
	return mix(vec3_splat(0.04), albedo, vec3_splat(metalness));
}

// Glossiness

float Shininess2Glossiness(float shininess)
{
	return log2(shininess) / log2(MAX_SHININESS);
}

float Glossiness2Shininess(float glossiness)
{
	return pow(MAX_SHININESS, glossiness);
}
