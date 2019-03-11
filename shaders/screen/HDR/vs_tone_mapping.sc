$input a_position
$output v_tc0

#include "../common/common.sh"

SAMPLER2D(lum_tex, 0);


void main()
{
	gl_Position = vec4(a_position, 1.0);

	vec2 homoCoord = a_position.xy;

	vec2 uv = vec2(homoCoord.x + 1.f, 1.f - homoCoord.y) * 0.5f;

	float adaptedLum = texture2DLod(lum_tex, vec2_splat(0.5f), 0).x;
	adaptedLum = exp(adaptedLum);

	v_tc0 = vec4(uv, adaptedLum, 0);
}
