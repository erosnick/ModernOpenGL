#version 460 core

layout(location = 0) out vec4 FragColor;

layout(location = 0) uniform ivec2 resolution;
layout(location = 1) uniform float time;

const float Min = 0.0;
const float Max = 1000.0;

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct Sphere
{
    vec3 center;
    float radius;
    vec4 color;
};

struct HitResult
{
    bool hit;
    float t;
    vec3 point;
    vec3 normal;
    bool frontFace;
};

highp float random(vec2 texelCoord)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt = dot(texelCoord.xy ,vec2(a, b));
    highp float sn = mod(dt, 3.14);
    return fract(sin(sn) * c);
}

highp float randomRange(vec2 texelCoord, float left, float right)
{
    return left + (right - left) * random(texelCoord);
}

// Gold Noise ©2015 dcerisano@standard3d.com
// - based on the Golden Ratio
// - uniform normalized distribution
// - fastest static noise generator function (also runs at low precision)
// - use with indicated fractional seeding method. 

float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio   

float goldNoise(vec2 xy, float seed)
{
       return fract(tan(distance(xy * PHI, xy) * seed) * xy.x);
}

vec3 at(Ray ray, float t)
{
    return ray.origin + ray.direction * t;
}

void setFaceNormal(inout HitResult hitResult, inout Ray ray, vec3 outwardNormal)
{
    hitResult.frontFace = dot(ray.direction, outwardNormal) < 0.0;
    hitResult.normal = hitResult.frontFace ? outwardNormal : -outwardNormal;
}

bool hitSphere(Ray ray, Sphere sphere, float tMin, float tMax, inout HitResult hitResult)
{
    vec3 oc = ray.origin - sphere.center;

    // float a = dot(ray.direction, ray.direction);

    // float b = dot(ray.direction, oc) * 2.0;
    // float c = dot(oc, oc) - sphere.radius * sphere.radius;

    // float discriminant = b * b - 4.0 * a * c;

    // Simplify
    float a = dot(ray.direction, ray.direction);
    float halfB = dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = halfB * halfB - a * c;

    if (discriminant < 0.0) 
    {
        return false;
    }

    float sqrtDiscriminant = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range
    float root = (-halfB - sqrtDiscriminant) / a;

    if (root < tMin || root > tMax)
    {
        root = (-halfB + sqrtDiscriminant) / a;

        if (root < tMin || root > tMax)
        {
            return false;
        }
    }

    hitResult.t = root;
    hitResult.point = at(ray, hitResult.t);
    hitResult.hit = true;

    vec3 outwardNormal = (hitResult.point - sphere.center) / sphere.radius;
    setFaceNormal(hitResult, ray, outwardNormal);

    return true;
}

bool hitSpheres(Ray ray, Sphere spheres[3], int objectCount, float tMin, float tMax, inout HitResult hitResult)
{
    bool hitAnything = false;
    float closestSoFar = tMax;
    HitResult tempHitResult;

    for (int i = 0; i < objectCount; i++)
    {
        if (hitSphere(ray, spheres[i], Min, closestSoFar, tempHitResult))
        {
            hitAnything = true;
            closestSoFar = tempHitResult.t;
            hitResult = tempHitResult;
        }
    }

    return hitAnything;
}

void main()
{
    vec3 color = vec3(0.3, 0.3, 0.3);
 
    float aspect = float(resolution.x) / resolution.y;
    
    float viewportHeight = 2.0;;
    float viewportWidth = viewportHeight * aspect;
    float focalLength = 1.0;

    float x = (float(gl_FragCoord.x * 2.0 - resolution.x) / resolution.x);
    float y = (float(gl_FragCoord.y * 2.0 - resolution.y) / resolution.y);  

    vec3 origin = vec3(0.0, 0.0, 0.0);

    vec3 horizontal = vec3(viewportWidth, 0.0, 0.0);
    vec3 vertical = vec3(0.0, viewportHeight, 0.0);

    vec3 lowerLeftCorner = origin - horizontal * 0.5 - vertical * 0.5 - vec3(0.0, 0.0, focalLength);

    x = (float(gl_FragCoord.x) / resolution.x);
    y = (float(gl_FragCoord.y) / resolution.y);

    vec3 direction = lowerLeftCorner + x * horizontal + y * vertical - origin;

    // direction = vec3(x * viewportWidth * 0.5, y * viewportHeight * 0.5, -1.0);

    Ray ray = Ray(origin, normalize(direction));

    Sphere spheres[3];

    spheres[0] = Sphere(vec3(0.0, 0.0, -1.0), 0.5, vec4(0.4, 0.4, 1.0, 1.0));
    spheres[1] = Sphere(vec3(1.0, 0.0, -1.0), 0.5, vec4(1.0, 0.4, 0.4, 1.0));
    spheres[2] = Sphere(vec3(-1.0, 0.0, -1.0), 0.5, vec4(0.4, 1.0, 0.4, 1.0));

    HitResult hitResult;

    bool hit = hitSpheres(ray, spheres, 3, Min, Max, hitResult);
    // bool hit = hitSphere(ray, spheres[0], Min, Max, hitResult);

    if (hit)
    {            
        // remap [-1, 1] to [0, 1]
        color = vec3(hitResult.normal.x + 1.0, hitResult.normal.y + 1.0, hitResult.normal.z + 1.0) * 0.5;
    }
    else
    {
        // blendedValue = (1 − t) ⋅ startValue + t ⋅ endValue,
        vec3 unitDirection = normalize(ray.direction);
        float t = 0.5 * (unitDirection.y + 1.0);
        color = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }

    float seed = fract(time);

    // color = vec3(goldNoise(gl_FragCoord.xy, seed + 0.1),
    //              goldNoise(gl_FragCoord.xy, seed + 0.2),
    //              goldNoise(gl_FragCoord.xy, seed + 0.3));

    FragColor = vec4(color, 1.0);
}