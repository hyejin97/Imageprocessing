#include <iostream>
#include <math.h>

#define ROW 512
#define COL 512


int objectct=0,largestobject;
int area[100000];


unsigned char gimage[ROW*COL],buffer[ROW*COL],buffer2[ROW*COL],buffer3[ROW*COL];
unsigned char cimage[ROW*COL*3],cbuffer[ROW*COL*3],ccopy[ROW*COL*3];
unsigned char gimage2[ROW][COL];

void changetogray(unsigned char cimg[], unsigned char gimg[])
{
    int i,j,k;
    int temp;
    
    for(i=0;i<ROW;i++) for(j=0;j<COL;j++)  {
        
        temp = (int)(0.114*cimg[(i*COL+j)*3]+0.587*cimg[(i*COL+j)*3+1]+0.299*cimg[(i*COL+j)*3+2]);
        gimg[i*COL+j] = temp;
        
    }
}

void changeto24bit(unsigned char gimg[],unsigned char cimg[])
{
    int i,j,k;
    
    for(i=0;i<ROW;i++) for(j=0;j<COL;j++) for(k=0;k<3;k++)  {
        cimg[(i*COL+j)*3+k] = gimg[i*COL+j];
        
    }
}


void erosion(unsigned char image[])
{
    int imgbackground = 0;
    unsigned char buffer[ROW*COL];
    int i,j,k,l,count;
    int str[25]= {   0, 0, 1, 0, 0,
        0, 1, 1, 1, 0,
        1, 1, 1, 1, 1,
        0, 1, 1, 1, 0,
        0, 0, 1, 0, 0  };
    int str_size = 5;
    int str_area = 13;
    
    for(i=0;i<ROW-str_size/2;i++)
        for(j=0;j<COL-str_size/2;j++) {
            count = 0;
            for(k= 0 ; k<str_size ; k++)
                for(l= 0 ; l<str_size ; l++)
                    if(str[k*str_size+l]!=0)
                        if(image[(i+k)*COL+(j+l)]!=imgbackground)    count++;
            
            if(count==str_area)
                buffer[(i+str_size/2)*COL+(j+str_size/2)] = 255-imgbackground;
            else
                buffer[(i+str_size/2)*COL+(j+str_size/2)] = imgbackground;
        }
    for(i=0;i<ROW;i++) for(j=0;j<COL;j++)  image[i*COL+j] = buffer[i*COL+j];
}


void dilation(unsigned char image[])
{
    int imgbackground = 0;
    unsigned char buffer[ROW*COL];
    int i,j,k,l,count;

    int str[25]= { 0, 1, 1, 1, 0,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        0, 1, 1, 1, 0   };
    int str_size = 5;
    int str_area = 13;
    
    
    for(i=0;i<ROW-str_size/2;i++)
        for(j=0;j<COL-str_size/2;j++) {
            count = 0;
            for(k= 0 ; k<str_size ; k++)
                for(l= 0 ; l<str_size ; l++)
                    if(str[k*str_size+l]!=0)
                        if(image[(i+k)*COL+(j+l)]!=imgbackground)    count++;
            
            if(count>0)
                buffer[(i+str_size/2)*COL+(j+str_size/2)] = 255-imgbackground;
            else
                buffer[(i+str_size/2)*COL+(j+str_size/2)] = imgbackground;
        }
    for(i=0;i<ROW;i++) for(j=0;j<COL;j++)  image[i*COL+j] = buffer[i*COL+j];
}


void Seed(int i0, int j0, unsigned char readimage[], int background)
{
    int i,j,l,m;
    int i1, j1, num, nnum;
    long pel_count,maxarea;
    int cm[8] = { 0,-1,-1,-1, 0, 1, 1, 1 };
    int cn[8] = { 1, 1, 0,-1,-1,-1, 0, 1 };
    
    unsigned int seed_x[100000],seed_y[100000];
    unsigned int nseed_x[100000],nseed_y[100000];
    
    pel_count = 0;
    
    num = 1;
    buffer[i0*COL+j0] = objectct;
    seed_x[1] = i0;
    seed_y[1] = j0;
    
    while(num != 0) {
        for(l = 1, nnum = 0; l <= num; l++) {
            i = seed_x[l]; j = seed_y[l];
            for(m = 0; m < 8; m += 2) {
                i1 = i + cm[m]; j1 = j + cn[m];
                if((i1 >= 0) && (i1 < ROW) && (j1 >= 0) && (j1 < COL))
                    if(readimage[int(i1*COL+j1)]!=background && buffer[i1*COL+j1] !=objectct) {
                        pel_count++;
                        nnum++;
                        buffer[i1*COL+j1] = objectct;
                        nseed_x[nnum] = i1; nseed_y[nnum] = j1;
                    }
            }
        }
        for(l = 1; l <= nnum; l++) {
            seed_x[l] = nseed_x[l];
            seed_y[l] = nseed_y[l];
            
        }
        num = nnum;
    }
    
    area[objectct]= pel_count;
    if(pel_count>maxarea) {
        largestobject = objectct; maxarea = pel_count;
        
    }
}

