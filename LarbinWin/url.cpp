#include <WinSock2.h>
#include <assert.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include "types.h"
#include "url.h"
#include "debug.h"
#include "text.h"

#ifdef COOKIES
#define initCookie() cookie=NULL
#else // COOKIES
#define initCookie() ((void) 0)
#endif // COOKIES
//静态函数的用法
static uint siteHashCode(char *host)
{
    uint h =0 ;
    uint i = 0;
    while (host[i] != 0)
    {
        h = 37*h + host[i];
        i++;
    }
    return h%namedSiteListSize;
}

/*将十六进制的字符转换为整形数字
* 若字符不是十六进制数返回 -1*/
static int int_of_hexa(char c)
{
    if(c >= '0' && c <= '9')
        return (c - '0');
    else if(c >= 'A' && c <= 'Z')
        return(c - 'A' + 10);
    else if(c >= 'a' && c <= 'z')
        return(c - 'a' + 10);
    else
        return -1;
}
/*url 规范化函数，仅针对修饰符和RFC1738编码做了处理
如果处理正确返回 true, 错误返回false*/
bool fileNormalize (char *file)
{
    int i = 0;
    while(file[i] != 0 && file[i] != '#')   //删除Fragment(#)
    {
        //删除多余的 //  /./   /../   /.       /..点修饰符
        if(file[i] == '/')
        {
            int j = i+1, k = j;
            while(file[j] == '.' || file[j] == '/') j++;
            while(file[j] !=0)
            {
                file[k] = file[j];
                j++, k++;
            }
            file[k] = 0;
            i++;
        }
        else if(file[i] == '%')    //处理RFC1738编码"%20"的情况，转换为实际字符
        {
            int v1 = int_of_hexa(file[i+1]);
            int v2 = int_of_hexa(file[i+1]);
            if(v1 < 0 || v2 < 0) return false;
            char c = v1 * 16 + v2;
            if(isgraph(c))
            {
                file[i] = c;
                int j = i+3;
                while (file[j] != 0)
                {
                    file[j-2] = file[j];
                    j++;
                }
                file[j-2] = 0;
            }
            else if (c == ' ' || c=='/')   //如果c是空格或者/则不对其进行转码
                i+=3;
            else  //错误url
                return false;
        }
        else
        {
            i ++;
        }



        //	if(file[i+1] == '.' && file[i+2] == '/')
        //	{
        //		/*删除多余的/./*/
        //		int j = i+3;
        //		while(file[j] != 0)
        //		{
        //			file[j-2] == file[j];
        //			j++;
        //		}
        //		file[j-1] = 0;
        //	}
        //
        //}
    }
    file[i] = 0;
    return true;

}



/**************************************/
/* definition of methods of class url */
/**************************************/

/* Constructor : Parses an url  url解析构造函数*/
url::url(char *u, int8_t depth, url *base)
{
    newUrl();
    this->depth = depth;
    host = NULL;
    port = 80;
    file = NULL;
    initCookie();
#ifdef URL_TAGS
    tag = 0;
#endif
    if(startWith("http://",u))
    {
        //将url解析为域名+端口+路径格式
        parse(u + 7);
        //格式化路径
        if(file != NULL && !normalize(file))
        {
            delete [] file;
            file = NULL;//避免野指针
            delete [] host;
            host = NULL;
        }
    }
    else if(base != NULL)
    {
        if(startWith("http:",u))
        {
            parseWithBase(u+5, base);
        }
        else if(isProtocol(u))
        {
            //是否包含http的协议头
        }
        else
        {
            parseWithBase(u, base);
        }
    }
}

//构造函数，用于由的url字符串构造url对象
url::url(char *line, int8_t depth)
{
    newUrl();
    this->depth = depth;
    host = NULL;
    port = 80;
    file = NULL;
    initCookie();
    int i = 0;
#ifdef URL_TAGS  //还不清楚作用
    tag = 0;
    while (line[i] >= '0' && line[i] <= '9')
    {
        tag = 10*tag + line[i] - '0';
        i++;
    }
    i++;
#endif // URL_TAGS
    if(startWith("http://",line + i))
    {
        //将url解析为域名+端口+路径格式
        parse(line + 7 + i);
        //格式化路径
        if(file != NULL && !normalize(file))
        {
            delete [] file;
            file = NULL;//避免野指针
            delete [] host;
            host = NULL;
        }
    }
}

//从文件中读取URL
url::url(char * line)
{
    newUrl();
    int i = 0;
    depth = 0;
    while(line[i] >= '0' && line[i] <='9')
    {
        depth = 10*depth + line[i] - '0';  //应该有条规则，待验证
        i++;
    }
#ifdef URL_TAGS
    // read tag
    tag = 0;
    i++;
    while (line[i] >= '0' && line[i] <= '9')
    {
        tag = 10*tag + line[i] - '0';
        i++;
    }
#endif // URL_TAGS
    int deb = ++i;
    //read host
    while(line[i] != ':')
        i++;
    line[i] = 0;
    host = newString(line+deb);
    i++;
    // read port
    port = 0;
    while (line[i] >= '0' && line[i] <= '9')
    {
        port = 10*port + line[i] - '0';
        i++;
    }
#ifndef COOKIES
    // Read file name
    file = newString(line+i);
#else // COOKIES
    char *cpos = strchr(line+i, ' ');
    if (cpos == NULL)
    {
        cookie = NULL;
    }
    else
    {
        *cpos = 0;
        // read cookies
        cookie = new char[maxCookieSize];
        strcpy(cookie, cpos+1);
    }
    // Read file name
    file = newString(line+i);
#endif // COOKIES
}
//由基本url构建
url::url(char *host, uint port, char *file)
{
    newUrl();
    initCookie();
    this->host = host;
    this->port = port;
    this->file = file;
}

