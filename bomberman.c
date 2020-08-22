/*  Universidade de Brasilia
	Instituto de Ciencias Exatas
	Departamento de Ciencia da Computacao

	Algoritmos e Programação de Computadores – 2/2019
	Aluno(a): Antonio Vinicius de Moura Rodrigues
	Matricula: 19/0084502
    Versão do compilador: GCC version 5.4.0

    Descricao: APCBOMBERMAN e um jogo com o objetivo de ensinar os alunos
    da materia de 'Algoritmo e programacao de computadores' a programarem de
    uma maneira divertida e intuitiva. Para ganhar o jogo e necessario matar
    todos os inimigos do campo, porem existe um tempo para que essa tarefa se
    torne mais desafiadora e empolgante! 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define LARGURA 15 /*Largura do campo*/
#define ALTURA 27 /*Altura do campo*/
#define PAREDE 40 /*Porcentagem de ter parede ou campo vazio no campo (0 a 100%)*/
#define MONSTRO 5 /*porcentagem de ter monstro no campo (0 a 100%)*/
#define MEDIA_TEMPO 7 /*valor medio que a bomba levara para explodir*/
#define ERRO_TEMPO 3 /*erro da media de tempo que a bomba levara para explodir*/
#define TEMPO_P 200 /*Tempo da partida*/
#define RAND(M, E) (M + (rand()%(2*E + 1) - E))

#ifdef _WIN32
#define CLEAR  system("cls");
#else
#define CLEAR  system("clear");
#endif

typedef struct{
    char nomejogador[21];
    int tempo_j;
} tipoRanking;

typedef struct {
    char save_nome[21];
    char save_tabuleiro[LARGURA][ALTURA];
    int save_l, save_c;
    int save_bl, save_bc;
    int save_qtdm;
    int save_tempo; /*Salva o tempo que falta para a partida acabar*/
    int save_tempo_b; /*Salva o tempo que falta para a bomba explodir*/
} estadoPartida;

/*Variaveis Globais*/

int l, c, bl = 0, bc = 0; /*Linha e coluna da posicao do personagem e da bomba*/
char campo[LARGURA][ALTURA];
int tempo_t = TEMPO_P, tempo, tempo_b; /*tempo_t: tempo total do jogo, tempo_b: tempo da bomba*/
int qtd_m = 0; /*Quantidade de monstro*/
char skin = '&';
tipoRanking ranking;

/*Funcoes*/

void criaCampo(); /*funcao que guarda as molduras e paredes do campo*/
void elementosCampo(); /* funcao que guarda a moldura e as paredes internas do campo*/
void printCampo(); /*funcao que imprime o campo*/
void menuInicial(); /*Funcao que cria um menu inicial*/
int  iniciaJogo(); /*funcao que inicia a partida*/
int  tempoPartida(); /*funcao calcula e imprime o tempo do jogo*/
int  moveUsuario(); /*funcao que move o personagem no campo*/
int  plantaBomba();  /*Funcao que passa as coordenadas do raio da explosao*/
int  raioExplosao(); /*Funcao que compara o que tem ao redor do raio da explosao*/
void mostraMensagem(); /*funcao que mostra a mensagem de saida do jogo*/
void printRanking(); /*funcao que imprime o ranking*/
void addRanking(); /*funcao que adiciona os dados no ranking no arquivo ranking.txt*/
void saveGame(); /*funcao que salva o jogo no arquivo jogo.bin*/
int  loadGame(); /*funcao que que recarrega um jogo*/

/*Codigo*/

int main(){
    /* funcao main que chama e controla as demais funcoes  */

    criaCampo();
    elementosCampo();
    menuInicial();

    return 0;
}

