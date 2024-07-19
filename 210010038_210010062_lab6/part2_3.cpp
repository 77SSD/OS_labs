#include<bits/stdc++.h>
#include<threads.h>
#include<atomic>
#include<vector>
#include<sys/wait.h>
#include<unistd.h>

using namespace std;

struct pixel_value{
	int red;
	int green;
	int blue;
};

struct info{
	int red;
	int blue;
	int green;
	int i;
	int j;
};



int Convert(int value,int factor,int max);
void change_brightness(char PPM_version[10],int width,int height,int factor,int max_color,vector<vector<pixel_value>> &image, int pipe_info[2], int pipe_final[2], char *argv);
void grayscale(vector<vector<pixel_value>> &image, int width, int height, int max_color, int pipe_info[2]);


int main(int argc, char** argv)
{
	int red;
	int green;
	int height, width;
	int max_color;
	int blue;
	char PPM_version[10];

	int pipe_info[2];

	int pipe_final[2];
	
	int pipe_created;
	
	pipe_created = pipe(pipe_info);
	
	pipe_created = pipe(pipe_final);
	
	
	FILE *original_image = fopen(argv[1], "r");
	fscanf(original_image,"%s%d%d%d",PPM_version,&width,&height,&max_color);
	
	vector<vector<pixel_value>> image(height,vector<pixel_value>(width));
	

	int var = height-1;
	while (var >= 0)
	{
		int temp = 0;
		while (temp<=width-1)
		{
			fscanf(original_image,"%d%d%d",&red,&green,&blue);
			image[var][temp].red=red;
			image[var][temp].green=green;
			image[var][temp].blue=blue;
			temp++;
		}
		var--;
	}
	

	fclose(original_image);
	
	
	int p1=fork();
	
	if (p1==0 ){
		grayscale(image, width,height,max_color,pipe_info);
	}
	
    if(p1 > 0){
	
	change_brightness(PPM_version,width,height,50,max_color,image,pipe_info,pipe_final, argv[2]);
	wait(NULL);
    }
}


int Convert(int value,int factor,int max)
{
	if (value + factor >= max)
	return max;

	else if (value + factor <=0)
	return 0;

	else return (value+ factor);
	
}
void change_brightness(char PPM_version[10],int width,int height,int factor,int max_color,vector<vector<pixel_value>> &image, int pipe_info[2], int pipe_final[2], char *argv)
{
		
	int red;
	int blue;
	int green;

	int count=0;

	int i;
	int j;
	struct info information[1];
	pixel_value temp_pixel;

	while (count < height*width)
	{
		read(pipe_info[0], information, sizeof(struct info));
		i=information[0].i;
		j=information[0].j;

		red=information[0].red;
		green=information[0].green;
		blue=information[0].blue;
		
		image[i][j].red=red;
		image[i][j].green=green;
		image[i][j].blue=blue;
		
		image[i][j].red=Convert(image[i][j].red,factor,max_color);
		image[i][j].green=Convert(image[i][j].green,factor,max_color);
		image[i][j].blue=Convert(image[i][j].blue,factor,max_color);
		count+=1;

		
		
	}
	
	FILE *morphed_image = fopen(argv,"w");
	fprintf(morphed_image,"%s\n%d %d\n%d\n",PPM_version, width, height, max_color);
	
	
	
	for (int i=height-1;i>=0;i--)
	{
		for (int j=0;j<=width-1;j++)
		{
			fprintf(morphed_image,"%d ", image[i][j].red);
			fprintf(morphed_image,"%d ", image[i][j].green);
			fprintf(morphed_image,"%d ", image[i][j].blue);
			
		}
		fprintf(morphed_image, "\n");
		
	}
	fclose(morphed_image);	
	
}

void grayscale(vector<vector<pixel_value>> &image, int width, int height, int max_color, int pipe_info[2])
{
	close(pipe_info[0]);
	int r,g,b;
	struct info information[1];
	int i=0;
	while(i<height)
	{  int j=0;
	   
		while (j<width)
		{  
			r=image[i][j].red;
			g=image[i][j].green;
			b=image[i][j].blue;
			
			image[i][j].red = ((r*0.2899) + (g*0.5804) + (b*0.1261));
			image[i][j].green = ((r*0.2899) + (g*0.5804) + (b*0.1261));
			image[i][j].blue = ((r*0.2899) + (g*0.5804) + (b*0.1261));
			
			r=image[i][j].red;
			g=image[i][j].green;
			b=image[i][j].blue;
		
			information[0].i=i;
			information[0].j=j;
			
			information[0].red=r;
			information[0].green=g;
			information[0].blue=b;
			
			//cout<<"hi";
			write(pipe_info[1],information,sizeof(struct info));

			j++;
		}
		//cout<<"jhnj";
		i++;
	}
	//cout<<"hi";
}
