# Trabalho final FCG - Arthur Hendges e Gabriel Pereira

## Objetivo
O objetivo do jogo é coletar todas as peças de xadrez. Para tal inspecione elas e aperte f em seguida. Algumas peças estão escondidas, para encontrá-las é necessário inspecionar os outros objetos da cena.

## Contribuições:
Ambos integrantes adicionaram objetos, texturas, fizeram correção de bugs e refatorações. Além disso tiveram contribuições únicas:

Arthur: movimentação e colisão cubo-cubo, animações inicial e de coleta, movimentação com curva de Bézier.
![colisão cubo-cubo](./gifs/colisão.gif)

![gaveta abrindo](./gifs/gaveta.gif)

Gabriel: inspeção dos objetos (skybox, rotação, detecção orientação do objeto), colisão cubo-esfera, animação final.
![colisão cubo-esfera](./gifs/colisao2.gif)

![inspecao](./gifs/inspeção.gif)
## Controles
Clicar com o botão esquerdo do mouse e arrastar muda a direção em que se está olhando.

Ao inspecionar objetos clicar com o botão esquerdo do mouse e arrastar altera a rotação do objeto.

W - move para frente. \
A - move para a esquerda. \
S - move para trás. \
D - move para a direita. \
E - inspeciona objetos \
F - abre e fecha gaveta, durante a inspeção também coleta objetos caso a peça esteja aparecendo.\
ESC - sai da inspeção e fecha o jogo após a animação final.
LSHIFT - corre.

Controles para debug/testes:\
H - inicia animação final do jogo.\
ESPAÇO - move para cima.\
CTRL - move para baixo.

OBS: os últimos dois controles só funcionam caso a variável y_axis_movement seja = true;

## Como executar

=== Windows
Para compilar e executar este projeto no Windows, utilize a IDE Code::Blocks e abra o arquivo Trabalho Final.cbp

## Processo de desenvolvimento  
O processo de desenvolvimento da nossa aplicação envolveu o Git como nossa ferramenta principal para versionar o código, o que nos permitiu acompanhar as alterações e trabalhar de forma colaborativa sem problemas. Inicialmente, discutimos e planejamos as tarefas, dividindo o trabalho de acordo com nossos interesses. Isso garantiu que ambos estivéssemos alinhados com as metas do projeto. Além disso, aproveitamos tanto o tempo livre quanto o tempo nos laboratórios para avançar no desenvolvimento, o que nos permitiu dedicar uma quantidade significativa de tempo ao projeto e alcançar nossos objetivos de maneira eficaz.  

- [X] **Malhas poligonais complexas**  
        Boa parte dos objetos usados na cena são complexos, um exemplo são as peças de xadrez que possuem inúmeros vértices.  
- [X] **Transformações geométricas controladas pelo usuário**  
        Adicionamos a opção do usuário poder interagir com os objetos inspecionados, podendo rotacioná-los (botão esquerdo + arrastar), e com gavetas, podendo abri-las e fechá-las.  
- [X] **Câmera livre e câmera look-at**  
        Fizemos um jogo em primeira pessoa, então a câmera principal é uma câmera livre. A câmera look-at foi implementada na cutscene inicial e na inspeção de objetos, onde toda vez que um objeto é inspecionado, a câmera de inspeção aponta para o centro do objeto.  
- [X] **Instâncias de objetos** 
        Modelamos o cenário com objetos instanciados de diversas complexidades.  
- [X] **Três tipos de testes de intersecção**  
        Utilizamos os seguintes testes de intersecção: Bounding box-Bounding box, Sphere-Bounding box para detectar as colisões durante a movimentação, Ray-Bounding box para detectar os objetos inspecionados.  
- [X] **Modelos de Iluminação Difusa e Blinn-Phong**  
        Aplicamos o modelo de iluminação difusa nos objetos que eram foscos e envelhecidos, como paredes de madeira, teto e livros, já o Blinn-Phong foi aplicado em objetos que poderiam apresentar uma reflexão, como mesas, piso, peças de porcelana e tv.  
- [X] **Modelos de Interpolação de Phong e Gouraud**  
        Utilizamos a interpolação de Phong na maioria dos objetos para obter uma qualidade de iluminação maior, para fins de teste aplicamos a interpolação de Gouraud no "bowl" que fica na mesa principal.  
- [X] **Mapeamento de texturas em todos os objetos**  
        Objetos que foram esticados, como piso, paredes, tetos e tabuleiro de xadrez foram texturizados utilizando o método "repeat" para nao deformar a textura, os demais objetos foram mapeados diretos da textura.  
- [X] **Movimentação com curva Bézier cúbica**  
        Implementamos duas situações que a curva de bezier é aplicada, a primeira é a cutscene inicial, que se desloca por 6 pontos para apresentar o cenário ao usuário, a segunda é a animação de coleta de itens, onde a câmera se aproxima do tabuleiro de xadrez, através de 2 pontos.  
- [X] **Animações baseadas no tempo ($\Delta t$)**  
        Todas as mecânicas que movimentam algum objeto foram feitas utilizando o delta t.
