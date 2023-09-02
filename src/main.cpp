//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Fundamentos de Computação Gráfica
//               Prof. Eduardo Gastal
//
//                   LABORATÓRIO 4
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers da biblioteca GLM: criação de matrizes e vetores.
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers da biblioteca para carregar modelos obj
#include <tiny_obj_loader.h>

#include <stb_image.h>

#include "types.h"
#include "collisions.h"
#include "mouse_picking.h"


// Headers locais, definidos na pasta "include/"
#include "utils.h"
#include "matrices.h"



#include "glm/gtx/string_cast.hpp"
// Estrutura que representa um modelo geométrico carregado a partir de um
// arquivo ".obj". Veja https://en.wikipedia.org/wiki/Wavefront_.obj_file .

#define PI  3.14159265359
#define PI2 1.57079632679
#define PI4 0.78539816339

// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
void BuildTrianglesAndAddToVirtualScene(ObjModel*); // Constrói representação de um ObjModel como malha de triângulos para renderização
void ComputeNormals(ObjModel* model); // Computa normais de um ObjModel, caso não existam.
void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
void DrawVirtualObject(const char* object_name); // Desenha um objeto armazenado em g_VirtualScene
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
void PrintObjModelInfo(ObjModel*); // Função para debugging

// Declaração de funções auxiliares para renderizar texto dentro da janela
// OpenGL. Estas funções estão definidas no arquivo "textrendering.cpp".
void TextRendering_Init();
float TextRendering_LineHeight(GLFWwindow* window);
float TextRendering_CharWidth(GLFWwindow* window);
void TextRendering_PrintString(GLFWwindow* window, const std::string &str, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrix(GLFWwindow* window, glm::mat4 M, float x, float y, float scale = 1.0f);
void TextRendering_PrintVector(GLFWwindow* window, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProduct(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductMoreDigits(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);
void TextRendering_PrintMatrixVectorProductDivW(GLFWwindow* window, glm::mat4 M, glm::vec4 v, float x, float y, float scale = 1.0f);

void TextRendering_Press_E_To_Inspect(GLFWwindow* window);
void TextRendering_Press_F_To_Collect(GLFWwindow* window);
void TextRendering_Press_F_To_Open(GLFWwindow* window);
void TextRendering_You_Lost(GLFWwindow* window);
void TextRendering_Press_esc_to_close_game(GLFWwindow* window);
void TextRendering_ShowFramesPerSecond(GLFWwindow* window);

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void LoadTextureImage(const char* filename);
void load_models();
void draw_objects();

// Abaixo definimos variáveis globais utilizadas em várias funções do código.

// A cena virtual é uma lista de objetos nomeados, guardados em um dicionário
// (map).  Veja dentro da função BuildTrianglesAndAddToVirtualScene() como que são incluídos
// objetos dentro da variável g_VirtualScene, e veja na função main() como
// estes são acessados.
std::map<std::string, SceneObject> g_VirtualScene;
std::map<std::string, glm::mat4> pieces_initial_position;
std::vector<SceneObject*> objects_to_draw;
std::vector<SceneObject*> chess_pieces;

// Razão de proporção da janela (largura/altura). Veja função FramebufferSizeCallback().
float g_ScreenRatio = 1.0f;

// Ângulos de Euler que controlam a rotação de um dos cubos da cena virtual
float g_AngleX = 0.0f;
float g_AngleY = 0.0f;
float g_AngleZ = 0.0f;

// "g_LeftMouseButtonPressed = true" se o usuário está com o botão esquerdo do mouse
// pressionado no momento atual. Veja função MouseButtonCallback().
bool g_LeftMouseButtonPressed = false;
bool g_RightMouseButtonPressed = false; // Análogo para botão direito do mouse
bool g_MiddleMouseButtonPressed = false; // Análogo para botão do meio do mouse

// Variáveis que definem a câmera em coordenadas esféricas, controladas pelo
// usuário através do mouse (veja função CursorPosCallback()). A posição
// efetiva da câmera é calculada dentro da função main(), dentro do loop de
// renderização.
float g_CameraTheta = 0.0f; // Ângulo no plano ZX em relação ao eixo Z
float g_CameraPhi = 0.0f;   // Ângulo em relação ao eixo Y
float g_CameraDistance = 3.5f; // Distância da câmera para a origem

// Número de texturas carregadas pela função LoadTextureImage()
GLuint g_NumLoadedTextures = 0;

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;
GLint g_model_uniform;
GLint g_view_uniform;
GLint g_projection_uniform;
GLint g_object_id_uniform;
GLint g_bbox_min_uniform;
GLint g_bbox_max_uniform;


SceneObject *interactable_object;
SceneObject *piece_to_reposition;
bool is_inspecting = false;
// Variaveis da free cam
float cameraX = -2.5f;
float cameraY = 1.0f;
float cameraZ = -1.0f;
float old_camera_x, old_camera_y, old_camera_z;
bool moving_forward   = false;
bool moving_backwards = false;
bool moving_left      = false;
bool moving_right     = false;
bool moving_up        = false;
bool moving_down      = false;
bool running        = false;
bool fst_anim       = true;
bool collect_anim   = false;
bool open_left_drawer  = false;
bool open_right_drawer = false;
bool hidden_pieces[6] = {true, true, true, true, true, true};
bool all_pieces_collected = false;
bool game_ended = false;
bool y_axis_movement = false;

glm::vec4 camera_view_vector;

glm::vec3 calculateBezierPoint(const std::vector<glm::vec3>& controlPoints, float t);
void move_with_collision(SceneObject player, std::vector<SceneObject*> objects_group, float delta_t, float speed, glm::vec4 w, glm::vec4 u);
void drawer(float delta_t, SceneObject player, SceneObject& drawer_left, SceneObject& drawer_right, SceneObject& collectable1);
void play_game_anim(float delta_t);
SceneObject* find_piece_by_name(std::string target_name);
bool all_pieces_in_starting_pos();
void capture_piece(SceneObject* piece);

int obj_index = 0;
/*
white_king -> bowl
black_king -> drawer_left
white_queen -> drawer
black_queen -> table
g_black_pawn -> sofa
*/
glm::vec4 h5_position = glm::vec4(-4.260000, 0.230000, -3.830000, 1.000000);
glm::vec4 f6_position = glm::vec4(-4.000000, 0.230000, -3.690000, 1.000000);
glm::vec4 c4_position = glm::vec4(-3.600000, 0.230000, -3.970000, 1.000000);
glm::vec4 c6_position = glm::vec4(-3.600000, 0.230000, -3.690000, 1.000000);
glm::vec4 e4_position = glm::vec4(-3.870000, 0.230000, -3.970000, 1.000000);
glm::vec4 e5_position = glm::vec4(-3.870000, 0.230000, -3.840000, 1.000000);

glm::vec4 captured_black_piece_next_position = glm::vec4(-4.450f,0.20f,-3.42f, 1.0f);
int main(int argc, char* argv[])
{
    // Inicializamos a biblioteca GLFW, utilizada para criar uma janela do
    // sistema operacional, onde poderemos renderizar com OpenGL.
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para impressão de erros da GLFW no terminal
    glfwSetErrorCallback(ErrorCallback);

    // Pedimos para utilizar OpenGL versão 3.3 (ou superior)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Pedimos para utilizar o perfil "core", isto é, utilizaremos somente as
    // funções modernas de OpenGL.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela do sistema operacional, com 800 colunas e 600 linhas
    // de pixels, e com título "INF01047 ...".
    GLFWwindow* window;
    window = glfwCreateWindow(800, 600, "INF01047 - Trabalho Final FCG - Arthur e Gabriel", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos a função de callback que será chamada sempre que o usuário
    // pressionar alguma tecla do teclado ...
    glfwSetKeyCallback(window, KeyCallback);
    // ... ou clicar os botões do mouse ...
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    // ... ou movimentar o cursor do mouse em cima da janela ...
    glfwSetCursorPosCallback(window, CursorPosCallback);
    // ... ou rolar a "rodinha" do mouse.
    glfwSetScrollCallback(window, ScrollCallback);

    // Indicamos que as chamadas OpenGL deverão renderizar nesta janela
    glfwMakeContextCurrent(window);

    // Carregamento de todas funções definidas por OpenGL 3.3, utilizando a
    // biblioteca GLAD.
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Definimos a função de callback que será chamada sempre que a janela for
    // redimensionada, por consequência alterando o tamanho do "framebuffer"
    // (região de memória onde são armazenados os pixels da imagem).
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    FramebufferSizeCallback(window, 800, 600); // Forçamos a chamada do callback acima, para definir g_ScreenRatio.

    // Imprimimos no terminal informações sobre a GPU do sistema
    const GLubyte *vendor      = glGetString(GL_VENDOR);
    const GLubyte *renderer    = glGetString(GL_RENDERER);
    const GLubyte *glversion   = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados
    // para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
    //
    LoadShadersFromFiles();

    LoadTextureImage("../../data/tc-earth_daymap_surface.jpg");      // TextureImage0
    LoadTextureImage("../../data/tc-earth_nightmap_citylights.gif"); // TextureImage1
    LoadTextureImage("../../data/box/texture.jpg");                  // TextureImage2
    LoadTextureImage("../../data/texture/floor/herringbone.jpg"); // TextureImage3
    LoadTextureImage("../../data/texture/floor/herringbone_r.png"); // TextureImage4
    LoadTextureImage("../../data/texture/wall_1/wood_trunk_wall_diff_1k.jpg");   // TextureImage5
    LoadTextureImage("../../data/texture/wall_1/wood_trunk_wall_disp_1k.png");   // TextureImage6
    LoadTextureImage("../../data/table/texture.jpg");                            // TextureImage7
    LoadTextureImage("../../data/chess/chess_board_texture.jpg");                // TextureImage8
    LoadTextureImage("../../data/bowl/Light_Oak.jpg");                           // TextureImage9
    LoadTextureImage("../../data/chess/white_piece_texture.jpg");                // TextureImage10
    LoadTextureImage("../../data/chess/black_piece_texture.jpg");                // TextureImage11
    LoadTextureImage("../../data/console_table/textures/console-table-004-col-metalness-4k.png"); // TextureImage12
    LoadTextureImage("../../data/sofa/textures/Hepburn_sofa_2_wire_008008136_Base_Color.png"); // TextureImage13
    LoadTextureImage("../../data/sofa/textures/Hepburn_sofa_2_wire_008008136_Mixed_AO.png");// TextureImage14
    LoadTextureImage("../../data/shelf/textures/shelf-040-col-metalness-4k.png");// TextureImage15
    LoadTextureImage("../../data/bed/textures/M_bed_BaseColor.png");// TextureImage16
    LoadTextureImage("../../data/chair/textures/armless-chair-003-col-metalness-4k.png");// TextureImage17
    LoadTextureImage("../../data/chair/textures/armless-chair-003-col-specular-4k.png");// TextureImage18
    LoadTextureImage("../../data/bookshelf/textures/bookshelf-031-col-metalness-4k.png");// TextureImage19
    LoadTextureImage("../../data/bookshelf/textures/books.jpg");// TextureImage20
    LoadTextureImage("../../data/texture/ceiling/ceiling.jpg"); // TextureImage21


    // Construímos a representação de objetos geométricos através de malhas de triângulos

    load_models();

    #define SPHERE      0
    #define BUNNY       1
    #define ROOM_FLOOR  2
    #define WALL_1      3
    #define SKYBOX      4
    #define WALL_1_SIDE 5
    #define TABLE       6
    #define CHESS       7
    #define BOWL        8
    #define WHITE_PIECE 9
    #define BLACK_PIECE 10
    #define CONSOLE_TABLE 11
    #define SOFA        12
    #define TV          13
    #define SHELF       14
    #define CHAIR       15
    #define BED         16
    #define BOOK_SHELF  17
    #define BOOKS       18
    #define ROOM_CEILING 19
    #define DRAWER      20


    /* Criacao de objetos */
    SceneObject player = g_VirtualScene.at("the_sphere");
    player.set_name("player");
    player.set_inspectable(false);
    objects_to_draw.push_back(&player);

    // Chão principal
    SceneObject room_floor = g_VirtualScene.at("the_plane");
    room_floor.set_name("floor");
    room_floor.set_position(0.0f, -1.0f, 0.0f);
    room_floor.scale(10.0f, 1.0f, 8.0f);
    room_floor.set_collision(false);
    room_floor.set_inspectable(false);
    room_floor.set_index(ROOM_FLOOR);
    objects_to_draw.push_back(&room_floor);

    // Teto
    SceneObject room_ceiling = g_VirtualScene.at("the_plane");
    room_ceiling.set_name("ceiling");
    room_ceiling.set_position(0.0f, +3.7f, 0.0f);
    room_ceiling.scale(10.0f, 1.0f, 8.0f);
    room_ceiling.mRotate(PI,0,0);
    room_ceiling.set_collision(false);
    room_ceiling.set_inspectable(false);
    room_ceiling.set_index(ROOM_CEILING);
    objects_to_draw.push_back(&room_ceiling);

    SceneObject wall1 = g_VirtualScene.at("box.jpg");
    wall1.set_name("wall_1");
    wall1.set_inspectable(false);
    wall1.scale(8.0f, 4.0f, 0.5f);
    wall1.set_position(0.0f,1.0f,-8.0f);
    wall1.set_index(WALL_1);
    objects_to_draw.push_back(&wall1);

    SceneObject wall2 = g_VirtualScene.at("box.jpg");
    wall2.set_name("wall_2");
    wall2.mRotate(0.0f,PI2,0.0f);
    wall2.scale(8.0f, 4.0f, 0.5f);
    wall2.set_position(-10.0f,1.0f,0.0f);
    wall2.set_inspectable(false);
    wall2.set_index(WALL_1);
    objects_to_draw.push_back(&wall2);

    // Parede 3
    SceneObject wall3 = g_VirtualScene.at("box.jpg");
    wall3.set_name("wall_3");
    wall3.set_position(0.0f,1.0f,8.0f);
    wall3.scale(8.0f, 4.0f, 0.5f);
    wall3.set_inspectable(false);
    wall3.set_index(WALL_1);
    objects_to_draw.push_back(&wall3);

    // Parede 4
    SceneObject wall4 = g_VirtualScene.at("box.jpg");
    wall4.set_name("wall_4");
    wall4.mRotate(0.0f,PI2,0.0f);
    wall4.scale(8.0f, 4.0f, 0.5f);
    wall4.set_position(10.0f,1.0f,0.0f);
    wall4.set_inspectable(false);
    wall4.set_index(WALL_1);
    objects_to_draw.push_back(&wall4);

    // Coelho
    SceneObject coelho = g_VirtualScene.at("the_bunny");
    coelho.set_name("coelho");
    coelho.scale(0.3f, 0.3f, 0.3f);
    coelho.set_position(5.5f,0.25f,-6.0f);
    coelho.set_index(SPHERE);
    objects_to_draw.push_back(&coelho);

    SceneObject bowl = g_VirtualScene.at("10315_soup_plate");
    bowl.set_name("bowl");
    bowl.scale(0.04,0.04,0.04);
    bowl.mRotate(-PI2,0.0f,0.0f);
    bowl.set_position(-6.0f,0.2f,-4.0f);
    bowl.set_index(BOWL);
    objects_to_draw.push_back(&bowl);

    // Mesa de canto
    SceneObject table = g_VirtualScene.at("the_table");
    table.set_name("table");
    table.scale(1.5f, 1.5f, 1.5f);
    table.set_position(-5.0f, -0.4f, -4.0f);
    table.set_index(TABLE);
    table.set_inspectable(true);
    objects_to_draw.push_back(&table);

    // Tabuleiro xadrez
    SceneObject chess_board = g_VirtualScene.at("chess_board");
    chess_board.set_name("chess_board");
    chess_board.scale(0.03f, 0.03f, 0.03f);
    chess_board.set_position(-3.8f, 0.2f,-3.9f);
    chess_board.set_index(CHESS);
    objects_to_draw.push_back(&chess_board);

    //Pecas
    float piece_height = 0.23;

    SceneObject right_white_rook = g_VirtualScene.at("rook");
    right_white_rook.set_name("right_white_rook");
    right_white_rook.scale(0.007f, 0.007, 0.007f);
    right_white_rook.set_position(-3.33f, piece_height,-4.37f);
    right_white_rook.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&right_white_rook);

    SceneObject right_black_rook = g_VirtualScene.at("rook");
    right_black_rook.set_name("right_black_rook");
    right_black_rook.scale(0.007f, 0.007, 0.007f);
    right_black_rook.set_position(-3.33f,piece_height,-3.42f);
    right_black_rook.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&right_black_rook);

    SceneObject right_white_knight = g_VirtualScene.at("knight");
    right_white_knight.set_name("right_white_knight");
    right_white_knight.translate(-3.46f,piece_height,-4.37f);
    right_white_knight.scale(0.007f, 0.007, 0.007f);
    right_white_knight.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&right_white_knight);

    SceneObject right_black_knight = g_VirtualScene.at("knight");
    right_black_knight.set_name("right_black_knight");
    right_black_knight.translate(-3.46f, piece_height,-3.42f);
    right_black_knight.scale(0.007f, 0.007, 0.007f);
    right_black_knight.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&right_black_knight);

    SceneObject right_white_bishop = g_VirtualScene.at("bishop");
    right_white_bishop.set_name("right_white_bishop");
    right_white_bishop.translate(-3.59f, piece_height,-4.37f);
    right_white_bishop.scale(0.007f, 0.007, 0.007f);
    right_white_bishop.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&right_white_bishop);

    SceneObject right_black_bishop = g_VirtualScene.at("bishop");
    right_black_bishop.set_name("right_black_bishop");
    right_black_bishop.translate(-3.59f, piece_height,-3.42f);
    right_black_bishop.scale(0.007f, 0.007, 0.007f);
    right_black_bishop.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&right_black_bishop);

    SceneObject white_queen = g_VirtualScene.at("queen");
    white_queen.set_name("white_queen");
    white_queen.translate(-3.73f, piece_height,-4.37f);
    white_queen.scale(0.007f, 0.007, 0.007f);
    white_queen.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&white_queen);

    SceneObject black_queen = g_VirtualScene.at("queen");
    black_queen.set_name("black_queen");
    black_queen.translate(-3.73f, piece_height,-3.42f);
    black_queen.scale(0.007f, 0.007, 0.007f);
    black_queen.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&black_queen);

    SceneObject white_king = g_VirtualScene.at("king");
    white_king.set_name("white_king");
    white_king.translate(-3.87f, piece_height,-4.37f);
    white_king.scale(0.007f, 0.007, 0.007f);
    white_king.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&white_king);

    SceneObject black_king = g_VirtualScene.at("king");
    black_king.set_name("black_king");
    black_king.translate(-3.87f, piece_height,-3.42f);
    black_king.scale(0.007f, 0.007, 0.007f);
    black_king.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&black_king);

    SceneObject left_white_bishop = g_VirtualScene.at("bishop");
    left_white_bishop.set_name("left_white_bishop");
    left_white_bishop.translate(-4.00f, piece_height,-4.37f);
    left_white_bishop.scale(0.007f, 0.007, 0.007f);
    left_white_bishop.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&left_white_bishop);

    SceneObject left_black_bishop = g_VirtualScene.at("bishop");
    left_black_bishop.set_name("left_black_bishop");
    left_black_bishop.translate(-4.00f, piece_height,-3.42f);
    left_black_bishop.scale(0.007f, 0.007, 0.007f);
    left_black_bishop.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&left_black_bishop);

    SceneObject left_white_knight = g_VirtualScene.at("knight");
    left_white_knight.set_name("left_white_knight");
    left_white_knight.translate(-4.14f,piece_height,-4.37f);
    left_white_knight.scale(0.007f, 0.007, 0.007f);
    left_white_knight.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&left_white_knight);

    SceneObject left_black_knight = g_VirtualScene.at("knight");
    left_black_knight.set_name("left_black_knight");
    left_black_knight.translate(-4.14f, piece_height,-3.42f);
    left_black_knight.scale(0.007f, 0.007, 0.007f);
    left_black_knight.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&left_black_knight);

    SceneObject left_white_rook = g_VirtualScene.at("rook");
    left_white_rook.set_name("left_white_rook");
    left_white_rook.scale(0.007f, 0.007, 0.007f);
    left_white_rook.set_position(-4.28f, piece_height,-4.37f);
    left_white_rook.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&left_white_rook);

    SceneObject left_black_rook = g_VirtualScene.at("rook");
    left_black_rook.set_name("left_black_rook");
    left_black_rook.scale(0.007f, 0.007, 0.007f);
    left_black_rook.set_position(-4.28f,piece_height,-3.42f);
    left_black_rook.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&left_black_rook);

    SceneObject a_white_pawn = g_VirtualScene.at("pawn");
    a_white_pawn.set_name("a_white_pawn");
    a_white_pawn.translate(-3.33f,piece_height,-4.24f);
    a_white_pawn.scale(0.007f, 0.007, 0.007f);
    a_white_pawn.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&a_white_pawn);

    SceneObject b_white_pawn = g_VirtualScene.at("pawn");
    b_white_pawn.set_name("b_white_pawn");
    b_white_pawn.translate(-3.465f,piece_height,-4.24f);
    b_white_pawn.scale(0.007f, 0.007, 0.007f);
    b_white_pawn.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&b_white_pawn);

    SceneObject c_white_pawn = g_VirtualScene.at("pawn");
    c_white_pawn.set_name("c_white_pawn");
    c_white_pawn.translate(-3.60f,piece_height,-4.24f);
    c_white_pawn.scale(0.007f, 0.007, 0.007f);
    c_white_pawn.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&c_white_pawn);

    SceneObject d_white_pawn = g_VirtualScene.at("pawn");
    d_white_pawn.set_name("d_white_pawn");
    d_white_pawn.translate(-3.735f,piece_height,-4.24f);
    d_white_pawn.scale(0.007f, 0.007, 0.007f);
    d_white_pawn.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&d_white_pawn);

    SceneObject e_white_pawn = g_VirtualScene.at("pawn");
    e_white_pawn.set_name("e_white_pawn");
    e_white_pawn.translate(-3.87f,piece_height,-4.24f);
    e_white_pawn.scale(0.007f, 0.007, 0.007f);
    e_white_pawn.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&e_white_pawn);

    SceneObject f_white_pawn = g_VirtualScene.at("pawn");
    f_white_pawn.set_name("f_white_pawn");
    f_white_pawn.translate(-4.005f,piece_height,-4.24f);
    f_white_pawn.scale(0.007f, 0.007, 0.007f);
    f_white_pawn.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&f_white_pawn);

    SceneObject g_white_pawn = g_VirtualScene.at("pawn");
    g_white_pawn.set_name("g_white_pawn");
    g_white_pawn.translate(-4.14f,piece_height,-4.24f);
    g_white_pawn.scale(0.007f, 0.007, 0.007f);
    g_white_pawn.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&g_white_pawn);

    SceneObject h_white_pawn = g_VirtualScene.at("pawn");
    h_white_pawn.set_name("h_white_pawn");
    h_white_pawn.translate(-4.275f,piece_height,-4.24f);
    h_white_pawn.scale(0.007f, 0.007, 0.007f);
    h_white_pawn.set_index(WHITE_PIECE);
    objects_to_draw.push_back(&h_white_pawn);

    SceneObject a_black_pawn = g_VirtualScene.at("pawn");
    a_black_pawn.set_name("a_black_pawn");
    a_black_pawn.translate(-3.33f,piece_height,-3.55f);
    a_black_pawn.scale(0.007f, 0.007, 0.007f);
    a_black_pawn.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&a_black_pawn);

    SceneObject b_black_pawn = g_VirtualScene.at("pawn");
    b_black_pawn.set_name("b_black_pawn");
    b_black_pawn.translate(-3.465f,piece_height,-3.55f);
    b_black_pawn.scale(0.007f, 0.007, 0.007f);
    b_black_pawn.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&b_black_pawn);

    SceneObject c_black_pawn = g_VirtualScene.at("pawn");
    c_black_pawn.set_name("c_black_pawn");
    c_black_pawn.translate(-3.60f,piece_height,-3.55f);
    c_black_pawn.scale(0.007f, 0.007, 0.007f);
    c_black_pawn.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&c_black_pawn);

    SceneObject d_black_pawn = g_VirtualScene.at("pawn");
    d_black_pawn.set_name("d_black_pawn");
    d_black_pawn.translate(-3.735f,piece_height,-3.55f);
    d_black_pawn.scale(0.007f, 0.007, 0.007f);
    d_black_pawn.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&d_black_pawn);

    SceneObject e_black_pawn = g_VirtualScene.at("pawn");
    e_black_pawn.set_name("e_black_pawn");
    e_black_pawn.translate(-3.87f,piece_height,-3.55f);
    e_black_pawn.scale(0.007f, 0.007, 0.007f);
    e_black_pawn.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&e_black_pawn);

    SceneObject f_black_pawn = g_VirtualScene.at("pawn");
    f_black_pawn.set_name("f_black_pawn");
    f_black_pawn.translate(-4.005f,piece_height,-3.55f);
    f_black_pawn.scale(0.007f, 0.007, 0.007f);
    f_black_pawn.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&f_black_pawn);

    SceneObject g_black_pawn = g_VirtualScene.at("pawn");
    g_black_pawn.set_name("g_black_pawn");
    g_black_pawn.translate(-4.14f,piece_height,-3.55f);
    g_black_pawn.scale(0.007f, 0.007, 0.007f);
    g_black_pawn.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&g_black_pawn);

    SceneObject h_black_pawn = g_VirtualScene.at("pawn");
    h_black_pawn.set_name("h_black_pawn") ;
    h_black_pawn.translate(-4.275f,piece_height,-3.55f);
    h_black_pawn.scale(0.007f, 0.007, 0.007f);
    h_black_pawn.set_index(BLACK_PIECE);
    objects_to_draw.push_back(&h_black_pawn);

    SceneObject table2 = g_VirtualScene.at("console-table");
    table2.set_name("console_table");
    table2.scale(3.0f, 2.5f, 2.5f);
    table2.set_position(5.0f,-1.0f,-6.0f);
    table2.set_index(CONSOLE_TABLE);
    table2.set_inspectable(false);
    objects_to_draw.push_back(&table2);

    SceneObject sofa = g_VirtualScene.at("Rectangle001");
    sofa.set_name("sofa");
    sofa.scale(0.002f, 0.002f, 0.0018f);
    sofa.set_position(1.0f,-1.0f,2.0f);
    sofa.mRotate(0,-PI2,0);
    sofa.set_index(SOFA);
    sofa.set_inspectable(false);
    objects_to_draw.push_back(&sofa);

    SceneObject shelf = g_VirtualScene.at("shelf");
    shelf.set_name("shelf");
    shelf.scale(7.0f,5.5f,5.5f);
    shelf.set_position(-9.0f,-1.0f,1.2f);
    shelf.mRotate(0,PI2,0);
    shelf.set_index(SHELF);
    shelf.set_inspectable(false);
    objects_to_draw.push_back(&shelf);

    SceneObject tv = g_VirtualScene.at("smart-tv_Text");
    tv.set_name("tv");
    tv.scale(0.5f, 0.5f, 0.5f);
    tv.set_position(-9.0f,0.5f,1.2f);
    tv.mRotate(0,PI2,0);
    tv.set_index(TV);
    objects_to_draw.push_back(&tv);

    SceneObject chair1 = g_VirtualScene.at("armless-chair-003");
    chair1.set_name("chair");
    chair1.scale(2.2f, 2.2f, 2.2f);
    chair1.set_position(-3.8f,-1.0f,-6.0f);
    chair1.set_index(CHAIR);
    objects_to_draw.push_back(&chair1);

    SceneObject coelho1 = g_VirtualScene.at("the_bunny");
    coelho1.set_name("coelho1");
    coelho1.scale(0.5f,0.5f,0.5f);
    coelho1.mRotate(0,PI2,0);
    coelho1.set_position(-9.0f, -0.5f, 1.2f);
    coelho1.set_index(BUNNY);
    objects_to_draw.push_back(&coelho1);

    SceneObject esfera1 = g_VirtualScene.at("the_sphere");
    esfera1.scale(0.5f, 0.5f, 0.5f);
    esfera1.set_name("esfera1");
    esfera1.set_position(-9.0f, -0.5f, 2.6f);
    esfera1.set_index(SPHERE);
    objects_to_draw.push_back(&esfera1);

    SceneObject esfera2 = g_VirtualScene.at("the_sphere");
    esfera2.scale(0.5f, 0.5f, 0.5f);
    esfera2.set_name("esfera2");
    esfera2.set_position(-9.0f, -0.5f, -0.2f);
    esfera2.set_index(SPHERE);
    objects_to_draw.push_back(&esfera2);

    SceneObject bed = g_VirtualScene.at("old_bed");
    bed.scale(0.013f, 0.013f, 0.013f);
    bed.mRotate(0,-PI2,0);
    bed.set_name("bed");
    bed.set_position(8.0f, -1.0f, -5.0f);
    bed.set_index(BED);
    objects_to_draw.push_back(&bed);

    SceneObject book_shelf = g_VirtualScene.at("bookshelf-031");
    book_shelf.set_name("bookshelf");
    book_shelf.scale(2.0f, 2.0f, 2.0f);
    book_shelf.mRotate(0,PI,0);
    book_shelf.set_position(-6.5f, -1.0f, 7.0f);
    book_shelf.set_index(BOOK_SHELF);
    book_shelf.set_inspectable(false);
    objects_to_draw.push_back(&book_shelf);

    SceneObject pack_book1 = g_VirtualScene.at("box.jpg");
    pack_book1.set_name("books");
    pack_book1.scale(0.6f, 0.35f, 0.4f);
    pack_book1.set_position(-7.2f, 0.38f, 7.0f);
    pack_book1.set_index(BOOKS);
    objects_to_draw.push_back(&pack_book1);

    SceneObject pack_book2 = g_VirtualScene.at("box.jpg");
    pack_book2.set_name("books");
    pack_book2.scale(0.6f, 0.35f, 0.4f);
    pack_book2.set_position(-7.0f, 1.32f, 7.0f);
    pack_book2.set_index(BOOKS);
    objects_to_draw.push_back(&pack_book2);

    SceneObject pack_book3 = g_VirtualScene.at("box.jpg");
    pack_book3.set_name("books");
    pack_book3.scale(0.6f, 0.35f, 0.4f);
    pack_book3.set_position(-7.2f, 2.22f, 7.0f);
    pack_book3.set_index(BOOKS);
    objects_to_draw.push_back(&pack_book3);

    SceneObject drawer_left = g_VirtualScene.at("drawer-left");
    drawer_left.set_name("drawer_left");
    drawer_left.scale(3.0f, 2.5f, 2.5f);
    drawer_left.set_position(5.0f,-1.0f,-6.0f);
    drawer_left.set_index(DRAWER);
    objects_to_draw.push_back(&drawer_left);

    SceneObject drawer_right = g_VirtualScene.at("drawer-right");
    drawer_right.set_name("drawer_right");
    drawer_right.scale(3.0f, 2.5f, 2.5f);
    drawer_right.set_position(5.0f,-1.0f,-6.0f);
    drawer_right.set_index(DRAWER);
    objects_to_draw.push_back(&drawer_right);

    SceneObject beam_bag = g_VirtualScene.at("Cube_Cube.001_Material.002");
    beam_bag.set_name("beam_bag");
    beam_bag.mRotate(0,PI/1.5,0);
    beam_bag.set_position(7.5f,-1.0f,5.5f);
    beam_bag.set_index(DRAWER);
    beam_bag.set_radius(1.0f);
    objects_to_draw.push_back(&beam_bag);

    for(SceneObject* obj : objects_to_draw){
        if(obj->get_index() == WHITE_PIECE || obj->get_index() == BLACK_PIECE){
            pieces_initial_position.insert(std::make_pair(obj->get_name(),obj->get_model()));
            chess_pieces.push_back(obj);
        }
    }

    std::vector<SceneObject*> objects_group = {&room_floor, &wall1, &wall2,
                                                &wall3, &wall4, &table, &coelho,
                                                &chess_board, &bowl, &black_king, &black_queen,
                                                &table2, &white_king, &white_queen, &g_black_pawn,
                                                &left_white_rook, &right_black_rook, &sofa,
                                                &shelf, &tv, &chair1, &coelho1, &esfera1,
                                                &esfera2, &bed, &book_shelf, &pack_book1,
                                                &pack_book2, &drawer_left, &drawer_right, &beam_bag};

    black_king.set_position(4.5f,piece_height+0.4,-6.0f);
    black_queen.set_position(-5.5f,piece_height,-3.3f);
    g_black_pawn.set_position(-0.8f, piece_height-0.3, 4.6f);
    g_black_pawn.mRotate(PI2,0,PI2);
    white_king.set_position(-6.0f,piece_height+0.1,-3.90f);
    white_king.mRotate(-PI2,0.0f,0.0f);
    white_queen.set_position(5.0f,piece_height-0.3f,-5.8f);
    left_white_rook.set_position(-6.2f, piece_height-0.1, 6.8f);
    right_black_rook.set_position(-6.0f, piece_height-0.1, 6.8f);
    left_white_bishop.set_position(-3.8f,-0.12f,-6.0f);
    left_white_bishop.mRotate(PI2, PI2, 0);



    if ( argc > 1 )
    {
        ObjModel model(argv[1]);
        BuildTrianglesAndAddToVirtualScene(&model);
    }

    // Inicializamos o código para renderização de texto.
    TextRendering_Init();

    // Habilitamos o Z-buffer. Veja slides 104-116 do documento Aula_09_Projecoes.pdf.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling. Veja slides 23-34 do documento Aula_13_Clipping_and_Culling.pdf e slides 112-123 do documento Aula_14_Laboratorio_3_Revisao.pdf.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    float speed = 5.0f; // Velocidade da câmera
    float prev_time = (float)glfwGetTime();

    float anim_speed = 2000;
    float t_bezier = 0.0f;
    float t_bezier2 = 0.0f;
    float t_bezier3 = 0.0f;
    float total_t = 0;
    float initial_t = 0;


    glm::mat4 model = Matrix_Identity();


    // Ficamos em um loop infinito, renderizando, até que o usuário feche a janela
    while (!glfwWindowShouldClose(window))
    {
        // Aqui executamos as operações de renderização

        // Definimos a cor do "fundo" do framebuffer como branco.  Tal cor é
        // definida como coeficientes RGBA: Red, Green, Blue, Alpha; isto é:
        // Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        //
        //           R     G     B     A
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

        // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
        // e também resetamos todos os pixels do Z-buffer (depth buffer).
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
        // os shaders de vértice e fragmentos).
        glUseProgram(g_GpuProgramID);

        // Computamos a posição da câmera utilizando coordenadas esféricas.  As
        // variáveis g_CameraDistance, g_CameraPhi, e g_CameraTheta são
        // controladas pelo mouse do usuário. Veja as funções CursorPosCallback()
        // e ScrollCallback().
        float r = g_CameraDistance;
        float y = r*sin(g_CameraPhi);
        float z = r*cos(g_CameraPhi)*cos(g_CameraTheta);
        float x = r*cos(g_CameraPhi)*sin(g_CameraTheta);

        // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
        // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::vec4 camera_position_c  = glm::vec4(cameraX,cameraY,cameraZ,1.0f); // Ponto "c", centro da câmera
        glm::vec4 camera_up_vector   = glm::vec4(0.0f,1.0f,0.0f,0.0f); // Vetor "up" fixado para apontar para o "céu" (eito Y global)
        //glm::vec4 camera_view_vector = camera_lookat_l - camera_position_c; // Vetor "view", sentido para onde a câmera está virada

        glm::vec4 camera_view_vector_mov = -glm::vec4(x, 0.0f, z, 0.0f);

        float current_time = (float)glfwGetTime();
        float delta_t = current_time - prev_time;
        prev_time = current_time;

        glm::vec3 direction_anim = glm::vec3(0,0,0);





        if(is_inspecting && interactable_object != NULL){
            glm::vec4 bbox_center = interactable_object->get_center();
            camera_view_vector = bbox_center - camera_position_c;
        } else if(collect_anim){
            /* Animação de coleta */
            glm::vec3 start_pos = glm::vec3(-3.8f,3.0f,-2.0f);
            glm::vec3 final_pos = glm::vec3(-3.8f,2.0f,-3.0f);
            glm::vec3 look_at = glm::vec3(-3.8f, 0.1f, -3.9f);
            glm::vec3 med_pos = calculateBezierPoint({start_pos, final_pos}, t_bezier3);

            cameraX = med_pos.x;
            cameraY = med_pos.y;
            cameraZ = med_pos.z;


            camera_view_vector = glm::vec4(look_at.x, look_at.y, look_at.z, 0) -
                                    glm::vec4(med_pos.x, med_pos.y, med_pos.z, 0);

            if(t_bezier3 <= 1.0f){
                t_bezier3 += 1.0f * delta_t * 2;
            } else {
                if(total_t == 0){
                    initial_t = (float)glfwGetTime();
                }
                float t = (float)glfwGetTime();
                float past_t = t - initial_t;
                total_t += past_t;
                initial_t = t;
                if(total_t >= 0.5f){
                    /* coloca a peca no tabuleiro */
                    glm::mat4 piece_model = pieces_initial_position.at(piece_to_reposition->get_name());
                    piece_to_reposition->set_model(piece_model);
                    piece_to_reposition->set_inspectable(false);
                }
                if(total_t >= 1.5f){
                    /* Sai da animação*/
                    all_pieces_collected = all_pieces_in_starting_pos();
                    if(!all_pieces_collected){
                        cameraX = old_camera_x;
                        cameraY = old_camera_y;
                        cameraZ = old_camera_z;
                    }

                    total_t = 0;
                    t_bezier3 = 0;
                    collect_anim = false;
                }
            }

        } else if(fst_anim){
            /* Animação inicial */
            glm::vec3 ponto1 = glm::vec3( 7.5f, 1.0f, -1.0f);
            glm::vec3 ponto2 = glm::vec3( 5.5f, 1.0f,  2.0f);
            glm::vec3 ponto3 = glm::vec3( 2.5f, 3.0f, -1.0f);
            glm::vec3 ponto4 = glm::vec3(-3.5f, 4.0f, -1.5f);
            glm::vec3 ponto5 = glm::vec3(-3.5f, 2.0f,  3.0f);
            glm::vec3 ponto6 = glm::vec3(-3.5f, 1.0f, -1.5f);
            glm::vec3 p_saida = calculateBezierPoint({ponto1, ponto2, ponto3, ponto4, ponto5, ponto6}, t_bezier);

            glm::vec3 vponto1 = glm::vec3( 5.5f, 1.0f, -1.0f);
            glm::vec3 vponto2 = glm::vec3( 5.5f, 0.0f,  2.0f);
            glm::vec3 vponto3 = glm::vec3( 2.5f, 0.0f, -1.0f);
            glm::vec3 vponto4 = glm::vec3(-3.5f, 0.0f, -1.5f);
            glm::vec3 vponto5 = glm::vec3(-3.5f, 0.0f,  3.0f);
            glm::vec3 vponto6 = glm::vec3(-4.5f, 0.0f, -6.5f);
            direction_anim = calculateBezierPoint({vponto1, vponto2, vponto3, vponto4, vponto5, vponto6}, t_bezier2);

            cameraX = p_saida.x;
            cameraY = p_saida.y;
            cameraZ = p_saida.z;

            camera_view_vector = glm::vec4(direction_anim.x, direction_anim.y, direction_anim.z, 1.0f)
                               - player.get_center();

            if(t_bezier <= 1.0f){
                if(t_bezier < 0.2f){
                    t_bezier += 0.0001f * delta_t * anim_speed;
                }else if(t_bezier > 0.7f){
                    t_bezier += 0.00002f * delta_t * anim_speed;
                } else if(t_bezier > 0.9f){
                    t_bezier += 0.00001f * delta_t * anim_speed;
                } else {
                    t_bezier += 0.0001f * delta_t * anim_speed;
                }
            }
            if(t_bezier2 <= 1.0f){
                    if(t_bezier2 > 0.8f){
                        t_bezier2 += 0.00003f * delta_t * anim_speed;
                    } else {
                        t_bezier2 += 0.00015f * delta_t * anim_speed;
                    }
            }

            if(t_bezier >= 1.0f){
                if(total_t == 0){
                    initial_t = (float)glfwGetTime();
                }
                float t = (float)glfwGetTime();
                float past_t = t - initial_t;
                total_t += past_t;
                initial_t = t;
                if(total_t > 2.0f){
                    fst_anim = false;
                    cameraX = 7.5f;;
                    cameraY = 1.0f;
                    cameraZ = -1.0f;
                    camera_view_vector = -glm::vec4(x, y, z, 0.0f);
                    total_t = 0;
                }
            }
        } else {
            camera_view_vector = -glm::vec4(x, y, z, 0.0f);
        }

        if(running){
            speed = 10.0f;
        } else {
            speed = 5.0f;
        }


        if(all_pieces_collected){
            total_t += delta_t;
            play_game_anim(total_t);
        }

        drawer(delta_t, player, drawer_left, drawer_right, black_king);

        // Computamos a matriz "View" utilizando os parâmetros da câmera para
        // definir o sistema de coordenadas da câmera.  Veja slides 2-14, 184-190 e 236-242 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
        glm::mat4 view = Matrix_Camera_View(camera_position_c, camera_view_vector, camera_up_vector);
        // Agora computamos a matriz de Projeção.
        glm::mat4 projection;

        // Note que, no sistema de coordenadas da câmera, os planos near e far
        // estão no sentido negativo! Veja slides 176-204 do documento Aula_09_Projecoes.pdf.
        float nearplane = -0.1f;  // Posição do "near plane"
        float farplane  = -50.0f; // Posição do "far plane"

        /* Att posicao de camera */



        glm::vec4 w = -camera_view_vector_mov/norm(camera_view_vector_mov);
        glm::vec4 u = crossproduct(camera_up_vector, w)/norm(crossproduct(camera_up_vector, w)); /*camera_up_vector * w;*/


        if(!fst_anim && !collect_anim && !all_pieces_in_starting_pos()){
            move_with_collision(player, objects_group, delta_t, speed, w, u);
        }



        float field_of_view = 3.141592 / 3.0f;
        projection = Matrix_Perspective(field_of_view, g_ScreenRatio, nearplane, farplane);

        // Enviamos as matrizes "view" e "projection" para a placa de vídeo
        // (GPU). Veja o arquivo "shader_vertex.glsl", onde estas são
        // efetivamente aplicadas em todos os pontos.
        glUniformMatrix4fv(g_view_uniform       , 1 , GL_FALSE , glm::value_ptr(view));
        glUniformMatrix4fv(g_projection_uniform , 1 , GL_FALSE , glm::value_ptr(projection));


        if(!is_inspecting){
            // PLAYER
            player.set_position(cameraX, cameraY, cameraZ);

            /* Desenha os objetos */
            draw_objects();

        }



        if(is_inspecting && interactable_object != NULL){
            //---------------------------- SKYBOX ----------------------------
            model = Matrix_Translate(camera_position_c.x,camera_position_c.y,camera_position_c.z);

            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);

            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, SKYBOX);
            DrawVirtualObject("skybox");

            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);

            //---------------------------- OBJETO INTERAGIDO ----------------------------
            glm::mat4 rotation_matrix = Matrix_Rotate_Z(g_AngleZ)
                                          * Matrix_Rotate_Y(g_AngleY)
                                          * Matrix_Rotate_X(g_AngleX);


            model = Matrix_Translate(interactable_object->get_center())
                      * rotation_matrix
                      * Matrix_Translate(-interactable_object->get_center())
                      * interactable_object->get_model();


            glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
            glUniform1i(g_object_id_uniform, interactable_object->get_index());
            DrawVirtualObject(interactable_object->get_model_name().c_str());

            if(interactable_object->get_index() == WHITE_PIECE || interactable_object->get_index() == BLACK_PIECE) {
                TextRendering_Press_F_To_Collect(window);
                piece_to_reposition = interactable_object;
            } else if(interactable_object->get_name() == "bowl" && hidden_pieces[0]){
            //---------------------------- OBJETOS SECUNDARIOS ----------------------------


                model = Matrix_Translate(interactable_object->get_center())
                      * rotation_matrix
                      * Matrix_Translate(-interactable_object->get_center())
                      * white_king.get_model();

                glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, white_king.get_index());
                DrawVirtualObject(white_king.get_model_name().c_str());

                piece_to_reposition = &white_king;

                glm::vec4 bowl_up = glm::vec4(0,1,0,0);
                glm::vec4 visible_v = ( rotation_matrix * bowl_up );
                visible_v.w = 0.0f;

                float inner_prod = dot(visible_v, -camera_view_vector);
                if(inner_prod > 1.0 ){
                    TextRendering_Press_F_To_Collect(window);
                }


            } else if(interactable_object->get_name() == "drawer_left" && hidden_pieces[1]){

                model = Matrix_Translate(interactable_object->get_center())
                      * rotation_matrix
                      * Matrix_Translate(-interactable_object->get_center())
                      * black_king.get_model();

                glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, black_king.get_index());
                DrawVirtualObject(black_king.get_model_name().c_str());

                piece_to_reposition = &black_king;

                glm::vec4 bowl_up = glm::vec4(0,1,0,0);
                glm::vec4 visible_v = ( rotation_matrix * bowl_up );
                visible_v.w = 0.0f;

                float inner_prod = dot(visible_v, -camera_view_vector);
                if(inner_prod > 1.2 ){
                    TextRendering_Press_F_To_Collect(window);
                }

            } else if(interactable_object->get_name() == "chair" && hidden_pieces[2]){
                model = Matrix_Translate(interactable_object->get_center())
                      * rotation_matrix
                      * Matrix_Translate(-interactable_object->get_center())
                      * left_white_bishop.get_model();

                glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(model));
                glUniform1i(g_object_id_uniform, left_white_bishop.get_index());
                DrawVirtualObject(left_white_bishop.get_model_name().c_str());

                piece_to_reposition = &left_white_bishop;

                glm::vec4 bowl_up = glm::vec4(0,-1,0,0);
                glm::vec4 visible_v = ( rotation_matrix * bowl_up );
                visible_v.w = 0.0f;

                float inner_prod = dot(visible_v, -camera_view_vector);
                if(inner_prod > 0 ){
                    TextRendering_Press_F_To_Collect(window);
                }
            }


        }

        if(!is_inspecting){
            interactable_object = GetInteractableObject(objects_group,camera_position_c,camera_view_vector);
        }

        // Imprimimos na tela os ângulos de Euler que controlam a rotação do
        // terceiro cubo.

        if(interactable_object != NULL && !is_inspecting && !fst_anim && !collect_anim && !all_pieces_collected){
            if(interactable_object->get_name() == "drawer_left"){
                TextRendering_Press_F_To_Open(window);
                if(open_left_drawer){
                    TextRendering_Press_E_To_Inspect(window);
                }
            } else if(interactable_object->get_name() == "drawer_right"){
                TextRendering_Press_F_To_Open(window);
                if(open_right_drawer){
                    TextRendering_Press_E_To_Inspect(window);
                }
            } else {
                TextRendering_Press_E_To_Inspect(window);
            }
        }

        if(game_ended){
            TextRendering_You_Lost(window);
            TextRendering_Press_esc_to_close_game(window);
        }

        // Imprimimos na tela informação sobre o número de quadros renderizados
        // por segundo (frames per second).
        TextRendering_ShowFramesPerSecond(window);

        // O framebuffer onde OpenGL executa as operações de renderização não
        // é o mesmo que está sendo mostrado para o usuário, caso contrário
        // seria possível ver artefatos conhecidos como "screen tearing". A
        // chamada abaixo faz a troca dos buffers, mostrando para o usuário
        // tudo que foi renderizado pelas funções acima.
        // Veja o link: https://en.wikipedia.org/w/index.php?title=Multiple_buffering&oldid=793452829#Double_buffering_in_computer_graphics
        glfwSwapBuffers(window);

        // Verificamos com o sistema operacional se houve alguma interação do
        // usuário (teclado, mouse, ...). Caso positivo, as funções de callback
        // definidas anteriormente usando glfwSet*Callback() serão chamadas
        // pela biblioteca GLFW.
        glfwPollEvents();
    }

    // Finalizamos o uso dos recursos do sistema operacional
    glfwTerminate();

    // Fim do programa
    return 0;
}

