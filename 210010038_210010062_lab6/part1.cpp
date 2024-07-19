#include<bits/stdc++.h>
#include<vector>
using namespace std;

struct pixel{
    int r;
    int g;
    int b;
};

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
    char ppm[10];
	
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
	scaleGray(image, width,height,maxColor);
	brightness_change(width,height,50,maxColor,image);
 	
	FILE *morphedImage = fopen(argv[2],"w");
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
            w2++;	
        }
        h2--;
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
            image[h][w].r=transformation(image[h][w].r,factor,maxColor);
            image[h][w].g=transformation(image[h][w].g,factor,maxColor);
            image[h][w].b=transformation(image[h][w].b,factor,maxColor);
            w = w+1;
		}
        h = h+1;
    }
}


int transformation(int value,int factor,int max)
{
    int sumofvaluefactor = value+factor;
	if (sumofvaluefactor >= max){
        return max;
    }else if (sumofvaluefactor <=0){
        return 0;
    }else{
        return sumofvaluefactor;
    }	
}

void scaleGray(vector<vector<pixel>> &image, int width, int height, int maxColor)
{
	int r,g,b;
    int h = 0;
    while (h<height)
	{
        int w = 0;
		while (w<width)
		{
            r=image[h][w].r;
            g=image[h][w].g;
            b=image[h][w].b;
                
            image[h][w].r = ((r*0.2899) + (g*0.5840) + (b*0.1261));
            image[h][w].g = ((r*0.2899) + (g*0.5840) + (b*0.1261));
            image[h][w].b = ((r*0.2899) + (g*0.5840) + (b*0.1261));
            w = w+1;
		}
        h = h+1;
	}
}