void criaCampo(){
    /* funcao que guarda a moldura e as paredes internas*/

    int i, j;

    for(i = 0; i < LARGURA; i++){ /*zera a matriz campo*/
        for(j = 0; j < ALTURA; j++){
            campo[i][j] = ' ';
        }
    }

    for(i = 1; i < LARGURA - 1; i++){ /*guarda as paredes laterais da moldura*/
        campo[i][0] = '|';
        campo[i][ALTURA - 1] = '|'; 
    }

    for(i = 1; i < ALTURA; i++){ /*guarda as paredes superior e inferior da moldura*/
        campo[0][i] = '_';
        campo[LARGURA - 1][i] = '_'; 
    }

    campo[0][ALTURA - 1] = '_';
    campo[LARGURA - 1][0] = '|';
    campo[LARGURA - 1][ALTURA - 1] = '|';

    for(i = 2; i < LARGURA - 1; i++){ /*guarda as paredes internas da moldura*/
        for(j = 2; j < ALTURA - 1; j++){
            campo[i][j] = '+';
            j++;
        }
        i++;
    }
}

void elementosCampo(){
    /*Funcao que coloca de forma aleatoria os elementos no campo*/

    int i, j, k;
    char elementos[3] = "# @";

    /*linha e coluna do personagem*/
    l = 13;
    c = 1;

    srand(time(0));

    for(i = 1; i < LARGURA - 1; i++){
        for(j = 1; j < ALTURA -1; j++){
            if(campo[i][j] != '+'){
               k = rand() % 100; /*Retorna um valor de 0 a 100*/
               if(k+1 <= PAREDE){
                   campo[i][j] = elementos[0]; /*Adiciona #*/
               }
               else {
                   campo[i][j] = elementos[1]; /*Adiciona campo vazio*/
               }

               k = rand() % 100;

               if(k+1 <= MONSTRO){
                   campo[i][j] = elementos[2]; /*Adiciona @*/
                   qtd_m++;
               }
            }
        }
    }

    if(campo[l][c] == elementos[2]){
        qtd_m--;
    }
    campo[l][c] = skin; /*inicializa o personagem*/

    if(campo[l - 1][c] != elementos[1]){ /*Campo vazio para o personagem se proteger da bomba no inicio*/
        if(campo[l - 1][c] == elementos[2]){
            qtd_m--;
        }
        campo[l - 1][c] = ' ';
    }
	if(campo[l - 2][c] != elementos[1]){
         if(campo[l - 2][c] == elementos[2]){
            qtd_m--;
        }
        campo[l - 2][c] = ' ';
    }
    if(campo[l][c +1] != elementos[1]){
         if(campo[l][c + 1] == elementos[2]){
            qtd_m--;
        }
        campo[l][c + 1] = ' ';
    }
    if(campo[l][c + 2] != elementos[1]){
         if(campo[l][c + 2] == elementos[2]){
            qtd_m--;
        }
        campo[l][c + 2] = ' ';
    }
}

void printCampo(){
    /* funcao que imprime o campo na tela*/

    int i, j;

    for(i = 0; i < LARGURA; i++){ /*imprime as todas as paredes da moldura*/
        for(j = 0; j < ALTURA; j++){
            printf("%c", campo[i][j]);
        }
        printf("\n");
    }
}

void menuInicial(){
    /* funcao que cria o menu inicial*/

    int opcao; /*opcao: opcao do menu; */

    do{ /*menu inicial*/
        CLEAR;

        printf("\n * * * * * Menu * * * * *\n\n");
        printf("  1. Iniciar\n");
        printf("  2. Continuar Jogo\n");
        printf("  3. Ranking\n");
        printf("  4. Sair\n");
        printf("\n * * * * * *  * * * * * *\n");
        printf("\n Digite a opcao: ");
        scanf("%d", &opcao);

        if (opcao == 1){
            CLEAR;
            printf("\n Digite o nome do jogador: ");
            scanf(" %[^\n]s", ranking.nomejogador);
            tempo_t = TEMPO_P;
            opcao = iniciaJogo(); /*quebra o loop caso o jogo termine*/
        }
        else if (opcao == 2){
            CLEAR;

            int lg = loadGame();
            if (tempo <= 0 || lg == 1){
                printf("\n Nao ha jogo salvo!\n\n");
                printf("\n Tecle <enter> para voltar para o menu.");
                getchar();
                getchar();
            }
            else{
                iniciaJogo();
            }
        }
        else if (opcao == 3){
            CLEAR;
            printRanking();
            printf("\n Tecle <enter> para voltar para o menu.");
            getchar();
			getchar();
        }

    }while(opcao != 4);

}

