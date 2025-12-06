#version 430 core

in vec2 uv;

in vec3 vWorldPosition;

out vec4 FragColor;

uniform vec3 uCameraPos;
uniform float uTime;

uniform float uBounceCount;
uniform int uNumRays;

struct Material
{
    vec4 color;
    float lightEmission;
};

struct Ray
{
    vec3 origin;
    vec3 dir;
};

struct Vertex
{
    vec3 position;
    vec3 normalVector;
};

struct Sphere
{
    vec3 center;
    Material material;
    float radius;

};

struct Triangle
{
    Vertex vertexA;
    Vertex vertexB;
    Vertex vertexC;
    vec3 normalVector;

    Material material;
};

struct Mesh
{
    uint startIndex;
    uint size;
    vec3 boundsMin;
    vec3 boundsMax;
};

layout(std430, binding = 0) readonly buffer Spheres {
    Sphere spheres[];
};
uniform int uNumSpheres;

layout(std430, binding = 1) readonly buffer Triangles {
    Triangle triangles[];
};

layout(std430, binding = 2) readonly buffer Meshes {
    Mesh meshes[];
};
uniform int uNumMeshes;

float RandomValue(inout uint state)
{
    state = state * 747796405 + 2891336453;
    uint result = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
    result = (result >> 22) ^ result;
    return result / 4294967295.0;
}

float RandomNormalDistribution(inout uint state)
{
    float theta = 2 * 3.1415926 * RandomValue(state);
    float rho = sqrt(-2 * log(RandomValue(state)));
    return rho * cos(theta);
}

vec3 RandomDir(inout uint state) {
    // Cosine-weighted hemisphere sampling using Malley's method

    float x = RandomNormalDistribution(state);
    float y = RandomNormalDistribution(state);
    float z = RandomNormalDistribution(state);

    return vec3(x, y, z);
}

struct CollisionInfo
{
    bool didCollide;
    float distance;
    vec3 collisionPoint;
    vec3 normalVector;
    Material material;
};

CollisionInfo RaySphere(Ray ray, Sphere sphere)
{
    CollisionInfo info;
    info.didCollide = false;  // Initialize this!
    vec3 offsetRayOrigin = ray.origin - sphere.center;
    float a = dot(ray.dir, ray.dir);
    float b = 2 * dot(offsetRayOrigin, ray.dir);
    float c = dot(offsetRayOrigin, offsetRayOrigin) - sphere.radius * sphere.radius;

    float discriminant = b * b - 4 * a * c;

    if (discriminant >= 0)
    {
        float dist = (-b - sqrt(discriminant)) / (2 * a);

        if (dist >= 0)
        {
            info.didCollide = true;
            info.distance = dist;
            info.collisionPoint = ray.origin + ray.dir * dist;
            info.normalVector = normalize(info.collisionPoint - sphere.center);
        }
    }

    return info;
}

CollisionInfo RayTriangle(Ray ray, Triangle triangle)
{
    CollisionInfo info;
    info.didCollide = false;

    // 1. Get edges
    vec3 edgeAB = triangle.vertexB.position - triangle.vertexA.position;
    vec3 edgeAC = triangle.vertexC.position - triangle.vertexA.position;
    // 2. Calculate determinant
    // P = RayDir x EdgeAC
    vec3 pvec = cross(ray.dir, edgeAC);
    // det = EdgeAB . P
    float det = dot(edgeAB, pvec);
    // Culling: if det is near zero, ray lies in plane of triangle
    if (abs(det) < 1e-6) return info;
    float invDet = 1.0 / det;
    // 3. Calculate U
    // T = Origin - VertexA
    vec3 tvec = ray.origin - triangle.vertexA.position;
    float u = dot(tvec, pvec) * invDet;
    if (u < 0.0 || u > 1.0) return info;
    // 4. Calculate V
    // Q = T x EdgeAB
    vec3 qvec = cross(tvec, edgeAB);
    float v = dot(ray.dir, qvec) * invDet;
    if (v < 0.0 || u + v > 1.0) return info;
    // 5. Calculate t (distance)
    float dist = dot(edgeAC, qvec) * invDet;
    if (dist > 1e-6) // Ray intersection
    {
        info.didCollide = true;
        info.collisionPoint = ray.origin + ray.dir * dist;
        info.normalVector = triangle.normalVector;
        info.distance = dist;
    }
    return info;
}

CollisionInfo CalculateCollision(Ray ray)
{
    CollisionInfo closestHit;
    closestHit.didCollide = false;  // Initialize this!
    closestHit.distance = 10e9;

    for (int i = 0; i < uNumSpheres; ++i)
    {
        Sphere sphere = spheres[i];
        CollisionInfo info = RaySphere(ray, sphere);

        if (info.didCollide && info.distance < closestHit.distance)
        {
            closestHit = info;
            closestHit.material = sphere.material;
        }
    }

    for (int i = 0; i < uNumMeshes; ++i)
    {
        Mesh mesh = meshes[i];

        for (int j = 0; j < mesh.size; ++j)
        {
            uint triangleIndex = mesh.startIndex + j;
            Triangle triangle = triangles[triangleIndex];
            CollisionInfo info = RayTriangle(ray, triangle);

            if (info.didCollide && info.distance < closestHit.distance)
            {
                closestHit = info;
                closestHit.material = triangle.material;
            }
        }
    }

    return closestHit;
}

vec3 Trace(Ray ray, inout uint state)
{
    vec3 light = vec3(0.0);
    vec3 rayColor = vec3(1.0);
    for (int i = 0; i <= uBounceCount; ++i)
    {
        CollisionInfo info = CalculateCollision(ray);
        if (info.didCollide)
        {

            light += info.material.lightEmission * info.material.color.rgb * rayColor;

            ray.dir = normalize(info.normalVector + RandomDir(state));
            rayColor *= info.material.color.rgb;
            ray.origin = info.collisionPoint + info.normalVector;
        }
        else
        {
            break;
        }
    }

    return light;
}

void main()
{
    uint state = uint(gl_FragCoord.x) * 1973u;
    state += uint(gl_FragCoord.y) * 9277u;
    //    state += uint(uTime * 1000.0) * 2663u;

    Ray ray;
    ray.origin = uCameraPos;
    ray.dir = normalize(vWorldPosition - ray.origin);

    vec3 totalLight = vec3(0.0);

    for (int i = 0; i < uNumRays; ++i)
    {
        totalLight += Trace(ray, state);
    }

    FragColor = vec4(totalLight / uNumRays, 1.0);
}