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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <curl/curl.h>

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int main(int argc, char *argv[])
{
    CURL *curl_handle;
    static const char *pagefilename = "page.out";
    FILE *pagefile;

    if(argc < 2 )
    {
        printf("Usage: %s <URL>\n", argv[0]);
        return 1;
    }

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    //z 每个session都获取一个handle么？
    curl_handle = curl_easy_init();

    /* set URL to get here */
    //z 设置url
    curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);

    /* Switch on full protocol/debug output while testing */
    //z 详细输出
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

    /* disable progress meter, set to 0L to enable and disable debug output */
    //z 是否显示进度条
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

    /* send all data to this function  */
    //z 将所有数据给指向到write_data函数
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

    /* open the file */
    //z 打开文件
    pagefile = fopen(pagefilename, "wb");
    if (pagefile)
    {
        /* write the page body to this file handle */
        //z 设置写文件目的地
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

        /* get it! */
        //z 执行
        curl_easy_perform(curl_handle);

        /* close the header file */
        //z 关闭文件
        fclose(pagefile);
    }

    /* cleanup curl stuff */
    //z 清理资源
    curl_easy_cleanup(curl_handle);

    //z 忘记调用 curl_global_cleanup() ？
    
    return 0;
}