int iniciaJogo(){
    /*Funcao qeu inicia a partida*/
    int perca_t, perca_m, perca_b = 0, tempo_i = time (0); /*perca_t: perca por tempo, perca_p: perca por movimento, perca_b: perca por bomba*/


    do{
        perca_t = tempoPartida(tempo_i);
        printCampo(); /*funcao que imprime o campo*/

        if (tempo_b != 0){
			perca_b = plantaBomba();
			if (perca_m == 4){
				printf("\n Ha uma bomba no jogo!");
				getchar();
			}
		}

		if (perca_t == 0){
			perca_m = moveUsuario();
		}

    }while(perca_t == 0 && (perca_m == 0 || perca_m == 4) && perca_b == 0 && qtd_m != 0);

    mostraMensagem(perca_t, perca_m, perca_b);
    return 4;
}

int tempoPartida(int tempo_i){
    /*funcao calcula e imprime o tempo do jogo*/

    printf("\n");
    CLEAR;
	
    printf("\n* * * * * Terminal-Bomberman * * * * *\n");

	tempo = tempo_t - (time(0) - tempo_i);

    if(tempo > 0){
	    printf("\n TEMPO RESTANTE: %d \n", tempo);
    }
    else{
        printf("\n TEMPO RESTANTE: 0 \n");
    }

    printf("\n A - ESQUERDA   D - DIREITA   W - CIMA");
    printf("\n S - BAIXO      B - BOMBA     E - ENCERRAR\n\n");
	
	if (tempo > 0){
		return 0;
	}
	else{
		return 1;
	}
}

int moveUsuario(){
    /*funcao que move o personagem no campo*/
	
	char move; /*move: variavel que guarda o movimento solicitado*/

	printf("\n Opcao: ");
	scanf("%c", &move);
	
	if(move == 'A' || move == 'a' ){
		if(campo[l][c - 1] == ' '){ /*verifica se do lado esquerdo do personagem e um campo vazio*/
			if(campo[l][c] == skin){ 
				campo[l][c] = ' '; /*coloca um campo vazio no lugar do personagem*/
			}
			c--;
			campo[l][c] = skin; /*coloca o personagem na posicao solicitada*/
		}
		else{
			if(campo[l][c - 1] == '@'){ /*verifica se do lado esquerdo do personagem e um monstro*/
				return 1;
			}
		}
	}
	else if(move == 'W' || move == 'w' ){
        if(campo[l -1][c] == ' '){
            if(campo[l][c] == skin){
                campo[l][c] = ' ';
            }
            l--;
            campo[l][c] = skin;
        }
        else{
            if(campo[l - 1][c] == '@'){
                return 1;
            }				
        }
    }
	else if(move == 'D' || move == 'd'){
        if(campo[l][c + 1] == ' '){
            if (campo[l][c] == skin){
                campo[l][c] = ' ';
            }
            c++;
            campo[l][c] = skin;
        }
        else{
            if (campo[l][c + 1] == '@'){
                return 1;
            }					
        }
	}
	else if(move == 'S' || move == 's'){
        if(campo[l + 1][c] == ' '){
            if (campo[l][c] == skin){
            campo[l][c] = ' ';
            }
            l++;
            campo[l][c] = skin;
        }
        else{
            if(campo[l + 1][c] == '@'){
                return 1;
            }		
        }
	}
    else if (move == 'B' || move == 'b'){   
            if (tempo_b == 0){
                campo[l][c] = '*';
                bl = l;
                bc = c;
                tempo_b =  tempo - (RAND(MEDIA_TEMPO, ERRO_TEMPO));
            }
            else{
                return 4;
            }
    }
    else if(move == 'E' || move == 'e'){

		saveGame(); /*chama a funcao que salva o estado do game*/

        return 2;
	}
	return 0;
}