void draw_objects(){
    for(SceneObject *obj: objects_to_draw){
        glUniformMatrix4fv(g_model_uniform, 1 , GL_FALSE , glm::value_ptr(obj->get_model()));
        glUniform1i(g_object_id_uniform, obj->get_index());
        DrawVirtualObject(const_cast<char*>(obj->get_model_name().c_str()));
    }
}



void load_models(){
    ObjModel plane_model("../../data/plane.obj");
    ComputeNormals(&plane_model);
    BuildTrianglesAndAddToVirtualScene(&plane_model);

    ObjModel sphere_model("../../data/sphere.obj");
    ComputeNormals(&sphere_model);
    BuildTrianglesAndAddToVirtualScene(&sphere_model);

    ObjModel bunny_model("../../data/bunny.obj");
    ComputeNormals(&bunny_model);
    BuildTrianglesAndAddToVirtualScene(&bunny_model);

    ObjModel boxmodel("../../data/box/box.obj");
    ComputeNormals(&boxmodel);
    BuildTrianglesAndAddToVirtualScene(&boxmodel);

    ObjModel skyboxmodel("../../data/skybox.obj");
    ComputeNormals(&skyboxmodel);
    BuildTrianglesAndAddToVirtualScene(&skyboxmodel);

    ObjModel table_model("../../data/table/table.obj");
    ComputeNormals(&table_model);
    BuildTrianglesAndAddToVirtualScene(&table_model);

    ObjModel chess_model("../../data/chess/ChessBoard.obj");
    ComputeNormals(&chess_model);
    BuildTrianglesAndAddToVirtualScene(&chess_model);

    ObjModel rook_model("../../data/chess/rook.obj");
    ComputeNormals(&rook_model);
    BuildTrianglesAndAddToVirtualScene(&rook_model);

    ObjModel knight_model("../../data/chess/knight.obj");
    ComputeNormals(&knight_model);
    BuildTrianglesAndAddToVirtualScene(&knight_model);

    ObjModel bishop_model("../../data/chess/bishop.obj");
    ComputeNormals(&bishop_model);
    BuildTrianglesAndAddToVirtualScene(&bishop_model);

    ObjModel queen_model("../../data/chess/queen.obj");
    ComputeNormals(&queen_model);
    BuildTrianglesAndAddToVirtualScene(&queen_model);

    ObjModel king_model("../../data/chess/king.obj");
    ComputeNormals(&king_model);
    BuildTrianglesAndAddToVirtualScene(&king_model);

    ObjModel pawn_model("../../data/chess/pawn.obj");
    ComputeNormals(&pawn_model);
    BuildTrianglesAndAddToVirtualScene(&pawn_model);

    ObjModel bowl_model("../../data/bowl/bowl.obj");
    ComputeNormals(&bowl_model);
    BuildTrianglesAndAddToVirtualScene(&bowl_model);

    ObjModel console_table_model("../../data/console_table/console-table-004.obj");
    ComputeNormals(&console_table_model);
    BuildTrianglesAndAddToVirtualScene(&console_table_model);

    ObjModel sofa_model("../../data/sofa/sofa.obj");
    ComputeNormals(&sofa_model);
    BuildTrianglesAndAddToVirtualScene(&sofa_model);

    ObjModel tv_model("../../data/smarttv.obj");
    ComputeNormals(&tv_model);
    BuildTrianglesAndAddToVirtualScene(&tv_model);

    ObjModel shelf_model("../../data/shelf/shelf-040.obj");
    ComputeNormals(&shelf_model);
    BuildTrianglesAndAddToVirtualScene(&shelf_model);

    ObjModel chair_model("../../data/chair/chair.obj");
    ComputeNormals(&chair_model);
    BuildTrianglesAndAddToVirtualScene(&chair_model);

    ObjModel bed_model("../../data/bed/Old_bed.obj");
    ComputeNormals(&bed_model);
    BuildTrianglesAndAddToVirtualScene(&bed_model);

    ObjModel bookshelf_model("../../data/bookshelf/bookshelf-031.obj");
    ComputeNormals(&bookshelf_model);
    BuildTrianglesAndAddToVirtualScene(&bookshelf_model);

    ObjModel beam_bag_model("../../data/beam bag.obj");
    ComputeNormals(&beam_bag_model);
    BuildTrianglesAndAddToVirtualScene(&beam_bag_model);
}


