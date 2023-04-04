#version 460 core

precision highp float;

out vec4 FragColor;

uniform ivec2 resolution;

uniform float time;

const int MAX_STEPS = 100;

const float MAX_DISTANCE = 100;

const float SURFACE_DISTANCE = 0.01;

float sdCapsule(vec3 point, vec3 a, vec3 b, float radius)
{
	vec3 ab = b - a;
	vec3 ap = point - a;

	float t = dot(ab, ap) / dot(ab, ab);

	t = clamp(t, 0.0, 1.0);

	vec3 c = a + t * ab;

	return length(point - c) - radius;
}

float sdTorus(vec3 point, vec2 radius)
{
	float x = length(point.xz) - radius.x;
	return length(vec2(x, point.y)) - radius.y;
}

float getDistance(vec3 point)
{
	vec4 sphere = vec4(-1.5, 0.5, 6.0, 0.5);
	float sphereDistance = length(point - sphere.xyz) - sphere.w;
	float planeDistance = point.y;

	float capsuleDistance = sdCapsule(point, vec3(0.0, 0.5, 6.0), vec3(0.0, 1.5, 6.0), 0.5);
	float torusDistance = sdTorus(point - vec3(2.0, 0.5, 6.0), vec2(1.0, 0.25));

	float distance = min(capsuleDistance, planeDistance);

	distance = min(distance, sphereDistance);

	distance = min(distance, torusDistance);

	return distance;
}

float rayMarch(vec3 rayOrigin, vec3 rayDirection)
{
	float distanceToOrigin = 0.0;

	for (int i = 0; i < MAX_STEPS; i++)
	{
		vec3 point = rayOrigin + distanceToOrigin * rayDirection;
		float distanceToScene = getDistance(point);

		distanceToOrigin += distanceToScene;

		if (distanceToScene < SURFACE_DISTANCE || distanceToOrigin > MAX_DISTANCE)
		{
			break;
		}
	}

	return distanceToOrigin;
}

vec3 getNormal(vec3 point)
{
	float distance = getDistance(point);
	vec2 e = vec2(0.01, 0.0);

	vec3 normal = vec3(distance - getDistance(point - e.xyy), distance - getDistance(point - e.yxy), distance - getDistance(point - e.yyx));

	return normalize(normal);
}

float getLight(vec3 point)
{
	vec3 lightPosition = vec3(0.0, 5.0, 6.0);

	lightPosition.xz += vec2(sin(time), cos(time));

	vec3 lightDirection = normalize(lightPosition - point);

	vec3 normal = getNormal(point);

	float diffuse = clamp(dot(lightDirection, normal), 0.0, 1.0);

	float distanceToLight = rayMarch(point + normal * SURFACE_DISTANCE * 2.0, lightDirection);

	if (distanceToLight < length(lightPosition - point))
	{
		diffuse *= 0.1;
	}

	return diffuse;
}

void main() 
{
	vec2 uv = (gl_FragCoord.xy - resolution.xy * 0.5) / resolution.y;
	// vec2 uv = gl_FragCoord.xy / resolution.y;

	vec3 rayOrigin = vec3(0.0, 2.0, 0.0);

	vec3 rayDirection = normalize(vec3(uv.x, uv.y, 1.0));

	float distanceToOrigin = rayMarch(rayOrigin, rayDirection);

	vec3 point = rayOrigin + rayDirection * distanceToOrigin;

	float diffuse = getLight(point);

	distanceToOrigin /= 6.0;

	// vec3 color = vec3(distanceToOrigin);
	vec3 color = vec3(diffuse);

	// color = getNormal(point);

	// Compute color for fragment
	FragColor = vec4(color, 1.0);
}
