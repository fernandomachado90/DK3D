/*
UNIVERSIDADE FEDERAL DE SÃO PAULO
INSTITUTO DE CIÊNCIA E TECNOLOGIA

Bacharelado em Ciência da Computação

COMPUTAÇÃO GRÁFICA

== DONKEY KONG 3D ==

Aluno: Luiz Fernando Machado Silva
Professora: Dra. Ana Luísa Dine Martins Lemos
*/

#include <glut.h>
#include <stdio.h>
#include <time.h>
#define LADDERS 9
#define FLOORS 7
#define BARRELS 20
#define TIMER 24

typedef struct t_coord{
    int x;
    int y;
    int z;
    int a;
} t_coord;

typedef struct BMPImagem
{
    int   width;
    int   height;
    char *data;
}BMPImage;

t_coord screen;
t_coord angle;
t_coord start;

t_coord mario;
t_coord pauline;
t_coord dk;
t_coord oil;

t_coord ladder[LADDERS];
t_coord barrel[BARRELS];
t_coord flooring[FLOORS][2];

int jumping;
int onJump;
int walking;
int climbing;
int onClimb;
int bros;
int barrelBlock;

int points;
int pointsLast;
double timeLeft;

int gameState;
int cameraMode;
float aspect;
int deathFloor;

int moving;

void getBitmapImageData( char *pFileName, BMPImage *pImage ){
    FILE *pFile = NULL;
    unsigned short nNumPlanes;
    unsigned short nNumBPP;
	int i;

    if( (pFile = fopen(pFileName, "rb") ) == NULL )
		printf("ERROR: getBitmapImageData - %s not found.\n", pFileName);

    // Seek forward to width and height info
    fseek( pFile, 18, SEEK_CUR );

    if( (i = fread(&pImage->width, 4, 1, pFile) ) != 1 )
		printf("ERROR: getBitmapImageData - Couldn't read width from %s.\n ", pFileName);

    if( (i = fread(&pImage->height, 4, 1, pFile) ) != 1 )
		printf("ERROR: getBitmapImageData - Couldn't read height from %s.\n ", pFileName);

    if( (fread(&nNumPlanes, 2, 1, pFile) ) != 1 )
		printf("ERROR: getBitmapImageData - Couldn't read plane count from %s.\n", pFileName);

    if( nNumPlanes != 1 )
		printf("ERROR: getBitmapImageData - Plane count from %s.\n ", pFileName);

    if( (i = fread(&nNumBPP, 2, 1, pFile)) != 1 )
		printf( "ERROR: getBitmapImageData - Couldn't read BPP from %s.\n ", pFileName);

    if( nNumBPP != 24 )
		printf("ERROR: getBitmapImageData - BPP from %s.\n ", pFileName);

    // Seek forward to image data
    fseek( pFile, 24, SEEK_CUR );

	// Calculate the image's total size in bytes. Note how we multiply the
	// result of (width * height) by 3. This is becuase a 24 bit color BMP
	// file will give you 3 bytes per pixel.
    int nTotalImagesize = (pImage->width * pImage->height) * 3;

    pImage->data = (char*) malloc( nTotalImagesize );

    if( (i = fread(pImage->data, nTotalImagesize, 1, pFile) ) != 1 )
		printf("ERROR: getBitmapImageData - Couldn't read image data from %s.\n ", pFileName);

    //
	// Finally, rearrange BGR to RGB
	//

	char charTemp;
    for( i = 0; i < nTotalImagesize; i += 3 )
	{
		charTemp = pImage->data[i];
		pImage->data[i] = pImage->data[i+2];
		pImage->data[i+2] = charTemp;
    }
}

int getDepth(int y){
    if((-900<=y)&&(y<-600))
        return 0;
    else if((-600<=y)&&(y<-300))
        return -100;
    else if((-300<=y)&&(y<0))
        return -200;
    else if((0<=y)&&(y<300))
        return -300;
    else if((300<=y)&&(y<600))
        return -400;
    else if((600<=y)&&(y<900))
        return -500;
    else if((900<=y)&&(y<1200))
        return -600;
    else
        return 100;
}

