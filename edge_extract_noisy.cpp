#include <iostream>
#include <stdlib.h>
#include <math.h>

#define ROW 512
#define COL 512

unsigned char gimage[ROW*COL],buffer[ROW*COL];
unsigned char cimage[ROW*COL*3],cbuffer[ROW*COL*3];
unsigned char gimage2[ROW][COL],buffer2[ROW][COL];
double g_window[100][100];

void binarizaiton(int thres)
{
    for(int i = 0; i<ROW*COL; i++) {
        buffer[i] = gimage[i];
        if(buffer[i]>=thres)
            buffer[i]=255;
        else
            buffer[i]=0;
    }
}

void histogrameq(unsigned char inimg[])
{
    int i,j,r;
    unsigned int histo[256];
    double pdf[256],cdf[256],s;
    
    for(i=0;i<256;i++)
        histo[i]=0;
    
    for(i=0;i<ROW;i++)
        for(j=0;j<COL;j++)
            histo[inimg[i*COL+j]]++;
    
    for(i=0;i<256;i++) {
        pdf[i] = histo[i]/(double)(ROW*COL);
        
        if(i==0) cdf[i] = pdf[i];
        else cdf[i] = cdf[i-1] + pdf[i];
    }
    
    for(i=0;i<ROW;i++)
        for(j=0;j<COL;j++) {
            r=inimg[i*COL+j];
            s=255.0*cdf[r];
            buffer[i*COL+j] = int(s+0.5);
        }
    
    for(i=0;i<ROW;i++)
        for(j=0;j<COL;j++) inimg[i*COL+j] = buffer[i*COL+j];
    
}


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


void Gaussian(unsigned char image[], int wsize,double std)
{
    
    int i,j,k,l,m;
    double temp;
    double Pi=3.141592;
    
    /*
     g_window = new double* [wsize];
     for(i=0; i<wsize; i++)
     g_window[i] = new double [wsize];
     */
    
    m=(int)(wsize/2.0);
    
    for(i=-m; i<=m; i++)
        for(j=-m; j<=m; j++) {
            g_window[m+i][m+j]=(double)((1./(2.*Pi*std*std))
                                        *(double)exp((double)(-((i*i)+(j*j))/(2.*std*std))));
            
            printf(" m+i ===> %d  m+j ===> % d  ---%lf\n ",i,j,g_window[m+i][m+j]);
        }
    
    for(i=m;i<ROW-m;i++)
        for(j=m;j<COL-m;j++){
            temp=0.;
            for( k=-m;k<=m;k++)
                for( l=-m;l<m;l++)
                    temp+=(double)image[(i+k)*COL+(j+l)] * g_window[k+m][l+m];
            
            if(temp>255) temp = 255;
            buffer[i*COL+j]=(unsigned char)(temp+0.5);
        }
    memcpy(image,buffer,ROW*COL);
    /*
     for(m=0; m<wsize; m++)
     delete[] g_window[m];
     delete[] g_window;
     */
    
}



void average(unsigned char image[],int wsize)
{
    
    int i,j,k,l,m;
    double temp;
    
    m=(int)(wsize/2.0);
    
    for(i=-m; i<=m; i++)
        for(j=-m; j<=m; j++)
        {
            g_window[m+i][m+j]=(double)(1./(wsize*wsize));
            
//            printf(" m+i ===> %d  m+j ===> % d  ---%lf\n ",i,j,g_window[m+i][m+j]);
        }
    
    
    for(i=m;i<ROW-m;i++)
        for(j=m;j<COL-m;j++){
            temp=0.;
            for( k=-m;k<=m;k++)
                for( l=-m;l<m;l++)
                    temp+=(double)image[(i+k)*COL+(j+l)] * g_window[k+m][l+m];
            
            if(temp>255) temp = 255; if(temp<0) temp =0;
            buffer[i*COL+j]=(unsigned char)(temp+0.5);
        }
    memcpy(image,buffer,ROW*COL);
    
    
    
}


