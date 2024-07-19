#include<bits/stdc++.h>
#include<semaphore.h>
#include<vector>
using namespace std;

struct pixel{
	int r;
	int g;
	int b;
};

sem_t use_lock;

int finish[2] = {-1, -1};



int transformation(int value,int factor,int max);
void brightness_change(int width,int height,int factor,int maxColor,vector<vector<pixel>> &image);
void scaleGray(vector<vector<pixel>> &image, int width, int height, int maxColor);



int main(int argc, char** argv)
{
	int height;
	int width;
	int maxColor;
	int r;
	int g;
	int b;
	int len = 10;
	char ppm[len];
	
	FILE *originalImage = fopen(argv[1], "r");
	fscanf(originalImage,"%s%d%d%d",ppm,&width,&height,&maxColor);
	
	vector<vector<pixel>> image(height,vector<pixel>(width));
	

	int h1 = height-1;
    while(h1>=0)
    {
        int w1 = 0;
        while (w1<=width-1)
        {
                fscanf(originalImage,"%d%d%d",&r,&g,&b);
                image[h1][w1].r=r;
                image[h1][w1].g=g;
                image[h1][w1].b=b;
                w1++;	
        }
        h1--;
    }
	fclose(originalImage);
	
	
	int x1=0;
	int x2=1;
	sem_init(&use_lock,x1,x2);
	
	scaleGray(image, width,height,maxColor);
	brightness_change(width,height,50,maxColor,image);
	
	int k = 2;
	FILE *morphedImage = fopen(argv[k],"w");
	fprintf(morphedImage,"%s\n%d %d\n%d\n",ppm, width, height, maxColor);
	
	int h2 = height-1;
    while(h2>=0)
    {
        int w2 = 0;
        while (w2<=width-1)
        {
            fprintf(morphedImage,"%d ", image[h2][w2].r);
            fprintf(morphedImage,"%d ", image[h2][w2].g);
            fprintf(morphedImage,"%d ", image[h2][w2].b);
            w2 ++;	
        }
        h2 --;
    }
	fclose(morphedImage);
	
	return 0;

}

void brightness_change(int width,int height,int factor,int maxColor,vector<vector<pixel>> &image)
{
    int h = 0;
	while(h<height)
	{
    	int w = 0;
		while (w<width)
		{
			if (h<=finish[0] && w<=finish[1])
			sem_wait(&use_lock);
			image[h][w].r=transformation(image[h][w].r,factor,maxColor);
			image[h][w].g=transformation(image[h][w].g,factor,maxColor);
			image[h][w].b=transformation(image[h][w].b,factor,maxColor);
	    	sem_post(&use_lock);
            w++;
		}
        h++;
	}
}


void scaleGray(vector<vector<pixel>> &image, int width, int height, int maxColor)
{
	int r;
	int g;
	int b;
    int h = 0;
    while (h<height)
	{
        int w = 0;
		while (w<width)
		{
            sem_wait(&use_lock);
			r=image[h][w].r;
			g=image[h][w].g;
			b=image[h][w].b;
			
			image[h][w].r = ((r*0.2899) + (g*0.5840) + (b*0.1261));
        	image[h][w].g = ((r*0.2899) + (g*0.5840) + (b*0.1261));
            image[h][w].b = ((r*0.2899) + (g*0.5840) + (b*0.1261));
			finish[0]=h;
			finish[1]=w;
			sem_post(&use_lock);
			w++;
		}
        h++;
	}
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