void drawMario(){
    //corpo
    if(bros == 0)
        glColor3f(248/255.0,56/255.0,0);
    else
        glColor3f(0,1,0);
    glPushMatrix();
    glutSolidCube(100);
    glPopMatrix();

    //macacao esq
    glColor3f(1,173/255.0,82/255.0);
    glPushMatrix();
    glTranslatef(-20,15,50);
    glutSolidCube(8);
    glPopMatrix();

    //macacao dir
    glColor3f(1,173/255.0,82/255.0);
    glPushMatrix();
    glTranslatef(20,15,50);
    glutSolidCube(8);
    glPopMatrix();

    //cabeca
    glColor3f(1,173/255.0,82/255.0);
    glPushMatrix();
    glTranslatef(0,80,0);
    glutSolidCube(70);
    glPopMatrix();

    //olho esquerdo
    glColor3f(0,0,173/255.0);
    glPushMatrix();
    glTranslatef(-15,90,35);
    glutSolidCube(10);
    glPopMatrix();

    //olho direito
    glColor3f(0,0,173/255.0);
    glPushMatrix();
    glTranslatef(15,90,35);
    glutSolidCube(10);
    glPopMatrix();

    //bigode
    glColor3f(0,0,173/255.0);
    glPushMatrix();
    glTranslatef(0,70,35);
    glScalef(3,1,2);
    glutSolidCube(10);
    glPopMatrix();

    glColor3f(0,0,173/255.0);
    glPushMatrix();
    glTranslatef(0,65,35);
    glScalef(2.5,1,1.5);
    glutSolidCube(10);
    glPopMatrix();

    //cabelo
    glColor3f(0,0,173/255.0);
    glPushMatrix();
    glTranslatef(0,100,-6);
    glScalef(1,0.6,1);
    glutSolidCube(76);
    glPopMatrix();

    //chapeu
    if(bros == 0)
        glColor3f(248/255.0,56/255.0,0);
    else
        glColor3f(0,1,0);
    glPushMatrix();
    glTranslatef(0,128,5);
    glScalef(0.9,0.25,1);
    glutSolidCube(60);
    glPopMatrix();

    if(bros == 0)
        glColor3f(248/255.0,56/255.0,0);
    else
        glColor3f(0,1,0);
    glPushMatrix();
    glTranslatef(0,135,5);
    glScalef(0.9,0.25,1);
    glutSolidCube(50);
    glPopMatrix();

    glPushMatrix();
    if((walking)&&(!jumping))
        glRotatef(mario.a*2,0,1,0);
    else if(jumping)
        glRotatef(-30,0,0,1);
    else if(onClimb)
        glRotatef(-45,0,0,1);

    //braco esq
    glColor3f(0,0,173/255.0);
    glPushMatrix();
    glTranslatef(-70,15,0);
    glScalef(1.2,1,1);
    glutSolidCube(35);
    glPopMatrix();

    //mao esq
    glColor3f(1,173/255.0,82/255.0);
    glPushMatrix();
    glTranslatef(-95,15,0);
    glScalef(0.5,1,1);
    glutSolidCube(30);
    glPopMatrix();

    glPopMatrix();

    glPushMatrix();
    if((walking)&&(!jumping))
        glRotatef(mario.a*2,0,1,0);
    else if(jumping)
        glRotatef(30,0,0,1);
    else if(onClimb)
        glRotatef(45,0,0,1);


    //braco dir
    glColor3f(0,0,173/255.0);
    glPushMatrix();
    glTranslatef(70,15,0);
    glScalef(1.2,1,1);
    glutSolidCube(35);
    glPopMatrix();

    //mao dir
    glColor3f(1,173/255.0,82/255.0);
    glPushMatrix();
    glTranslatef(95,15,0);
    glScalef(0.5,1,1);
    glutSolidCube(30);
    glPopMatrix();

    glPopMatrix();


    //pe esq
    glColor3f(0,0,173/255.0);
    glPushMatrix();
    if(((walking)||((climbing)&&(onClimb)))&&(!jumping))
        glTranslatef(-30,-55+mario.a,0);
    else
        glTranslatef(-30,-65,0);
    glScalef(0.8,1.2,1);
    glutSolidCube(40);
    glPopMatrix();


    //pe dir
    glColor3f(0,0,173/255.0);
    glPushMatrix();
    if(((walking)||((climbing)&&(onClimb)))&&(!jumping))
        glTranslatef(30,-55-mario.a,0);
    else
        glTranslatef(30,-65,0);
    glScalef(0.8,1.2,1);
    glutSolidCube(40);
    glPopMatrix();
}

void drawPauline(){
    glPushMatrix();
    glTranslatef(0,30+pauline.a,0);

    //corpo
    glColor3f(1,43/255.0,170/255.0);
    glPushMatrix();
    glScalef(1,1,0.75);
    glutSolidCube(100);
    glPopMatrix();

    //detalhe branco
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(0,-40,0);
    glScalef(1.2,0.1,0.9);
    glutSolidCube(100);
    glPopMatrix();

    //detalhe azul
    glColor3f(0,0,173/255.0);
    glPushMatrix();
    glTranslatef(0,-5,0);
    glScalef(1.05,0.1,0.8);
    glutSolidCube(100);
    glPopMatrix();

    //cabeca
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(0,80,0);
    glutSolidCube(70);
    glPopMatrix();

    //olho esquerdo
    glColor3f(1,85/255.0,0);
    glPushMatrix();
    glTranslatef(-15,90,35);
    glutSolidCube(10);
    glPopMatrix();

    //olho direito
    glColor3f(1,85/255.0,0);
    glPushMatrix();
    glTranslatef(15,90,35);
    glutSolidCube(10);
    glPopMatrix();

    //boca
    glColor3f(1,85/255.0,0);
    glPushMatrix();
    glTranslatef(0,70,35);
    glScalef(1.5,1,1);
    glutSolidCube(10);
    glPopMatrix();

    //cabelo
    glColor3f(1,85/255.0,0);
    glPushMatrix();
    glTranslatef(0,100,-5);
    glScalef(1.2,0.8,0.9);
    glutSolidCube(75);
    glPopMatrix();

    glColor3f(1,85/255.0,0);
    glPushMatrix();
    glTranslatef(0,100,-5);
    glScalef(1.1,1.1,0.9);
    glutSolidCube(75);
    glPopMatrix();

    glColor3f(1,85/255.0,0);
    glPushMatrix();
    glTranslatef(0,80,-50);
    glScalef(1,0.95,1.4);
    glutSolidCube(50);
    glPopMatrix();

    glPushMatrix();
    if(gameState == 0)
        glRotatef(-30,0,0,1);
    else
        glRotatef(-pauline.a/2,0,0,1);

    //braco esq
    glColor3f(1,43/255.0,170/255.0);
    glPushMatrix();
    glTranslatef(-65,15,0);
    glScalef(1.2,1,1);
    glutSolidCube(35);
    glPopMatrix();

    //mao esq
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(-95,15,0);
    glScalef(0.5,1,1);
    glutSolidCube(30);
    glPopMatrix();

    glPopMatrix();


    glPushMatrix();

    if(gameState == 0)
        glRotatef(30,0,0,1);
    else
        glRotatef(pauline.a/2,0,0,1);

    //braco dir
    glColor3f(1,43/255.0,170/255.0);
    glPushMatrix();
    glTranslatef(65,15,0);
    glScalef(1.2,1,1);
    glutSolidCube(35);
    glPopMatrix();

    //mao dir
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(95,15,0);
    glScalef(0.5,1,1);
    glutSolidCube(30);
    glPopMatrix();

    glPopMatrix();


    //pe esq
    glColor3f(0,0,173/255.0);
    glPushMatrix();
    glTranslatef(-20,-65,0);
    glScalef(0.8,1.2,1);
    glutSolidCube(40);
    glPopMatrix();

    //pe dir
    glColor3f(0,0,173/255.0);
    glPushMatrix();
    glTranslatef(20,-65,0);
    glScalef(0.8,1.2,1);
    glutSolidCube(40);
    glPopMatrix();

    glPopMatrix();
}

