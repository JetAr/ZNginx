字典树（Trie树）的C语言实现
作者是 在线疯狂 on 五月 16, 2014 在 数据结构.
字典树也称Trie树，是一种哈希树，适用于字符串的查找，存储，统计，排序等操作。

Trie树的C语言实现代码如下：

#define MAX 26    //26个字母
#define SLEN 100   //节点中存储的字符串长度

//Trie结构体定义
struct Trie
{
    //z 每个Trie节点可能有26个next节点。
    struct Trie *next[MAX];
    char s[SLEN];      //节点处存储的字符串
    int isword;         //节点处是否为单词
    char val;           //节点的代表字符
} *root;

//初始化Trie树
struct Trie *init()
{
    struct Trie *root = (struct Trie *)malloc(sizeof(struct Trie));
    int i;
    for (i = 0; i < MAX; i++)
    {
        root -> next[i] = NULL;
    }
    root -> isword = 0;
    root -> val = 0;
    
    //z s 部分需要初始化么？    
    return root;
}

//按照指定路径path 插入字符串 s
void insert(char path[], char s[])
{
    struct Trie *t, *p = root;
    int i, j, n = strlen(path);

    for (i = 0; i < n; i++)
    {
        //z 如果下一处字符为NULL，分配一个trie节点
        if (p -> next[path[i] - 'a'] == NULL)
        {
            t = (struct Trie *)malloc(sizeof(struct Trie));
            for (j = 0; j < MAX; j++)
            {
                t -> next[j] = NULL;
                //z 此处多余了哈，应该放循环外面。
                t -> isword = 0;
            }
            t -> val = path[i];
            p -> next[path[i] - 'a'] = t;
        }
        //z 继续下一个字符
        p = p -> next[path[i] - 'a'];
    }
    
    //z 是一个word
    p -> isword = 1;
    //z 2015-05-18 17:24:39 L.227'23721 T4034645950.K
    //z 拷贝待插入的字符串
    strcpy(p -> s , s);
}

//按照指定路径 path 查找
char *find(char path[], int delflag)
{
    //z 从root开始
    struct Trie *p = root;
    int i = 0, n = strlen(path);
    
    //z 找到对应的path
    while (p && path[i])
    {
        p = p -> next[path[i++] - 'a'];
    }
    
    if (p && p -> isword)
    {
        //z 设置 delflag 是做什么了？
        p -> isword = delflag;
        return p->s;
    }
    return NULL;
}

//删除整棵Trie树
void del(struct Trie *root)
{
    int i;
    //z 如果 root 为NULL，那么直接返回。
    if (!root)
        return;

    for (i = 0; i < MAX; i++)
    {
        if (root->next[i])
            del(root->next[i]);//z 递归调用；一般对tree进行操作，都是这样来完成的。
        free(root->next[i]);//z 释放节点。
    }
}
//z 2015-05-18 17:30:00 L.227'23400 T4035688015.K 大概的实现知道了，原理和应用场景需要进一步了解
