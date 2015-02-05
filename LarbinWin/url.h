#ifndef URL_H_
#define URL_H_
#include <stdlib.h>
#include <stdint.h>
#include "types.h"
#include <WinSock2.h>
class url
{
private:
    char *host;
    char *file;
    uint16_t port;
    int8_t depth;
    /*url解析函数,将url解析域名host,端口port和路径file存入成员变量
    *输入:去掉协议头的url： www.google.com/reader/view/stream/
    */
    void parse(char *s);
    /*url解析函数
    输入 base:该url对应的上一级url对象的指针 如www.google.com/reader/view/stream/ 对应www.google.com/reader/view/
    	 u：该url在上一级url基础上新添加的路径 如 strem/
    */
    void parseWithBase(char *u, url *base);
    /*url 路径部分规范化*/
    bool normalize(char *file);
    /*判断url中是否包含协议其他非http的协议*/
    bool isProtocol(char *s);
    /*私有构造函数*/
    url(char *host, uint port, char *file);

public:
    //构造函数，输入为上一级url对象的指针和新添加的部分路径
    url(char *u, int8_t depth, url *base);

    /*构造函数，输入url地址字符串和url深度*/
    url(char *line, int8_t depth);

    /*文件中读取url*/
    url(char *line);

    ~url();

    in_addr addr;
    /*验证url是否有效*/
    bool isValid();
    /*打印url*/
    void print();
    /*获取域名*/
    char *getHost()
    {
        return host;
    }
    /*获取端口函数*/
    uint getPort()
    {
        return port;
    }
    /*获取路径函数*/
    char *getFile()
    {
        return file;
    }
    /*获取深度函数*/
    int8_t getDepth()
    {
        return depth;
    }

    bool initOK(url *from);
    //返回上一级url
    url *giveBase();
    //返回 url的字符串指针，give表示需要调用方delete该对象
    char *giveUrl();
    //将url复制到buf中去
    int writeUrl(char *buf);

    char *serialize();
    //返回指向url字符串的指针
    char *getUrl();
    //host hash函数
    uint hostHashCode();
    //url hash函数
    uint hashCode();


#ifdef URL_TAGS
    /* tag associated to this url */
    uint tag;
#endif // URL_TAGS

#ifdef COOKIES
    /* cookies associated with this page */
    char *cookie;
    void addCookie(char *header);
#else // COOKIES
    inline void addCookie(char *header) {}
#endif // COOKIES
};

#endif