#version 460 core

precision highp float;

#define SOLUTION_CYLINDER_AND_PLANE
//#define SOLUTION_SHADOW
#define SOLUTION_REFLECTION_REFRACTION
#define SOLUTION_FRESNEL
// #define SOLUTION_BOOLEAN

out vec4 FragColor;

uniform ivec2 viewport;

struct PointLight
{
    vec3 position;
    vec3 color;
};

struct Material
{
    vec3 diffuse;
    vec3 specular;
    float glossiness;
    float reflection;
    float refraction;
    float ior;
};

struct Sphere
{
    vec3 position;
    float radius;
    Material material;
};

struct Plane
{
    vec3 normal;
    float d;
    Material material;
};

struct Cylinder
{
    vec3 position;
    vec3 direction;
    float radius;
    Material material;
};

#define BOOLEAN_MODE_AND 0  // and
#define BOOLEAN_MODE_MINUS 1// minus

struct Boolean
{
    Sphere spheres[2];
    int mode;
};

const int lightCount = 2;
const int sphereCount = 3;
const int planeCount = 1;
const int cylinderCount = 2;
const int booleanCount = 2;

struct Scene
{
    vec3 ambient;
    // PointLight[lightCount] lights;
    // Sphere[sphereCount] spheres;
    // Plane[planeCount] planes;
    // Cylinder[cylinderCount] cylinders;
    // Boolean[booleanCount] booleans;
    PointLight lights[lightCount];
    Sphere spheres[sphereCount];
    Plane planes[planeCount];
    Cylinder cylinders[cylinderCount];
    Boolean booleans[booleanCount];
};

struct Ray
{
    vec3 origin;
    vec3 direction;
};

// Contains all information pertaining to a ray/object intersection
struct HitInfo
{
    bool hit;
    float t;
    vec3 position;
    vec3 normal;
    Material material;
    bool enteringPrimitive;
};

HitInfo getEmptyHit()
{
    return HitInfo(
        false,
        0.,
        vec3(0.),
        vec3(0.),
        Material(vec3(0.0),vec3(0.0), 0.0, 0.0, 0.0, 0.0), false);
}

// Sorts the two t values such that t1 is smaller than t2
void sortT(inout float t1,inout float t2)
{
    // Make t1 the smaller t
    if(t2<t1){
        float temp = t1;
        t1 = t2;
        t2 = temp;
    }
}

// Tests if t is in an interval
bool isTInInterval(const float t,const float tMin,const float tMax)
{
    return t > tMin && t < tMax;
}

// Get the smallest t in an interval.
bool getSmallestTInInterval(float t0, float t1, const float tMin, const float tMax, inout float smallestTInInterval) 
{
  
	sortT(t0, t1);

	// As t0 is smaller, test this first
	if(isTInInterval(t0, tMin, tMax)) 
    {
		smallestTInInterval = t0;
		return true;
	}

	// If t0 was not in the interval, still t1 could be
	if(isTInInterval(t1, tMin, tMax)) 
    {
		smallestTInInterval = t1;
		return true;
	}  

	// None was
	return false;
}

HitInfo intersectSphere(const Ray ray, const Sphere sphere, const float tMin, const float tMax) 
{              
    vec3 to_sphere = ray.origin - sphere.position;
  
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(ray.direction, to_sphere);
    float c = dot(to_sphere, to_sphere) - sphere.radius * sphere.radius;
    float D = b * b - 4.0 * a * c;

    if (D > 0.0)
    {
		float t0 = (-b - sqrt(D)) / (2.0 * a);
		float t1 = (-b + sqrt(D)) / (2.0 * a);
      
      	float smallestTInInterval;
      	if(!getSmallestTInInterval(t0, t1, tMin, tMax, smallestTInInterval)) 
        {
            return getEmptyHit();
        }
      
      	vec3 hitPosition = ray.origin + smallestTInInterval * ray.direction;      
		
		//Checking if we're inside the sphere by checking if the ray's origin is inside. If we are, then the normal 
		//at the intersection surface points towards the center. Otherwise, if we are outside the sphere, then the normal 
		//at the intersection surface points outwards from the sphere's center. This is important for refraction.
      	vec3 normal = 
          	length(ray.origin - sphere.position) < sphere.radius + 0.001? 
          	-normalize(hitPosition - sphere.position): 
      		normalize(hitPosition - sphere.position);      
		
		//Checking if we're inside the sphere by checking if the ray's origin is inside,
		// but this time for IOR bookkeeping. 
		//If we are inside, set a flag to say we're leaving. If we are outside, set the flag to say we're entering.
		//This is also important for refraction.
		bool enteringPrimitive = 
          	length(ray.origin - sphere.position) < sphere.radius + 0.001 ? 
          	false:
		    true; 

        return HitInfo(
          	true,
          	smallestTInInterval,
          	hitPosition,
          	normal,
          	sphere.material,
			enteringPrimitive);
    }
    return getEmptyHit();
}

