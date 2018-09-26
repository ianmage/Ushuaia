$input a_position
$output v_tc0

#include "../common/common.sh"


void main()
{
	gl_Position = mul(u_proj, vec4(a_position, 1.0));

	v_tc0 = vec4_splat(0);
}
