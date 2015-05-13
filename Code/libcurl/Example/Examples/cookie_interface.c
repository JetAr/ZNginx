/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2012, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
//z 2015-05-13 15:03:00 L.232'32220 T1110295240.K
 //z 简单cookie的使用
 /* This example shows usage of simple cookie interface. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include <curl/curl.h>

static void
print_cookies(CURL *curl)
{
    CURLcode res;
    struct curl_slist *cookies;
    struct curl_slist *nc;
    int i;

    printf("Cookies, curl knows:\n");
    //z 获取 cookie slist
    res = curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &cookies);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "Curl curl_easy_getinfo failed: %s\n", curl_easy_strerror(res));
        exit(1);
    }
    nc = cookies, i = 1;
    //z 使用 slist 存储的字符串
    while (nc)
    {
        printf("[%d]: %s\n", i, nc->data);
        nc = nc->next;
        i++;
    }
    if (i == 1)
    {
        printf("(none)\n");
    }
    curl_slist_free_all(cookies);
}

int
main(void)
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl)
    {
        char nline[256];

        curl_easy_setopt(curl, CURLOPT_URL, "http://www.example.com/");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        //z 通过这个开启 cookie 引擎
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* just to start the cookie engine */
        //z 执行，get url 么？
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
            return 1;
        }
        
        //z 输出 cookie
        print_cookies(curl);

        printf("Erasing curl's knowledge of cookies!\n");
        //z 2015-05-13 15:06:55 L.232'31985 T1110403221.K
        //z 不明白。
        curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");

        print_cookies(curl);

        printf("-----------------------------------------------\n"
               "Setting a cookie \"PREF\" via cookie interface:\n");
#ifdef WIN32
#define snprintf _snprintf
#endif
        /* Netscape format cookie */
        snprintf(nline, sizeof(nline), "%s\t%s\t%s\t%s\t%lu\t%s\t%s",
                 ".google.com", "TRUE", "/", "FALSE", (unsigned long)time(NULL) + 31337UL, "PREF", "hello google, i like you very much!");
        res = curl_easy_setopt(curl, CURLOPT_COOKIELIST, nline);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Curl curl_easy_setopt failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        /* HTTP-header style cookie */
        snprintf(nline, sizeof(nline),
                 "Set-Cookie: OLD_PREF=3d141414bf4209321; "
                 "expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/; domain=.google.com");
        res = curl_easy_setopt(curl, CURLOPT_COOKIELIST, nline);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Curl curl_easy_setopt failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        print_cookies(curl);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Curl perform failed: %s\n", curl_easy_strerror(res));
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Curl init failed!\n");
        return 1;
    }

    curl_global_cleanup();
    return 0;
}
