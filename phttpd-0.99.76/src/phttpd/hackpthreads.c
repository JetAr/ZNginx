#include <pthread.h>
#include <errno.h>
#include <sys/mman.h>

#define	NOTOK	-1

#ifdef __sgi
#  ifndef _POSIX1C

/*
 *  If using Proven's pThread implementation, he maps
 *  a different set of file descriptors to the actual ones
 *  so we need to look up the relevant table
 */

extern struct fd_table_entry **fd_table;

void	*mmap(
    void		*addr,
    size_t	len,
    int		prot,
    int		flags,
    int		fd,
    off_t	off)
{
    int	fd_kern;
    void	*memory;

    fd_kern = fd_table[fd]->fd.i;
    if (fd_kern == NOTOK)
    {
        errno = EBADF;
        return((caddr_t) NOTOK);
    }
    memory = (void *) _mmap(addr, len, prot, flags, fd_kern, off);
    return(memory);
}

int	getdents(
    int			fd,
    struct dirent	*buf,
    unsigned		nbyte)
{
    int	fd_kern;
    int	bytesRead;

    fd_kern = fd_table[fd]->fd.i;
    if (fd_kern == NOTOK)
    {
        errno = EBADF;
        return(NOTOK);
    }
    bytesRead = _getdents(fd_kern, buf, nbyte);
    return(bytesRead);
}
#  endif
#endif

