$input a_position
$output v_tc0

#include "../common/common.sh"

uniform vec4 PV_viewVec;


void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));

	vec2 homoCoord = gl_Position.xy / gl_Position.w;
	vec2 viewVec = homoCoord * PV_viewVec.xy;
	vec2 uv = vec2(homoCoord.x + 1.f, 1.f - homoCoord.y) * 0.5f;
	uv += PV_viewVec.zw;

	v_tc0 = vec4(uv, viewVec);
}