//析构函数
url::~url()
{
    delUrl();
    /*delete [] file;
    delete [] host;*/
#ifdef COOKIES
    delete [] cookie;
#endif // COOKIES
}

//is it a valid url?
bool url::isValid()
{
    if(host == NULL) return false;
    int lh = strlen(host);
    printf("%i  %i",lh + strlen(file) + 18, maxSiteSize );
    return (file!=NULL && lh < maxSiteSize
            && lh + strlen(file) + 18 < maxUrlSize);
}

bool url::initOK(url *from)
{
    //need class NameSite
    return true;
}
//给出url的上一级base 的url
url *url::giveBase()
{
    int i = strlen(file);
    assert( file[0] == '/');
    while(file[i] != '/')
        i--;
    char *newFile = new char(i+2);
    memcpy(newFile,file,i+1);
    newFile[i+1] = 0;
    return new url(newString(host),port,newFile);
}
/*返回url的字符串，需要调用方delete url的字符串*/
char *url::giveUrl()
{
    char *tmp;
    int i = strlen(file);
    int j = strlen(host);
    tmp = new char[18+i+j];

    strcpy(tmp,"http://");
    strcpy(tmp+7,host);
    j+=7;
    if (port != 80)
    {
        j += sprintf(tmp + j, ":%u", port);
    }
    // Copy file name
    while (i >= 0)
    {
        tmp [j+i] = file[i];
        i--;
    }
    return tmp;
}
/** write the url in a buffer
* buf must be at least of size maxUrlSize
* returns the size of what has been written (not including '\0')
*/
int url::writeUrl(char *buf)
{
    if(port == 80)
        return sprintf(buf,"http://%s%s",host,file);
    else
        return sprintf(buf,"http://%s:%u%s",host,port,file);
}



void url::print()
{
    printf("http://%s:%u%s\n",host,port,file);
}

//序列化url 提供给 persistent fifo
char *url::serialize()
{
    // this buffer is protected by the lock of PersFifo
    static char statstr[maxUrlSize+40+maxCookieSize];
    int pos = sprintf(statstr,"%u ", depth);
#ifdef URL_TAGS
    pos += sprintf(statstr+pos, "%u ", tag);
#endif // URL_TAGS
    pos += sprintf(statstr+pos, "%s:%u%s", host, port, file);
#ifdef COOKIES
    if (cookie != NULL)
    {
        pos += sprintf(statstr+pos, " %s", cookie);
    }
#endif // COOKIES
    statstr[pos] = '\n';
    statstr[pos+1] = 0;
    return statstr;

}

char *url::getUrl()
{
    static char statstr[maxUrlSize+40];  //通过设置静态数组的方式返回静态数组指针，可以借鉴 此变量对多个对象来说只有一个吗？
    sprintf(statstr, "http://%s:%u%s", host, port, file);
    return statstr;
}

uint url::hostHashCode()
{

    return siteHashCode(host);  //为什么将其作伪静态函数放在外边？别的地方也有引用吗？
}

/* return a hashcode for this url */
uint url::hashCode ()
{
    unsigned int h=port;
    unsigned int i=0;
    while (host[i] != 0)
    {
        h = 31*h + host[i];
        i++;
    }
    i=0;
    while (file[i] != 0)
    {
        h = 31*h + file[i];
        i++;
    }
    return h % hashSize;
}

bool url::isProtocol(char *s)
{
    uint i = 0;
    while(isalnum(s[i])) i++;
    return s[i] == ':';

}
void url::parseWithBase(char *u, url *base)
{
    if(u[0] == '/')
    {
        file = newString(u);
    }
    else
    {
        uint lenb = strlen(base->file);
        char *tmp = new char (lenb + strlen(u) +1);
        memcpy(tmp, base->file,lenb);
        strcpy(tmp+lenb,u);
        file = tmp;
    }
    if(!normalize(file))
    {
        delete [] file;
        file = NULL;
        return;
    }
    host = base ->host;
    port = base->port;
}
/**

*/
bool url::normalize(char *file)
{
    return fileNormalize(file);
}

void url::parse(char *arg)
{
    int deb = 0, fin = deb;

    //get host
    while( arg[fin] != '/' && arg[fin] != ':' && arg[fin] != 0)
    {
        fin ++;
    }
    if(fin == 0) return;
    host = new char[fin+1];

    for(int i = 0; i < fin; i++)
    {
        host[i] = arg[i];
    }
    host[fin] = 0;

    //get port
    if(arg[fin] == ':')
    {
        fin ++;
        port = 0;
        while(arg[fin] >='0' && arg[fin] <='9')
        {
            port = port*10 +(arg[fin] - '0');
            fin ++;
        }
    }

    //获取路径 filename
    if(arg[fin] != '/')
        file = newString("/");
    else
        file = newString(arg+fin);

}

#ifdef COOKIES
#define addToCookie(s) len = strlen(cookie); \
	strncpy(cookie+len, s, maxCookieSize-len); \
	cookie[maxCookieSize-1] = 0;

/* see if a header contain a new cookie */
void url::addCookie(char *header)
{
    if (startWithIgnoreCase("set-cookie: ", header))
    {
        char *pos = strchr(header+12, ';');
        if (pos != NULL)
        {
            int len;
            if (cookie == NULL)
            {
                cookie = new char[maxCookieSize];
                cookie[0] = 0;
            }
            else
            {
                addToCookie("; ");
            }
            *pos = 0;
            addToCookie(header+12);
            *pos = ';';
        }
    }
}
#endif // COOKIES
