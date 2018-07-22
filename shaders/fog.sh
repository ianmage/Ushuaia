// FOG

float CalcFogFactor(float viewDist)
{
	float density = 0.0035;
	float zd = density * viewDist;
	float LOG2 = 1.442695;
	float factor = 1.0 / exp2(zd * zd * LOG2);
	return clamp(factor, 0.0, 1.0);
}