void median(unsigned char out[][COL],unsigned char image[][COL],int size)
{
    int i,j;
    int k, l;
    int first, search, temp, count;
    int *med = new int [size*size];
    
    for(i = (int)(size/2); i < ROW - (int)(size/2); i++)
        for(j = (int)(size/2); j < COL - (int)(size/2); j++) {
            count = 0;
            for(k = -1*(int)(size/2); k <=(int)(size/2); k++)
                for(l = -1*(int)(size/2); l <=(int)(size/2); l++)
                    med[count++] = image[i+k][j+l];
            
            for(first = 0; first < size*size - 1; first++)
                for(search = first + 1; search < size*size; search++)
                    if(med[search] > med[first]) {
                        temp = med[search];
                        med[search] = med[first];
                        med[first] = temp;
                    }
            out[i][j] = med[(int)(size*size/2)];
            
        }
    delete [] med;
}



void sobel1d(unsigned char out[],unsigned char in[])
{
    
    int i,j,k,m;
    int sumx=0,sumy=0,val=0;
    
    int xwin[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    int ywin[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};
    
    for(i=1;i<ROW-1;i++)
        for(j=1;j<COL-1;j++) {
            
            sumx = sumy = 0;
            for(k=-1;k<=1;k++)
                for(m=-1;m<=1;m++) {
                    sumx += in[(i+k)*COL+(j+m)]*xwin[k+1][m+1];
                    sumy += in[(i+k)*COL+(j+m)]*ywin[k+1][m+1];
                }
            val = abs(sumx)+abs(sumy);
            if(val>255) val = 255;
            out[i*COL+j] = val;
        }
}


void Laplacian1d(unsigned char out[],unsigned char in[])
{
    
    int i,j,k,m;
    int sum=0,val=0;
    
    int win[3][3] = {{1,1,1},{1,-8,1},{1,1,1}};
    
    
    for(i=1;i<ROW-1;i++)
        for(j=1;j<COL-1;j++) {
            
            sum = 0;
            for(k=-1;k<=1;k++)
                for(m=-1;m<=1;m++) {
                    sum += in[(i+k)*COL+(j+m)]*win[k+1][m+1];  // Be careful you must use double parentheses.
                    
                }
            val = abs(sum);
            if(val>255) val = 255;
            out[i*COL+j] = val;
        }
}

void imagenormalize(unsigned char inimg[])
{
    
    int i,j,min=255,max=0;
    
    for(i=0;i<ROW;i++)
        for(j=0;j<COL;j++) {
            if (min>inimg[i*COL+j]) min = inimg[i*COL+j];
            if (max<inimg[i*COL+j]) max = inimg[i*COL+j];
        }
    
    for(i=0;i<ROW;i++)
        for(j=0;j<COL;j++) {
            inimg[i*COL+j] = (int)(255.0*(inimg[i*COL+j]-min)/(double)(max-min));
        }
}


int main()
{
    
    char readfilename[50] = "/Users/hyejin/Desktop/noise/t2n2.bmp";
    char writefilename[50] = "/Users/hyejin/Desktop/noise/t2n2_result.bmp";
    char header[2048];
    int i,j,x,y,z;
    
    FILE *fpread,*fpwrite;
    fpread = fopen(readfilename,"rb");
    fpwrite = fopen(writefilename,"wb");
    
    if(fpread==NULL||fpwrite==NULL) { printf("\nFile Open Error"); return 0; }
    
    fread(header,1,54,fpread);  // Be careful that image has been rotated by 180 as defined in the header information
    fread(cimage,3,ROW*COL,fpread);
    
    changetogray(cimage,gimage);
    
    //histogram equalization
    histogrameq(gimage);
    
    //median filter
    for(i=1;i<ROW-1;i++) for(j=1;j<COL-1;j++) gimage2[i][j]=gimage[i*COL+j];
    median(buffer2,gimage2,20);
    for(i=1;i<ROW-1;i++) for(j=1;j<COL-1;j++) gimage[i*COL+j]=buffer2[i][j];

    //Gaussian filter
    Gaussian(gimage,3,2.0);
    
    //sobel filter
    sobel1d(buffer,gimage);
    
    changeto24bit(buffer,cimage);
    
    fwrite(header,1,54,fpwrite);
    fwrite(cimage,3,ROW*COL,fpwrite);
    
    fclose(fpread);
    fclose(fpwrite);
    
    
    return 0;
}




