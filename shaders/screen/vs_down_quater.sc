$input a_position
$output v_tc0, v_tc1

#include "../common/common.sh"

uniform vec4 uTexCoordOffset[2];


void main()
{
	gl_Position = vec4(a_position, 1.0);

	vec2 homoCoord = a_position.xy;

	vec2 uv = vec2(homoCoord.x + 1.f, 1.f - homoCoord.y) * 0.5f;

	vec4 uvuv = vec4(uv, uv);

	v_tc0 = uvuv + uTexCoordOffset[0];
	v_tc1 = uvuv + uTexCoordOffset[1];
}