void drawDK(){
    glPushMatrix();
    glRotatef(dk.a,0,1,0);

    //corpo
    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(0,-8,-10);
    glScalef(1.1,1.3,1.0);
    glutSolidCube(105);
    glPopMatrix();

    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(0,-8,-2);
    glScalef(1.1,1.1,0.9);
    glutSolidCube(105);
    glPopMatrix();

    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(0,18,-2);
    glScalef(1.15,0.6,0.9);
    glutSolidCube(105);
    glPopMatrix();

    //peito
    glColor3f(1,170/255.0,85/255.0);
    glPushMatrix();
    glTranslatef(0,-8,-2);
    glScalef(1,1.2,1);
    glutSolidCube(105);
    glPopMatrix();


    //peito esquerdo
    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(-25,18,50);
    glutSolidCube(8);
    glPopMatrix();

    //peito direito
    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(25,18,50);
    glutSolidCube(8);
    glPopMatrix();

    //gravata
    glColor3f(248/255.0,56/255.0,0);
    glPushMatrix();
    glTranslatef(0,6,50);
    glScalef(2,12,1);
    glutSolidCube(8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,5,0);
    glRotatef(dk.a,0,1,0);

    //cabeca
    glColor3f(1,173/255.0,82/255.0);
    glPushMatrix();
    glTranslatef(0,88,0);
    glutSolidCube(70);
    glPopMatrix();

    //cabelo
    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(0,90,-5);
    glutSolidCube(76);
    glPopMatrix();

    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(0,90,-5);
    glScalef(1.1,0.9,1);
    glutSolidCube(76);
    glPopMatrix();

    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(0,90,-5);
    glScalef(0.9,1.1,1);
    glutSolidCube(76);
    glPopMatrix();

    //olho esquerdo
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(-15,98,30);
    glutSolidCube(12);
    glPopMatrix();

    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(-12,95,35);
    glutSolidCube(7);
    glPopMatrix();

    //olho direito
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(15,98,30);
    glutSolidCube(12);
    glPopMatrix();

    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(12,95,35);
    glutSolidCube(7);
    glPopMatrix();

    //boca
    if(gameState != 0){
        glColor3f(1,1,1);
        glPushMatrix();
        glTranslatef(0,75+pauline.a/10,35);
        glScalef(4.2,0.8,1.1);
        glutSolidCube(10);
        glPopMatrix();
    }

    glColor3f(0,0,0);
    glPushMatrix();
    glTranslatef(0,70,31);
    if(gameState != 0)
        glScalef(4.2,0.8,0.9);
    else
        glScalef(4.2,1.6,0.9);
    glutSolidCube(10);
    glPopMatrix();

    if(gameState != 0){
        glColor3f(1,1,1);
        glPushMatrix();
        glTranslatef(0,65-pauline.a/10,35);
        glScalef(4.2,0.8,1.1);
        glutSolidCube(10);
        glPopMatrix();
    }

    glPopMatrix();


    glPushMatrix();
    glRotatef(dk.a/2,0,0,1);

    //braco esq
    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(-75,15,0);
    glScalef(1.5,1,1);
    glutSolidCube(35);
    glPopMatrix();

    //mao esq
    glColor3f(1,170/255.0,85/255.0);
    glPushMatrix();
    glTranslatef(-115,15,0);
    glScalef(1,1,1);
    glutSolidCube(36);
    glPopMatrix();

    glPopMatrix();

    glPushMatrix();
    glRotatef(dk.a/2,0,0,1);

    //braco dir
    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(75,15,0);
    glScalef(1.5,1,1);
    glutSolidCube(35);
    glPopMatrix();

    //mao dir
    glColor3f(1,170/255.0,85/255.0);
    glPushMatrix();
    glTranslatef(115,15,0);
    glScalef(1,1,1);
    glutSolidCube(36);
    glPopMatrix();

    glPopMatrix();

    //perna esq
    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(-28,-70,0);
    glScalef(1,1.5,1);
    glutSolidCube(40);
    glPopMatrix();

    //pe esq
    glColor3f(1,170/255.0,85/255.0);
    glPushMatrix();
    glTranslatef(-28,-104,4);
    glScalef(1,0.2,1.2);
    glutSolidCube(40);
    glPopMatrix();

    //perna dir
    glColor3f(170/255.0,0,0);
    glPushMatrix();
    glTranslatef(28,-70,0);
    glScalef(1,1.5,1);
    glutSolidCube(40);
    glPopMatrix();

    //pe dir
    glColor3f(1,170/255.0,85/255.0);
    glPushMatrix();
    glTranslatef(28,-104,4);
    glScalef(1,0.2,1.2);
    glutSolidCube(40);
    glPopMatrix();

    glPopMatrix();
}