int plantaBomba(){
    /*Funcao que passa as coordenadas do raio da explosao*/
    
    int skin_explodiu = 0;

    if(tempo_b >= tempo){
        skin_explodiu += raioExplosao(bl,bc); /*coordenada do da bomba*/

        skin_explodiu += raioExplosao(bl,bc-1); /*coordenada do lado esquerdo da bomba*/
        skin_explodiu += raioExplosao(bl,bc+1); /*coordenada do lado direito da bomba*/

        skin_explodiu += raioExplosao(bl-1,bc); /*coordenada do lado de cima da bomba*/
        skin_explodiu += raioExplosao(bl+1,bc); /*coordenada do lado de baixo da bomba*/

        skin_explodiu += raioExplosao(bl-1,bc-1); /*coordenada do canto superior direito da bomba*/
        skin_explodiu += raioExplosao(bl-1,bc+1); /*coordenada do canto superior esquerdo da bomba*/
        skin_explodiu += raioExplosao(bl+1,bc-1); /*coordenada do canto inferior direito da bomba*/
        skin_explodiu += raioExplosao(bl+1,bc+1); /*coordenada do canto inferior esquerdo da bomba*/

        campo[bl][bc] = ' '; /*Coloca um campo vazio no lugar do '*' */
		tempo_b = 0;
    }
    
    if (skin_explodiu == 0){
        return 0;
    }
    else{
        return 3;
    }
}

int raioExplosao(int bl, int bc){
    /*Funcao que compara o que tem ao redor do raio da explosao*/

    if(campo[bl][bc] == '#'){
        campo[bl][bc] = ' ';
    }
    else{
        if(campo[bl][bc] == '@'){
            qtd_m--; /*Diminui a quantidade de monstro*/
            campo[bl][bc] = ' ';
        }
        else{
            if(campo[bl][bc] == skin || (bl == l && bc == c)){
            campo[bl][bc] = ' ';
            return 1; /*Retorna um caso o personagem morra pela bomba*/
            }			
        }
    }
    return 0;
}

void mostraMensagem(int perca_t, int perca_m, int perca_b){
    /*funcao que mostra a mensagem de saida do jogo*/

    CLEAR;

    if(perca_t == 0 && qtd_m == 0){
        ranking.tempo_j = tempo; /*Salva o tempo em que o jogador finalizou o jogo*/
        printf ("\n PARABENS!\n .\n .\n .\n Voce venceu o jogo.");
        addRanking();
        tempo = 0;
        saveGame();
        printf ("\n\n Tecle <enter> para encerrar");
        getchar();
        getchar();
    }
    else if(perca_t == 1){
        printf ("\n PERDEU!\n .\n .\n .\n O tempo acabou.");
        printf ("\n\n Tecle <enter> para encerrar\n");
        getchar();	
        getchar();
    }
    else if(perca_m == 1){
        printf ("\n PERDEU!\n .\n .\n .\n Um monstro te atacou.");
        printf ("\n\n Tecle <enter> para encerrar\n");
        getchar();	
        getchar();	
    }
    else if(perca_m == 2){
        printf ("\n JOGO ENCERRADO!\n .\n .\n .\n O jogo foi cancelado.");
        printf ("\n\n Tecle <enter> para encerrar\n");
        getchar();	
        getchar();
    }
    else if(perca_b == 3){
        printf ("\n PERDEU \n .\n .\n .\n A bomba te explodiu.");
        printf ("\n\n Tecle <enter> para encerrar\n");
        getchar();	
        getchar();
    }      
}

