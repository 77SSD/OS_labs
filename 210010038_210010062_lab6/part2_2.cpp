#include<bits/stdc++.h>
#include<vector>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<semaphore.h>
#include<fcntl.h>
#include <cstdlib> 
using namespace std;
using namespace std::chrono;

struct pixel{
	int r;
	int g;
	int b;
};

#define semaphorelockid "/use_lock"
#define SEM_PERMS (S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP)
#define INIT_VALUE 1


int transformation(int value,int factor,int max);
void brightness_change(int width,int height,int factor,int maxColor,key_t key_pixel, key_t key_semaphore);
void scaleGray(int width, int height, int maxColor, key_t key_pixel, key_t key_semaphore);



int main(int argc, char** argv)
{
	int height;
	int width;
	int maxColor;
	int r;
	int g;
	int b;
	char ppm[10];
	
	FILE *originalImage = fopen(argv[1], "r");
	fscanf(originalImage,"%s%d%d%d",ppm,&width,&height,&maxColor);
	
	vector<vector<pixel>> image(height,vector<pixel>(width));
	
	struct pixel *matrix;
	
	key_t key_pixel= 0x1256;
	int shmid_pixel = shmget(key_pixel,sizeof(struct pixel)*height*width, 0666|IPC_CREAT);
	
	matrix = (struct pixel *)shmat(shmid_pixel, NULL, 0);
	struct pixel temp;


	int h1 = height-1;
    while(h1>=0)
    {
        int w1 = 0;
        while (w1<=width-1)
        {
            
            fscanf(originalImage,"%d%d%d",&r,&g,&b);
			temp.r=r;
			temp.g=g;
			temp.b=b;
			matrix[(h1*width)+w1]=temp;
                w1++;	
        }
		h1 --;	
	}
       
    
	fclose(originalImage);
	
	key_t key_semaphore = 0x1256;
	int *finished;
	int shmid_semaphore = shmget(key_semaphore, sizeof(int) *2, 0666 | IPC_CREAT);
	finished = (int*)shmat(shmid_semaphore, NULL, 0);
	finished[0]=-1;
	finished[1]=-1;
	
	
	sem_t *use_lock = sem_open(semaphorelockid, O_CREAT | O_EXCL, SEM_PERMS, INIT_VALUE);

	int p1=fork();

	
	if(p1 == 0 )
	{
		scaleGray(width,height,maxColor,key_pixel,key_semaphore);

	}
	if(p1 > 0 )
	{
		
		wait(NULL);	
		brightness_change(width,height,50,maxColor,key_pixel,key_semaphore);
	}
	
	
	FILE *morphedImage = fopen(argv[2],"w");
	fprintf(morphedImage,"%s\n%d %d\n%d\n",ppm, width, height, maxColor);

	int h2 = height-1;
    while(h2>=0)
    {
        int w2 = 0;
        while (w2<=width-1)
        {
               temp=matrix[h2*width+w2];
				fprintf(morphedImage,"%d ", temp.r);
				fprintf(morphedImage,"%d ", temp.g);
				fprintf(morphedImage,"%d ", temp.b);
                w2 ++;		
        }

        h2 --;
    }
	
	fclose(morphedImage);

	return 0;

}



int transformation(int value,int factor,int max)
{	int sumvaluefactor=value+ factor;
	if (sumvaluefactor >= max){
        return max;
    }else if (sumvaluefactor <=0){
        return 0;
    }else{
        return sumvaluefactor;
    }	
}



void brightness_change(int width,int height,int factor,int maxColor,key_t key_pixel, key_t key_semaphore)
{
	
	int shmid_pixel = shmget(key_pixel, sizeof(struct pixel) * height * width, 0666 | IPC_CREAT);
	
	struct pixel *matrix;
	matrix = (struct pixel *) shmat(shmid_pixel, NULL, 0);
	sem_t *use_lock = sem_open(semaphorelockid, O_RDWR);
	
	int *finished;
	
	int shmid_semaphore = shmget(key_semaphore, sizeof(int)*2, 0666 | IPC_CREAT);
	
	finished = (int *) shmat(shmid_semaphore, NULL, 0);

	int h = 0;
    while (h<height)
	{
        int w = 0;
		while (w<width)
		{     
			if (h<=finished[0] && w<=finished[1])
			sem_wait(use_lock);
			struct pixel temp = matrix[(h*width)+w];
				temp.r=transformation(temp.r,factor,maxColor);
				temp.g=transformation(temp.g,factor,maxColor);
				temp.b=transformation(temp.b,factor,maxColor);
				matrix[(h*width)+w]=temp;	
			sem_post(use_lock);
			w ++;
		}
        h ++;
	}
}



void scaleGray(int width, int height, int maxColor, key_t key_pixel, key_t key_semaphore)
{	

	int shmid_pixel = shmget(key_pixel, sizeof(struct pixel) * height * width, 0666 | IPC_CREAT);
	struct pixel *matrix;
	matrix = (struct pixel *) shmat(shmid_pixel, NULL, 0);
	sem_t *use_lock = sem_open(semaphorelockid, O_RDWR);
	
	int *finished;
	
	int shmid_semaphore = shmget(key_semaphore, sizeof(int)*2, 0666 | IPC_CREAT);
	
	finished = (int *) shmat(shmid_semaphore, NULL, 0);
	int r,g,b;

	 int h = 0;
    while (h<height)
	{
        int w = 0;
		while (w<width)
		{
            sem_wait(use_lock);
			struct pixel temp;
			temp = matrix[(h*width) +w];
			
			r=temp.r;
			g=temp.g;
			b=temp.b;
			temp.r = ((r*0.2899) + (g*0.5840) + (b*0.1261));
			temp.g = ((r*0.2899) + (g*0.5840) + (b*0.1261));
			temp.b = ((r*0.2899) + (g*0.5840) + (b*0.1261));
			
			matrix[(h*width) +w]=temp;
			finished[0]=h;
			finished[1]=w;
			
			sem_post(use_lock);
            
			w ++;
		}
        h ++;
	}
	exit(0) ;
}