HitInfo intersectPlane(const Ray ray,const Plane plane, const float tMin, const float tMax) 
{
#ifdef SOLUTION_CYLINDER_AND_PLANE
    float degree = dot(plane.normal, ray.direction);
	
	if (degree == 0.0){
		return getEmptyHit();
	}
	
	float to_sphere = plane.d -  dot(plane.normal, ray.origin);

	float t = to_sphere/degree;
	
	if (t < 0.0){
		return getEmptyHit();
	}

	if (!isTInInterval(t, tMin, tMax)) {
		return getEmptyHit();
	}
	
	vec3 hitPosition = ray.origin + t * ray.direction;
	
	return HitInfo(
		true,
		t,
		hitPosition,
		plane.normal,
		plane.material,
		true);
		
#endif  
	return getEmptyHit();
}

float lengthSquared(vec3 x) 
{
	return dot(x, x);
}

HitInfo intersectCylinder(const Ray ray, const Cylinder cylinder, const float tMin, const float tMax) 
{
#ifdef SOLUTION_CYLINDER_AND_PLANE
	vec3 to_cylinder = ray.origin - cylinder.position;
  
    float a = lengthSquared(ray.direction) - dot(ray.direction, cylinder.direction) * dot(ray.direction, cylinder.direction) ;
    float b = 2.0 * dot(ray.direction, to_cylinder) - 2.0*dot(ray.direction, cylinder.direction) * dot(to_cylinder, cylinder.direction);
    float c = lengthSquared(to_cylinder) - dot(to_cylinder, cylinder.direction)*dot(to_cylinder, cylinder.direction) - cylinder.radius*cylinder.radius;
	
    float D = b * b - 4.0 * a * c;
	if (D > 0.0)
    {
		float t0 = (-b - sqrt(D)) / (2.0 * a);
		float t1 = (-b + sqrt(D)) / (2.0 * a);
      
      	float smallestTInInterval;
      	if(!getSmallestTInInterval(t0, t1, tMin, tMax, smallestTInInterval)) 
        {
            return getEmptyHit();
        }
      
      	vec3 hitPosition = ray.origin + smallestTInInterval * ray.direction;      
		
		float m = dot(ray.direction, cylinder.direction)*smallestTInInterval + dot(to_cylinder, cylinder.direction);
		
		//Checking if we're inside the cylinder by checking if the ray's origin is inside. If we are, then the normal 
		//at the intersection surface points towards the center. Otherwise, if we are outside the sphere, then the normal 
		//at the intersection surface points outwards from the sphere's center. This is important for refraction.
      	vec3 normal =   length(ray.origin - cylinder.position) < cylinder.radius + 0.001? 
          -normalize(hitPosition - cylinder.position - cylinder.direction*m): 
      	    normalize(hitPosition - cylinder.position - cylinder.direction*m);    
		
		//Checking if we're inside the cylinder by checking if the ray's origin is inside,
		// but this time for IOR bookkeeping. 
		//If we are inside, set a flag to say we're leaving. If we are outside, set the flag to say we're entering.
		//This is also important for refraction.
		bool enteringPrimitive = 
          	length(ray.origin - cylinder.position) < cylinder.radius + 0.001 ? 
          	false:
		    true; 

        return HitInfo(
          	true,
          	smallestTInInterval,
          	hitPosition,
          	normal,
          	cylinder.material,
			enteringPrimitive);
    } 
#endif  
    return getEmptyHit();
}