void printRanking(){
    /*funcao que imprime o ranking*/

    FILE *fp;
    char file_ranking[] = "ranking.txt";
    fp = fopen(file_ranking, "r");
    int qtd_ranking = 0, i, x = 0, y, troca = 1;
    tipoRanking *vetor_ranking, aux;

    if (fp == NULL){
        printf("\n O Ranking esta vazio!\n\n");
        return;
    }

    while (fscanf(fp, "%d %[^\n]s \n", &ranking.tempo_j, ranking.nomejogador) > 0){
        qtd_ranking++;
    }	
    rewind(fp);

    vetor_ranking = (tipoRanking*) malloc (sizeof(tipoRanking)*qtd_ranking);
  
    for(i = 0; i < qtd_ranking; i++){
        fscanf(fp, "\n%d %[^\n]s", &vetor_ranking[i].tempo_j, vetor_ranking[i].nomejogador);
    }

    /*Bubllesort para organizar o ranking*/
    while (troca == 1){
        troca = 0;
    
        for( x = 0; x < qtd_ranking; x++ ){
            for( y = x + 1; y < qtd_ranking; y++ ){
            if ( vetor_ranking[x].tempo_j < vetor_ranking[y].tempo_j ){
                aux = vetor_ranking[x];
                vetor_ranking[x] = vetor_ranking[y];
                vetor_ranking[y] = aux;
                troca = 1;
                }
            }
        } 
    }

    printf("\n * * * * * * * RANKING * * * * * * * \n\n");
    for(i = 0; i < qtd_ranking; i++){
       printf("\t\t\t\t %d \r %d %s \t \n", vetor_ranking[i].tempo_j, i+1, vetor_ranking[i].nomejogador);
    }
    printf("\n * * * * * * * * * * * * * * * * * * \n\n");

    fclose(fp);

    free(vetor_ranking);
}

void addRanking(){
    /*funcao que adiciona os dados no ranking*/

    FILE *fp;
    char file_ranking[] = "ranking.txt";
    fp = fopen(file_ranking, "a");

    if (fp == NULL){
       fp = fopen(file_ranking, "w");
    }

    fprintf(fp, "\n%d %s", ranking.tempo_j, ranking.nomejogador);

    fclose(fp);
}

void saveGame(){
    /*funcao que salva o jogo no arquivo jogo.bin*/

    FILE *fp;
    char file_save[] = "jogo.bin";
    int i,j;
    estadoPartida jogo;
    fp = fopen(file_save, "w+b");

     for(i = 0; i < LARGURA; i++){
        for(j = 0; j < ALTURA; j++){
            jogo.save_tabuleiro[i][j] = campo[i][j];
        }
    }

    strcpy(jogo.save_nome,ranking.nomejogador);
    jogo.save_l = l;
    jogo.save_c = c;
    jogo.save_bl = bl;
    jogo.save_bc = bc;
    jogo.save_qtdm = qtd_m;
    jogo.save_tempo = tempo;
    jogo.save_tempo_b = tempo_b;

    fwrite(&jogo, sizeof(jogo), 1, fp);

    fclose(fp);
}

int loadGame(){
    /*funcao que que recarrega um jogo*/

    FILE *fp;
    char file_save[] = "jogo.bin";
    int i,j;
    estadoPartida jogo;
    
    fp = fopen(file_save, "rb");

    if (fp == NULL){
        return 1;
    }

    fread(&jogo, sizeof(jogo), 1, fp);

    for(i = 0; i < LARGURA; i++){
        for(j = 0; j < ALTURA; j++){
            campo[i][j] = jogo.save_tabuleiro[i][j];
        }
    }

    strcpy(ranking.nomejogador, jogo.save_nome);
    l = jogo.save_l;
    c = jogo.save_c;
    bl = jogo.save_bl;
    bc = jogo.save_bc;
    qtd_m = jogo.save_qtdm;
    tempo_t = jogo.save_tempo;
    tempo = jogo.save_tempo;
    tempo_b = jogo.save_tempo_b;

    fclose(fp);

    return 0;
}