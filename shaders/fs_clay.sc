$input v_normal, v_pos

#include "../common/common.sh"
#include "forward_rendering.sh"
#include "fog.sh"

uniform vec4 PM_albedoMetal;
uniform vec4 PM_normalGloss;


void main()
{
	vec3 v = v_pos;
	vec3 n = normalize(v_normal);

#if 1
	gl_FragColor.xyz = PV_lightDirDir.xyz * 0.5 + 0.5;
#elif 0
	gl_FragColor.xyz = u_mtlAlbedoMetal.xyz;
#else
	vec3 final = ShadingFS(n, v, PM_albedoMetal, PM_normalGloss);
	
	//Fog.
	vec3 fogColor = vec3_splat(0.0);
	float fogFactor = CalcFogFactor(length(v));
	
	final = mix(fogColor, final, fogFactor);
	gl_FragColor.xyz = toGamma(final);
#endif
	gl_FragColor.w = 1.0;
}
