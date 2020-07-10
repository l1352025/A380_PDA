 #include <stdio.h>

// int main()
// {
//     FILE *fp = fopen("test.txt", "w");
    
//     printf("xxx > test.txt start ...\r\n");
//     for(int i = 0; i < 1024; i++){
//         for(int j = 0; j < 1024; j++){
//             fwrite("XXXXXXXXX\n", 10, 1, fp);
//         }
//         fflush(fp);
//     }
//     fclose(fp);
//     printf("xxx > test.txt finish\r\n");
//     getchar();

//     return 0;
// }


void foo(int *a,int *b){
	printf("*a=%d,*b=%d",*a,*b);
}

void foo2(int c){
	printf(" c=%d",c);
}

// void main() {
// 	int x=1,y=2, z=3;
// 	void *p = NULL;
	
//     p = foo;
//     ((void (*)(void*, void*))p)(&x,&y);

//     p = foo2;
//     ((void (*)(int c))p)(z);
// }


void main(){
    char chars[10];

    sprintf(chars, "12345678987");
    printf("chars[10] size: %d \n", sizeof(chars));
    printf("chars[10] str : %s \n", chars);
}