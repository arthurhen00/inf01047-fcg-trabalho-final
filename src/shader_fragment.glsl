#version 330 core

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

in vec4 color_v;
// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
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

uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;
uniform sampler2D TextureImage3;
uniform sampler2D TextureImage4;
uniform sampler2D TextureImage5;
uniform sampler2D TextureImage6;
uniform sampler2D TextureImage7;
uniform sampler2D TextureImage8;
uniform sampler2D TextureImage9;
uniform sampler2D TextureImage10;
uniform sampler2D TextureImage11;
uniform sampler2D TextureImage12;
uniform sampler2D TextureImage13;
uniform sampler2D TextureImage14;
uniform sampler2D TextureImage15;
uniform sampler2D TextureImage16;
uniform sampler2D TextureImage17;
uniform sampler2D TextureImage18;
uniform sampler2D TextureImage19;
uniform sampler2D TextureImage20;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

// Constantes
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923


void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(0.0f,5.0f,0.0f, 0.0f) - vec4(-1.0f,4.0f,0.0f, 0.0f));
    vec4 l1 = normalize(vec4(0.0f,5.0f,0.0f, 0.0f) - vec4(1.0f,4.0f,0.0f, 0.0f));


    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2 * n * dot(n, l); // PREENCHA AQUI o vetor de reflexão especular ideal
    vec4 r1 = -l1 + 2 * n * dot(n, l1);

    vec4 h = normalize(v + l);

    float U = 0.0;
    float V = 0.0;


    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    if ( object_id == SPHERE ){

        float raio = 1;
        vec4 bbox_center = (bbox_min + bbox_max) / 2.0;
        vec4 pl = bbox_center + raio*(position_model - bbox_center)/length(position_model - bbox_center);

        vec4 p2  = pl - bbox_center;
        float theta = atan( p2.x, p2.z );
        float phi   = asin( p2.y/raio );

        U = (theta + M_PI) / (2 * M_PI);
        V = (phi + M_PI_2) / M_PI;
        // PREENCHA AQUI
        // Propriedades espectrais da esfera
        Kd = texture(TextureImage0, vec2(U,V)).rgb;
        Ks = vec3(0.0,0.0,0.0);
        Ka = Kd/2;
        q = 1.0;
    } else if ( object_id == BUNNY ){
        float uR = 1;
        float vR = 0.2;

        U = position_model.x * uR - floor(position_model.x * uR);
        V = position_model.y * vR - floor(position_model.y * vR);

        Kd = texture(TextureImage0, vec2(U,V)).rgb;
        Ks = vec3(0.8,0.8,0.8);
        Ka = Kd/2;
        q = 32.0;
    } else if ( object_id == ROOM_FLOOR ){
        float uR = 2;
        float vR = 1.5;

        U = position_model.x * uR - floor(position_model.x * uR);
        V = position_model.z * vR - floor(position_model.z * vR);

        Kd = texture(TextureImage3, vec2(U,V)).rgb * texture(TextureImage4, vec2(U,V)).rgb;
        Ks = vec3(0.0,0.0,0.0);
        Ka = Kd/2;
        q = 20.0;
    } else if (object_id == WALL_1){
        float uR = 1.9;
        float vR = 1;

        U = position_model.x * uR - floor(position_model.x * uR);
        V = position_model.y * vR - floor(position_model.y * vR);

        Kd= texture(TextureImage5, vec2(U,V)).rgb * texture(TextureImage6, vec2(U,V)).rgb;
        Ks= vec3(0.0,0.0,0.0);
        Ka= Kd/2;
        q = 2.0;
    } else if (object_id == TABLE){
        U = texcoords.x;
        V = texcoords.y;

        Kd= texture(TextureImage7, vec2(U,V)).rgb;
        Ks= vec3(0.04,0.04,0.04);
        Ka= Kd/8;
        q = 2.0;
    } else if(object_id == CHESS){
        float uR = 0.11;
        float vR = 0.11;

        U = position_model.x * uR - floor(position_model.x * uR);
        V = position_model.z * vR - floor(position_model.z * vR);

        Kd= texture(TextureImage8, vec2(U,V)).rgb;
        Ks= vec3(0.1,0.1,0.1);
        Ka= Kd/8;
        q = 2.0;
    } else if (object_id == WHITE_PIECE){
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx)/(maxx - minx);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = texture(TextureImage10, vec2(U,V)).rgb;
        Ks = vec3(0.2,0.2,0.2);
        Ka = Kd/2;
        q = 1.0;

    } else if (object_id == BLACK_PIECE){
        float minx = bbox_min.x;
        float maxx = bbox_max.x;

        float miny = bbox_min.y;
        float maxy = bbox_max.y;

        float minz = bbox_min.z;
        float maxz = bbox_max.z;

        U = (position_model.x - minx)/(maxx - minx);
        V = (position_model.y - miny)/(maxy - miny);

        Kd = texture(TextureImage11, vec2(U,V)).rgb;
        Ks = vec3(0.2,0.2,0.2);
        Ka = Kd/2;
        q = 1.0;

    }else if (object_id == SKYBOX){
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 32.0;

    } else if(object_id == CONSOLE_TABLE){

        U = texcoords.x;
        V = texcoords.y;

        Kd = texture(TextureImage12, vec2(U,V)).rgb;
        Ks = vec3(0.01,0.01,0.01);
        Ka = Kd;
        q = 1.0;
    } else if(object_id == SOFA){
        U = texcoords.x;
        V = texcoords.y;

        Kd = texture(TextureImage13, vec2(U,V)).rgb * texture(TextureImage14, vec2(U,V)).rgb;
        Ks = vec3(0,0,0);
        Ka = Kd/3;
        q = 1.0f;
    } else if(object_id == TV){
        Kd = vec3(0.01f,0.01f,0.01f);
        Ks = vec3(0.02f,0.02f,0.02f);
        Ka = vec3(0,0,0);
        q = 1.0;
    } else if(object_id == SHELF){
        U = texcoords.x;
        V = texcoords.y;

        Kd = texture(TextureImage15, vec2(U,V)).rgb;
        Ks = vec3(0.01f,0.01f,0.01f);
        Ka = Kd/6;
        q = 1.0f;
    } else if(object_id == CHAIR){
        U = texcoords.x;
        V = texcoords.y;

        Kd = texture(TextureImage17, vec2(U,V)).rgb;
        Ks = texture(TextureImage18, vec2(U,V)).rgb;
        Ka = Kd/6;
        q = 1.0f;
    } else if(object_id == BED){
        U = texcoords.x;
        V = texcoords.y;

        Kd = texture(TextureImage16, vec2(U,V)).rgb;
        Ks = vec3(0.0f,0.0f,0.0f);
        Ka = Kd;
        q = 1.0f;
    } else if(object_id == BOOK_SHELF){
        U = texcoords.x;
        V = texcoords.y;

        Kd = texture(TextureImage19, vec2(U,V)).rgb;
        Ks = vec3(0.0f,0.0f,0.0f);
        Ka = Kd/2;
        q = 1.0f;
    } else if(object_id == BOOKS){
        float uR = 0.4;
        float vR = 0.7;

        U = position_model.x * uR - floor(position_model.x * uR);
        V = position_model.y * vR + 0.5;

        Kd = texture(TextureImage20, vec2(U,V)).rgb;
        Ks = vec3(0.0f,0.0f,0.0f);
        Ka = Kd/2;
        q = 1.0f;
    } else {
        Kd = vec3(0.0,0.0,0.0);
        Ks = vec3(0.0,0.0,0.0);
        Ka = vec3(0.0,0.0,0.0);
        q = 1.0;
    }

    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0); // PREENCH AQUI o espectro da fonte de luz

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2); // PREENCHA AQUI o espectro da luz ambiente

    // Obtemos a refletância difusa a partir da leitura da imagem TextureImage0

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l)); // PREENCHA AQUI o termo difuso de Lambert

    // Termo ambiente
    vec3 ambient_term = Ka * Ia; // PREENCHA AQUI o termo ambiente

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term  = Ks * I * pow(max(0, dot(r, v)), q); // PREENCH AQUI o termo especular de Phong
    vec3 bling_phong_specular_term  = Ks * I * pow(max(0, dot(n, h)), q);

    // NOTE: Se você quiser fazer o rendering de objetos transparentes, é
    // necessário:
    // 1) Habilitar a operação de "blending" de OpenGL logo antes de realizar o
    //    desenho dos objetos transparentes, com os comandos abaixo no código C++:
    //      glEnable(GL_BLEND);
    //      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // 2) Realizar o desenho de todos objetos transparentes *após* ter desenhado
    //    todos os objetos opacos; e
    // 3) Realizar o desenho de objetos transparentes ordenados de acordo com
    //    suas distâncias para a câmera (desenhando primeiro objetos
    //    transparentes que estão mais longe da câmera).
    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Cor final do fragmento calculada com uma combinação dos termos difuso,
    // especular, e ambiente. Veja slide 129 do documento Aula_17_e_18_Modelos_de_Iluminacao.pdf.

    color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;


    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);


    if(object_id == BOWL){
        color = color_v;
    }
}

