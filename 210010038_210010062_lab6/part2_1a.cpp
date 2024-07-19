#include<bits/stdc++.h>
#include<chrono>
#include<thread>
#include<vector>
#include<pthread.h>
#include<assert.h>

using namespace std;
using namespace std::chrono;

int finished[2]={-1,-1};

atomic_flag atomicFlag2False= ATOMIC_FLAG_INIT;

struct pixel_value{
	int red;
	int green;
	int blue;
};


int Convert(int value, int factor, int max);
void Gray_pic_scale(vector<vector<pixel_value>> &image, int width, int height, int max_color);
void Cng_Brightness(int width,int height,int factor,int max_color,vector<vector<pixel_value>> &image);


int main(int argc, char** argv)
{
	
	
	int height;
	int width;
	int max_color;
	int red;
	int green;
	int blue;
	char PPM[10];
	
	FILE *original_image = fopen(argv[1], "r");
	fscanf(original_image,"%s%d%d%d",PPM,&width,&height,&max_color);
	
	vector<vector<pixel_value>> image(height,vector<pixel_value>(width));
	
	int h1 = height-1;
    while(h1>=0)
    {
        int w1 = 0;
        while (w1<=width-1)
        {
                fscanf(original_image,"%d%d%d",&red,&green,&blue);
                image[h1][w1].red=red;
                image[h1][w1].green=green;
                image[h1][w1].blue=blue;
                w1++;	
            	
        }
        h1--;
    }
	fclose(original_image);	
	
	thread t1(Gray_pic_scale, std::ref(image),width,height,max_color);
	thread t2(Cng_Brightness, width, height, 50, max_color, std::ref(image));
	t1.join();
	t2.join();
	FILE *morphed_image = fopen(argv[2],"w");

	fprintf(morphed_image,"%s\n%d %d\n%d\n",PPM, width, height, max_color);
	
	int h2 = height-1;
    while(h2>=0)
    {
        int w2 = 0;
        while (w2<=width-1)
        {
            fprintf(morphed_image,"%d ", image[h2][w2].red);
            fprintf(morphed_image,"%d ", image[h2][w2].green);
            fprintf(morphed_image,"%d ", image[h2][w2].blue);
            w2++;	
        }
        h2--;
    }
	
	fclose(morphed_image);	
	return 0;
}



int Convert(int value,int factor,int max)
{
	if (value + factor >= max)
	return max;

	else if (value + factor <=0)
	return 0;

	else return (value+ factor);	
}
 

void Gray_pic_scale(vector<vector<pixel_value>> &image, int width, int height, int max_color)
{

	int red;
	int green;
	int blue;

	for (int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
			while(atomic_flag_test_and_set(&atomicFlag2False));
			red=image[i][j].red;
			green=image[i][j].green;
			blue=image[i][j].blue;

			
			image[i][j].red = ((red*0.2899) + (green*0.5804) + (blue*0.1261));
			image[i][j].green = ((red*0.2899) + (green*0.5804) + (blue*0.1261));
			image[i][j].blue = ((red*0.2899) + (green*0.5804) + (blue*0.1261));

			finished[0]=i;
			finished[1]=j;

			atomic_flag_clear(&atomicFlag2False);
			
		}
	}
}


void Cng_Brightness(int width,int height,int factor,int max_color,vector<vector<pixel_value>> &image)
{
	for(int i=0;i<height;i++)
	{
		for (int j=0;j<width;j++)
		{
		while(atomic_flag_test_and_set(&atomicFlag2False));
		if( i<=finished[0] && j<=finished[1])
		{
			image[i][j].red=Convert(image[i][j].red,factor,max_color);
			image[i][j].green=Convert(image[i][j].green,factor,max_color);
			image[i][j].blue=Convert(image[i][j].blue,factor,max_color);
			
		}
		else
		{
			j=j-1;
		}
		atomic_flag_clear(&atomicFlag2False);
		}
	}
}


