﻿#include "stdafx.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include "sterm.h"
#include "ganzhi.h"

#include "mystring.h"
#include "solar.h"

#include "ftvl.h"
//--------节日函数--------//

//y年m月的第n个星期day是几月几号
char* nDayDate(int y,int m,int n,int day,int calType,char* strin)
{
    if(!strin)
    {
        return 0;
    }
    int firstDay=Day(y,m,1,calType);
    int d;
    if(day>=firstDay)
    {
        d=(n-1)*7+day-firstDay+1;
    }
    else
    {
        d=n*7+day-firstDay+1;
    }


    sprintf(strin,"%2d%2d",m,d);
    return strin;
}

//y年m月的倒数第n个星期day是几月几号
char* rnDayDate(int y,int m,int n,int day,int calType,char* strin)
{
    if(!strin)
    {
        return 0;
    }
    int lastDate;
    if(y<12)
    {
        lastDate=D0(y,m+1,1,calType)-D0(y,m,1,calType);
    }
    else
    {
        lastDate=31;
    }
    int lastDay=Day(y,m,lastDate,calType);

    int d;
    if(day<=lastDay)
    {
        d=lastDate-lastDay-(n-1)*7+day;
    }
    else
    {
        d=lastDate-lastDay-n*7+day;
    }


    sprintf(strin,"%2d%2d",m,d);
    return strin;
}

//alert(nDayDate(2003,5,1,6,1));
//alert(rnDayDate(2003,6,1,0,1));

