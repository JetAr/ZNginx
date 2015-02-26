#ifndef GANZHI
#define GANZHI
//天干
int gan(int x);

//地支
int zhi(int x);

//年干支
int yGz(int y,int m,int d,int h,int calType);

//月干支
int  mGz(int y,int m,int d,int h,int calType);

//日干支
int  dGz(int y,int m,int d,int h,int calType);

//时干支
int  hGz(int y,int m,int d,int h,int calType);

//甲子纳音
char* GZNY(int gz);
//二十八宿
int stars28(int y,int m,int d,int calType);

#endif