void drawFloor(){
    glColor3f(1,33/255.0,85/255.0);
    glPushMatrix();
    glScalef(1,0.1,1);
    glutSolidCube(150);
    glPopMatrix();

    glColor3f(1,33/255.0,85/255.0);
    glPushMatrix();
    glTranslatef(0,-30,0);
    glScalef(1,0.1,1);
    glutSolidCube(150);
    glPopMatrix();

    glColor3f(151/255.0,0,0);
    glPushMatrix();
    glTranslatef(0,-15,0);
    glScalef(1,0.1,0.9);
    glutSolidCube(150);
    glPopMatrix();
}

void drawLadder(){
    glColor3f(0,1,1);
    glPushMatrix();
    glTranslatef(-30,-1,-38);
    glScalef(0.1,2.08,0.1);
    glutSolidCube(150);
    glPopMatrix();

    glColor3f(0,1,1);
    glPushMatrix();
    glTranslatef(30,-1,-38);
    glScalef(0.1,2.08,0.1);
    glutSolidCube(150);
    glPopMatrix();

    int i;
    for(i=0;i<7;i++){
        glColor3f(0,1,1);
        glPushMatrix();
        glTranslatef(0,120+(-40*i),-38);
        glScalef(0.5,0.1,0.1);
        glutSolidCube(150);
        glPopMatrix();
    }
}

void drawOil(){
    int i;

    glPushMatrix();
    glTranslatef(-90,10,0);
    glScalef(-1,1,1);
    glRotatef(90,0,0,1);


    for(i=0;i<9;i++){
        glPushMatrix();

        glRotatef(i*40,0,1,0);

        glColor3f(0,0,1);
        glPushMatrix();
        glScalef(0.85,0.5,0.75);
        glutSolidCube(150);
        glPopMatrix();

        glColor3f(0,0,1);
        glPushMatrix();
        glScalef(0.75,0.75,0.75);
        glutSolidCube(150);
        glPopMatrix();

        glColor3f(0,1,1);
        glPushMatrix();
        glScalef(0.9,0.2,0.8);
        glutSolidCube(150);
        glPopMatrix();

        glColor3f(1,1,1);
        glPushMatrix();
        glTranslatef(0,-30,0);
        glScalef(0.9,0.05,0.8);
        glutSolidCube(150);
        glPopMatrix();

        glColor3f(0,0,50/255.0);
        glPushMatrix();
        glTranslatef(0,55,0);
        glScalef(0.7,0.025,0.7);
        glutSolidCube(150);
        glPopMatrix();

        glPopMatrix();
    }


    if(gameState == 1){
        glPushMatrix();
        glScalef(0.75,1,0.75);
        glRotatef(dk.a,0,1,0);

        for(i=0;i<5;i++){
            glColor3f(1,213/255.0,0);
            glPushMatrix();
            glTranslatef(-50+rand()%100,60+rand()%100,0);
            glScalef(1,1,10);
            glutSolidCube(5+rand()%15);
            glPopMatrix();

            glColor3f(1,170/255.0,0);
            glPushMatrix();
            glTranslatef(-50+rand()%100,60+rand()%90,0);
            glScalef(1,1,8);
            glutSolidCube(5+rand()%15);
            glPopMatrix();

            glColor3f(1,0,0);
            glPushMatrix();
            glTranslatef(-50+rand()%100,60+rand()%80,0);
            glScalef(1,1,6);
            glutSolidCube(5+rand()%15);
            glPopMatrix();

            glColor3f(1,1,1);
            glPushMatrix();
            glTranslatef(-50+rand()%100,60+rand()%70,0);
            glScalef(1,1,4);
            glutSolidCube(5+rand()%15);
            glPopMatrix();
        }
        glPopMatrix();
    }


    glPopMatrix();
}

void drawBarrel(){
    int i;

    for(i=0;i<9;i++){

        glPushMatrix();

        glTranslatef(0,0,20);


        glRotatef(40*i,0,0,1);
        glScalef(1,1,0.75);

        glColor3f(1,85/255.0,0);
        glPushMatrix();
        glScalef(0.5,0.5,0.75);
        glutSolidCube(150);
        glPopMatrix();

        glColor3f(1,170/255.0,85/255.0);
        glPushMatrix();
        glScalef(0.6,0.5,0.7);
        glutSolidCube(150);
        glPopMatrix();

        glColor3f(1,170/255.0,85/255.0);
        glPushMatrix();
        glScalef(0.5,0.6,0.7);
        glutSolidCube(150);
        glPopMatrix();

        glColor3f(0,0,1);
        glPushMatrix();
        glTranslatef(0,0,-35);
        glRotatef(90,0,1,0);
        glScalef(0.05,0.45,0.65);
        glutSolidCube(150);
        glPopMatrix();

        glColor3f(0,0,1);
        glPushMatrix();
        glTranslatef(0,0,35);
        glRotatef(90,0,1,0);
        glScalef(0.05,0.45,0.65);
        glutSolidCube(150);
        glPopMatrix();

        glColor3f(0,0,1);
        glPushMatrix();
        glTranslatef(0,0,-35);
        glRotatef(90,0,1,0);
        glScalef(0.05,0.65,0.45);
        glutSolidCube(150);
        glPopMatrix();

        glColor3f(0,0,1);
        glPushMatrix();
        glTranslatef(0,0,35);
        glRotatef(90,0,1,0);
        glScalef(0.05,0.65,0.45);
        glutSolidCube(150);
        glPopMatrix();

        glColor3f(0,0,1);
        glPushMatrix();
        glTranslatef(0,35,0);
        glScalef(0.075,0.075,0.8);
        glutSolidCube(150);
        glPopMatrix();

        glPopMatrix();

    }
}

