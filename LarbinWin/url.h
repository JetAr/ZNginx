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
    /*url��������,��url��������host,�˿�port��·��file�����Ա����
    *����:ȥ��Э��ͷ��url�� www.google.com/reader/view/stream/
    */
    void parse(char *s);
    /*url��������
    ���� base:��url��Ӧ����һ��url�����ָ�� ��www.google.com/reader/view/stream/ ��Ӧwww.google.com/reader/view/
    	 u����url����һ��url����������ӵ�·�� �� strem/
    */
    void parseWithBase(char *u, url *base);
    /*url ·�����ֹ淶��*/
    bool normalize(char *file);
    /*�ж�url���Ƿ����Э��������http��Э��*/
    bool isProtocol(char *s);
    /*˽�й��캯��*/
    url(char *host, uint port, char *file);

public:
    //���캯��������Ϊ��һ��url�����ָ�������ӵĲ���·��
    url(char *u, int8_t depth, url *base);

    /*���캯��������url��ַ�ַ�����url���*/
    url(char *line, int8_t depth);

    /*�ļ��ж�ȡurl*/
    url(char *line);

    ~url();

    in_addr addr;
    /*��֤url�Ƿ���Ч*/
    bool isValid();
    /*��ӡurl*/
    void print();
    /*��ȡ����*/
    char *getHost()
    {
        return host;
    }
    /*��ȡ�˿ں���*/
    uint getPort()
    {
        return port;
    }
    /*��ȡ·������*/
    char *getFile()
    {
        return file;
    }
    /*��ȡ��Ⱥ���*/
    int8_t getDepth()
    {
        return depth;
    }

    bool initOK(url *from);
    //������һ��url
    url *giveBase();
    //���� url���ַ���ָ�룬give��ʾ��Ҫ���÷�delete�ö���
    char *giveUrl();
    //��url���Ƶ�buf��ȥ
    int writeUrl(char *buf);

    char *serialize();
    //����ָ��url�ַ�����ָ��
    char *getUrl();
    //host hash����
    uint hostHashCode();
    //url hash����
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