void labeling(unsigned char gimage[])
{
    int background = 0;
    for(int i=0;i<ROW;i++) for(int j=0;j<COL;j++)  buffer[i*COL+j] = 255;
    
    for(int i=0;i<ROW;i++)
        for(int j=0;j<COL;j++) {
            if((gimage[i*COL+j]!=background) && buffer[i*COL+j]==255) {
                //   if(objectct>254) objectct=0;
                objectct++;
                Seed(i, j, gimage, background);
            }
        }
    
    for(int i=0;i<ROW;i++) for(int j=0;j<COL;j++)  {
        if(buffer[i*COL+j] != largestobject) gimage[i*COL+j] = background;
        //gimage[i*COL+j] = buffer[i*COL+j];
    }
}


void thresholding(unsigned char image[], int lth, int uth, int output)
{
    int i,j;
    
    for(i=0;i<ROW;i++) for(j=0;j<COL;j++)  {
        if(image[i*COL+j]>=lth&&image[i*COL+j]<=uth) image[i*COL+j] = (unsigned char)output;
        else image[i*COL+j] = 255-output;
    }
    
}



void roi_threshold(unsigned char image[], int lth, int uth, int output){
    for(int i=0;i<ROW;i++)
        for(int j=0;j<COL;j++){
            if(i >= ROW-450 && i < ROW-300 && j >= COL- 340 && j < COL - 270){
                if(image[i*COL+j]>=30 && image[i*COL+j]<=125) image[i*COL+j] = (unsigned char)output;
                else if(image[i*COL+j] > 170 ) image[i*COL+j] = (unsigned char)output;
                else image[i*COL+j] = 255-output;
            }
            else if(i >= ROW-300 && i < ROW-120 && j >= COL- 340 && j < COL - 270){
                if(image[i*COL+j]>=40 && image[i*COL+j]<=115) image[i*COL+j] = (unsigned char)output;
                else if(image[i*COL+j] > 175 && image[i*COL+j] < 185) image[i*COL+j] = (unsigned char)output;
                else image[i*COL+j] = 255-output;
            }
            else if(i >= ROW-450 && i < ROW-123 && j >= COL- 270 && j < COL - 160){
                if(image[i*COL+j]>=90 && image[i*COL+j]<=100) image[i*COL+j] = (unsigned char)output;
                else image[i*COL+j] = 255-output;
            }
            else if(i >= ROW-450 && i < ROW-123 && j >= COL- 160 && j < COL - 70){
                if(image[i*COL+j]>=20 && image[i*COL+j]<=100) image[i*COL+j] = (unsigned char)output;
                else if(image[i*COL+j] > 130) image[i*COL+j] = (unsigned char)output;
                else image[i*COL+j] = 255-output;
            }
            else image[i*COL+j] = output;
        }
}

void binary2img(unsigned char cimage[],unsigned char cbuffer[])
{
    
    for(int i=0;i<ROW;i++) {
        for(int j=0;j<COL;j++){
            for(int c=0;c<3;c++){
                if(cimage[(i*COL+j)*3+c] == 0) cbuffer[(i*COL+j)*3+c] = 255;
                
                else cbuffer[(i*COL+j)*3+c] = ccopy[(i*COL+j)*3+c];
            }
        }
    }
    
}


int main()
{
    
    char readfilename[50] = "/Users/hyejin/Desktop/statue.bmp";
    char writefilename[50] = "/Users/hyejin/Desktop/statue_result.bmp";
    char header[2048];
    
    FILE *fpread,*fpwrite;
    fpread = fopen(readfilename,"rb");
    fpwrite = fopen(writefilename,"wb");
    
    if(fpread==NULL||fpwrite==NULL) { printf("\nFile Open Error"); return 0; }
    
    fread(header,1,54,fpread);
    fread(cimage,3,ROW*COL,fpread);
    
    for(int i=0;i<ROW;i++) for(int j=0;j<COL;j++) for(int k=0;k<3;k++) ccopy[(i*COL+j)*3+k] = cimage[(i*COL+j)*3+k];
    
    
    changetogray(cimage, gimage);
    
    roi_threshold(gimage, 20, 140, 0);
//
    erosion(gimage);
    dilation(gimage);
//
    labeling(gimage);
    dilation(gimage);
//
    erosion(gimage);
    dilation(gimage);
//
    dilation(gimage);
//    erosion(gimage);
//
//
    changeto24bit(gimage, cimage);
//
    binary2img(cimage, cimage);
//
    
    fwrite(header,1,54,fpwrite);
    fwrite(cimage,3,ROW*COL,fpwrite);
    
    
    fclose(fpread);
    fclose(fpwrite);
    
    
    return 0;
}





