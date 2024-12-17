#version 460 core
layout(local_size_x = 8, local_size_y = 4) in;
layout(rgba32f, binding = 0) uniform image2D screen;
layout(rgba32f, binding = 1) uniform image3D voxelGrid;

uniform int renderDist;

uniform vec2 angle;
uniform float fov;

uniform vec3 playerPos;
uniform vec3 sunDir;
uniform vec3 skyColor;

struct Ray {
	vec3 pos;
	vec3 dir;
};

struct Hit {
	bool hit;
	ivec3 voxelPos;
    ivec3 normal;
    float distance;
};

Hit raytrace(Ray ray)
{
    Hit hit = Hit(false, ivec3(ceil(ray.pos)), ivec3(0), 0);

    const ivec3 step = ivec3(sign(ray.dir));
    const vec3 tDelta = abs(1.0 / ray.dir);
    
    vec3 tMax = (hit.voxelPos + step * 0.5 - 0.5 - ray.pos) / ray.dir;
    
    for (int i = 0; i < renderDist; i++) {
        if (hit.voxelPos.x < 0) {
            if (step.x <= 0) return hit;
        } else {
            if (hit.voxelPos.y < 0) {
                if (step.y <= 0) return hit;
            } else {
                if (hit.voxelPos.z < 0) {
                    if (step.z <= 0) return hit;
                } else {
					if (hit.voxelPos.x >= imageSize(voxelGrid).x) {
                        if (step.x >= 0) return hit;
                    } else {
                        if (hit.voxelPos.y >= imageSize(voxelGrid).y) {
                            if (step.y >= 0) return hit;
                        } else {
                            if (hit.voxelPos.z >= imageSize(voxelGrid).z) {
                                if (step.z >= 0) return hit;
                            } else {                        
                                if (imageLoad(voxelGrid, hit.voxelPos).a != 0.0) {
                                    hit.distance = abs(dot((tMax - tDelta) * hit.normal, vec3(1)));
                                    hit.hit = true;
                                    return hit;
                                }
					        }
                        }
			        }
				}
            }
		}
        
        if (tMax.x < tMax.y) {
            if (tMax.x < tMax.z) {
                hit.voxelPos.x += step.x;
                tMax.x += tDelta.x;
                hit.normal = ivec3(step.x, 0, 0);
            }
            else {
                hit.voxelPos.z += step.z;
                tMax.z += tDelta.z;
                hit.normal = ivec3(0, 0, step.z);
            }
        }
        else {
            if (tMax.y < tMax.z) {
                hit.voxelPos.y += step.y;
                tMax.y += tDelta.y;
                hit.normal = ivec3(0, step.y, 0);
            }
            else {
                hit.voxelPos.z += step.z;
                tMax.z += tDelta.z;
                hit.normal = ivec3(0, 0, step.z);
            }
        }
    }
    
    return hit;
}

void main()
{	    	
	vec3 cameraDir = vec3(cos(angle[1]) * -sin(angle[0]), sin(angle[1]), cos(angle[1]) * -cos(angle[0]));
	vec3 cameraUp = vec3(sin(angle[1]) * sin(angle[0]), cos(angle[1]), sin(angle[1]) * cos(angle[0]));

	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims = imageSize(screen);

    float u = (2.0 * pixel_coords.x / dims.x - 1.0) * tan(fov / 2.0);
    float v = -(2.0 * pixel_coords.y / dims.y - 1.0) * tan(fov / 2.0) * dims.y / dims.x;
    
    vec3 rayDir = normalize(cameraDir - u * cross(cameraDir, cameraUp) + v * cameraUp);
    vec3 rayPos = playerPos;

    vec3 color = skyColor;

    Hit hit = raytrace(Ray(rayPos, rayDir));
	if (hit.hit) {
        color = imageLoad(voxelGrid, hit.voxelPos).rgb;
    }

    Hit shadowHit = raytrace(Ray(rayPos + rayDir * (hit.distance - 0.1), sunDir * vec3(-1.0)));
    if (shadowHit.hit) {
		color *= 0.5;
	}

	imageStore(screen, pixel_coords, vec4(color, 1.0));
}