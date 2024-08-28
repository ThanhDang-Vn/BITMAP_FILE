#include "header.h" 


void readBitmapPixel(char* fileName, BMP& bmp, pixelArray* tmp ) {
	fstream file ; 
	int padding = (4 - ((bmp.dib.biWidth * (sizeof(color))) % 4)) % 4;
	file.open(fileName , ios::in | ios::binary ) ; 
	// get padding 
	bmp.pixelArray.padding = padding; 

	// move pointer here 
	file.seekg( sizeof(BMPSignature )+sizeof(BMPheader) + sizeof(BMPdib), ios::beg) ; 

	// allocate memory for the pixel array row by row 
	tmp->pixels = (color**)calloc(bmp.dib.biHeight, sizeof(color*));
	for (int i = 0 ; i < bmp.dib.biHeight ; i ++ ) {
		tmp->pixels[i] = (color*)calloc(bmp.dib.biWidth,sizeof(color)) ; 
	}
	color* buffer = new color[bmp.dib.biWidth] ; 
	for (int i = 0 ; i < bmp.dib.biHeight ; i ++ ) {
		file.read((char*) buffer , bmp.dib.biWidth*sizeof(color)) ; 
		for (int j = 0 ; j < bmp.dib.biWidth ; j ++ ) {
			tmp->pixels[bmp.dib.biHeight - i - 1 ][j] = buffer[j] ;  
		}
		file.seekg(padding,ios::cur ) ;
	}

	delete [] buffer ; 

}

void readBMP(char* fileName , BMP& bmp) {

    fstream file(fileName, ios::in | ios::binary) ;

	if (!file.is_open() ) {
		cout << "Error: can not open this file " << endl ;
		return  ; 
	}

	// read signiture header 

	file.read((char*)&bmp.signature,sizeof(BMPSignature))  ; 
	if ((bmp.signature.s[0] != 'B') || (bmp.signature.s[1] != 'M')) {
		cout << "Error: this is not a BMP file " << endl ;
		return ; 
	}

	// read header of bitmap file 
	//file.seekg(0, ios::cur) ; 
	file.read((char*)&bmp.head, sizeof(BMPheader)) ;

	// read dib of bitmap file 
	file.seekg(0 , ios::cur) ; 
	file.read((char*)&bmp.dib, sizeof(BMPdib));
	bmp.pixelArray.height = bmp.dib.biHeight ; 
	bmp.pixelArray.width = bmp.dib.biWidth ; 
	readBitmapPixel(fileName,bmp,&bmp.pixelArray) ;
	file.close() ; 
}


void releaseBMP(BMP& bmp ) {
	for (int i = 0 ; i < bmp.dib.biHeight ;  i ++ ) {
		delete [] bmp.pixelArray.pixels[i] ; 
		bmp.pixelArray.pixels[i] = NULL; 
	}
	delete [] bmp.pixelArray.pixels ; 
	bmp.pixelArray.pixels = NULL;
}

void printBMP(BMP bmp) {
	cout << "File size: " << bmp.head.fileSize << endl ; 
	cout << "Width: " << bmp.dib.biWidth << endl ; 
	cout << "Height: " << bmp.dib.biHeight << endl ; 
	cout << "Depth : " << bmp.dib.biDepth << endl ; 
	cout << "Size of image: " << bmp.dib.biSizeImage << endl ; 
	cout << "Padding: " << bmp.pixelArray.padding << endl ;  
}

void writePixelArray(fstream& file, BMP bmp, int padding) {
    for (int i = bmp.dib.biHeight - 1; i >= 0; i--) {
        file.write((char*)bmp.pixelArray.pixels[i], bmp.dib.biWidth * sizeof(color)) ; 
        char zero = 0;
        for (int j = 0; j < padding; j++) {  
			// add 0 to padding for each line
			file.write(&zero,sizeof(char)) ; 
        } 
    }
}

void writeBMPfile(char* fileName, BMP bmp )
{
    fstream file(fileName, ios::out | ios::binary  ) ; 
    if (!file.is_open()) {
		cout << "Error : can not open file " << endl ;
		return ; 
	}
	// write signature header
	file.write((char*)&bmp.signature,sizeof(BMPSignature)) ; 
	// write head of bitmap file
	file.write((char*)&bmp.head,sizeof(BMPheader)) ; 
	// write dib of bitmap file
	file.write((char*)&bmp.dib,sizeof(BMPdib)) ; 

    int padding = (4 - (bmp.dib.biWidth * (bmp.dib.biDepth / 8) % 4)) % 4;
    writePixelArray(file,bmp, padding);
    file.close() ; 
}

void copyHead(BMP& destination, BMP source, uint32_t HEIGHT , uint32_t WIDTH ) {
	destination.signature = source.signature  ; 
	destination.head = source.head ;
	int padding = (4- (WIDTH*(source.dib.biDepth / 8 ) % 4 )) % 4 ; 
	destination.head.fileSize = (WIDTH *source.dib.biDepth / 8 + padding ) * HEIGHT +54 ; 
}