//公历节日
char* sFtvl(int y,int m,int d,int calType,char* strin)
{
    if(!strin)
    {
        return 0;
    }
    char sFtv[132+16][256] =
    {
        "0101元旦",
        "0106主显节",
        "0202世界湿地日",
        "0207国际声援南非日",
        "0210国际气象节",
        "0214情人节",
        "0215中国12亿人口日",
        "0229世界居住条件调查日",
        "0301国际海豹日",
        "0303全国爱耳日",
        "0305学雷锋活动日(中国青年志愿者服务日)",
        "0308国际妇女节",
        "0312植树节",
        "0314国际警察日",
        "0315消费者权益日",
        "0316手拉手情系贫困小伙伴全国统一行动日",
        "0317中国国医节 国际航海日",
        "0318全国科技人才活动日",
        "0321世界森林日 消除种族歧视国际日 世界儿歌日",
        "0322世界水日",
        "0323世界气象日",
        "0324世界防治结核病日",
        "0325全国中小学生安全教育日",
        "0330巴勒斯坦国土日",
        "0401愚人节",
        "0402国际儿童图书日",
        "0407世界卫生日",
        "0412世界航天节",
        "0415非洲自由日",
        "0421全国企业家活动日",
        "0422世界地球日 世界法律日",
        "0423世界图书和版权日",
        "0424亚非新闻工作者日",
        "0425全国预防接种宣传日",
        "0426世界知识产权日",
        "0427联谊城日",
        "0430全国交通安全反思日 ",
        "0501国际劳动节",
        "0503世界新闻自由日",
        "0504五四青年节",
        "0505碘缺乏病防治日",
        "0508世界红十字日 世界微笑日",
        "0512国际护士节",
        "0515国际家庭(咨询)日",
        "0517世界电信日",
        "0518国际博物馆日",
        "0520全国母乳喂养宣传日 全国学生营养日",
        "0525非洲解放日",
        "0526世界向人体条件挑战日",
        "0530“五卅”反对帝国主义运动纪念日",
        "0531世界无烟日",
        "0601国际儿童节",
        "0604国际被侵略无辜受害儿童日",
        "0605世界环境日",
        "0606全国爱眼日",
        "0611中国人口日",
        "0617防治荒漠化和干旱日",
        "0620世界难民日",
        "0622中国儿童慈善活动日",
        "0623国际奥林匹克日 世界手球日 国际SOS儿童村节",
        "0625全国土地日",
        "0626联合国宪章日 国际反毒品日",
        "0627世界糖尿病日",
        "0630世界青年联欢日",
        "0701中国共产党建党日 香港回归纪念日 世界建筑日 亚洲“三十亿人口日”",
        "0702国际体育记者日",
        "0704国际合作日",
        "0707中国人民抗日战争纪念日",
        "0711世界(50亿)人口日",
        "0718国际海洋日",
        "0726世界语(言)创立日",
        "0728第一次世界大战爆发纪念日",
        "0730 非洲妇女日",
        "0801八一建军节",
        "0806国际电影节",
        "0808中国男子节(爸爸节)",
        "0809世界土著人日",
        "0815日本正式宣布无条件投降日",
        "0826全国律师咨询日",
        "0903中国抗日战争胜利纪念日",
        "0908国际扫盲日 国际新闻工作者日",
        "0910中国教师节",
        "09119·11纪念日",
        "0914世界清洁地球日",
        "0917国际臭氧层保护日",
        "0918“九·一八”事变纪念日(中国国耻日)",
        "0920作者的生日 国际爱牙日 世界球茎花卉日",
        "0927世界旅游日",
        "1001国庆节 国际音乐日 国际老人节",
        "1002国际和平与民主自由斗争日",
        "1004世界动物日",
        "1008全国高血压日 世界视觉日 国际左撇子日",
        "1009世界邮政日(万国邮联日)",
        "1010辛亥革命纪念日 世界精神卫生日 世界居室卫生日",
        "1011声援南非政治犯日",
        "1013世界保健日 国际教师节 中国少年先锋队诞辰日 采用格林威治时间为国际标准时间日",
        "1014世界标准日",
        "1015国际盲人节(白手杖节)",
        "1016世界粮食日",
        "1017世界消除贫困日 国际学生节",
        "1022世界传统医药日",
        "1024联合国日 世界发展信息日",
        "1028世界“男性健康日”",
        "1031世界勤俭日 万圣节前夜",
        "1107十月社会主义革命纪念日",
        "1108中国记者日",
        "1109全国消防安全宣传教育日",
        "1110世界青年节",
        "1114世界糖尿病日",
        "1116国际宽容日",
        "1117国际大学生节 国际学生日",
        "1120非洲工业化日 国际儿童日",
        "1121世界问候日 世界电视日",
        "1129国际声援巴勒斯坦人民国际日",
        "1201世界爱滋病日",
        "1202废除一切形式奴役世界日",
        "1203世界残疾人日",
        "1204中国法制宣传日",
        "1205国际经济和社会发展志愿人员日 世界弱能人士日",
        "1207国际民航日",
        "1208国际儿童电视日",
        "1209纪念一二·九运动 世界足球日",
        "1210世界人权日",
        "1211世界防治哮喘日",
        "1212西安事变纪念日",
        "1213南京大屠杀(1937年)纪念日",
        "1215世界强化免疫日",
        "1220澳门回归纪念日",
        "1221国际篮球日",
        "1224平安夜",
        "1225圣诞节",
        "1229国际生物多样性日"
    };

    //某月的第几个星期几的节日
    int len0=132;
    char strDate[8];
    strcpy(sFtv[len0],nDayDate(y,1,1,0,calType,strDate));
    strcat(sFtv[len0],"黑人日");
    strcpy(sFtv[++len0],nDayDate(y,1,1,0,calType,strDate));
    strcat(sFtv[len0],"国际麻风节");
    strcpy(sFtv[++len0],nDayDate(y,3,1,1,calType,strDate));
    strcat(sFtv[len0],"中小学生安全教育日");
    strcpy(sFtv[++len0],nDayDate(y,4,1,3,calType,strDate));
    strcat(sFtv[len0],"秘书节");
    strcpy(sFtv[++len0],nDayDate(y,5,2,0,calType,strDate));
    strcat(sFtv[len0],"国际母亲节 救助贫困母亲日");
    strcpy(sFtv[++len0],nDayDate(y,5,3,0,calType,strDate));
    strcat(sFtv[len0],"全国助残日");
    strcpy(sFtv[++len0],nDayDate(y,5,3,2,calType,strDate));
    strcat(sFtv[len0],"国际牛奶日");
    strcpy(sFtv[++len0],nDayDate(y,6,3,0,calType,strDate));
    strcat(sFtv[len0],"国际父亲节");
    strcpy(sFtv[++len0],nDayDate(y,9,3,2,calType,strDate));
    strcat(sFtv[len0],"国际和平日");
    strcpy(sFtv[++len0],nDayDate(y,9,3,6,calType,strDate));
    strcat(sFtv[len0],"全民国防教育日");
    strcpy(sFtv[++len0],nDayDate(y,9,4,0,calType,strDate));
    strcat(sFtv[len0],"国际聋人节");
    strcpy(sFtv[++len0],nDayDate(y,9,1,0,calType,strDate));
    strcat(sFtv[len0],"世界心脏日 世界海事日");
    strcpy(sFtv[++len0],nDayDate(y,10,1,1,calType,strDate));
    strcat(sFtv[len0],"国际住房日");
    strcpy(sFtv[++len0],nDayDate(y,10,1,3,calType,strDate));
    strcat(sFtv[len0],"国际减轻自然灾害日");
    strcpy(sFtv[++len0],nDayDate(y,11,4,4,calType,strDate));
    strcat(sFtv[len0],"感恩节");
    strcpy(sFtv[++len0],nDayDate(y,12,2,0,calType,strDate));
    strcat(sFtv[len0],"国际儿童电视广播日");




    int i = 0;
    for(i = 0; i < 132+16; i++)
    {
        int y = 0;
        sscanf(sFtv[i],"%d",&y);
        if(y==100*m+d)
        {
            sscanf(sFtv[i],"%d%s",&y,strin);
            break;
        }
    }
    return strin;
}

