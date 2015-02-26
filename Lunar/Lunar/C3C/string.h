#ifndef MYSTRING
#define MYSTRING
//将单位天的纯小数转换成如同5:08的时间格式
char* dToStr(double dv,char* time);
//公元年份
char* yearStr(int v,char* year);
//星期
char* dayStr(int v,char* day);
char* dayENStr(int v,char* day);
//星座
char* szodStr(int v,char* star);
//干支
char* gzStr(int v,char* ganzhi);
//生肖
char* zodStr(int v,char* sx);
//农历月数
char* lunMStr(int v,char* yueshu);
//农历日数
char* lunDStr(int v,char* nongli);
//节气
char* sStr(int v,char* jieqi);
//公历类型
char* calTypeStr(int v,char* instr);
//是否为格里历
char* ifgStr(int v,char* instr);
//朔望
char* syzygyStr(char* syzygyType,char* suowang);
//日月食
char* ecliStr(int v,char* instr);
//二十八宿
char* stars28Str(int n,char* instr);
//月大月小
char* mLStr(int mL,char* instr);

#endif
