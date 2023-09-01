#include "matrices.h"
#include "glm/gtx/string_cast.hpp"
struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor lê o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true)
    {
        printf("Carregando objetos do arquivo \"%s\"...\n", filename);

        // Se basepath == NULL, então setamos basepath como o dirname do
        // filename, para que os arquivos MTL sejam corretamente carregados caso
        // estejam no mesmo diretório dos arquivos OBJ.
        std::string fullpath(filename);
        std::string dirname;
        if (basepath == NULL)
        {
            auto i = fullpath.find_last_of("/");
            if (i != std::string::npos)
            {
                dirname = fullpath.substr(0, i+1);
                basepath = dirname.c_str();
            }
        }

        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, basepath, triangulate);

        if (!err.empty())
            fprintf(stderr, "\n%s\n", err.c_str());

        if (!ret)
            throw std::runtime_error("Erro ao carregar modelo.");

        for (size_t shape = 0; shape < shapes.size(); ++shape)
        {
            if (shapes[shape].name.empty())
            {
                fprintf(stderr,
                        "*********************************************\n"
                        "Erro: Objeto sem nome dentro do arquivo '%s'.\n"
                        "Veja https://www.inf.ufrgs.br/~eslgastal/fcg-faq-etc.html#Modelos-3D-no-formato-OBJ .\n"
                        "*********************************************\n",
                    filename);
                throw std::runtime_error("Objeto sem nome.");
            }
            printf("- Objeto '%s'\n", shapes[shape].name.c_str());
        }

        printf("OK.\n");
    }
};


// Definimos uma estrutura que armazenará dados necessários para renderizar
// cada objeto da cena virtual.
struct SceneObject
{
    private:
    std::string  name;        // Nome do objeto
    std::string  model_name;
    size_t       first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    size_t       num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo
    glm::vec4    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec4    bbox_max;
    glm::mat4    model = Matrix_Identity();
    float        radius = 0;
    int          index;
    bool         inspectable = true;
    bool         collision = true;

    public:
    SceneObject(){
    }

    SceneObject(int first_index, int num_indices, GLenum rendering_mode,
                GLuint vertex_array_object_id, int obj_index,
                const glm::vec3& bbox_min, const glm::vec3& bbox_max)
        : first_index(first_index),
          num_indices(num_indices),
          rendering_mode(rendering_mode),
          vertex_array_object_id(vertex_array_object_id),
          bbox_min(glm::vec4(bbox_min,1.0f)),
          bbox_max(glm::vec4(bbox_max,1.0f)),
          index(obj_index) {
    }

    size_t get_first_index(){
        return first_index;
    }
    size_t get_num_indices(){
        return num_indices;
    }
    GLenum get_rendering_mode(){
        return rendering_mode;
    }
    GLuint get_vertex_array_object_id(){
        return vertex_array_object_id;
    }
    void set_index(int index){
        this->index = index;
    }

    void set_collision(bool collision){
        this->collision = collision;
    }

    void set_inspectable(bool inspectable){
        this->inspectable = inspectable;
    }

    void set_name(std::string name){
        this->name = name;
    }

    void set_model_name(std::string model_name){
        this->model_name = model_name;
    }

    int get_index(){
        return index;
    }

    std::string get_name(){
        return name;
    }

    std::string get_model_name(){
        return model_name;
    }



    glm::mat4 get_model(){
        return model;
    }

    void set_model(glm::mat4 new_model){
        model = new_model;
    }

    bool has_collision(){
        return collision;
    }

    bool is_inspectable(){
        return inspectable;
    }

    glm::vec4 get_bbox_min(){
        glm::vec4 bbox_max_global = model * bbox_min;
        glm::vec4 bbox_min_global = model * bbox_max;

        return glm::min(bbox_min_global,bbox_max_global);
    }

    glm::vec4 get_bbox_max(){
        glm::vec4 bbox_max_global = model * bbox_min;
        glm::vec4 bbox_min_global = model * bbox_max;

        return glm::max(bbox_min_global,bbox_max_global);
    }

    glm::vec4 get_center(){
        return model * glm::vec4(   (bbox_max.x + bbox_min.x)/2,
                                    (bbox_max.y + bbox_min.y)/2,
                                    (bbox_max.z + bbox_min.z)/2,
                                     1.0f
                                     ) ;
    }

    void set_position(float x, float y, float z){
        model[3] = glm::vec4(x,y,z,1);
    }

    void set_position(glm::vec4 position){
        model[3] = position;
    }

    glm::vec4 get_position(){
        return model[3];
    }

    void translate(float x, float y, float z){
        model = model * Matrix_Translate(x, y, z);
    }
    void scale(float x, float y, float z){
        model = model * Matrix_Scale(x, y, z);
    }
    void mRotate(float x, float y, float z){
        model = model * Matrix_Rotate_X(x)
                      * Matrix_Rotate_Y(y)
                      * Matrix_Rotate_Z(z);
    }

    bool is_sphere(){
        return radius != 0;
    }
    float get_radius(){
        return radius;
    }
    void set_radius(float new_radius){
        radius = new_radius;
    }
};
