#ifndef _MOUSE_PICKING_H
#define _MOUSE_PICKING_H

#include <glm/gtc/type_ptr.hpp>
#include <map>


SceneObject *GetInteractableObject(std::vector<SceneObject*> &virtual_scene ,glm::vec4& camera_position_c, glm::vec4& camera_view_vector ){
    SceneObject *interactableObject = NULL;

    float minDistance = 10000;

    int scene_size = virtual_scene.size();
    for(int i = 0; i < scene_size; i++){
        SceneObject *obj = virtual_scene[i];

        float distance;

        if(obj->is_inspectable() && isRayBoudingBox(camera_view_vector,camera_position_c,*obj,distance)){
           if(distance < minDistance){
                minDistance = distance;
                if(minDistance < 1 && minDistance > 0){
                    interactableObject = obj;
                }
            }
        }
    }
    return interactableObject;
}
#endif
