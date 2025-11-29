#include<iostream>
#include<vector>
#include<cstdio>
#include<cmath>
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
const char *READ_PATH="sobel1.ppm";
const char *WRITE_PATH1="sobel_x.ppm";
const char *WRITE_PATH2="sobel_y.ppm";
const char *WRITE_PATH3="sobel_xy.ppm";

int ERR_STATE = 0;
enum {
	ERR_FILE_NOT_FOUND = 1,
	ERR_WRONG_FORMAT_HEADER,
	ERR_ILLEGAL_SIZE,
	ERR_FILE_BROKEN,
	ERR_FAILED_TO_WRITE
};

PPM_image inPPM,outPPM;
PPM_image gPPM,outPPM_x,outPPM_y,outPPM_xy;

const int sobel_x[3][3]={{-1,0,1},
						 {-2,0,2},
						 {-1,0,1}};
const int sobel_y[3][3]={{-1,-2,-1},
						 {0 ,0 ,0 },
						 {1 ,2 ,1 }};
const int T=100;//阈值


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

RGBPixel BLACK={0,0,0};
RGBPixel WHITE={255,255,255};

RGBPixel getPixel(PPM_image &source,int x,int y)
{
	if(x<0||y<0||x>= source.width||y>= source.height)
	{
		return BLACK;
	}
	return source.data[y*source.width+x];
}

int sobel_gpu(PPM_image &source ,int x,int y,const int (&sobel_k)[3][3])
{
	int temp=0;
	for(int i=-1;i<=1;i++){
		for(int j=-1;j<=1;j++){
			temp+=getPixel(source,x+i,y+j).r*sobel_k[i+1][j+1];
		}
	}
	return temp;
}


void handle()
{
	if(checkError())
	{
		return;
	}
	unsigned int width=inPPM.width;
	unsigned int height=inPPM.height;
	gPPM.width=width;
	gPPM.height=height;
	gPPM.maxColor=inPPM.maxColor;
	
	for(int y=0;y<height;y++)
		for(int x=0;x<width;x++)
		{
			gPPM.data.push_back(Gray(inPPM.data[y*width+x]));
		}
	
	vector<int> Gx(gPPM.data.size());
	vector<int> Gy(gPPM.data.size());
	vector<int> Gxy(gPPM.data.size());
	
	
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			Gx[y*width+x]=sobel_gpu(gPPM,x,y,sobel_x);
			Gy[y*width+x]=sobel_gpu(gPPM,x,y,sobel_y);
			Gxy[y*width+x]=(Gx[y*width+x])+abs(Gy[y*width+x]);
		}
	}
	
	outPPM_x.width=width;
	outPPM_x.height=height;
	outPPM_x.maxColor=inPPM.maxColor;
	outPPM_y.width=width;
	outPPM_y.height=height;
	outPPM_y.maxColor=inPPM.maxColor;
	outPPM_xy.width=width;
	outPPM_xy.height=height;
	outPPM_xy.maxColor=inPPM.maxColor;
	
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			if(Gx[y*width+x]>T) outPPM_x.data.push_back(WHITE);
			else outPPM_x.data.push_back(BLACK);
			if(Gy[y*width+x]>T) outPPM_y.data.push_back(WHITE);
			else outPPM_y.data.push_back(BLACK);
			if(Gxy[y*width+x]>T) outPPM_xy.data.push_back(WHITE);
			else outPPM_xy.data.push_back(BLACK);
		}
	}
	
	
	return;
}

int main()
{
	read();
	handle();
	outPPM=outPPM_x;
	write(WRITE_PATH1);
	outPPM=outPPM_y;
	write(WRITE_PATH2);
	outPPM=outPPM_xy;
	write(WRITE_PATH3);
	return ERR_STATE;
}