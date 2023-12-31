#include <cmath>

bool isBoundingBoxIntersection(SceneObject& ob1, SceneObject& ob2) {

    if(!ob2.has_collision()){
        return false;
    }

    glm::vec4 cur_bbox_max1 = ob1.get_bbox_max();
    glm::vec4 cur_bbox_min1 = ob1.get_bbox_min();

    glm::vec4 cur_bbox_max2 = ob2.get_bbox_max();
    glm::vec4 cur_bbox_min2 = ob2.get_bbox_min();

    bool xAxis = cur_bbox_min1.x <= cur_bbox_max2.x && cur_bbox_max1.x >= cur_bbox_min2.x;
    bool yAxis = cur_bbox_min1.y <= cur_bbox_max2.y && cur_bbox_max1.y >= cur_bbox_min2.y;
    bool zAxis = cur_bbox_min1.z <= cur_bbox_max2.z && cur_bbox_max1.z >= cur_bbox_min2.z;

    //std::cout<<ob2.name<<std::endl;
    //printf("MIN1:(%.4f)(%.4f)(%.4f)\n", cur_bbox_min1.x,cur_bbox_min1.y,cur_bbox_min1.z);
    //printf("MAX1:(%.4f)(%.4f)(%.4f)\n", cur_bbox_max1.x,cur_bbox_max1.y,cur_bbox_max1.z);
    //printf("MIN2:(%.4f)(%.4f)(%.4f)\n", cur_bbox_min2.x,cur_bbox_min2.y,cur_bbox_min2.z);
    //printf("MAX2:(%.4f)(%.4f)(%.4f)\n", cur_bbox_max2.x,cur_bbox_max2.y,cur_bbox_max2.z);
    //printf("|%d\n", yAxis && xAxis && zAxis);

    return yAxis && xAxis && zAxis;
}
// FONTE: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection.html
bool isRayBoudingBox(glm::vec4 ray, glm::vec4 ray_origin, SceneObject& obj1, float& intersection_distance){

    glm::vec4 cur_bbox_max = obj1.get_bbox_max();
    glm::vec4 cur_bbox_min = obj1.get_bbox_min();

    float tmin = (cur_bbox_min.x - ray_origin.x) / ray.x;
    float tmax = (cur_bbox_max.x - ray_origin.x) / ray.x;

    if(tmin > tmax){
        std::swap(tmin, tmax);
    }

    float tymin = (cur_bbox_min.y - ray_origin.y) / ray.y;
    float tymax = (cur_bbox_max.y - ray_origin.y) / ray.y;

    if (tymin > tymax){
        std::swap(tymin, tymax);
    }

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (cur_bbox_min.z - ray_origin.z) / ray.z;
    float tzmax = (cur_bbox_max.z - ray_origin.z) / ray.z;

    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    intersection_distance = tmin;

    return true;
}

glm::vec3 clamp_to_bbox(glm::vec3 bbox_min, glm::vec3 bbox_max, glm::vec3 sphere_center){
    glm::vec3 closest_point_to_sphere = sphere_center;

    if(closest_point_to_sphere.x < bbox_min.x){
        closest_point_to_sphere.x = bbox_min.x;
    }else if(closest_point_to_sphere.x > bbox_max.x){
        closest_point_to_sphere.x = bbox_max.x;
    }


    if(closest_point_to_sphere.y < bbox_min.y){
        closest_point_to_sphere.y = bbox_min.y;
    }else if(closest_point_to_sphere.y > bbox_max.y){
        closest_point_to_sphere.y = bbox_max.y;
    }

    if(closest_point_to_sphere.z < bbox_min.z){
        closest_point_to_sphere.z = bbox_min.z;
    }else if(closest_point_to_sphere.z > bbox_max.z){
        closest_point_to_sphere.z = bbox_max.z;
    }


    return closest_point_to_sphere;

}


bool isCubeIntersectingSphere(glm::vec3 bbox_min, glm::vec3 bbox_max, glm::vec3 sphere_center, float radius)
{
    glm::vec3 closest_point_to_sphere = clamp_to_bbox(bbox_min, bbox_max, sphere_center);

    float distance = glm::length(closest_point_to_sphere - sphere_center);

    return distance <= radius;
}




