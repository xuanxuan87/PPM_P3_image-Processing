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
const char *READ_PATH1="p1.ppm";
const char *READ_PATH2="p2.ppm";
const char *WRITE_PATH="pout.ppm";
const double alpha=0.5;

int ERR_STATE = 0;
enum {
	ERR_FILE_NOT_FOUND = 1,
	ERR_WRONG_FORMAT_HEADER,
	ERR_ILLEGAL_SIZE,
	ERR_FILE_BROKEN,
	ERR_FAILED_TO_WRITE
};

PPM_image inPPM1,inPPM2,outPPM;

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

void read(const char *path,PPM_image &inPPM)
{
	unsigned int width,height,maxColor;
	char Magic_Number[10];
	
	if(freopen(path,"r",stdin) == NULL)
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

RGBPixel MixPixel(const RGBPixel& source1,const RGBPixel& source2)
{
	RGBPixel p;
	p.r=alpha*source1.r+(1-alpha)*source2.r;
	p.g=alpha*source1.g+(1-alpha)*source2.g;
	p.b=alpha*source1.b+(1-alpha)*source2.b;
	return p;
}

void handle()
{
	if(checkError())
	{
		return;
	}
	unsigned int width=max(inPPM1.width,inPPM2.width);
	unsigned int height=max(inPPM1.height,inPPM2.height);
	outPPM.width=width;
	outPPM.height=height;
	outPPM.maxColor=max(inPPM1.maxColor,inPPM2.maxColor);
	
	for(int y=0;y<height;y++)
		for(int x=0;x<width;x++)
		{
			if(y<inPPM1.height&&y<inPPM2.height&&x<inPPM1.width&&x<inPPM2.width) 
				outPPM.data.push_back(MixPixel(inPPM1.data[y*inPPM1.width+x],inPPM2.data[y*inPPM2.width+x]));
			else if(y<inPPM1.height&&x<inPPM1.width)
				outPPM.data.push_back(inPPM1.data[y*inPPM1.width+x]);
			else
				outPPM.data.push_back(inPPM2.data[y*inPPM2.width+x]);
		}
	
	
	return;
}

int main()
{
	read(READ_PATH1,inPPM1);
	read(READ_PATH2,inPPM2);
	
	handle();
	write(WRITE_PATH);
	return ERR_STATE;
}