void copyDib(BMP& destination, BMP source, uint32_t HEIGHT, uint32_t WIDTH ) {
	destination.dib = source.dib ; 
	// change width and height
	destination.dib.biWidth = WIDTH ; 
	destination.dib.biHeight = HEIGHT ; 
	// change size of image 
	int padding = (4 - (WIDTH * (source.dib.biDepth / 8 ) % 4 )) % 4  ; 
	destination.dib.pixelArraySize = ((WIDTH * source.dib.biDepth / 8 )+ padding ) * HEIGHT ;
}

color makeColor(uint8_t red, uint8_t green, uint8_t blue) {
	color c ; 
	c.red = red ; 
	c.green = green ; 
	c.blue = blue ; 
	return c ; 
}

void copyPixel(BMP& destination, const BMP& source, uint32_t HEIGHT, uint32_t WIDTH) {
    destination.pixelArray.height = HEIGHT;
    destination.pixelArray.width = WIDTH;
    destination.pixelArray.pixels = new color*[HEIGHT];

    int padding = (4 - (WIDTH * (destination.dib.biDepth / 8) % 4)) % 4;

    for (int i = 0; i < HEIGHT; i++) {
        destination.pixelArray.pixels[i] = new color[WIDTH];

        for (int j = 0; j < WIDTH; j++) {
            destination.pixelArray.pixels[i][j] = source.pixelArray.pixels[i][j];
        }
        for (int j = WIDTH; j < WIDTH + padding; j++) {
            destination.pixelArray.pixels[i][j] = makeColor(0, 0, 0);
        }
    }
}



BMP cutBMP(BMP& bmp , uint32_t startRow , uint32_t startColum , uint32_t sizeRow , uint32_t sizeColum ) {
	BMP temp ; 
	temp.signature = bmp.signature ; 
	copyHead(temp,bmp,sizeRow,sizeColum) ; 
	copyDib(temp,bmp,sizeRow,sizeColum) ; 

	pixelArray tmp = bmp.pixelArray , tmp_2 ; 
	tmp_2.height = sizeRow ; 
	tmp_2.width = sizeColum ;
	tmp_2.pixels= new color* [sizeRow ] ;
	for (int i = 0 ;i < sizeRow ; i ++ ) {
		tmp_2.pixels[i] = new color[sizeColum ] ; 
		for (int j = 0 ; j < sizeColum ; j ++ ) {
			tmp_2.pixels[i][j] = tmp.pixels[i + startRow][j + startColum] ;
		}
	}
	temp.pixelArray = tmp_2 ;
	return temp ; 
}

void remove_after_last_dot(char*& str) {
    char* last_dot = strrchr(str, '.');

    if (last_dot != NULL) {
        *last_dot = '\0';
    }
}

char* get_fileName(char* fileName) {
    char* path_temp = _strdup(fileName);
    remove_after_last_dot(path_temp);

    char* result = _strdup(path_temp);

    return result;
}

void splitBMP(BMP bmp ,char* fileName , int part_In_Height, int part_In_Width ) {

	// allocate memory for the pixel array row by row
	BMP** bmp_part = new BMP* [part_In_Height] ;
	for (int i = 0 ; i < part_In_Height ; i ++ ) {
		bmp_part[i] = new BMP [part_In_Width ] ; 
	}

 	// divide the image 
	int part_H = bmp.dib.biHeight / part_In_Height ; 
	int part_W = bmp.dib.biWidth / part_In_Width ;
	// cut bmp file 
	int a = 0 ; 
	for (int i = 0 ; i < part_In_Height ; i ++ ) {
		int b = 0 ; 
		for (int j = 0 ; j < part_In_Width ; j ++ ) {
			BMP tmp = cutBMP(bmp,a,b,part_H,part_W) ;
			copyHead(bmp_part[i][j],bmp,part_H,part_W) ;
			copyDib(bmp_part[i][j],bmp,part_H,part_W) ;
			copyPixel(bmp_part[i][j],tmp,part_H,part_W) ;
			b += part_W ;
		}
		a += part_H ;
	}

	// cut file part 
	char name[1024];
    sprintf(name, "%s_part", get_fileName(fileName));
    for (int i = 0; i < part_In_Height; i++) {
        for (int j = 0; j < part_In_Width; j++) {
            char output_filename[1024];
            sprintf(output_filename, "%s%02d.bmp",name  , i * part_In_Width + j + 1); 
            writeBMPfile(output_filename, bmp_part[i][j]);
        }
    }

    for (int i = 0; i < part_In_Height; i++) {
        for (int j = 0; j < part_In_Width; j++) {
            delete[] bmp_part[i][j].pixelArray.pixels;
        }
        delete[] bmp_part[i];
    }
    delete[] bmp_part;
} 