// Função que desenha um objeto armazenado em g_VirtualScene. Veja definição
// dos objetos na função BuildTrianglesAndAddToVirtualScene().
void DrawVirtualObject(const char* object_name)
{
    // "Ligamos" o VAO. Informamos que queremos utilizar os atributos de
    // vértices apontados pelo VAO criado pela função BuildTrianglesAndAddToVirtualScene(). Veja
    // comentários detalhados dentro da definição de BuildTrianglesAndAddToVirtualScene().
    glBindVertexArray(g_VirtualScene[object_name].get_vertex_array_object_id());

    // Pedimos para a GPU rasterizar os vértices dos eixos XYZ
    // apontados pelo VAO como linhas. Veja a definição de
    // g_VirtualScene[""] dentro da função BuildTrianglesAndAddToVirtualScene(), e veja
    // a documentação da função glDrawElements() em
    // http://docs.gl/gl3/glDrawElements.
    glDrawElements(
        g_VirtualScene[object_name].get_rendering_mode(),
        g_VirtualScene[object_name].get_num_indices(),
        GL_UNSIGNED_INT,
        (void*)(g_VirtualScene[object_name].get_first_index() * sizeof(GLuint))
    );

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Função que carrega os shaders de vértices e de fragmentos que serão
// utilizados para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estão fixados, sendo que assumimos a existência
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo
    // (GPU)! Veja arquivo "shader_vertex.glsl" e "shader_fragment.glsl".
    g_model_uniform      = glGetUniformLocation(g_GpuProgramID, "model"); // Variável da matriz "model"
    g_view_uniform       = glGetUniformLocation(g_GpuProgramID, "view"); // Variável da matriz "view" em shader_vertex.glsl
    g_projection_uniform = glGetUniformLocation(g_GpuProgramID, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    g_object_id_uniform  = glGetUniformLocation(g_GpuProgramID, "object_id"); // Variável "object_id" em shader_fragment.glsl
    g_bbox_min_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_min");
    g_bbox_max_uniform   = glGetUniformLocation(g_GpuProgramID, "bbox_max");

    // Variáveis em "shader_fragment.glsl" para acesso das imagens de textura
    glUseProgram(g_GpuProgramID);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage0"), 0);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage1"), 1);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage2"), 2);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage3"), 3);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage4"), 4);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage5"), 5);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage6"), 6);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage7"), 7);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage8"), 8);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage9"), 9);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage10"), 10);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage11"), 11);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage12"), 12);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage13"), 13);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage14"), 14);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage15"), 15);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage16"), 16);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage17"), 17);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage18"), 18);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage19"), 19);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage20"), 20);
    glUniform1i(glGetUniformLocation(g_GpuProgramID, "TextureImage21"), 21);
    glUseProgram(0);
}