//农历节日
char* lunFtvl(int lunM,int lunD,char* strin)
{
    if(!strin)
    {
        return 0;
    }
    char lunFtv[84][128]=
    {
        "0101新年",
        "0103天庆节",
        "0105五路财神日",
        "0108江东神诞",
        "0109昊天皇帝诞",
        "0111太均娘娘诞",
        "0113散花灯 哥升节",
        "0115元宵节",
        "0116馄饨节 门神诞",
        "0119丘处机诞",
        "0120女娲补天日 黄道婆祭",
        "0125填仓节",
        "0127天地水三官诞",
        "0201忌鸟节（瑶族）",
        "0202龙头节 太昊伏羲氏祭",
        "0203文昌诞",
        "0208芳春节 插花节",
        "0210彩蛋节",
        "0212花朝节",
        "0215老子诞",
        "0219观音诞",
        "0228寒潮节 岱诞",
        "0303上巳节 踏青节",
        "0305大禹诞",
        "0310撒种节",
        "0315孙膑诞 龙华会",
        "0316蒙恬诞",
        "0318中岳节",
        "0320鲁班诞",
        "0322子孙娘娘诞",
        "0323天后玛祖诞",
        "0328仓颉先师诞",
        "0401清和节",
        "0402公输般日",
        "0408洗佛放生节 牛王诞 跳月节",
        "0410葛洪诞",
        "0411孔子祭",
        "0414吕洞宾诞 菖蒲日",
        "0415钟离权诞 外萨卡佛陀日",
        "0417金花女诞",
        "0418锡伯迁移节",
        "0419浣花日",
        "0426炎帝神农氏诞",
        "0428扁鹊诞",
        "0501女儿节",
        "0504采花节",
        "0505端午节",
        "0511范蠡祭",
        "0513关羽诞",
        "0522曹娥日",
        "0523泼水节（阿昌族）",
        "0529祖娘节",
        "0606天贶节 盘古逝",
        "0612彭祖笺铿诞",
        "0615捕鱼祭",
        "0616爬坡节",
        "0619太阳日 观音日",
        "0623火神诞",
        "0624观莲节 火把节",
        "0707乞巧节",
        "0712地狱开门日",
        "0713轩辕诞",
        "0715中元节",
        "0723诸葛亮诞",
        "0727黄老诞",
        "0801天医节",
        "0803华佗诞",
        "0815中秋节",
        "0818观潮节",
        "0824稻节",
        "0909重阳节 中国老年节(义务助老活动日)",
        "0913钉鞋日",
        "0916伯余诞",
        "0919观音逝",
        "0930采参节",
        "1001送寒衣节 祭祖节",
        "1015下元节 文成公主诞",
        "1016盘古节",
        "1205姊妹饭（苗族）",
        "1208腊八节",
        "1212百福日 蚕花娘娘诞",
        "1223洗灶日",
        "1224小年",
        "1225上帝下界之辰"
        "1230除夕"
    };

    int i = 0;
    for(i = 0; i < 84; i++)
    {
        int y = 0;
        sscanf(lunFtv[i],"%d",&y);
        if(y==(100*lunM+lunD))
        {
            sscanf(lunFtv[i],"%d%s",&y,strin);
            break;
        }
    }
    return strin;
}

