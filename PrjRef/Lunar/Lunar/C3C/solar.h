#ifndef SOLAR
#define SOLAR
double tail(double x);

double  rem(double  x,double  w);

int nnr(int x,int w);

double round(double x);

//判断Gregorian历还是Julian历
int ifGr(int y,int m,int d,int calType = 1);

//日差天数
int  D0(int y,int m,int d,int calType = 1);

//反日差天数
int  revD0(int y,int x,int calType = 1);
//年差天数
int  D(int y,int calType = 1);

//标准天数(Standard Days)(y年m月d日距该历制的1年1月0日的天数)
int  SD(int y,int m,int d,int calType = 1);

//等效标准天数(Equivalent Standard Days)(y年m月d日距该历制的1年1月0日的天数)
int  ESD(int y,int m,int d,int calType = 1);
//儒略日
int  JD(int y,int m,int d,int h,int min,int sec,int zone,int calType = 0);

//反标准天数
int  revSD(int x,int calType,int ifG);

//反儒略日
int revJD(int x,int zone,int calType = 1);

//======儒略历与格里历相互转化的函数======
//儒略历转换成格里历
int Ju2Gr(int y,int m,int d);

//格里历转换成儒略历
int  Gr2Ju(int y,int m,int d );

//儒略历转换成标准历
int  Ju2No(int y,int m,int d);

//标准历转换成儒略历
int  No2Ju(int y,int m,int d);

//格里历转换成标准历
int  Gr2No(int y,int m,int d);

//标准历转换成格里历
int  No2Gr(int y,int m,int d);



//星期
int  Day(int y,int m,int d,int calType = 1);

//星座
int  sZod(int m,int d);

#endif