void renderString(const char *txt){
    int i;

    for(i=0;txt[i]!='\0';i++){
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, txt[i]);
    }
}

void drawText(){

    char text[5];
    glDisable(GL_LIGHTING);

    glLineWidth(3.5f);
    glColor3d(1,1,1);


    //pause
    if(gameState == 2){
        glLineWidth(10.0f);

        glPushMatrix();
        glTranslatef(-750,-200,-100);
        glScalef(3,3,1);
        renderString("PAUSE");
        glPopMatrix();
    }

    //score
    glLineWidth(4.0f);
    glPushMatrix();
    glTranslatef(640,975,-600);
    glScalef(0.9,0.75,1);
    renderString("SCORE");
    glPopMatrix();
    glPushMatrix();
    glTranslatef(640,860,-600);
    glScalef(0.9,0.75,1);
    sprintf(text, "%.5d", points);
    renderString(text);
    glPopMatrix();

    //time
    glLineWidth(4.0f);
    glPushMatrix();
    glTranslatef(150,975,-600);
    glScalef(0.9,0.75,1);
    renderString("TIME");
    glPopMatrix();
    glPushMatrix();
    glTranslatef(150,860,-600);
    glScalef(0.9,0.75,1);
    sprintf(text, "%.4d", (int) timeLeft);
    renderString(text);
    glPopMatrix();

    glEnable(GL_LIGHTING);

}

void drawScene(){

    //text
    drawText();

    //mario
    glPushMatrix();
    glTranslatef(mario.x,mario.y,getDepth(mario.y));
    if(gameState == -1)
        glRotatef(90*(deathFloor+1),1,0,0);
    if(walking)
        glRotatef(90,0,walking,0);
    if(onClimb)
        glRotatef(180,0,1,0);
    glScalef(0.5,0.5,0.5);
    drawMario();
    glPopMatrix();

    //dk
    glPushMatrix();
    glTranslatef(dk.x,dk.y,getDepth(600)-50);
    if(gameState == 0){
        glTranslatef(0,0,-150);
        glRotatef(deathFloor*10,1,1,1);
    }
    glScalef(1.5,1.5,1.5);
    drawDK();
    glPopMatrix();

    //pauline
    glPushMatrix();
    glTranslatef(pauline.x,pauline.y,getDepth(900));
    glScalef(0.5,0.5,0.5);
    drawPauline();
    glPopMatrix();

    //floor
    glPushMatrix();
    glTranslatef(0,-950,getDepth(-900));
    glScalef(13,0.75,0.75);
    drawFloor();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-150,-650,getDepth(-600));
    glScalef(11,0.75,0.75);
    drawFloor();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(150,-350,getDepth(-300));
    glScalef(11,0.75,0.75);
    drawFloor();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-150,-50,getDepth(0));
    glScalef(11,0.75,0.75);
    drawFloor();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(150,250,getDepth(300));
    glScalef(11,0.75,0.75);
    drawFloor();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-150,550,getDepth(600));
    glScalef(11,0.75,0.75);
    drawFloor();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-250,850,getDepth(900));
    glScalef(2,0.75,0.75);
    drawFloor();
    glPopMatrix();

    int i;
    for(i=0;i<LADDERS;i++){
        glPushMatrix();
        glTranslatef(ladder[i].x,ladder[i].y,getDepth(ladder[i].y-100));
        drawLadder();
        glPopMatrix();
    }

    //barrels
    for(i=0;i<BARRELS;i++){
        if(barrel[i].z != 0){
            glPushMatrix();
            glTranslatef(barrel[i].x,barrel[i].y+5,getDepth(barrel[i].y)-10);
            if(abs(barrel[i].z) == 1)
                glRotatef(barrel[i].a,0,0,1);
            else if(abs(barrel[i].z) == 2){
                glTranslatef(-16,0,0);
                glRotatef(90,0,1,0);
                glRotatef(abs(barrel[i].a),0,0,1);
            }
            drawBarrel();
            glPopMatrix();
        }
    }

    //static barrels
    if(gameState != 0){
        glPushMatrix();
        glTranslatef(-675,550,getDepth(900)-10);
        glScalef(4,0.75,0.75);
        drawFloor();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-370,753,getDepth(600)-10);
        glRotatef(90,0,1,0);
        glScalef(1.25,0.625,1);
        drawLadder();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-370,700,getDepth(900)-10);
        glRotatef(90,0,1,0);
        glScalef(1.25,1,1);
        drawLadder();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-325,850,getDepth(600)-10);
        glScalef(1,0.75,0.75);
        drawFloor();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-925,600,getDepth(600)-10);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-925,700,getDepth(600)-10);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-925,800,getDepth(600)-10);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-925,900,getDepth(600)-10);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-925,1000,getDepth(600)-10);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-925,600,getDepth(900)-10);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-925,700,getDepth(900)-10);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-925,800,getDepth(900)-10);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-925,900,getDepth(900)-10);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-925,1000,getDepth(900)-10);
        drawBarrel();
        glPopMatrix();
    }
    else{
        glPushMatrix();
        glTranslatef(-775,550,getDepth(900)-200);
        glTranslatef(-75,-deathFloor*18,0);
        glRotatef(deathFloor*20,1,1,1);
        glScalef(4,0.75,0.75);
        drawFloor();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-370,745,getDepth(600)-150);
        glTranslatef(50,-deathFloor*17,0);
        glRotatef(-deathFloor*19,1,1,1);
        glRotatef(90,0,1,0);
        glScalef(1.25,0.7,1);
        drawLadder();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-370,700,getDepth(900)-150);
        glTranslatef(-50,-deathFloor*19,0);
        glRotatef(deathFloor*18,1,1,1);
        glRotatef(90,0,1,0);
        glScalef(1.25,1,1);
        drawLadder();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-325,850,getDepth(900)-200);
        glTranslatef(-50,-deathFloor*18,0);
        glRotatef(deathFloor*20,1,1,1);
        glScalef(1,0.75,0.75);
        drawFloor();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-525,600-deathFloor*10,getDepth(900)-250);
        glRotatef(deathFloor*10,1,0,0);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-625,685-deathFloor*9,getDepth(900)-250);
        glRotatef(deathFloor*10,1,1,0);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-725,770-deathFloor*8,getDepth(900)-250);
        glRotatef(deathFloor*10,1,1,1);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-825,855-deathFloor*13,getDepth(900)-250);
        glRotatef(deathFloor*10,1,0,1);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-925,940-deathFloor*11,getDepth(900)-250);
        glRotatef(deathFloor*10,0,1,1);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-925,600,getDepth(600));
        glRotatef(45,1,1,1);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-825,600,getDepth(600));
        glRotatef(90,1,1,1);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-725,600,getDepth(600));
        glRotatef(135,1,1,1);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-625,600,getDepth(600));
        glRotatef(180,1,1,1);
        drawBarrel();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-525,600,getDepth(600));
        glRotatef(280,1,1,1);
        drawBarrel();
        glPopMatrix();
    }

    //oil
    glPushMatrix();
    glTranslatef(oil.x,oil.y,getDepth(-900));
    drawOil();
    glPopMatrix();
}

