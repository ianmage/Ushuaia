$input a_position, a_normal, a_texcoord0
#ifndef DEFERRED
$output v_normal, v_pos, v_tc0
#else
$output v_normal, v_tc0
#endif

#include "../common/common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));

	vec4 normal = vec4(a_normal.xyz * 2.0 - 1.0, 0);
	v_normal = normalize(mul(u_modelView, normal).xyz);
#ifndef DEFERRED
	v_pos = mul(u_modelView, vec4(a_position, 1.0)).xyz;
#endif
	v_tc0 = a_texcoord0;
}
