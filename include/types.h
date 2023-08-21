
struct ObjModel
{
    tinyobj::attrib_t                 attrib;
    std::vector<tinyobj::shape_t>     shapes;
    std::vector<tinyobj::material_t>  materials;

    // Este construtor l� o modelo de um arquivo utilizando a biblioteca tinyobjloader.
    // Veja: https://github.com/syoyo/tinyobjloader
    ObjModel(const char* filename, const char* basepath = NULL, bool triangulate = true)
    {
        printf("Carregando objetos do arquivo \"%s\"...\n", filename);

        // Se basepath == NULL, ent�o setamos basepath como o dirname do
        // filename, para que os arquivos MTL sejam corretamente carregados caso
        // estejam no mesmo diret�rio dos arquivos OBJ.
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


// Definimos uma estrutura que armazenar� dados necess�rios para renderizar
// cada objeto da cena virtual.
struct SceneObject
{
    std::string  name;        // Nome do objeto
    size_t       first_index; // �ndice do primeiro v�rtice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    size_t       num_indices; // N�mero de �ndices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
    GLenum       rendering_mode; // Modo de rasteriza��o (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
    GLuint       vertex_array_object_id; // ID do VAO onde est�o armazenados os atributos do modelo
    glm::vec4    bbox_min; // Axis-Aligned Bounding Box do objeto
    glm::vec4    bbox_max;
    glm::mat4    model;
    int          obj_index;
    bool         inspectable = true;
    bool         hasCollision = true;

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

    glm::vec4 get_bbox_center(){
        return model * glm::vec4(   (bbox_max.x + bbox_min.x)/2,
                                    (bbox_max.y + bbox_min.y)/2,
                                    (bbox_max.z + bbox_min.z)/2,
                                     1.0f
                                     ) ;
    }
};