//节气节日
char* jqFtvl(int y,int m,int d,int calType,char* instr)
{
    char jqFtv[24][12];
    char strtmp[3];
    char strjq[8];
    for(int i=0; i<24; i++)
    {
        sprintf(strtmp,"%02d",i+1);
        strcpy(jqFtv[i],strtmp);
        strcat(jqFtv[i],sStr(i,strjq));
    }

    char addName[24][32]= {"","","","","","","节",""," 麦饭日","","","","","","","","","","","","","","",""};

    char str[24]; //节气节日
    int thisD0=D0(y,m,d,calType);

    for(int i = 0; i < 24; i++)
    {
        int date = 0;
        sscanf(jqFtv[i],"%d",&date);
        if(floor(S(y,date,1,calType))==thisD0)
        {
            sscanf(jqFtv[i],"%d%s",&date,str);
            strcat(str,addName[i]);
            break;
        }
    }

    //梅雨
    int dG=gan(dGz(y,m,d,0,calType));
    int dZ=zhi(dGz(y,m,d,0,calType));

    int s11=floor(S(y,11,1,calType));
    if(thisD0>=s11&&thisD0<s11+10&&dG==3)
    {
        strcat(instr," 入梅");
    }
    int s13=floor(S(y,13,1,calType));
    if(thisD0>=s13&&thisD0<s13+12&&dZ==8)
    {
        strcat(instr," 出梅");
    }

    //三伏
    int s12=floor(S(y,12,1,calType));
    int s15=floor(S(y,15,1,calType));
    int n=(dG-7)%10+1;
    if(n<=0)
        n+=10;
    int firsrD0=thisD0-n+1;
    if(firsrD0>=s12+20&&firsrD0<s12+30)
    {
        strcat(instr, "初伏第");
        char strtmp[4];
        sprintf(strtmp,"%d",n);
        strcat(instr,strtmp);
        strcat(instr,"天");
    }
    if(firsrD0>=s15&&firsrD0<s15+10)
    {
        strcat(instr, "末伏第");
        char strtmp[4];
        sprintf(strtmp,"%d",n);
        strcat(instr,strtmp);
        strcat(instr,"天");
    }
    else
    {
        if(firsrD0>=s12+30&&firsrD0<s12+40)
        {
            strcat(instr, "中伏第");
            char strtmp[4];
            sprintf(strtmp,"%d",n);
            strcat(instr,strtmp);
            strcat(instr,"天");

        }
        if(firsrD0>=s12+40&&firsrD0<s12+50)
        {
            strcat(instr, "中伏第");
            char strtmp[4];
            sprintf(strtmp,"%d",n+10);
            strcat(instr,strtmp);
            strcat(instr,"天");
        }
    }

    //九九
    int s24=floor(S(y,24,1,calType));
    int s_24=floor(S(y-1,24,1,calType));
    int d1=thisD0-s24;
    int d2=thisD0-s_24+D0(y-1,12,31,calType)-D0(y-1,1,0,calType);
    int w = 0;
    int v = 0;
    if(d1>=0||d2<=80)
    {
        if(m==12)
        {
            w=1;
            v=d1+1;
            if(v>9)
            {
                w+=1;
                v-=9;
            }
        }
        else
        {
            int w=floor((float)(d2/9))+1;
            int v=round(rem(d2,9))+1;
        }
        strcat(instr," ");
        char strtmp[12];
        lunDStr(w,strtmp);
        strtmp[4] = 0;
        strcat(instr,strtmp);
        strcat(instr,"九第");
        sprintf(strtmp,"%d",v);
        strcat(instr,strtmp);
        strcat(instr,"天");
    }

    return instr;
}


