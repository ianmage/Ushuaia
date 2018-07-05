$input a_position, a_normal, i_data0, i_data1, i_data2
$output v_normal, v_pos

#include "../common/common.sh"

void main()
{
	mat4 mtxModel;
#if 1
	mtxModel[0] = vec4(i_data0.x, i_data0.y, i_data0.z, 0);
	mtxModel[1] = vec4(i_data0.w, i_data1.x, i_data1.y, 0);
	mtxModel[2] = vec4(i_data1.z, i_data1.w, i_data2.x, 0);
	mtxModel[3] = vec4(i_data2.y, i_data2.z, i_data2.w, 1);
#elif 0
	mtxModel[0] = vec4(i_data0.x, i_data0.w, i_data1.z, i_data2.y);
	mtxModel[1] = vec4(i_data0.y, i_data1.x, i_data1.w, i_data2.z);
	mtxModel[2] = vec4(i_data0.z, i_data1.y, i_data2.x, i_data2.w);
	mtxModel[3] = vec4(0, 0, 0, 1);
#elif 0
	mtxModel[0] = vec4(i_data0.x, i_data1.x, i_data2.x, 0);
	mtxModel[1] = vec4(i_data0.y, i_data1.y, i_data2.y, 0);
	mtxModel[2] = vec4(i_data0.z, i_data1.z, i_data2.z, 0);
	mtxModel[3] = vec4(i_data0.w, i_data1.w, i_data2.w, 1);
#elif 0
	mtxModel[0] = vec4(i_data0.x, i_data0.y, i_data0.z, i_data0.w);
	mtxModel[1] = vec4(i_data1.x, i_data1.y, i_data1.z, i_data1.w);
	mtxModel[2] = vec4(i_data2.x, i_data2.y, i_data2.z, i_data2.w);
	mtxModel[3] = vec4(0, 0, 0, 1);
#else
	mtxModel[0] = i_data0;
	mtxModel[1] = i_data1;
	mtxModel[2] = i_data2;
	mtxModel[3] = i_data3;
#endif

	vec4 worldPos = instMul(mtxModel, vec4(a_position, 1.0));

	gl_Position = mul(u_modelViewProj, worldPos );

	vec4 normal = a_normal * 2.0 - 1.0;
	v_normal = normalize(mul(u_modelView, vec4(normal.xyz, 0.0) ).xyz);
	v_pos = mul(u_modelView, vec4(a_position, 1.0)).xyz;
}
