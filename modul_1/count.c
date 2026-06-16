#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define PI 3.14159265358979323846

int main(int argc, char* argv[]){

    double a, b, c, result;

    if (argc == 4 ){
        printf("first program -----\n");
        a = atof(argv[1]);
        b = atof(argv[3]);

        
        if(strcmp(argv[2], "+") == 0){
            result = a + b;
        }else if(strcmp(argv[2], "-") == 0){
            result = a - b;
        }else if(strcmp(argv[2], "/") == 0){
            result = a / b;
        }else if(strcmp(argv[2], "x") == 0){
            result = a * b;
        }else if(strcmp(argv[2], "div") == 0){
            result = floor(a / b);
        }else if(strcmp(argv[2], "mod") == 0){
            result = fmod(a,b);
        }else if(strcmp(argv[2], "pangkat") == 0){
            result = pow(a, b);
        }else if(strcmp(argv[2], "log") == 0){
            result = log(b) / log(a);
        }else
        {
            printf("\n ----operator value didn't exist----");
        }
        

        printf("the calculation of %s %s %s = %g", argv[1], argv[2], argv[3], result);
    
    }else if(argc == 3){
        printf("second program -----");
        c = atof(argv[2]);
    
        
        if(strcmp(argv[1], "log") == 0){
            result = log10(c);
        }else if(strcmp(argv[1], "ln") == 0){
            result = log(c);
        }else if(strcmp(argv[1], "sin") == 0){
            result = sin(c * (PI / 180));
        }else if(strcmp(argv[1], "cos") == 0){
            result = cos(c * (PI / 180));
        }else if(strcmp(argv[1], "tan") == 0){
            result = tan(c * (PI / 180));
        }else if(strcmp(argv[1], "factorial") == 0){
            if ( c <= 0){
                printf("\nError: Logarithm input must be positive!");
                return 0;
            }
            result = 1;
            while(c >=1 ){
                result *= c ;
                c --;
            }
        }else{
            printf("\n ----operator value didn't exist----");
        }
        
        printf("\nthe calculation of %s %s  = %g", argv[1], argv[2], result);
    }else{
        printf("invalid command, please try again");
    }
    
    return 0;
}
