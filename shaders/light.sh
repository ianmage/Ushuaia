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
uniform vec4 u_lightDirInner[MAX_SPOT_LIGHT];


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


vec3 evalMtlAmbient()
{
	vec3 amb = u_lightAmbColor.xyz * u_materialKa.xyz;
	return amb;
}


vec3 evalMtlDirect(Light l, vec3 n, vec3 vd, vec4 _color)
{
	vec2 lc = lit(l.dir, n, vd, u_materialKs.w) * l.attn;

	vec3 diff = _color.xyz * u_materialKd.xyz * lc.x;
	vec3 spec = _color.xyz * _color.w * u_materialKs.xyz * lc.y;

	return diff + spec;
}
