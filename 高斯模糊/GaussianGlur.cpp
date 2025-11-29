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
const char *READ_PATH="gs1.ppm";
const char *WRITE_PATH="gs2.ppm";
const int glur_size=21;//一定是奇数
const double SIGMA=3;

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

RGBPixel BLACK={0,0,0};

RGBPixel getPixel(PPM_image &source,int x,int y)
{
	if(x<0||y<0||x>= source.width||y>= source.height)
	{
		return BLACK;
	}
	return source.data[y*source.width+x];
}

vector<vector<double>> createGaussianKernel(int size,double sigma)
{
	if(size%2==0) return vector<vector<double>>();
	
	vector<vector<double>> gk(size);
	int center=size/2;
	double sum=0;
	
	for(int x=-center;x<=center;x++)
		for(int y=-center;y<=center;y++)
		{
			double value=exp(-(x*x+y*y)/(2*sigma*sigma));
			value/=(2*M_PI*sigma*sigma);
			gk[x+center].push_back(value);
			sum+=value;
		}
		
	for(int i=0;i<size;i++)
		for(int j=0;j<size;j++)
		{
			gk[i][j]/=sum;
		}
	return gk;
}

RGBPixel blur_gaussian(PPM_image &source,int x,int y,int radius,vector<vector<double>> &gk)
{
	RGBPixel p;
	p.r=p.g=p.b=0;
	
	double r=0,g=0,b=0;
	RGBPixel temp;
	for(int i=-radius;i<=radius;i++)
	{
		for(int j=-radius;j<=radius;j++)
		{
			temp=getPixel(source,x+i,y+j);
			r+=temp.r*gk[radius+i][radius+j];
			g+=temp.g*gk[radius+i][radius+j];
			b+=temp.b*gk[radius+i][radius+j];
		}
	}
	
	p.r=r;
	p.g=g;
	p.b=b;
	return p;
}

void handle(int x1,int y1,int x2,int y2)
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
	
	vector<vector<double>> gk=createGaussianKernel(glur_size,SIGMA);
	
	for(int y=0;y<height;y++)
		for(int x=0;x<width;x++)
		{
			if(x1<=x&&x<=x2 && y1<=y&&y<=y2)
			{
				outPPM.data.push_back(blur_gaussian(inPPM,x,y,glur_size/2,gk));
			}
			else
			{
				outPPM.data.push_back(inPPM.data[y*width+x]);
			}
		}
	
	
	return;
}

int main()
{
	read();
	handle(214,339,690,467);
	write(WRITE_PATH);
	return ERR_STATE;
}