$input a_position
$output v_tc0

#include "../common/common.sh"

uniform vec4 PV_viewVec;


void main()
{
	gl_Position = vec4(a_position, 1.0);

	vec2 homoCoord = a_position.xy;

	vec2 uv = vec2(homoCoord.x + 1.f, 1.f - homoCoord.y) * 0.5f;

	v_tc0 = vec4(uv, PV_viewVec.xy);
}
