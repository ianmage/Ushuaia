$input a_position, a_normal, i_data0, i_data1, i_data2
$output v_normal, v_pos

#include "../common/common.sh"

void main()
{
	mat4 mtxModel;
	mtxModel[0] = i_data0;
	mtxModel[1] = i_data1;
	mtxModel[2] = i_data2;
	mtxModel[3] = i_data3;

	vec4 worldPos = instMul(mtxModel, vec4(a_position, 1.0));

	gl_Position = mul(u_modelViewProj, worldPos );

	vec4 normal = a_normal * 2.0 - 1.0;
	v_normal = normalize(mul(u_modelView, vec4(normal.xyz, 0.0) ).xyz);
	v_pos = mul(u_modelView, vec4(a_position, 1.0)).xyz;
}
