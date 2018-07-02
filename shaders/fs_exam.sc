$input v_normal, v_view

#include "light.sh"


void main()
{
	vec3 v = v_view;
	vec3 vd = -normalize(v_view);
	vec3 n = v_normal;
	/*
	Light light = evalLight(v, u_lightPosition, u_spotDirection, u_spotInner, u_spotOuter, u_lightAttnParams);

	vec2 lc = lit(light.ld, n, vd, u_materialKs.w) * light.attn;
	Shader shader = evalShader(lc.x, lc.y);
	*/
	//Fog.
	vec3 fogColor = vec3_splat(0.0);
	float fogDensity = 0.0035;
	float LOG2 = 1.442695;
	float z = length(v);
	float fogFactor = clamp(1.0/exp2(fogDensity*fogDensity*z*z*LOG2), 0.0, 1.0);
	/*
	vec3 color = u_color.xyz;

	vec3 ambient = shader.ambi * color;
	vec3 brdf    = (shader.diff + shader.spec) * color;

	vec3 final = toGamma(abs(ambient + brdf));
	*/
	vec3 final = v_normal * 0.5 + 0.5;
	gl_FragColor.xyz = mix(fogColor, final, fogFactor);
	gl_FragColor.w = 1.0;
}
