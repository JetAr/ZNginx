/* Buffering read.
   Copyright (C) 1995, 1996, 1997, 1998 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* This is a simple implementation of buffering IO-read functions.  */

#include "config.h"

#include <stdio.h>

#include "wget.h"
#include "rbuf.h"
#include "connect.h"

void
rbuf_initialize (struct rbuf *rbuf, int fd)
{
    rbuf->fd = fd;
    rbuf->buffer_pos = rbuf->buffer;
    rbuf->buffer_left = 0;
}

int
rbuf_initialized_p (struct rbuf *rbuf)
{
    return rbuf->fd != -1;
}

void
rbuf_uninitialize (struct rbuf *rbuf)
{
    rbuf->fd = -1;
}

/* Currently unused -- see RBUF_READCHAR.  */
#if 0
/* Function version of RBUF_READCHAR.  */
int
rbuf_readchar (struct rbuf *rbuf, char *store)
{
    return RBUF_READCHAR (rbuf, store);
}
#endif

/* Like rbuf_readchar(), only don't move the buffer position.  */
//z 从buffer中读取一个字符串，但是不移动buffer的位置
//z 注意，调用该函数可能变更buf中的内容
int
rbuf_peek (struct rbuf *rbuf, char *store)
{
    //z buf 中是否还有剩下的字符串，如果无，那么从fd中读取的。
    if (!rbuf->buffer_left)
    {
        int res;
        rbuf->buffer_pos = rbuf->buffer;
        rbuf->buffer_left = 0;
        res = iread (rbuf->fd, rbuf->buffer, sizeof (rbuf->buffer));
        //z 从 fd 中读取的字节数为0；无更多。
        if (res <= 0)
            return res;
        rbuf->buffer_left = res;
    }
    //z 返回指向的字符串
    *store = *rbuf->buffer_pos;
    return 1;
}

/* Flush RBUF's buffer to WHERE.  Flush MAXSIZE bytes at most.
   Returns the number of bytes actually copied.  If the buffer is
   empty, 0 is returned.  */
int
rbuf_flush (struct rbuf *rbuf, char *where, int maxsize)
{
	//z 如果 buffer_left 为0
    if (!rbuf->buffer_left)
        return 0;
    else
    {
		//z 最大也只出来 maxsize
        int howmuch = MINVAL (rbuf->buffer_left, maxsize);

		//z 拷贝到 where 。
        if (where)
            memcpy (where, rbuf->buffer_pos, howmuch);
		
        rbuf->buffer_left -= howmuch;
        rbuf->buffer_pos += howmuch;
        return howmuch;
    }
}

/* Discard any cached data in RBUF.  */
void
rbuf_discard (struct rbuf *rbuf)
{
    rbuf->buffer_left = 0;
    rbuf->buffer_pos = rbuf->buffer;
}
