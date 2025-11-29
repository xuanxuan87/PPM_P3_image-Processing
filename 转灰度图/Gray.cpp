#include<iostream>
#include<vector>
#include<cstdio>
using namespace std;

//TYPE
struct RGBPixel{
	unsigned int r;
	unsigned int g;
	unsigned int b;
};

struct PPM_image{
	unsigned int width;
	unsigned int height;
	unsigned int maxColor;
	
	vector<RGBPixel> data;
};

//VAR
const char *READ_PATH="gray1.ppm";
const char *WRITE_PATH="gray2.ppm";

int ERR_STATE = 0;
enum {
	ERR_FILE_NOT_FOUND = 1,
	ERR_WRONG_FORMAT_HEADER,
	ERR_ILLEGAL_SIZE,
	ERR_FILE_BROKEN,
	ERR_FAILED_TO_WRITE
};

PPM_image inPPM,outPPM;

//FCUNFION
bool checkError()
{
	return ERR_STATE;
}
void throwError(int error)
{
	ERR_STATE=error;
}

void skipComments() {
    int c;
    while ((c = getchar()) != EOF) 
	{
        if (c == '#')
		{
            while ((c = getchar()) != EOF && c != '\n');
        } 
		else if (!isspace(c)) 
		{
            ungetc(c, stdin);
            break;
        }
    }
}

void read()
{
	unsigned int width,height,maxColor;
	char Magic_Number[10];
	
	if(freopen(READ_PATH,"r",stdin) == NULL)
	{
		throwError(ERR_FILE_NOT_FOUND);
		return;
	}
	
	scanf("%s",Magic_Number);
	if(strcmp(Magic_Number,"P3")!=0)
	{
		throwError(ERR_WRONG_FORMAT_HEADER);
		return;
	}
	
	skipComments();
	scanf("%u%u",&width,&height);
	skipComments();
	scanf("%u",&maxColor);
	
	if(width<=0 || height<=0)
	{
		throwError(ERR_ILLEGAL_SIZE);
		return;
	}
	
	inPPM.width = width;
	inPPM.height=height;
	inPPM.maxColor=maxColor;
	unsigned int r,g,b;
	for(int y=0;y<height;y++)
		for(int x=0;x<width;x++)
		{
			skipComments();
			if(scanf("%u%u%u",&r,&g,&b)==3)
			{
				inPPM.data.push_back({r,g,b});
			}
			else
			{
				throwError(ERR_FILE_BROKEN);
				fclose(stdin);
				return;
			}
		}
	
	fclose(stdin);
	return ;
}

void write(const char *path)
{
	if(checkError())
	{
		return;
	}
	FILE *file=fopen(path,"wb");
	if(file==NULL)
	{
		throwError(ERR_FAILED_TO_WRITE);
		return;
	}
	 
	int flag=0;
	flag |= 0 >= fprintf(file,"P3\n");
	flag |= 0 >= fprintf(file,"%u %u\n",outPPM.width,outPPM.height);
	flag |= 0 >= fprintf(file,"%u\n",outPPM.maxColor);
	
	int width=outPPM.width,height=outPPM.height;
	for(int y=0;y<height;y++)
		for(int x=0;x<width;x++)
		{
			flag |= 0 >= fprintf(file,"%u %u %u\n",outPPM.data[y*width+x].r,outPPM.data[y*width+x].g,outPPM.data[y*width+x].b);
		}
	if (flag) 
	{
		throwError(ERR_FAILED_TO_WRITE);
		return;
	}
	fflush(file);
	fclose(file);
	
	outPPM.data.clear();
	return ;
}

RGBPixel Gray(const RGBPixel &source)
{
	RGBPixel p;
	p.r=p.g=p.b=(source.r+source.g+source.b)/3;
	return p;
}

void handle()
{
	if(checkError())
	{
		return;
	}
	unsigned int width=inPPM.width;
	unsigned int height=inPPM.height;
	outPPM.width=width;
	outPPM.height=height;
	outPPM.maxColor=inPPM.maxColor;
	
	for(int y=0;y<height;y++)
		for(int x=0;x<width;x++)
		{
			outPPM.data.push_back(Gray(inPPM.data[y*width+x]));
		}
	
	
	return;
}

int main()
{
	read();
	handle();
	write(WRITE_PATH);
	return ERR_STATE;
}