bool inside(const vec3 position, const Sphere sphere) 
{
	return length(position - sphere.position) < sphere.radius;
}

HitInfo intersectBoolean(const Ray ray, const Boolean boolean, const float tMin, const float tMax) 
{
#ifdef SOLUTION_BOOLEAN
	if (boolean.mode == BOOLEAN_MODE_MINUS)
	{
		// A on the left, B on the right
		Sphere leftSphere = boolean.spheres[0];
		Sphere rightSphere = boolean.spheres[1];
		
		HitInfo initialHitInfoA = intersectSphere(ray, leftSphere, tMin, tMax);
		HitInfo initialHitInfoB = intersectSphere(ray, rightSphere, tMin, tMax);
		
		if (initialHitInfoA.enteringPrimitive)
		{
			// 创建一条从A内部发射的新射线
			Ray nextRay;
			nextRay.direction = ray.direction;
			nextRay.origin = initialHitInfoA.position + nextRay.direction * 0.01;
		
			// 用新射线分别对A和B求交
			HitInfo hitInfoA = intersectSphere(nextRay, leftSphere, tMin, tMax);
			
			bool leftHit = hitInfoA.hit;
			
			bool insideRight = inside(hitInfoA.position, rightSphere);
			
			if (leftHit && insideRight)
			{
				return hitInfoA;
			}
		}
		
		if (initialHitInfoB.enteringPrimitive)
		{
			// 创建一条从B内部发射的新射线
			Ray nextRay;
			nextRay.direction = ray.direction;
			nextRay.origin = initialHitInfoB.position + nextRay.direction * 0.01;
		
			// 用新射线分别对A和B求交
			HitInfo hitInfoA = intersectSphere(nextRay, leftSphere, tMin, tMax);
			HitInfo hitInfoB = intersectSphere(nextRay, rightSphere, tMin, tMax);
			
			bool leftHit = hitInfoA.hit;
			bool rightHit = hitInfoB.hit;
			bool insideLeft = !hitInfoA.enteringPrimitive;
			bool insideRight = !hitInfoB.enteringPrimitive;
			
			insideLeft =  inside(hitInfoB.position, leftSphere);
			
			if (rightHit && !insideLeft)
			{
				return initialHitInfoB;
			}
		}
	}

    if (boolean.mode == BOOLEAN_MODE_AND)
	{
		// A on the left, B on the right
		Sphere leftSphere = boolean.spheres[1];
		Sphere rightSphere = boolean.spheres[0];
		
		HitInfo initialHitInfoA = intersectSphere(ray, leftSphere, tMin, tMax);
		HitInfo initialHitInfoB = intersectSphere(ray, rightSphere, tMin, tMax);
		
		if (initialHitInfoA.enteringPrimitive)
		{
			// 创建一条从B内部发射的新射线
			Ray nextRay;
			nextRay.direction = -initialHitInfoA.normal;
			nextRay.origin = initialHitInfoA.position + nextRay.direction * 0.01;
		
			// 用新射线分别对A和B求交
			HitInfo hitInfoA = intersectSphere(nextRay, leftSphere, tMin, tMax);
			HitInfo hitInfoB = intersectSphere(nextRay, rightSphere, tMin, tMax);
			
			bool leftHit = hitInfoA.hit;
			bool rightHit = hitInfoB.hit;
			bool insideLeft = !hitInfoA.enteringPrimitive;
			bool insideRight = !hitInfoB.enteringPrimitive;

			if (leftHit && insideRight)
			{
				return initialHitInfoA;
			}
		}
		
		if (initialHitInfoB.enteringPrimitive)
		{
			// 创建一条从B内部发射的新射线
			Ray nextRay;
			nextRay.direction = -initialHitInfoB.normal;
			nextRay.origin = initialHitInfoB.position + nextRay.direction * 0.01;
		
			// 用新射线分别对A和B求交
			HitInfo hitInfoA = intersectSphere(nextRay, leftSphere, tMin, tMax);
			HitInfo hitInfoB = intersectSphere(nextRay, rightSphere, tMin, tMax);
			
			bool leftHit = hitInfoA.hit;
			bool rightHit = hitInfoB.hit;
			bool insideLeft = !hitInfoA.enteringPrimitive;
			bool insideRight = !hitInfoB.enteringPrimitive;
			
			if (rightHit && insideLeft)
			{
				return initialHitInfoB;
			}
		}
    }
#else
    // Put your code for the boolean task in the #ifdef above!
#endif
    return getEmptyHit();
}

