#include "header.h" 

int main(int argc,char** argv ) {
		BMP bmp ; 
		if (argc < 3 ) {
			cout << "Usage : " << argv[0] << "<file name> [-h <parts in height ] [-w parts in width ]" << endl ;  
			return 1 ;  
		}
		int height = 1, width = 1 ; 
		for (int i = 2 ; i < argc ; i += 2 ) {
			if (strcmp(argv[i],"-h") == 0 ) {
				height = atoi(argv[i + 1]) ; 
			} else if (strcmp(argv[i],"-w") == 0 ) {
				width = atoi(argv[i + 1]) ; 
			} else {
				cout << "Usage : " << argv[0] << "<file name> [-h <parts in height ] [-w parts in width ]" << endl ;  
			    return 1 ;  
			}
		}

		readBMP(argv[1],bmp) ; 
		printBMP(bmp) ; 
		splitBMP(bmp,argv[1],height,width) ;
		releaseBMP(bmp) ; 
		return 0 ; 

}