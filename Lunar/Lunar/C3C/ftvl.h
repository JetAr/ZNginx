#ifndef FTVL
#define FTVL
char* nDayDate(int y,int m,int n,int day,int calType,char* strin);

//y年m月的倒数第n个星期day是几月几号
char* rnDayDate(int y,int m,int n,int day,int calType,char* strin);

//alert(nDayDate(2003,5,1,6,1));
//alert(rnDayDate(2003,6,1,0,1));

//公历节日
char* sFtvl(int y,int m,int d,int calType,char* strin);

//农历节日
char* lunFtvl(int lunM,int lunD,char* strin);

//节气节日
char* jqFtvl(int y,int m,int d,int calType,char* instr);

//名人纪念日
char* manFtvl(int m,int d,char* strin);


#endif
