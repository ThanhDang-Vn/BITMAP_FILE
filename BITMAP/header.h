#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <stdlib.h>
using namespace std ;

struct BMPSignature{
    unsigned char s[2];
};

struct BMPheader {
	uint32_t fileSize ; 
	uint16_t reserved1 ; 
	uint16_t reserved2 ;
	uint32_t offBits ; 
};

struct BMPdib {
	uint32_t biSizeImage; 
	int32_t biWidth ; 
	int32_t biHeight ;
	uint16_t biBitCount;
	uint16_t biDepth ; 
	uint32_t compression;
	uint32_t pixelArraySize ; 
	int32_t xResolution;
	int32_t yResolution;
	uint32_t colorsUsed;
	uint32_t importantColors;
} ; 

struct color {
	uint8_t blue  ; 
	uint8_t red ; 
	uint8_t green ; 
} ; 

struct pixelArray {
	color** pixels;
	int padding ; 
	uint32_t height;
	uint32_t width;
} ; 

struct BMP {
	BMPSignature signature ;
	BMPheader head ; 
	BMPdib dib ; 
	pixelArray pixelArray;
} ;

void printBMP(BMP bmp);
void readBMP(char* fileName , BMP& bmp);
void readBitmapPixel(char* fileName, BMP& bmp , pixelArray* tmp);
void writePixelArray(fstream& file, BMP bmp, int padding) ;
void writeBMPfile(char* fileName, BMP bmp );
void copyHead(BMP& destination, BMP source, uint32_t HEIGHT , uint32_t WIDTH );
void copyDib(BMP& destination, BMP source, uint32_t HEIGHT, uint32_t WIDTH ) ;
color makeColor(uint8_t red, uint8_t green, uint8_t blue);
void copyPixel(BMP& destination,const BMP& source, uint32_t HEIGHT, uint32_t WIDTH );
BMP cutBMP(BMP& bmp , uint32_t startRow , uint32_t startColum , uint32_t sizeRow , uint32_t sizeColum ) ;
char* get_fileName(char* fileName );
void splitBMP(BMP bmp ,char* fileName , int part_In_Height, int part_In_Width );
void releaseBMP(BMP& bmp ) ; 