void reshape(GLsizei w, GLsizei h){
    if(h == 0) h = 1;

    glViewport(0, 0, w, h);

    aspect = (GLfloat)w/(GLfloat)h;

    screen.x = w;
    screen.y = h;
}

void setCamera(){
    if(cameraMode == 1){
        //active camera
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(mario.x, mario.y+250, 1500, //posição da câmera (P_0)
              mario.x/4, mario.y/4, 0, //para onde a câmera aponta (P_ref)
              0, 1, 0);//vetor view-up (V)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(75.0, aspect, 1.0, -1.0);
    }
    else{
        //static camera
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, 1, //posição da câmera (P_0)
              0, 0, 0, //para onde a câmera aponta (P_ref)
              0, 1, 0);//vetor view-up (V)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if (aspect >= 1.0)
          glOrtho(-screen.z * aspect, screen.z * aspect, -screen.z, screen.z, -screen.z, screen.z);
        else
          glOrtho(-screen.z, screen.z, -screen.z / aspect, screen.z / aspect, -screen.z, screen.z);

    }
}

void setLightning(){
	GLfloat luzAmbiente[4]={0.25,0.25,0.25,1.0};
	GLfloat luzDifusa[4]={0.5,0.5,0.5,1.0};
	GLfloat posicaoLuz[4]={0, 0, 1, 0};

	// Habilita o modelo de colorização de Gouraud
	glShadeModel(GL_SMOOTH);

	// Ativa o uso da luz ambiente
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os parâmetros da luz de número 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	//glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );

	// Habilita a definição da cor do material a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de iluminação
	glEnable(GL_LIGHTING);
	// Habilita a luz de número 0
	glEnable(GL_LIGHT0);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //limpa o buffer

    setCamera();
    setLightning();

    glMatrixMode(GL_MODELVIEW);

    drawScene();

	glutSwapBuffers();
}

void reset(){
    gameState = 1;
    cameraMode = 1;
    aspect = 1.0;
    deathFloor = 0;

    points = 0;
    pointsLast = -1;
    timeLeft = 300;

    angle.x = 0;
    angle.y = 0;
    angle.z = 0;

    start.x = 0;
    start.y = 0;
    start.z = 0;

    mario.x = -700;
    mario.y = -900;
    mario.z = 1;
    mario.a = 0;

    dk.x = -650;
    dk.y = 718;
    dk.z = 1;
    dk.a = 0;

    pauline.x = -325;
    pauline.y = 900;
    pauline.z = 1;
    pauline.a = 0;

    oil.x = -850;
    oil.y = -888;

    ladder[0].x = 575;
    ladder[0].y = -800;
    ladder[1].x = -150;
    ladder[1].y = -500;
    ladder[2].x = -575;
    ladder[2].y = -500;
    ladder[3].x = 0;
    ladder[3].y = -200;
    ladder[4].x = 575;
    ladder[4].y = -200;
    ladder[5].x = -300;
    ladder[5].y = 100;
    ladder[6].x = -575;
    ladder[6].y = 100;
    ladder[7].x = 575;
    ladder[7].y = 400;
    ladder[8].x = -175;
    ladder[8].y = 700;

    int i;
    for(i=0;i<BARRELS;i++){
        barrel[i].x = -480;
        barrel[i].y = 600;
        barrel[i].z = 0;
        barrel[i].a = 0;
    }

    flooring[0][0].x= -740;
    flooring[0][0].y= -900;
    flooring[0][1].x= 950;
    flooring[0][1].y= -750;

    flooring[1][0].x= -950;
    flooring[1][0].y= -600;
    flooring[1][1].x= 650;
    flooring[1][1].y= -450;

    flooring[2][0].x= -650;
    flooring[2][0].y= -300;
    flooring[2][1].x= 950;
    flooring[2][1].y= -150;

    flooring[3][0].x= -950;
    flooring[3][0].y= 0;
    flooring[3][1].x= 650;
    flooring[3][1].y= 150;

    flooring[4][0].x= -650;
    flooring[4][0].y= 300;
    flooring[4][1].x= 950;
    flooring[4][1].y= 450;

    flooring[5][0].x= -350;
    flooring[5][0].y= 600;
    flooring[5][1].x= 650;
    flooring[5][1].y= 750;

    flooring[6][0].x= -225;
    flooring[6][0].y= 900;
    flooring[6][1].x= -130;
    flooring[6][1].y= 1050;

    bros = 0;

    barrelBlock = 0;

    onJump = 0;
    onClimb = 0;

    jumping = 0;
    walking = 0;
    climbing = 0;

    reshape(glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT));
}