// Função que computa as normais de um ObjModel, caso elas não tenham sido
// especificadas dentro do arquivo ".obj"
void ComputeNormals(ObjModel* model)
{
    if ( !model->attrib.normals.empty() )
        return;

    // Primeiro computamos as normais para todos os TRIÂNGULOS.
    // Segundo, computamos as normais dos VÉRTICES através do método proposto
    // por Gouraud, onde a normal de cada vértice vai ser a média das normais de
    // todas as faces que compartilham este vértice.

    size_t num_vertices = model->attrib.vertices.size() / 3;

    std::vector<int> num_triangles_per_vertex(num_vertices, 0);
    std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f,0.0f,0.0f,0.0f));

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            glm::vec4  vertices[3];
            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
            }

            const glm::vec4  a = vertices[0];
            const glm::vec4  b = vertices[1];
            const glm::vec4  c = vertices[2];

            // PREENCHA AQUI o cálculo da normal de um triângulo cujos vértices
            // estão nos pontos "a", "b", e "c", definidos no sentido anti-horário.
            const glm::vec4  n = crossproduct(b-a,c-a);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];
                num_triangles_per_vertex[idx.vertex_index] += 1;
                vertex_normals[idx.vertex_index] += n;
                model->shapes[shape].mesh.indices[3*triangle + vertex].normal_index = idx.vertex_index;
            }
        }
    }

    model->attrib.normals.resize( 3*num_vertices );

    for (size_t i = 0; i < vertex_normals.size(); ++i)
    {
        glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
        n /= norm(n);
        model->attrib.normals[3*i + 0] = n.x;
        model->attrib.normals[3*i + 1] = n.y;
        model->attrib.normals[3*i + 2] = n.z;
    }
}

