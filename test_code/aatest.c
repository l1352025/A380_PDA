#include <stdio.h>

int main()
{
    FILE *fp = fopen("test.txt", "w");
    
    printf("xxx > test.txt start ...\r\n");
    for(int i = 0; i < 1024; i++){
        for(int j = 0; j < 1024; j++){
            fwrite("XXXXXXXXX\n", 10, 1, fp);
        }
        fflush(fp);
    }
    fclose(fp);
    printf("xxx > test.txt finish\r\n");
    getchar();

    return 0;
}