uniform float time;

HitInfo getBetterHitInfo(const HitInfo oldHitInfo, const HitInfo newHitInfo) 
{
	if(newHitInfo.hit)
  		if(newHitInfo.t < oldHitInfo.t)  // No need to test for the interval, this has to be done per-primitive
          return newHitInfo;
  	return oldHitInfo;
}

HitInfo intersectScene(const Scene scene, const Ray ray, const float tMin, const float tMax) 
{
	HitInfo bestHitInfo;
	bestHitInfo.t = tMax;
	bestHitInfo.hit = false;

	for (int i = 0; i < booleanCount; ++i) 
    {
    	bestHitInfo = getBetterHitInfo(bestHitInfo, intersectBoolean(ray, scene.booleans[i], tMin, tMax));
	}

    for (int i = 0; i < planeCount; ++i) 
    {
        bestHitInfo = getBetterHitInfo(bestHitInfo, intersectPlane(ray, scene.planes[i], tMin, tMax));
    }

    for (int i = 0; i < sphereCount; ++i) 
    {
        bestHitInfo = getBetterHitInfo(bestHitInfo, intersectSphere(ray, scene.spheres[i], tMin, tMax));
    }

    for (int i = 0; i < cylinderCount; ++i) 
    {
        bestHitInfo = getBetterHitInfo(bestHitInfo, intersectCylinder(ray, scene.cylinders[i], tMin, tMax));
    }
	
	return bestHitInfo;
}

vec3 shadeFromLight(
  const Scene scene,
  const Ray ray,
  const HitInfo hit_info,
  const PointLight light)
{ 
  vec3 hitToLight = light.position - hit_info.position;
  
  vec3 lightDirection = normalize(hitToLight);
  vec3 viewDirection = normalize(hit_info.position - ray.origin);
  vec3 reflectedDirection = reflect(viewDirection, hit_info.normal);
  float diffuse_term = max(0.0, dot(lightDirection, hit_info.normal));
  float specular_term  = pow(max(0.0, dot(lightDirection, reflectedDirection)), hit_info.material.glossiness);

#ifdef SOLUTION_SHADOW
#else
  // Put your shadow test here
  float visibility = 1.0;
  Ray shadow_ray;
  shadow_ray.origin = hit_info.position;
  shadow_ray.direction = hitToLight;
  HitInfo shadow_hit = intersectScene(scene, shadow_ray, 0.0001, 100000.0);
  visibility = shadow_hit.hit && isTInInterval(shadow_hit.t, 0.0 , 1.0) ?  0.0 : 1.0;
#endif
  return 	visibility * 
    		light.color * (
    		specular_term * hit_info.material.specular +
      		diffuse_term * hit_info.material.diffuse);
}

vec3 background(const Ray ray) 
{
    // A simple implicit sky that can be used for the background
    return vec3(0.2) + vec3(0.8, 0.6, 0.5) * max(0.0, ray.direction.y);
}

// It seems to be a WebGL issue that the third parameter needs to be inout instea dof const on Tobias' machine
vec3 shade(const Scene scene, const Ray ray, inout HitInfo hitInfo) 
{	
  	if(!hitInfo.hit) 
    {
  		return background(ray);
  	}
  
    vec3 shading = scene.ambient * hitInfo.material.diffuse;
    for (int i = 0; i < lightCount; ++i) 
    {
        shading += shadeFromLight(scene, ray, hitInfo, scene.lights[i]); 
    }
    return shading;
}