// Constrói triângulos para futura renderização a partir de um ObjModel.
void BuildTrianglesAndAddToVirtualScene(ObjModel* model)
{
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;

    for (size_t shape = 0; shape < model->shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = model->shapes[shape].mesh.num_face_vertices.size();

        const float minval = std::numeric_limits<float>::min();
        const float maxval = std::numeric_limits<float>::max();

        glm::vec3 bbox_min = glm::vec3(maxval,maxval,maxval);
        glm::vec3 bbox_max = glm::vec3(minval,minval,minval);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert(model->shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = model->shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(first_index + 3*triangle + vertex);

                const float vx = model->attrib.vertices[3*idx.vertex_index + 0];
                const float vy = model->attrib.vertices[3*idx.vertex_index + 1];
                const float vz = model->attrib.vertices[3*idx.vertex_index + 2];
                //printf("tri %d vert %d = (%.2f, %.2f, %.2f)\n", (int)triangle, (int)vertex, vx, vy, vz);
                model_coefficients.push_back( vx ); // X
                model_coefficients.push_back( vy ); // Y
                model_coefficients.push_back( vz ); // Z
                model_coefficients.push_back( 1.0f ); // W

                bbox_min.x = std::min(bbox_min.x, vx);
                bbox_min.y = std::min(bbox_min.y, vy);
                bbox_min.z = std::min(bbox_min.z, vz);
                bbox_max.x = std::max(bbox_max.x, vx);
                bbox_max.y = std::max(bbox_max.y, vy);
                bbox_max.z = std::max(bbox_max.z, vz);

                // Inspecionando o código da tinyobjloader, o aluno Bernardo
                // Sulzbach (2017/1) apontou que a maneira correta de testar se
                // existem normais e coordenadas de textura no ObjModel é
                // comparando se o índice retornado é -1. Fazemos isso abaixo.

                if ( idx.normal_index != -1 )
                {
                    const float nx = model->attrib.normals[3*idx.normal_index + 0];
                    const float ny = model->attrib.normals[3*idx.normal_index + 1];
                    const float nz = model->attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx ); // X
                    normal_coefficients.push_back( ny ); // Y
                    normal_coefficients.push_back( nz ); // Z
                    normal_coefficients.push_back( 0.0f ); // W
                }

                if ( idx.texcoord_index != -1 )
                {
                    const float u = model->attrib.texcoords[2*idx.texcoord_index + 0];
                    const float v = model->attrib.texcoords[2*idx.texcoord_index + 1];
                    texture_coefficients.push_back( u );
                    texture_coefficients.push_back( v );
                }
            }
        }

        size_t last_index = indices.size() - 1;




        SceneObject *theobject = new SceneObject (first_index,
                                                         last_index - first_index + 1,
                                                         GL_TRIANGLES,
                                                         vertex_array_object_id,
                                                         obj_index++,
                                                         bbox_min,
                                                         bbox_max);



        theobject->set_name(model->shapes[shape].name);
        theobject->set_model_name(model->shapes[shape].name);
        g_VirtualScene[model->shapes[shape].name] = *theobject;
    }

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if ( !normal_coefficients.empty() )
    {
        GLuint VBO_normal_coefficients_id;
        glGenBuffers(1, &VBO_normal_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if ( !texture_coefficients.empty() )
    {
        GLuint VBO_texture_coefficients_id;
        glGenBuffers(1, &VBO_texture_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
        location = 2; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
    //

    // "Desligamos" o VAO, evitando assim que operações posteriores venham a
    // alterar o mesmo. Isso evita bugs.
    glBindVertexArray(0);
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Os "Shader Objects" podem ser marcados para deleção após serem linkados
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    // Retornamos o ID gerado acima
    return program_id;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula ({+ViewportMapping2+}).
    glViewport(0, 0, width, height);

    // Atualizamos também a razão que define a proporção da janela (largura /
    // altura), a qual será utilizada na definição das matrizes de projeção,
    // tal que não ocorra distorções durante o processo de "Screen Mapping"
    // acima, quando NDC é mapeado para coordenadas de pixels. Veja slides 205-215 do documento Aula_09_Projecoes.pdf.
    //
    // O cast para float é necessário pois números inteiros são arredondados ao
    // serem divididos!
    g_ScreenRatio = (float)width / height;
}

// Variáveis globais que armazenam a última posição do cursor do mouse, para
// que possamos calcular quanto que o mouse se movimentou entre dois instantes
// de tempo. Utilizadas no callback CursorPosCallback() abaixo.
double g_LastCursorPosX, g_LastCursorPosY;

// Função callback chamada sempre que o usuário aperta algum dos botões do mouse
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_LeftMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_LeftMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_LeftMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_RightMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_RightMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_RightMouseButtonPressed = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a
        // posição atual do cursor nas variáveis g_LastCursorPosX e
        // g_LastCursorPosY.  Também, setamos a variável
        // g_MiddleMouseButtonPressed como true, para saber que o usuário está
        // com o botão esquerdo pressionado.
        glfwGetCursorPos(window, &g_LastCursorPosX, &g_LastCursorPosY);
        g_MiddleMouseButtonPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a
        // variável abaixo para false.
        g_MiddleMouseButtonPressed = false;
    }
}

// Função callback chamada sempre que o usuário movimentar o cursor do mouse em
// cima da janela OpenGL.
void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (g_LeftMouseButtonPressed)
    {
        // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
        float dx = xpos - g_LastCursorPosX;
        float dy = ypos - g_LastCursorPosY;


        if(is_inspecting){
            g_AngleX += 0.002*dx;
            g_AngleY += 0.002*dy;
        }else if(!fst_anim && !collect_anim){
            // Atualizamos parâmetros da câmera com os deslocamentos
            g_CameraTheta -= 0.01f*dx;
            g_CameraPhi   += 0.01f*dy;

            // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
            float phimax = 3.141592f/2;
            float phimin = -phimax;

            if (g_CameraPhi > phimax)
                g_CameraPhi = phimax;

            if (g_CameraPhi < phimin)
                g_CameraPhi = phimin;

        }

        // Atualizamos as variáveis globais para armazenar a posição atual do
        // cursor como sendo a última posição conhecida do cursor.
        g_LastCursorPosX = xpos;
        g_LastCursorPosY = ypos;
    }
}