//名人纪念日
char* manFtvl(int m,int d,char* strin)
{
    if(!strin)
    {
        return 0;
    }
    char manFtv[77][128]=
    {
        "0104雅各布·格林诞辰",
        "0108周恩来逝世纪念日",
        "0106圣女贞德诞辰",
        "0112杰克·伦敦诞辰",
        "0115莫里哀诞辰",
        "0117富兰克林诞辰",
        "0119瓦特诞辰",
        "0122培根诞辰",
        "0123郎之万诞辰",
        "0127莫扎特诞辰",
        "0129罗曼·罗兰诞辰",
        "0130甘地诞辰",
        "0131舒柏特诞辰",
        "0203门德尔松诞辰",
        "0207门捷列夫诞辰",
        "0211爱迪生诞辰，狄更斯诞辰",
        "0212林肯，达尔文诞辰",
        "0217布鲁诺诞辰",
        "0218伏打诞辰",
        "0219哥白尼诞辰",
        "0222赫兹，叔本华，华盛顿诞辰",
        "0226雨果诞辰",
        "0302斯美塔那诞辰",
        "0304白求恩诞辰",
        "0305周恩来诞辰",
        "0306布朗宁，米开朗琪罗诞辰",
        "0307竺可桢诞辰",
        "0314爱因斯坦诞辰",
        "0321巴赫，穆索尔斯基诞辰",
        "0322贺龙诞辰",
        "0328高尔基诞辰",
        "0401海顿，果戈理诞辰",
        "0415达·芬奇诞辰",
        "0416卓别林诞辰",
        "0420祖冲之诞辰",
        "0422列宁，康德，奥本海默诞辰",
        "0423普朗克，莎士比亚诞辰",
        "0430高斯诞辰",
        "0505马克思诞辰",
        "0507柴可夫斯基，泰戈尔诞辰",
        "0511冼星海诞辰",
        "0511李比希诞辰",
        "0520巴尔扎克诞辰",
        "0522瓦格纳诞辰",
        "0531惠特曼诞辰",
        "0601杜威诞辰",
        "0602哈代诞辰",
        "0608舒曼诞辰",
        "0715伦勃朗诞辰",
        "0805阿贝尔诞辰",
        "0808狄拉克诞辰",
        "0826陈毅诞辰",
        "0828歌德诞辰",
        "0909毛泽东逝世纪念日",
        "0925鲁迅诞辰",
        "0926巴甫洛夫诞辰",
        "0928孔子诞辰",
        "0929奥斯特洛夫斯基诞辰",
        "1011伯辽兹诞辰",
        "1021诺贝尔诞辰",
        "1022李斯特诞辰",
        "1026伽罗瓦诞辰",
        "1029李大钊诞辰",
        "1007居里夫人诞辰",
        "1108哈雷诞辰",
        "1112孙中山诞辰",
        "1124刘少奇诞辰",
        "1128恩格斯诞辰",
        "1201朱德诞辰",
        "1205海森堡诞辰",
        "1211玻恩诞辰",
        "1213海涅诞辰",
        "1216贝多芬诞辰",
        "1221斯大林诞辰",
        "1225牛顿诞辰",
        "1226毛泽东诞辰",
        "1229阿·托尔斯泰诞辰"
    };



    int i = 0;
    for(i = 0; i < 77; i++)
    {
        int y = 0;
        sscanf(manFtv[i],"%d",&y);
        if(y==(100*m+d))
        {
            sscanf(manFtv[i],"%d%s",&y,strin);
            break;
        }
    }


    return strin;
}