Ray getFragCoordRay(const vec2 frag_coord) 
{
  	float sensorDistance = 1.0;
  	vec2 sensorMin = vec2(-1, -0.5);
  	vec2 sensorMax = vec2(1, 0.5);
  	vec2 pixelSize = (sensorMax- sensorMin) / vec2(viewport.x, viewport.y);
  	vec3 origin = vec3(0, 0, sensorDistance);
    vec3 direction = normalize(vec3(sensorMin + pixelSize * frag_coord, -sensorDistance));  
  
  	return Ray(origin, direction);
}

float fresnel(const vec3 viewDirection, const vec3 normal, const float sourceIOR, const float destIOR) 
{
#ifdef SOLUTION_FRESNEL
    float cosi = clamp(-1.0, 1.0, dot(viewDirection, normal)); 
    float etai = sourceIOR;
    float etat = destIOR;

    if (cosi > 0.0) 
    { 
        float temp = etai;
        etai = etat;
        etat = temp;
    }

    // Compute sini using Snell's law
    float sint = etai / etat * sqrt(max(0.0, 1.0 - cosi * cosi));

    // Total internal reflection
    float kr = 0.0;
    if (sint >= 1.0) 
    { 
        kr = 1.0; 
    } 
    else 
    {
        float cost = sqrt(max(0.0, 1.0 - sint * sint)); 
        cosi = abs(cosi); 
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost)); 
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        kr = (Rs * Rs + Rp * Rp) / 2.0;
    } 

    return kr;
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
#else
  	// Put your code to compute the Fresnel effect in the ifdef above
	return 1.0;
#endif
}

float schlick(vec3 viewDirection, vec3 normal, float n1, float n2)
{
    // find the cosine of the angle between the eye and normal vectors
    float cos = dot(viewDirection, normal);

    // total internal reflection can only occur if n1 > n2
    if (n1 > n2)
    {
        float n = n1 / n2;

        float sin2_t = n * n * (1.0 - cos * cos);

        if (sin2_t > 1.0)
        {
            return 1.0;
        }

        // compute cosine of theta_t using trig identity
        float cos_t = sqrt(1.0 - sin2_t);
        
        // when n1 > n2, use cos(theta_t) instead
        cos = cos_t;
    }

    float r0 = pow(((n1 - n2) / (n1 + n2)), 2.0);

    return r0 + (1.0 - r0) * pow((1.0 - cos), 5.0);
}