// Função callback chamada sempre que o usuário movimenta a "rodinha" do mouse.
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    if(is_inspecting && (yoffset < 0 || g_CameraDistance > 0.5 )){
        g_CameraDistance -= 0.1f*yoffset;
    }
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // ==================
    // Não modifique este loop! Ele é utilizando para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // ==================

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        is_inspecting = false;
        g_CameraDistance = 3.5;


        if(game_ended){
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

    }


    // Se o usuário apertar a tecla H, fazemos um "toggle" do texto informativo mostrado na tela.
    if (key == GLFW_KEY_H && action == GLFW_PRESS)
    {
        all_pieces_collected = true;
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        LoadShadersFromFiles();
        fprintf(stdout,"Shaders recarregados!\n");
        fflush(stdout);
    }

    if(key == GLFW_KEY_E && action == GLFW_PRESS && !fst_anim && !collect_anim && !all_pieces_collected){
        if(interactable_object != NULL && !is_inspecting){
            if((interactable_object->get_name() == "drawer_left" && !open_left_drawer) ||
                (interactable_object->get_name() == "drawer_right" && !open_right_drawer)){
                // gaveta fechada
            } else {
                is_inspecting = true;
                g_AngleX = 0.0;
                g_AngleY = 0.0;
                g_AngleZ = 0.0;
            }
        }
    }

    /* Abrir gaveta */
    if(key == GLFW_KEY_F && interactable_object && action == GLFW_PRESS && !is_inspecting){
        if(interactable_object->get_name() == "drawer_left"){
            open_left_drawer = !open_left_drawer;
        }
        if(interactable_object->get_name() == "drawer_right"){
            open_right_drawer = !open_right_drawer;
        }
    }

    /*Coleta*/
    if(key == GLFW_KEY_F && GLFW_PRESS & is_inspecting){
        old_camera_x = cameraX;
        old_camera_y = cameraY;
        old_camera_z = cameraZ;
        if(interactable_object->get_name() == "bowl" && hidden_pieces[0]){
            //white king
            glm::vec4 bowl_up = glm::vec4(0,1,0,0);
            glm::vec4 visible_v = ( Matrix_Rotate_Z(g_AngleZ)
                                   * Matrix_Rotate_Y(g_AngleY)
                                   * Matrix_Rotate_X(g_AngleX)
                                   * bowl_up
                                   );
            visible_v.w = 0.0f;

            float inner_prod = dot(visible_v, -camera_view_vector);
            if(inner_prod > 1.0 ){
                is_inspecting = false;
                collect_anim = true;
                hidden_pieces[0] = false;

            }

        } else if(interactable_object->get_index() == WHITE_PIECE || interactable_object->get_index() == BLACK_PIECE){
            is_inspecting = false;
            collect_anim = true;
        } else if(interactable_object->get_name() == "drawer_left" && hidden_pieces[1]){
            //black king
            glm::vec4 bowl_up = glm::vec4(0,1,0,0);
            glm::vec4 visible_v = ( Matrix_Rotate_Z(g_AngleZ)
                                   * Matrix_Rotate_Y(g_AngleY)
                                   * Matrix_Rotate_X(g_AngleX)
                                   * bowl_up
                                   );
            visible_v.w = 0.0f;

            float inner_prod = dot(visible_v, -camera_view_vector);
            if(inner_prod > 1.2 ){
                is_inspecting = false;
                collect_anim = true;
                hidden_pieces[1] = false;
            }
        } else if(interactable_object->get_name() == "chair" && hidden_pieces[2]){
            //left white bishop
            glm::vec4 bowl_up = glm::vec4(0,-1,0,0);
            glm::vec4 visible_v = ( Matrix_Rotate_Z(g_AngleZ)
                                   * Matrix_Rotate_Y(g_AngleY)
                                   * Matrix_Rotate_X(g_AngleX)
                                   * bowl_up
                                   );
            visible_v.w = 0.0f;

            float inner_prod = dot(visible_v, -camera_view_vector);
            if(inner_prod > 0.0 ){
                is_inspecting = false;
                collect_anim = true;
                hidden_pieces[2] = false;
            }
        }

    }

    /* coordenadas da camera: x, y, z*/
    /* W -> move para frente */

    if(!is_inspecting && action == GLFW_PRESS){
        if(key == GLFW_KEY_W ){
            moving_forward = true;
        }

        /* S -> move para tras */
        if(key == GLFW_KEY_S){
            moving_backwards = true;
        }

        /* A -> move para esquerda */
        if(key == GLFW_KEY_A){
            moving_left = true;
        }

        /* D -> move para direita */
        if(key == GLFW_KEY_D ){
            moving_right = true;
        }

        if(key == GLFW_KEY_LEFT_SHIFT){
            running = true;
        }

        /* Space -> move para cima */
        if(key == GLFW_KEY_SPACE ){
            moving_up = true;
        }

        /* ctrl -> move para baixo */
        if(key == GLFW_KEY_LEFT_CONTROL){
            moving_down = true;
        }
    }

    if(action == GLFW_RELEASE){
        if(key == GLFW_KEY_W ){
            moving_forward = false;
        }

        /* S -> move para tras */
        if(key == GLFW_KEY_S){
            moving_backwards = false;
        }

        /* A -> move para esquerda */
        if(key == GLFW_KEY_A){
            moving_left = false;
        }

        /* D -> move para direita */
        if(key == GLFW_KEY_D ){
            moving_right = false;
        }

        if(key == GLFW_KEY_LEFT_SHIFT){
            running = false;
        }

        /* Space -> move para cima */
        if(key == GLFW_KEY_SPACE ){
            moving_up = false;
        }

        /* ctrl -> move para baixo */
        if(key == GLFW_KEY_LEFT_CONTROL){
            moving_down = false;
        }
    }


}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// Esta função recebe um vértice com coordenadas de modelo p_model e passa o
// mesmo por todos os sistemas de coordenadas armazenados nas matrizes model,
// view, e projection; e escreve na tela as matrizes e pontos resultantes
// dessas transformações.

