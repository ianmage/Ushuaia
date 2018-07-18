$input a_position, a_texcoord0
$output v_pos, v_texcoord0

#include "../common/common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));

	v_pos = mul(u_modelView, vec4(a_position, 1.0)).xyz;

	v_texcoord0 = a_texcoord0;
}
