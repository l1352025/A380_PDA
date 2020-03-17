#include <stdio.h>

extern void gps_Convert( double wgs_lat, double wgs_lon, double& gcj_lat, double& gcj_lon);
extern void bd_encrypt(double gcj_lat, double gcj_lon, double &bd_lat, double &bd_lon);

int main()
{
	/*
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
	*/
	
	double db1,db2;
	double db3,db4;
	gps_Convert(21.123456, 121.456789, db1, db2);
	
	bd_encrypt(db1, db2, db3, db4);
	
	printf("21.123456, 121.456789  --> %f, %f  --> %f, %f \n", db1, db2, db3, db4);
    getchar();
	
    return 0;
}