vec3 colorForFragment(const Scene scene, const vec2 fragCoord) 
{     
    Ray initialRay = getFragCoordRay(fragCoord);  
  	HitInfo initialHitInfo = intersectScene(scene, initialRay, 0.001, 10000.0);  
  	vec3 result = shade(scene, initialRay, initialHitInfo);
	
  	Ray currentRay;
  	HitInfo currentHitInfo;
  	
  	// Compute the reflection
  	currentRay = initialRay;
  	currentHitInfo = initialHitInfo;
  	
  	// The initial strength of the reflection
  	float reflectionWeight = 1.0;
	
	// The initial medium is air
  	float currentIOR = 1.0;
	
    float sourceIOR = 1.0;
	float destIOR = 1.0;
  	
  	const int maxReflectionStepCount = 2;
    
  	for(int i = 0; i < maxReflectionStepCount; i++) 
    {  
        if(!currentHitInfo.hit) break;

    #ifdef SOLUTION_REFLECTION_REFRACTION
        reflectionWeight *= currentHitInfo.material.reflection;
    #else
        // Put your reflection weighting code in the ifdef above
    #endif

    #ifdef SOLUTION_FRESNEL
        if (currentHitInfo.enteringPrimitive && currentHitInfo.material.ior > 1.0)
        {
            sourceIOR = 1.0;
            destIOR = currentHitInfo.material.ior;
        }

        if (!currentHitInfo.enteringPrimitive && currentHitInfo.material.ior > 1.0)
        {
            sourceIOR = currentHitInfo.material.ior;
            destIOR = 1.0;
        }

    	vec3 viewDirection = normalize(currentRay.direction);
	    float reflectance = schlick(-viewDirection, currentHitInfo.normal, sourceIOR, destIOR);
        
        if (currentHitInfo.material.reflection > 0.0 && currentHitInfo.material.refraction > 0.0)
        {
            reflectionWeight = reflectance;
        }
        else
        {
            reflectionWeight *= 0.25;
        }
    #else
        // Replace with Fresnel code in the ifdef above
        reflectionWeight *= 0.5;
    #endif

        Ray nextRay;

    #ifdef SOLUTION_REFLECTION_REFRACTION
        vec3 reflected = reflect(normalize(currentRay.direction), currentHitInfo.normal);

        nextRay.origin = currentHitInfo.position;
        nextRay.direction = reflected;
    #else
        // Put your code to compute the reflection ray in the ifdef above
    #endif
        currentRay = nextRay;

        currentHitInfo = intersectScene(scene, currentRay, 0.001, 10000.0);      
            
        result += reflectionWeight * shade(scene, currentRay, currentHitInfo);
        // result += shade(scene, currentRay, currentHitInfo);
    }
  
  	// Compute the refraction
  	currentRay = initialRay;  
  	currentHitInfo = initialHitInfo;
   
  	// The initial strength of the refraction.
  	float refractionWeight = 1.0;
  
  	const int maxRefractionStepCount = 2;
  	for(int i = 0; i < maxRefractionStepCount; i++) 
    {
    #ifdef SOLUTION_REFLECTION_REFRACTION
        refractionWeight *= currentHitInfo.material.refraction;
    #else
        // Put your refraction weighting code in the ifdef above
        refractionWeight *= 0.5;      
    #endif

    #ifdef SOLUTION_FRESNEL
        if (currentHitInfo.enteringPrimitive && currentHitInfo.material.ior > 1.0)
        {
            sourceIOR = 1.0;
            destIOR = currentHitInfo.material.ior;
        }

        if (!currentHitInfo.enteringPrimitive && currentHitInfo.material.ior > 1.0)
        {
            sourceIOR = currentHitInfo.material.ior;
            destIOR = 1.0;
        }

    	vec3 viewDirection = normalize(currentRay.direction);
	    float reflectance = schlick(-viewDirection, currentHitInfo.normal, sourceIOR, destIOR);

        if (currentHitInfo.material.reflection > 0.0 && currentHitInfo.material.refraction > 0.0)
        {
            refractionWeight = (1.0 - reflectance);
        }
        else
        {
            refractionWeight *= 0.25;
        }
    #else
        // Put your Fresnel code in the ifdef above 
    #endif      
        Ray nextRay;

    #ifdef SOLUTION_REFLECTION_REFRACTION
        float sourceIOR = 1.0;
        float destIOR = currentHitInfo.material.ior;

        float refractionRatio = currentHitInfo.enteringPrimitive ? (sourceIOR / destIOR) : destIOR;

        vec3 unitDirection = normalize(currentRay.direction);
        vec3 refracted = refract(unitDirection, currentHitInfo.normal, refractionRatio);

        nextRay.origin = currentHitInfo.position;
        nextRay.direction = refracted;

        currentRay = nextRay;
    #else
        // Put your code to compute the reflection ray and track the IOR in the ifdef above
    #endif
        currentHitInfo = intersectScene(scene, currentRay, 0.001, 10000.0);

        result += refractionWeight * shade(scene, currentRay, currentHitInfo);
        // result += shade(scene, currentRay, currentHitInfo);

        if(!currentHitInfo.hit) break;
    }
    return result;
}

Material getDefaultMaterial() 
{
    return Material(vec3(0.3), vec3(0), 0.0, 0.0, 0.0, 0.0);
}

Material getPaperMaterial() 
{
    return Material(vec3(0.7, 0.7, 0.7), vec3(0, 0, 0), 5.0, 0.0, 0.0, 0.0);
}

Material getPlasticMaterial() 
{
	return Material(vec3(0.9, 0.3, 0.1), vec3(1.0), 10.0, 0.9, 0.0, 0.0);
}

Material getGlassMaterial() 
{
	return Material(vec3(0.0), vec3(0.0), 5.0, 1.0, 1.0, 1.5);
}

Material getSteelMirrorMaterial() 
{
	return Material(vec3(0.1), vec3(0.3), 20.0, 0.8, 0.0, 0.0);
}