int checkLadder(int x, int y, int range){
    int i;

    for(i=0;i<LADDERS;i++){
        if((ladder[i].x-range <= x)&&(x <= ladder[i].x+range)&&(ladder[i].y-100 <= y)&&(y <= ladder[i].y+200)){
            return 1;
        }
    }

    return 0;
}

int checkFloor(int x, int y){
    int i;

    for(i=0;i<FLOORS;i++){
        if((flooring[i][0].x <= x)&&(x <= flooring[i][1].x)&&(flooring[i][0].y <= y)&&(y <= flooring[i][1].y)){
            return 1;
        }
    }

    return 0;
}

int checkFall(int x, int y, int dir){
    int i;

    for(i=0;i<FLOORS;i++){
        if((dir==1)&&(flooring[i][1].x <= x)&&(flooring[i][0].y <= y)&&(y <= flooring[i][1].y)){
            return 1;
        }
        else if((dir==-1)&&(flooring[i][0].x >= x)&&(flooring[i][0].y <= y)&&(y <= flooring[i][1].y)){
            return 1;
        }
    }

    return 0;
}

int checkCollision(int x1, int y1, int x2, int y2, int rx, int ry){
    if((x1-rx <= x2)&&(x2 <= x1+rx)&&(y1-ry <= y2)&&(y2 <= y1+ry)&&((getDepth(y1)==getDepth(y2))))
        return 1;

    return 0;
}

int findFreeBarrel(){
    int i;

    for(i=0;i<BARRELS;i++){
        if(barrel[i].z == 0)
            return i;
    }

    return -1;
}

void returnBarrel(int i){
    barrel[i].x = -480;
    barrel[i].y = 600;
    barrel[i].z = 0;
    barrel[i].a = 0;
}

void addPoints(int p){
    if(points+p > 99999)
        points = 99999;
    else
        points = points+p;
}

void animate(int value){

    if(gameState == 1){
        int i;

        timeLeft = timeLeft - 0.075;

        if(timeLeft <= 0.0){
            timeLeft = 0.0;
            gameState = -1;
        }

        dk.a = dk.a + dk.z;
        if(dk.a == dk.z*30){
            dk.z = -dk.z;
        }

        pauline.a = pauline.a + pauline.z*3;
        if(pauline.a == pauline.z*30){
            pauline.z = -pauline.z;
        }

        mario.a = mario.a + mario.z*2;
        if(mario.a == mario.z*16){
            mario.z = -mario.z;
        }

        if(barrelBlock==0){
            if(rand()%25 == 0){
                i = findFreeBarrel();
                if(i >= 0){
                    barrel[i].z = 1;
                    barrelBlock = 125;
                }
            }
        }
        else{
            barrelBlock--;
        }

        for(i=0;i<BARRELS;i++){
            if(barrel[i].z != 0){
                if(barrel[i].a < 360)
                    barrel[i].a = barrel[i].a + (barrel[i].z*-5);
                else
                    barrel[i].a = 0;

                if(checkCollision(barrel[i].x,barrel[i].y,mario.x,mario.y,70,95)){
                    //death
                    mario.y = mario.y - abs(mario.y%10);

                    returnBarrel(i);
                    walking = 0;
                    climbing = 0;
                    onClimb = 0;
                    jumping = 1;
                    gameState = -1;
                }
                else if((checkCollision(barrel[i].x,barrel[i].y,mario.x,mario.y,10,200)) && (jumping != 0) && (pointsLast != i)){
                    //jump over barrel
                    addPoints(500);
                    pointsLast = i;
                }
                else if(checkCollision(barrel[i].x,barrel[i].y,oil.x,oil.y,35,25)){
                    //barrel on fire
                    returnBarrel(i);
                }
                else if(checkCollision(pauline.x,pauline.y,mario.x,mario.y,100,250)){
                    //win
                    dk.x = -660;
                    dk.y = 710;

                    pauline.x = -315;
                    pauline.y = 885;
                    pauline.z = 1;
                    pauline.a = 0;

                    walking = 0;
                    climbing = 0;
                    onClimb = 0;
                    jumping = 1;
                    gameState = 0;
                }

                if(abs(barrel[i].z) == 1){
                    if(((checkFall(barrel[i].x+(-barrel[i].z*75),barrel[i].y,barrel[i].z)))||
                       ((rand()%5==0)&&(checkLadder(barrel[i].x,barrel[i].y-150,0)))){
                        if(barrel[i].z == 1)
                            barrel[i].z = 2;
                        else if(barrel[i].z == -1)
                            barrel[i].z = -2;

                        barrel[i].a = 0;
                        barrel[i].y = barrel[i].y - 10;
                    }
                    else{
                        barrel[i].x = barrel[i].x + (barrel[i].z*5);
                    }
                }
                else if(abs(barrel[i].z) == 2){
                    if(barrel[i].y%300 != 0){
                        barrel[i].y = barrel[i].y - 10;
                    }
                    else{
                        if(barrel[i].z == 2)
                            barrel[i].z = -1;
                        else if(barrel[i].z == -2)
                            barrel[i].z = 1;
                        barrel[i].a = 0;
                    }
                }
            }
        }

        if(jumping != 0){
            if(jumping == 1){
                if(mario.y < onJump+90){
                    mario.y = mario.y + 10;
                }
                else if(mario.y < onJump+150){
                    mario.y = mario.y + 5;
                }
                else{
                    jumping = -1;
                }
            }
            else{
                if(mario.y > onJump){
                    mario.y = mario.y - 5;
                }
                else{
                    jumping = 0;
                }
            }
        }

        if((walking != 0)&&(checkFloor(mario.x+(walking*5),mario.y))){
            mario.x = mario.x + (walking*5);
        }

        if(climbing != 0){
            if(checkLadder(mario.x, mario.y+(climbing*5),20))
                mario.y = mario.y + (climbing*5);
            else
                onClimb = 0;
        }

    }
    else if(gameState == -1){
        mario.y = mario.y - 10;

        if(mario.y%300 == 0)
            deathFloor++;

        if(mario.y <= -1200)
            reset();
    }
    else if(gameState == 0){

        if((int) timeLeft > 0.0){
            addPoints(100);
            timeLeft = timeLeft - 1.0;
        }

        dk.y = dk.y - 10;
        deathFloor++;

        if(dk.y <= -2500)
            reset();
    }

	glutPostRedisplay();
	glutTimerFunc(TIMER,animate,value);
}