// Escrevemos na tela os ângulos de Euler definidos nas variáveis globais
// g_AngleX, g_AngleY, e g_AngleZ.
void TextRendering_Press_E_To_Inspect(GLFWwindow* window)
{

    char buffer[20];
    snprintf(buffer, 20, "Press E to inspect\n");

    TextRendering_PrintString(window, buffer, -0.15, -0.5f, 2.0f);
}

void TextRendering_Press_F_To_Collect(GLFWwindow* window)
{

    char buffer[20];
    snprintf(buffer, 20, "Press F to collect\n");

    TextRendering_PrintString(window, buffer, -0.15, -0.5f, 2.0f);
}

void TextRendering_You_Lost(GLFWwindow* window)
{

    char buffer[20];
    snprintf(buffer, 20, "You Lost!\n");

    TextRendering_PrintString(window, buffer, -0.19, +0.6f, 5.0f);

}
void TextRendering_Press_esc_to_close_game(GLFWwindow* window)
{

    char buffer[20];
    snprintf(buffer, 20, "Press Esc to close the game.\n");

    TextRendering_PrintString(window, buffer, -0.15, +0.5f, 2.0f);

}


void TextRendering_Press_F_To_Open(GLFWwindow* window)
{

    float pad = TextRendering_LineHeight(window);

    char buffer[22];
    snprintf(buffer, 22, "Press F to open/close\n");

    TextRendering_PrintString(window, buffer, -0.15, -0.6f, 2.0f);
}


// Escrevemos na tela o número de quadros renderizados por segundo (frames per
// second).
void TextRendering_ShowFramesPerSecond(GLFWwindow* window)
{
    // Variáveis estáticas (static) mantém seus valores entre chamadas
    // subsequentes da função!
    static float old_seconds = (float)glfwGetTime();
    static int   ellapsed_frames = 0;
    static char  buffer[20] = "?? fps";
    static int   numchars = 7;

    ellapsed_frames += 1;

    // Recuperamos o número de segundos que passou desde a execução do programa
    float seconds = (float)glfwGetTime();

    // Número de segundos desde o último cálculo do fps
    float ellapsed_seconds = seconds - old_seconds;

    if ( ellapsed_seconds > 1.0f )
    {
        numchars = snprintf(buffer, 20, "%.2f fps", ellapsed_frames / ellapsed_seconds);

        old_seconds = seconds;
        ellapsed_frames = 0;
    }

    float lineheight = TextRendering_LineHeight(window);
    float charwidth = TextRendering_CharWidth(window);

    TextRendering_PrintString(window, buffer, 1.0f-(numchars + 1)*charwidth, 1.0f-lineheight, 1.0f);
}

void LoadTextureImage(const char* filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Parâmetros de amostragem da textura.
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = g_NumLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    g_NumLoadedTextures += 1;
}

