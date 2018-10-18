$input a_position
$output v_tc0

#include "../common/common.sh"


void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));

	float u = gl_Position.x * 0.5 + 0.5;
	float v = 0.5 - gl_Position.y * 0.5;

	v_tc0 = vec4(u, v, 0, 0);
}
