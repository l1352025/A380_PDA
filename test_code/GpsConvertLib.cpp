#include <math.h>

extern "C" __declspec(dllexport) void OriginGpsToStdMap( double wgs_lat, double wgs_lon, double& gcj_lat, double& gcj_lon);
extern "C" __declspec(dllexport) void StdMapToBaiduMap(double gcj_lat, double gcj_lon, double &bd_lat, double &bd_lon);
extern "C" __declspec(dllexport) void BaiduMapToStdMap(double bd_lat, double bd_lon, double &gcj_lat, double &gcj_lon);

const double pi = 3.14159265358979324; 
const double x_pi = 3.14159265358979324 * 3000.0 / 180.0; 

//  
// Krasovsky 1940  
//   
// a = 6378245.0, 1/f = 298.3   
// b = a * (1 - f)  
// ee = (a^2 - b^2) / a^2;   
const double a = 6378245.0;  
const double ee = 0.00669342162296594323;  


static bool outOfChina(double lat, double lon)   
{   
    if (lon < 72.004 || lon > 137.8347)  
        return true;   
    if (lat < 0.8293 || lat > 55.8271)  
        return true;   

    return false;  
}   
static double transformLat(double x, double y)  
{   
    double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * sqrt(abs(x));  
        ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;   
        ret += (20.0 * sin(y * pi) + 40.0 * sin(y / 3.0 * pi)) * 2.0 / 3.0;   
        ret += (160.0 * sin(y / 12.0 * pi) + 320 * sin(y * pi / 30.0)) * 2.0 / 3.0;   
    return ret;  
}   
static double transformLon(double x, double y)  
{   
    double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * sqrt(abs(x));   
        ret += (20.0 * sin(6.0 * x * pi) + 20.0 * sin(2.0 * x * pi)) * 2.0 / 3.0;   
        ret += (20.0 * sin(x * pi) + 40.0 * sin(x / 3.0 * pi)) * 2.0 / 3.0;  
        ret += (150.0 * sin(x / 12.0 * pi) + 300.0 * sin(x / 30.0 * pi)) * 2.0 / 3.0;   
    return ret;  
}   

/**
 *  描述：将 WGS-84 坐标转换成 GCJ-02 坐标  
 *  输入：
 *  wgs_lat - WGS-84 纬度
 *  wgs_lon - WGS-84 经度 
 *  输出：
 *  gcj_lat - GCJ-02纬度
 *  gcj_lon - GCJ-02经度 
*/  
void OriginGpsToStdMap( double wgs_lat, double wgs_lon, double& gcj_lat, double& gcj_lon)   
{   
    if (outOfChina(wgs_lat, wgs_lon)) {   
        gcj_lat = wgs_lat;  
        gcj_lon = wgs_lon;  
        return;   
    }   

    double dLat = transformLat(wgs_lon - 105.0, wgs_lat - 35.0);   
    double dLon = transformLon(wgs_lon - 105.0, wgs_lat - 35.0);   
    double radLat = wgs_lat / 180.0 * pi; double magic = sin(radLat);  
    magic = 1 - ee * magic * magic; double sqrtMagic = sqrt(magic);  
    dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);  
    dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * pi);  
    gcj_lat = wgs_lat + dLat; gcj_lon = wgs_lon + dLon;  
}  

/**
 *  描述：将 GCJ-02 坐标转换成 百度地图 BD-09 坐标  
 *  输入：
 *  gcj_lat - GCJ-02纬度
 *  gcj_lon - GCJ-02经度 
 *  输出：
 *  bd_lat - BD-09纬度
 *  bd_lon - BD-09经度 
*/  
void StdMapToBaiduMap(double gcj_lat, double gcj_lon, double &bd_lat, double &bd_lon)
{
    double x = gcj_lon, y = gcj_lat;
    double z = sqrt(x * x + y * y) + 0.00002 * sin(y * x_pi);
    double theta = atan2(y, x) + 0.000003 * cos(x * x_pi);
    bd_lon = z * cos(theta) + 0.0065;
    bd_lat = z * sin(theta) + 0.006;
}

/**
 *  描述：将 百度地图 BD-09 坐标 转换成 GCJ-02 坐标 
 *  输入：
 *  gcj_lat - GCJ-02纬度
 *  gcj_lon - GCJ-02经度 
 *  输出：
 *  bd_lat - BD-09纬度
 *  bd_lon - BD-09经度 
*/  
void BaiduMapToStdMap(double bd_lat, double bd_lon, double &gcj_lat, double &gcj_lon)
{
    double x = bd_lon - 0.0065, y = bd_lat - 0.006;
    double z = sqrt(x * x + y * y) - 0.00002 * sin(y * x_pi);
    double theta = atan2(y, x) - 0.000003 * cos(x * x_pi);
    gcj_lon = z * cos(theta);
    gcj_lat = z * sin(theta);
}