void keyboardSpecial(int key, int x, int y) {
    if(gameState == 1){

        switch(key) {
            case GLUT_KEY_LEFT: {
                if((onClimb == 0)&&(checkFloor(mario.x-5,mario.y)))
                    walking = -1;
            }
            break;
            case GLUT_KEY_RIGHT: {
                if((onClimb == 0)&&(checkFloor(mario.x+5,mario.y)))
                    walking = 1;
            }
            break;
            case GLUT_KEY_UP: {
                if((jumping == 0)&&(walking == 0)&&(checkLadder(mario.x,mario.y+5,20))){
                    climbing = 1;
                    onClimb = 1;
                }
                else
                    onClimb = 0;
            }
            break;
            case GLUT_KEY_DOWN: {
                if((jumping == 0)&&(walking == 0)&&(checkLadder(mario.x,mario.y-5,20))){
                    climbing = -1;
                    onClimb = 1;
                }
                else
                    onClimb = 0;
            }
            break;
        }
    }
}

void keyboardSpecialUp(int key, int x, int y) {
    if(gameState == 1){

        switch(key) {
            case GLUT_KEY_LEFT: {
                walking = 0;
            }
            break;
            case GLUT_KEY_RIGHT: {
                walking = 0;
            }
            break;
            case GLUT_KEY_UP: {
                climbing = 0;
            }
            break;
            case GLUT_KEY_DOWN: {
                climbing = 0;
            }
            break;
        }

    }
}

void pause(){
    if(gameState == 2){
        gameState = 1;
        walking = 0;
    }
    else if(gameState == 1){
        gameState = 2;
    }
}

void keyboard(unsigned char key, int x, int y) {

    if(gameState == 1){
        switch(key) {
            case 32:
                if((jumping == 0)&&(climbing == 0)&&(onClimb == 0)&&(mario.y < 900)){
                    jumping = 1;
                    onJump = mario.y;
                }
            break;
            case 'L':
            case 'l':
                bros = (bros+1)%2;
            break;
            case 'W':
                 mario.x = -180;
                 mario.y = 900;
            break;
        }
    }

    switch(key) {
        case 'R':
        case 'r':
            reset();
        break;
        case 'C':
        case 'c':
            cameraMode = (cameraMode+1)%2;
            angle.x = 0;
            angle.y = 0;
        break;
        case 13:
            pause();
        break;
    }
}

void mouse(int button, int state, int x, int y){
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            moving = 1;
            start.x = x;
            start.y = y;
        }
        if (state == GLUT_UP) {
            moving = 0;
        }
    }
}

void motion(int x, int y){
    if (moving) {
        angle.x = angle.x + (x - start.x);
        angle.y = angle.y + (y - start.y);
        start.x = x;
        start.y = y;
    }
}

void init(){
    glClearColor(0.0,0.0,0.0,0.0);
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char *argv[]){
    screen.x = 1000;
    screen.y = 1000;
    screen.z = 1000+100;

    srand(time(NULL));

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(1000,50);
    glutInitWindowSize(screen.x,screen.y);
    glutCreateWindow("DONKEY KONG 3D");
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpecial);
    glutSpecialUpFunc(keyboardSpecialUp);
    glutReshapeFunc(reshape);

    printf("==============\n");
    printf("DONKEY KONG 3D\n");
    printf("==============\n\n");
    printf("Objetivo:\n\tSalvar Pauline das garras de Donkey Kong.\n");
    printf("Comandos:\n");
    printf("\tSetas esquerda e direita: Movimentar o personagem\n");
    printf("\tSetas cima e baixo: Subir ou descer escadas\n");
    printf("\tBarra de espaços: Pular\n");
    printf("\tENTER: Pausar o jogo\n");
    printf("\tC: Trocar o modo de câmera\n");
    printf("\tL: Trocar o personagem jogável\n");
    printf("\tR: Reiniciar o jogo\n");

    reset();

    glutTimerFunc(TIMER,animate,0);

    init();
    glutMainLoop();

    return 0;
}
