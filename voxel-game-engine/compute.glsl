#version 460 core
layout(local_size_x = 8, local_size_y = 4) in;
layout(rgba32f, binding = 0) uniform image2D screen;
layout(rgba32f, binding = 1) uniform image3D voxelGridColor;
layout(r32f,    binding = 2) uniform image3D voxelGridProperties;

uniform int renderDist;

uniform vec2 angle;
uniform float fov;

uniform vec3 playerPos;
uniform vec3 sunDir;
uniform vec3 skyColor;

#define MAX_QUEUE_SIZE 10

#define REFLECTION_DEPTH 50
#define SHADOW_DEPTH 8

#define SHADOW_POWER 0.8f
#define MIN_ENERGY 0.1f

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

    // barva voxelu na zacatku paprsku
    const vec4 voxelColor = imageLoad(voxelGridColor, hit.voxelPos);

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
					if (hit.voxelPos.x >= imageSize(voxelGridColor).x) {
                        if (step.x >= 0) return hit;
                    } else {
                        if (hit.voxelPos.y >= imageSize(voxelGridColor).y) {
                            if (step.y >= 0) return hit;
                        } else {
                            if (hit.voxelPos.z >= imageSize(voxelGridColor).z) {
                                if (step.z >= 0) return hit;
                            } else {                        
                                if (imageLoad(voxelGridColor, hit.voxelPos) != voxelColor) {
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
	const vec3 cameraDir = vec3(cos(angle[1]) * -sin(angle[0]), sin(angle[1]), cos(angle[1]) * -cos(angle[0]));
	const vec3 cameraUp =  vec3(sin(angle[1]) *  sin(angle[0]), cos(angle[1]), sin(angle[1]) *  cos(angle[0]));

	const vec2 screenPos = vec2((2.0 * gl_GlobalInvocationID.x / imageSize(screen).x - 1.0) * tan(fov / 2.0), 
                               -(2.0 * gl_GlobalInvocationID.y / imageSize(screen).y - 1.0) * tan(fov / 2.0) * imageSize(screen).y / imageSize(screen).x);

    vec3 color = vec3(0);
    
    struct QueueItem {
		Ray ray;
		vec3 energy;
	};

    QueueItem queue[MAX_QUEUE_SIZE];
	queue[0] = QueueItem(Ray(playerPos, normalize(cameraDir - screenPos.x * cross(cameraDir, cameraUp) + screenPos.y * cameraUp)), vec3(1));
    int queueSize = 1;
    
    for (int i = 0; i < queueSize; i++) {
        Hit hit = raytrace(queue[i].ray);

        // pruhlednost
        if (imageLoad(voxelGridColor, ivec3(ceil(queue[i].ray.pos))).a != 0.0) {
            queue[i].energy *= imageLoad(voxelGridColor, ivec3(ceil(queue[i].ray.pos))).rgb;
		}
        
        if (hit.hit) {                        
            if (imageLoad(voxelGridColor, hit.voxelPos).a == 1.0) { 
                Ray sunRay = Ray(queue[i].ray.pos + queue[i].ray.dir * (hit.distance - 0.001), -sunDir);
                Hit sunHit = Hit(true, ivec3(0), ivec3(0), 0);
                vec3 sunEnergy = vec3(1);
                
                for (int j = 0; j < SHADOW_DEPTH && sunHit.hit; j++) {
                    sunHit = raytrace(sunRay);
                    
                    if (imageLoad(voxelGridColor, ivec3(ceil(sunRay.pos))).a != 0.0) {
                        sunEnergy *= imageLoad(voxelGridColor, ivec3(ceil(sunRay.pos))).rgb;
                    }

                    if (imageLoad(voxelGridColor, sunHit.voxelPos).a == 1.0) {
                        sunEnergy = vec3(0);
                        break;
                    }

                    sunRay.pos += sunRay.dir * (sunHit.distance + 0.001);
				}

                sunEnergy = sunEnergy * SHADOW_POWER + 1 - SHADOW_POWER;

                color += imageLoad(voxelGridColor, hit.voxelPos).rgb * queue[i].energy * sunEnergy * (1 - imageLoad(voxelGridProperties, hit.voxelPos).x);
            } else {
                // pruhlednost
                if (queueSize < REFLECTION_DEPTH) {
                    vec3 energy = queue[i].energy * (1 - imageLoad(voxelGridProperties, hit.voxelPos).x);
                    if (dot(energy, vec3(1)) > MIN_ENERGY) {
                        queue[queueSize] = QueueItem(Ray(queue[i].ray.pos + queue[i].ray.dir * (hit.distance + 0.001), queue[i].ray.dir), energy);
                        queueSize++;
                    }
                }
            }
            
            // odraz
            if (imageLoad(voxelGridProperties, hit.voxelPos).x > 0) {
                if (queueSize < REFLECTION_DEPTH) {
                    vec3 energy = queue[i].energy * imageLoad(voxelGridProperties, hit.voxelPos).x;
                    if (dot(energy, vec3(1)) > MIN_ENERGY) {
						queue[queueSize] = QueueItem(Ray(queue[i].ray.pos + queue[i].ray.dir * (hit.distance - 0.001), reflect(queue[i].ray.dir, hit.normal)), energy);
                        queueSize++;
					}
                }
            }
		} else {
			// skybox
            color += skyColor * queue[i].energy;
		}
    }
	
	imageStore(screen, ivec2(gl_GlobalInvocationID.xy), /*vec4(1.0)*/vec4(color, 1.0));
}