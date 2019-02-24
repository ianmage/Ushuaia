$input a_position
$output v_tc0, v_tc1, v_tc2

#include "../common/common.sh"

uniform vec4 tcOffsets[2];


void main()
{
	gl_Position = vec4(a_position, 1.0);

	vec2 homoCoord = a_position.xy;

	vec2 uv = vec2(homoCoord.x + 1.f, 1.f - homoCoord.y) * 0.5f;

	v_tc0 = uv;

	v_tc1 = uv.xxxx + tcOffsets[0];
	v_tc2 = uv.xxxx + tcOffsets[1];
}