// Função para debugging: imprime no terminal todas informações de um modelo
// geométrico carregado de um arquivo ".obj".
// Veja: https://github.com/syoyo/tinyobjloader/blob/22883def8db9ef1f3ffb9b404318e7dd25fdbb51/loader_example.cc#L98
void PrintObjModelInfo(ObjModel* model)
{
  const tinyobj::attrib_t                & attrib    = model->attrib;
  const std::vector<tinyobj::shape_t>    & shapes    = model->shapes;
  const std::vector<tinyobj::material_t> & materials = model->materials;

  printf("# of vertices  : %d\n", (int)(attrib.vertices.size() / 3));
  printf("# of normals   : %d\n", (int)(attrib.normals.size() / 3));
  printf("# of texcoords : %d\n", (int)(attrib.texcoords.size() / 2));
  printf("# of shapes    : %d\n", (int)shapes.size());
  printf("# of materials : %d\n", (int)materials.size());

  for (size_t v = 0; v < attrib.vertices.size() / 3; v++) {
    printf("  v[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.vertices[3 * v + 0]),
           static_cast<const double>(attrib.vertices[3 * v + 1]),
           static_cast<const double>(attrib.vertices[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.normals.size() / 3; v++) {
    printf("  n[%ld] = (%f, %f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.normals[3 * v + 0]),
           static_cast<const double>(attrib.normals[3 * v + 1]),
           static_cast<const double>(attrib.normals[3 * v + 2]));
  }

  for (size_t v = 0; v < attrib.texcoords.size() / 2; v++) {
    printf("  uv[%ld] = (%f, %f)\n", static_cast<long>(v),
           static_cast<const double>(attrib.texcoords[2 * v + 0]),
           static_cast<const double>(attrib.texcoords[2 * v + 1]));
  }

  // For each shape
  for (size_t i = 0; i < shapes.size(); i++) {
    printf("shape[%ld].name = %s\n", static_cast<long>(i),
           shapes[i].name.c_str());
    printf("Size of shape[%ld].indices: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.indices.size()));

    size_t index_offset = 0;

    assert(shapes[i].mesh.num_face_vertices.size() ==
           shapes[i].mesh.material_ids.size());

    printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

    // For each face
    for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
      size_t fnum = shapes[i].mesh.num_face_vertices[f];

      printf("  face[%ld].fnum = %ld\n", static_cast<long>(f),
             static_cast<unsigned long>(fnum));

      // For each vertex in the face
      for (size_t v = 0; v < fnum; v++) {
        tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
        printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f),
               static_cast<long>(v), idx.vertex_index, idx.normal_index,
               idx.texcoord_index);
      }

      printf("  face[%ld].material_id = %d\n", static_cast<long>(f),
             shapes[i].mesh.material_ids[f]);

      index_offset += fnum;
    }

    printf("shape[%ld].num_tags: %lu\n", static_cast<long>(i),
           static_cast<unsigned long>(shapes[i].mesh.tags.size()));
    for (size_t t = 0; t < shapes[i].mesh.tags.size(); t++) {
      printf("  tag[%ld] = %s ", static_cast<long>(t),
             shapes[i].mesh.tags[t].name.c_str());
      printf(" ints: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].intValues.size(); ++j) {
        printf("%ld", static_cast<long>(shapes[i].mesh.tags[t].intValues[j]));
        if (j < (shapes[i].mesh.tags[t].intValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" floats: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].floatValues.size(); ++j) {
        printf("%f", static_cast<const double>(
                         shapes[i].mesh.tags[t].floatValues[j]));
        if (j < (shapes[i].mesh.tags[t].floatValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");

      printf(" strings: [");
      for (size_t j = 0; j < shapes[i].mesh.tags[t].stringValues.size(); ++j) {
        printf("%s", shapes[i].mesh.tags[t].stringValues[j].c_str());
        if (j < (shapes[i].mesh.tags[t].stringValues.size() - 1)) {
          printf(", ");
        }
      }
      printf("]");
      printf("\n");
    }
  }

  for (size_t i = 0; i < materials.size(); i++) {
    printf("material[%ld].name = %s\n", static_cast<long>(i),
           materials[i].name.c_str());
    printf("  material.Ka = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].ambient[0]),
           static_cast<const double>(materials[i].ambient[1]),
           static_cast<const double>(materials[i].ambient[2]));
    printf("  material.Kd = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].diffuse[0]),
           static_cast<const double>(materials[i].diffuse[1]),
           static_cast<const double>(materials[i].diffuse[2]));
    printf("  material.Ks = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].specular[0]),
           static_cast<const double>(materials[i].specular[1]),
           static_cast<const double>(materials[i].specular[2]));
    printf("  material.Tr = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].transmittance[0]),
           static_cast<const double>(materials[i].transmittance[1]),
           static_cast<const double>(materials[i].transmittance[2]));
    printf("  material.Ke = (%f, %f ,%f)\n",
           static_cast<const double>(materials[i].emission[0]),
           static_cast<const double>(materials[i].emission[1]),
           static_cast<const double>(materials[i].emission[2]));
    printf("  material.Ns = %f\n",
           static_cast<const double>(materials[i].shininess));
    printf("  material.Ni = %f\n", static_cast<const double>(materials[i].ior));
    printf("  material.dissolve = %f\n",
           static_cast<const double>(materials[i].dissolve));
    printf("  material.illum = %d\n", materials[i].illum);
    printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
    printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
    printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
    printf("  material.map_Ns = %s\n",
           materials[i].specular_highlight_texname.c_str());
    printf("  material.map_bump = %s\n", materials[i].bump_texname.c_str());
    printf("  material.map_d = %s\n", materials[i].alpha_texname.c_str());
    printf("  material.disp = %s\n", materials[i].displacement_texname.c_str());
    printf("  <<PBR>>\n");
    printf("  material.Pr     = %f\n", materials[i].roughness);
    printf("  material.Pm     = %f\n", materials[i].metallic);
    printf("  material.Ps     = %f\n", materials[i].sheen);
    printf("  material.Pc     = %f\n", materials[i].clearcoat_thickness);
    printf("  material.Pcr    = %f\n", materials[i].clearcoat_thickness);
    printf("  material.aniso  = %f\n", materials[i].anisotropy);
    printf("  material.anisor = %f\n", materials[i].anisotropy_rotation);
    printf("  material.map_Ke = %s\n", materials[i].emissive_texname.c_str());
    printf("  material.map_Pr = %s\n", materials[i].roughness_texname.c_str());
    printf("  material.map_Pm = %s\n", materials[i].metallic_texname.c_str());
    printf("  material.map_Ps = %s\n", materials[i].sheen_texname.c_str());
    printf("  material.norm   = %s\n", materials[i].normal_texname.c_str());
    std::map<std::string, std::string>::const_iterator it(
        materials[i].unknown_parameter.begin());
    std::map<std::string, std::string>::const_iterator itEnd(
        materials[i].unknown_parameter.end());

    for (; it != itEnd; it++) {
      printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
    }
    printf("\n");
  }
}

glm::vec3 calculateBezierPoint(const std::vector<glm::vec3>& controlPoints, float t){
    int n = controlPoints.size() - 1;
    std::vector<glm::vec3> tempPoints = controlPoints;

    while (n > 0){
        for (int i = 0; i < n; i++){
            tempPoints[i] = tempPoints[i] + t*(tempPoints[i + 1] - tempPoints[i]);
        }
        n--;
    }

    return tempPoints[0];
}

void move_with_collision(SceneObject player,
                         std::vector<SceneObject*> objects_group,
                         float delta_t,
                         float speed,
                         glm::vec4 w,
                         glm::vec4 u){
    bool colFX = false;
    bool colFZ = false;
    bool movF = false;

    bool colBX = false;
    bool colBZ = false;
    bool movB = false;

    bool colRX = false;
    bool colRZ = false;
    bool movR = false;

    bool colLX = false;
    bool colLZ = false;
    bool movL = false;

    for(SceneObject *i : objects_group){
        SceneObject obj = *i;

        if(obj.has_collision()){
            float nextX = cameraX;
            float nextZ = cameraZ;
            SceneObject player_dislocated_in_X = player;
            SceneObject player_dislocated_in_Z = player;
            if (moving_forward){
                movF = true;
                nextX += -w.x * delta_t * speed;
                nextZ += -w.z * delta_t * speed;
                player_dislocated_in_X.set_position(nextX,cameraY,cameraZ);
                player_dislocated_in_Z.set_position(cameraX,cameraY,nextZ);

                if(obj.is_sphere()){
                    if(isCubeIntersectingSphere(player_dislocated_in_X.get_bbox_min(),
                                                player_dislocated_in_X.get_bbox_max(),
                                                obj.get_center(),
                                                obj.get_radius())){
                        colFX = true;
                    }
                    if(isCubeIntersectingSphere(player_dislocated_in_Z.get_bbox_min(),
                                                player_dislocated_in_Z.get_bbox_max(),
                                                obj.get_center(),
                                                obj.get_radius())){
                        colFZ = true;
                    }
                }else{
                    if(isBoundingBoxIntersection(player_dislocated_in_X, obj)){
                    colFX = true;
                    }
                    if(isBoundingBoxIntersection(player_dislocated_in_Z, obj)){
                        colFZ = true;
                    }
                }


            }
             if(moving_backwards){
                movB = true;
                nextX += w.x * delta_t * speed;
                nextZ += w.z * delta_t * speed;
                player_dislocated_in_X.set_position(nextX,cameraY,cameraZ);
                player_dislocated_in_Z.set_position(cameraX,cameraY,nextZ);

                if(obj.is_sphere()){
                    if(isCubeIntersectingSphere(player_dislocated_in_X.get_bbox_min(),
                                                player_dislocated_in_X.get_bbox_max(),
                                                obj.get_center(),
                                                obj.get_radius())){
                        colBX = true;
                    }
                    if(isCubeIntersectingSphere(player_dislocated_in_Z.get_bbox_min(),
                                                player_dislocated_in_Z.get_bbox_max(),
                                                obj.get_center(),
                                                obj.get_radius())){
                        colBZ = true;
                    }
                }else{
                    if(isBoundingBoxIntersection(player_dislocated_in_X, obj)){
                        colBX = true;
                    }
                    if(isBoundingBoxIntersection(player_dislocated_in_Z, obj)){
                        colBZ = true;
                    }
                }
            }
            if(moving_right){
                movR = true;
                nextX += u.x * delta_t * speed;
                nextZ += u.z * delta_t * speed;
                player_dislocated_in_X.set_position(nextX,cameraY,cameraZ);
                player_dislocated_in_Z.set_position(cameraX,cameraY,nextZ);
                if(obj.is_sphere()){
                    if(isCubeIntersectingSphere(player_dislocated_in_X.get_bbox_min(),
                                                player_dislocated_in_X.get_bbox_max(),
                                                obj.get_center(),
                                                obj.get_radius())){
                        colRX = true;
                    }
                    if(isCubeIntersectingSphere(player_dislocated_in_Z.get_bbox_min(),
                                                player_dislocated_in_Z.get_bbox_max(),
                                                obj.get_center(),
                                                obj.get_radius())){
                        colRZ = true;
                    }
                }else{
                    if(isBoundingBoxIntersection(player_dislocated_in_X, obj)){
                        colRX = true;
                    }
                    if(isBoundingBoxIntersection(player_dislocated_in_Z, obj)){
                        colRZ = true;
                    }
                }
            }
            if(moving_left){
                movL = true;
                nextX += -u.x * delta_t * speed;
                nextZ += -u.z * delta_t * speed;
                player_dislocated_in_X.set_position(nextX,cameraY,cameraZ);
                player_dislocated_in_Z.set_position(cameraX,cameraY,nextZ);

                if(obj.is_sphere()){
                    if(isCubeIntersectingSphere(player_dislocated_in_X.get_bbox_min(),
                                                player_dislocated_in_X.get_bbox_max(),
                                                obj.get_center(),
                                                obj.get_radius())){
                        colLX = true;
                    }
                    if(isCubeIntersectingSphere(player_dislocated_in_Z.get_bbox_min(),
                                                player_dislocated_in_Z.get_bbox_max(),
                                                obj.get_center(),
                                                obj.get_radius())){
                        colLZ = true;
                    }
                }else{
                    if(isBoundingBoxIntersection(player_dislocated_in_X, obj)){
                        colLX = true;
                    }
                    if(isBoundingBoxIntersection(player_dislocated_in_Z, obj)){
                        colLZ = true;
                    }
                }
            }
        }
    }
    // Atualizar posicao depois de fazer todos os testes
    if(!colFX && movF){
        cameraX += -w.x * delta_t * speed;
    }
    if(!colFZ && movF){
        cameraZ += -w.z * delta_t * speed;
    }
    if(!colBX && movB){
        cameraX += w.x * delta_t * speed;
    }
    if(!colBZ && movB){
        cameraZ += w.z * delta_t * speed;
    }
    if(!colRX && movR){
        cameraX += u.x * delta_t * speed;
    }
    if(!colRZ && movR){
        cameraZ += u.z * delta_t * speed;
    }
    if(!colLX && movL){
        cameraX += -u.x * delta_t * speed;
    }
    if(!colLZ && movL){
        cameraZ += -u.z * delta_t * speed;
    }

    /* Movimentacao no Y *Testes* */

    if(y_axis_movement){
        if(moving_up){
            cameraY += (u.y + 1) * delta_t * speed;
        }
        if(moving_down){
            cameraY += -(u.y + 1) * delta_t * speed;
        }
    }


}

void drawer(float delta_t, SceneObject player, SceneObject& drawer_left, SceneObject& drawer_right,
            SceneObject& collectable1){
    if(open_left_drawer){
        // abertura
        float new_z = delta_t * 2;
        SceneObject new_pos = drawer_left;
        new_pos.translate(0,0,new_z);
        if(drawer_left.get_position().z <= -5.6f &&
           !isBoundingBoxIntersection(player, new_pos) ){
            drawer_left.translate(0,0,new_z);
            if(collectable1.get_position() != pieces_initial_position.at("black_king")[3]){
                collectable1.translate(0,0,new_z);
            }
        }
    } else {
        float new_z = delta_t * 2;
        SceneObject new_pos = drawer_left;
        new_pos.translate(0,0,new_z);
        if(drawer_left.get_position().z >= -6.0f &&
           !isBoundingBoxIntersection(player, new_pos) ){
            drawer_left.translate(0,0,-new_z);
            if(collectable1.get_position() != pieces_initial_position.at("black_king")[3]){
                collectable1.translate(0,0,-new_z);
            }
        }
    }

    if(open_right_drawer){
        // abertura
        float new_z = delta_t * 2;
        SceneObject new_pos = drawer_right;
        new_pos.translate(0,0,new_z);
        if(drawer_right.get_position().z <= -5.6f &&
           !isBoundingBoxIntersection(player, new_pos) ){
            drawer_right.translate(0,0,new_z);
        }
    } else {
        float new_z = delta_t * 2;
        SceneObject new_pos = drawer_right;
        new_pos.translate(0,0,new_z);
        if(drawer_right.get_position().z >= -6.0f &&
           !isBoundingBoxIntersection(player, new_pos) ){
            drawer_right.translate(0,0,-new_z);
        }
    }

}

void play_game_anim(float t){
    SceneObject* e_white_pawn = find_piece_by_name("e_white_pawn");
    SceneObject* e_black_pawn = find_piece_by_name("e_black_pawn");
    SceneObject* left_white_bishop = find_piece_by_name("left_white_bishop");
    SceneObject* left_black_knight = find_piece_by_name("left_black_knight");
    SceneObject* white_queen = find_piece_by_name("white_queen");
    SceneObject* right_black_knight = find_piece_by_name("right_black_knight");
    SceneObject* f_black_pawn = find_piece_by_name("f_black_pawn");

    glm::vec3 look_at = glm::vec3(-3.8f, 0.1f, -3.9f);
    camera_view_vector = glm::vec4(look_at, 0) -
                                    glm::vec4(cameraX, cameraY, cameraZ, 0);

    std::cout << glm::to_string(f_black_pawn->get_position()) << std::endl;

    if (t > 3 && t < 5){
        e_white_pawn->set_position(e_white_pawn->get_position() +
                                   ((3-t)/2) *
                                   (e_white_pawn->get_position()-
                                   e4_position)
                                   );
    }else if(t > 5 && t < 7){
        e_black_pawn->set_position(e_black_pawn->get_position() +
                                   ((5-t)/2) *
                                   (e_black_pawn->get_position()-
                                   e5_position)
                                   );
    }else if(t > 7 && t < 9){
        left_white_bishop->set_position(left_white_bishop->get_position() +
                                       ((7-t)/2) *
                                       (left_white_bishop->get_position()-
                                       c4_position)
                                        );
    }else if(t > 9 && t < 11){
        left_black_knight->set_position(left_black_knight->get_position() +
                                       ((9-t)/2) *
                                       (left_black_knight->get_position()-
                                        f6_position)
                                        );
    }else if(t > 11 && t < 13){
        white_queen->set_position(white_queen->get_position() +
                                   ((11-t)/2) *
                                   (white_queen->get_position()-
                                    h5_position)
                                  );
    }else if(t > 13 && t < 15){
        right_black_knight->set_position(right_black_knight->get_position() +
                                           ((13-t)/2) *
                                           (right_black_knight->get_position()-
                                            c6_position)
                                            );
    }else if(t > 15 && t < 17){
        //if(glm::all(glm::equal(white_queen->get_position(),h5_position))){
        if(f_black_pawn->get_position().x > captured_black_piece_next_position.x){
                white_queen->set_position(white_queen->get_position() +
                                       ((15-t)/2) *
                                       (white_queen->get_position()-
                                        f_black_pawn->get_position())
                                          );
        }

        if(glm::all(glm::equal(f_black_pawn->get_position(), white_queen->get_position()))){
            capture_piece(f_black_pawn);
            game_ended = true;
        }

    }
}


SceneObject* find_piece_by_name(std::string target_name){

    std::vector<SceneObject*>::iterator it = chess_pieces.begin();
    while (it++ != chess_pieces.end()){
      if((*it)->get_name() == target_name){
        return *it;
      }
    }
}

bool all_pieces_in_starting_pos(){
    for (SceneObject* piece : chess_pieces){
        if(!glm::all(
                    glm::equal(piece->get_model()[3],
                    pieces_initial_position.at(piece->get_name())[3])
                    )){
           return false;
        }
    }
    return true;
}

void capture_piece(SceneObject* piece){
    if(piece->get_index() == BLACK_PIECE){
        if(piece->get_position().x > captured_black_piece_next_position.x){
            piece->set_position(captured_black_piece_next_position);
            captured_black_piece_next_position.z+=0.13;
        }
    }

}


// set makeprg=cd\ ..\ &&\ make\ run\ >/dev/null
// vim: set spell spelllang=pt_br :