Material getMetaMaterial() 
{
	return Material(vec3(0.1, 0.2, 0.5), vec3(0.3, 0.7, 0.9), 20.0, 0.8, 0.0, 0.0);
}

vec3 tonemap(const vec3 radiance) 
{
    const float monitorGamma = 2.0;
    return pow(radiance, vec3(1.0 / monitorGamma));
}

void main() {
	// Setup scene
	Scene scene;
	scene.ambient = vec3(0.12, 0.15, 0.2);

	scene.lights[0].position = vec3(5.0, 15.0, -5.0);
	scene.lights[0].color    = 0.5 * vec3(0.9, 0.5, 0.1);

	scene.lights[1].position = vec3(-15.0, 5.0, 2.0);
	scene.lights[1].color    = 0.5 * vec3(0.1, 0.3, 1.0);

	// Primitives
    bool soloBoolean = false;
	
	#ifdef SOLUTION_BOOLEAN
	#endif

	if(!soloBoolean) {
		scene.spheres[0].position            	= vec3(10.0, -5.0, -16.0);
		scene.spheres[0].radius              	= 6.0;
		scene.spheres[0].material 				= getPaperMaterial();

		scene.spheres[1].position            	= vec3(-7.0, -2.0, -13.0);
		scene.spheres[1].radius             	= 4.0;
		scene.spheres[1].material				= getPlasticMaterial();

		scene.spheres[2].position            	= vec3(0.0, 0.5, -5.0);
		scene.spheres[2].radius              	= 2.0;
		scene.spheres[2].material   			= getGlassMaterial();

		scene.planes[0].normal            		= normalize(vec3(0.0, 1.0, 0.0));
		scene.planes[0].d              			= -4.5;
		scene.planes[0].material				= getSteelMirrorMaterial();

		scene.cylinders[0].position            	= vec3(-1.0, 1.0, -26.0);
		scene.cylinders[0].direction            = normalize(vec3(-2.0, 2.0, -1.0));
		scene.cylinders[0].radius         		= 1.5;
		scene.cylinders[0].material				= getPaperMaterial();

		scene.cylinders[1].position            	= vec3(4.0, 1.0, -5.0);
		scene.cylinders[1].direction            = normalize(vec3(1.0, 4.0, 1.0));
		scene.cylinders[1].radius         		= 0.4;
		scene.cylinders[1].material				= getPlasticMaterial();

	} else {
		scene.booleans[0].mode = BOOLEAN_MODE_MINUS;
		
		// sphere A 
		scene.booleans[0].spheres[0].position      	= vec3(3, 0, -10);
		scene.booleans[0].spheres[0].radius      	= 2.75;
		scene.booleans[0].spheres[0].material      	= getPaperMaterial();
		
		// sphere B
		scene.booleans[0].spheres[1].position      	= vec3(6, 1, -13);	
		scene.booleans[0].spheres[1].radius      	= 4.0;
		scene.booleans[0].spheres[1].material      	= getPaperMaterial();
		
		scene.booleans[1].mode = BOOLEAN_MODE_AND;
		
		scene.booleans[1].spheres[0].position      	= vec3(-3.0, 1, -12);
		scene.booleans[1].spheres[0].radius      	= 4.0;
		scene.booleans[1].spheres[0].material      	= getPaperMaterial();
		
		scene.booleans[1].spheres[1].position      	= vec3(-6.0, 1, -12);	
		scene.booleans[1].spheres[1].radius      	= 4.0;
		scene.booleans[1].spheres[1].material      	= getMetaMaterial();
		
		scene.planes[0].normal            		= normalize(vec3(0, 0.8, 0));
		scene.planes[0].d              			= -4.5;
		scene.planes[0].material				= getSteelMirrorMaterial();
		
		scene.lights[0].position = vec3(-5, 25, -5);
		scene.lights[0].color    = vec3(0.9, 0.5, 0.1);

		scene.lights[1].position = vec3(-15, 5, 2);
		scene.lights[1].color    = 0.0 * 0.5 * vec3(0.1, 0.3, 1.0);
	}

	// Compute color for fragment
	FragColor.rgb = tonemap(colorForFragment(scene, gl_FragCoord.xy));
    
	FragColor.a = 1.0;
}
