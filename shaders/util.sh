// utils

vec3 ReconstructViewPos(vec2 viewVec, float z, vec2 scrCoord)
{	// (z / f) = (x / (hv)), we let vec = v/f, x = zhVec
	vec2 homoCoord = vec2(scrCoord.x * 2 - 1, 1 - scrCoord.y * 2);
	return vec3(z * homoCoord * viewVec, z);
}
