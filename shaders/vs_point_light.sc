$input a_position
$output v_tc0

#include "../common/common.sh"

uniform vec4 PV_lightPos;
uniform vec4 PV_lightAttnOuter;


void main()
{
	vec3 vPos = a_position * PV_lightPos.w + PV_lightPos.xyz;
	gl_Position = mul(u_proj, vec4(vPos, 1.0));

	float u = gl_Position.x * 0.5 + 0.5;
	float v = 0.5 - gl_Position.y * 0.5;

	v_tc0 = vec4(u, v, 0, 0);
}
