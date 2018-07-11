$input a_position, a_normal, i_data0, i_data1, i_data2
$output v_normal, v_pos

#include "../common/common.sh"

void main()
{
	mat4 mtxInst;
#if 1
	mtxInst[0] = vec4(i_data0.x, i_data0.y, i_data0.z, 0);
	mtxInst[1] = vec4(i_data0.w, i_data1.x, i_data1.y, 0);
	mtxInst[2] = vec4(i_data1.z, i_data1.w, i_data2.x, 0);
	mtxInst[3] = vec4(i_data2.y, i_data2.z, i_data2.w, 1);
#elif 0
	mtxInst[0] = vec4(i_data0.x, i_data0.w, i_data1.z, i_data2.y);
	mtxInst[1] = vec4(i_data0.y, i_data1.x, i_data1.w, i_data2.z);
	mtxInst[2] = vec4(i_data0.z, i_data1.y, i_data2.x, i_data2.w);
	mtxInst[3] = vec4(0, 0, 0, 1);
#elif 0
	mtxInst[0] = vec4(i_data0.x, i_data1.x, i_data2.x, 0);
	mtxInst[1] = vec4(i_data0.y, i_data1.y, i_data2.y, 0);
	mtxInst[2] = vec4(i_data0.z, i_data1.z, i_data2.z, 0);
	mtxInst[3] = vec4(i_data0.w, i_data1.w, i_data2.w, 1);
#elif 0
	mtxInst[0] = vec4(i_data0.x, i_data0.y, i_data0.z, i_data0.w);
	mtxInst[1] = vec4(i_data1.x, i_data1.y, i_data1.z, i_data1.w);
	mtxInst[2] = vec4(i_data2.x, i_data2.y, i_data2.z, i_data2.w);
	mtxInst[3] = vec4(0, 0, 0, 1);
#else
	mtxInst[0] = i_data0;
	mtxInst[1] = i_data1;
	mtxInst[2] = i_data2;
	mtxInst[3] = i_data3;
#endif

	vec4 worldPos = instMul(mtxInst, vec4(a_position, 1.0));
	//vec4 worldPos = mul(mtxInst, vec4(a_position, 1.0));

	gl_Position = mul(u_modelViewProj, worldPos);

	vec4 normal = vec4(a_normal.xyz * 2.0 - 1.0, 0);
	normal = mul(mtxInst, normal);
	v_normal = normalize(mul(u_modelView, normal).xyz);
	v_pos = mul(u_modelView, vec4(a_position, 1.0)).xyz;
}
