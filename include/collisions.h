bool isBoundingBoxIntersection(SceneObject& ob1, SceneObject& ob2) {

    if(!ob2.hasCollision){
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
