/* grep.c - main driver file for grep.
	Copyright (C) 1992 Free Software Foundation, Inc.

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2, or (at your option)
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

	Written July 1992 by Mike Haertel.  */

/*
	Ported to Win32 by Ahmad (Sami) Abualsamid 1997

	Win32 port modified by Tim Charron Nov. 1998 (tcharron@interlog.com)
	 - Addition of "-S" flag to allow subdirectories to be searched.
	 - New "Makefile.watcom" makefile for use with Watcom C/C++ 11.0 compiler
	 - Bug fix.  The original version was displaying the wrong filename when
		wildcards were used!
	 - Directories are listed to stderr as searched if -S is used
	 - Added grep.txt file outlining syntax of grep
	 - Added "-?" option to display help summary
	Full zip file with source available from http://www.interlog.com/~tcharron/grep.html
*/

#include <errno.h>
#include <stdio.h>
#define STDC_HEADERS
#ifndef errno
extern int errno;
#endif

#include <io.h>

#ifdef STDC_HEADERS
#include <stdlib.h>
#else
#include <sys/types.h>
extern char *malloc(), *realloc();
extern void free();
#endif

#if defined(STDC_HEADERS) || defined(HAVE_STRING_H)
#include <string.h>
#ifdef NEED_MEMORY_H
#include <memory.h>
#endif
#else
#include <strings.h>
#ifdef __STDC__
extern void *memchr();
#else
extern char *memchr();
#endif
#define strrchr rindex
#endif

#ifdef HAVE_UNISTD_H
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#else
#define O_RDONLY 0
extern int open(), read(), close();
#endif

#include "getpagesize.h"
#include "grep.h"
#include "getopt.h"
#include <windows.h>

#undef MAX
#define MAX(A,B) ((A) > (B) ? (A) : (B))

/* Provide missing ANSI features if necessary. */

#ifndef HAVE_STRERROR
extern int sys_nerr;
extern char *sys_errlist[];
//z extern char **sys_errlist;
#define strerror(E) ((E) < sys_nerr ? sys_errlist[(E)] : "bogus error number")
#endif

/* Define flags declared in grep.h. */
char *matcher;
int match_icase;
int match_words;
int match_lines;

/* Functions we'll use to search. */
static void (*compile)();
static char *(*execute)();

/* For error messages. */
static char *prog;
static char *filename;
static int errseen;

static int globalsubdirs=0;

/* Print a message and possibly an error string.  Remember
	that something awful happened. */
static void
error(mesg, errnum)
#ifdef __STDC__
const
#endif
char *mesg;
int errnum;
{
    if ( errnum )
        fprintf(stderr, "%s: %s: %s\n", prog, mesg, strerror(errnum));
    else
        fprintf(stderr, "%s: %s\n", prog, mesg);
    errseen = 1;
}

/* Like error(), but die horribly after printing. */
void
fatal(mesg, errnum)
#ifdef __STDC__
const
#endif
char *mesg;
int errnum;
{
    error(mesg, errnum);
    exit(2);
}

/* Interface to handle errors and fix library lossage. */
char *
xmalloc(size)
size_t size;
{
    char *result;

    result = malloc(size);
    if ( size && !result )
        fatal("memory exhausted", 0);
    return (result);
}

/* Interface to handle errors and fix some library lossage. */
char *
xrealloc(ptr, size)
char *ptr;
size_t size;
{
    char *result;

    if ( ptr )
        result = realloc(ptr, size);
    else
        result = malloc(size);
    if ( size && !result )
        fatal("memory exhausted", 0);
    return (result);
}

#if !defined(HAVE_VALLOC)
#define valloc malloc
#else
#ifdef __STDC__
extern void *valloc(size_t);
#else
extern char *valloc();
#endif
#endif

/* Hairy buffering mechanism for grep.  The intent is to keep
	all reads aligned on a page boundary and multiples of the
	page size. */

static char *buffer;										 /* Base of buffer. */
static size_t bufsalloc;								 /* Allocated size of buffer save region. */
static size_t bufalloc;									 /* Total buffer size. */
static int bufdesc;										 /* File descriptor. */
static char *bufbeg;										 /* Beginning of user-visible stuff. */
static char *buflim;										 /* Limit of user-visible stuff. */

#if defined(HAVE_WORKING_MMAP)
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

static int bufmapped;									 /* True for ordinary files. */
static struct stat bufstat;							 /* From fstat(). */
static off_t bufoffset;									 /* What read() normally remembers. */
#endif

/* Reset the buffer for a new file.  Initialize
	on the first time through. */
void
reset(fd)
int fd;
{
    static int initialized;

    if ( !initialized )
    {
        initialized = 1;
#ifndef BUFSALLOC
        bufsalloc = MAX(8192, getpagesize());
#else
        bufsalloc = BUFSALLOC;
#endif
        bufalloc = 5 * bufsalloc;
        /* The 1 byte of overflow is a kludge for dfaexec(), which
        inserts a sentinel newline at the end of the buffer
        being searched.  There's gotta be a better way... */
        buffer = valloc(bufalloc + 1);
        if ( !buffer )
            fatal("memory exhausted", 0);
        bufbeg = buffer;
        buflim = buffer;
    }
    bufdesc = fd;
#if defined(HAVE_WORKING_MMAP)
    if ( fstat(fd, &bufstat) < 0 || !S_ISREG(bufstat.st_mode) )
        bufmapped = 0;
    else
    {
        bufmapped = 1;
        bufoffset = lseek(fd, 0, 1);
    }
#endif
}

/* Read new stuff into the buffer, saving the specified
	amount of old stuff.  When we're done, 'bufbeg' points
	to the beginning of the buffer contents, and 'buflim'
	points just after the end.  Return count of new stuff. */
static int
fillbuf(save)
size_t save;
{
    char *nbuffer, *dp, *sp;
    int cc;
#if defined(HAVE_WORKING_MMAP)
    caddr_t maddr;
#endif
    static int pagesize;

    if ( pagesize == 0 && (pagesize = getpagesize()) == 0 )
        abort();

    if ( save > bufsalloc )
    {
        while ( save > bufsalloc )
            bufsalloc *= 2;
        bufalloc = 5 * bufsalloc;
        nbuffer = valloc(bufalloc + 1);
        if ( !nbuffer )
            fatal("memory exhausted", 0);
    }
    else
        nbuffer = buffer;

    sp = buflim - save;
    dp = nbuffer + bufsalloc - save;
    bufbeg = dp;
    while ( save-- )
        *dp++ = *sp++;

    /* We may have allocated a new, larger buffer.  Since
    	there is no portable vfree(), we just have to forget
    	about the old one.  Sorry. */
    buffer = nbuffer;

#if defined(HAVE_WORKING_MMAP)
    if ( bufmapped && bufoffset % pagesize == 0
            && bufstat.st_size - bufoffset >= bufalloc - bufsalloc )
    {
        maddr = buffer + bufsalloc;
        maddr = mmap(maddr, bufalloc - bufsalloc, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_FIXED, bufdesc, bufoffset);
        if ( maddr == (caddr_t) -1 )
        {
            fprintf(stderr, "%s: warning: %s: %s\n", filename,
                    strerror(errno));
            goto tryread;
        }
#if 0
        /* You might thing this (or MADV_WILLNEED) would help,
        but it doesn't, at least not on a Sun running 4.1.
        In fact, it actually slows us down about 30%! */
        madvise(maddr, bufalloc - bufsalloc, MADV_SEQUENTIAL);
#endif
        cc = bufalloc - bufsalloc;
        bufoffset += cc;
    }
    else
    {
tryread:
        /* We come here when we're not going to use mmap() any more.
        Note that we need to synchronize the file offset the
        first time through. */
        if ( bufmapped )
        {
            bufmapped = 0;
            lseek(bufdesc, bufoffset, 0);
        }
        cc = read(bufdesc, buffer + bufsalloc, bufalloc - bufsalloc);
    }
#else
    cc = read(bufdesc, buffer + bufsalloc, bufalloc - bufsalloc);
#endif
    if ( cc > 0 )
        buflim = buffer + bufsalloc + cc;
    else
        buflim = buffer + bufsalloc;
    return (cc);
}

/* Flags controlling the style of output. */
static int out_quiet;									 /* Suppress all normal output. */
static int out_invert;									 /* Print nonmatching stuff. */
static int out_file;										 /* Print filenames. */
static int out_line;										 /* Print line numbers. */
static int out_byte;										 /* Print byte offsets. */
static int out_before;									 /* Lines of leading context. */
static int out_after;									 /* Lines of trailing context. */

/* Internal variables to keep track of byte count, context, etc. */
static size_t totalcc;									 /* Total character count before bufbeg. */
static char *lastnl;										 /* Pointer after last newline counted. */
static char *lastout;									 /* Pointer after last character output;
												 NULL if no character has been output
												 or if it's conceptually before bufbeg. */
static size_t totalnl;									 /* Total newline count before lastnl. */
static int pending;										 /* Pending lines of output. */
static int list_files;

static void
nlscan(lim)
char *lim;
{
    char *beg;

    for ( beg = lastnl; beg < lim; ++beg )
        if ( *beg == '\n' )
            ++totalnl;
    lastnl = beg;
}

static void
prline(beg, lim, sep, NAME)
char *beg;
char *lim;
char sep;
char * NAME;
{
    if ( out_file )
    {
        printf("%s%c", NAME, sep);
    }

    if ( out_line )
    {
        nlscan(beg);
        printf("%d%c", ++totalnl, sep);
        lastnl = lim;
    }
    if ( out_byte )
        printf("%lu%c", totalcc + (beg - bufbeg), sep);
    fwrite(beg, 1, lim - beg, stdout);
    if ( ferror(stdout) )
        error("writing output", errno);
    lastout = lim;
}

/* Print pending lines of trailing context prior to LIM. */
static void
prpending(lim, NAME)
char *lim;
char *NAME;
{
    char *nl;

    if ( !lastout )
        lastout = bufbeg;
    while ( pending > 0 && lastout < lim )
    {
        --pending;
        if ( (nl = memchr(lastout, '\n', lim - lastout)) != 0 )
            ++nl;
        else
            nl	= lim;
        prline(lastout, nl, '-', NAME);
    }
}

/* Print the lines between BEG and LIM.  Deal with context crap.
	If NLINESP is non-null, store a count of lines between BEG and LIM. */
static void
prtext(beg, lim, nlinesp, NAME)
char *beg;
char *lim;
int *nlinesp;
char * NAME;
{
    static int used;										 /* avoid printing "--" before any output */
    char *bp, *p, *nl;
    int i, n;

    if ( !out_quiet && pending > 0 )
        prpending(beg, NAME);

    p = beg;

    if ( !out_quiet )
    {
        /* Deal with leading context crap. */

        bp = lastout ? lastout : bufbeg;
        for ( i = 0; i < out_before; ++i )
            if ( p > bp )
                do
                    --p;
                while ( p > bp && p[-1] != '\n' );

        /* We only print the "--" separator if our output is
        discontiguous from the last output in the file. */
        if ( (out_before || out_after) && used && p != lastout )
            puts("+---------+---------+---------+---------+---------+---------+---------+---------+");

        while ( p < beg )
        {
            nl = memchr(p, '\n', beg - p);
            prline(p, nl + 1, '-', NAME);
            p = nl + 1;
        }
    }

    if ( nlinesp )
    {
        /* Caller wants a line count. */
        for ( n = 0; p < lim; ++n )
        {
            if ( (nl = memchr(p, '\n', lim - p)) != 0 )
                ++nl;
            else
                nl	= lim;
            if ( !out_quiet )
                prline(p, nl, ':', NAME);
            p = nl;
        }
        *nlinesp = n;
    }
    else if ( !out_quiet )
        prline(beg, lim, ':', NAME);

    pending = out_after;
    used = 1;
}

/* Scan the specified portion of the buffer, matching lines (or
	between matching lines if OUT_INVERT is true).  Return a count of
	lines printed. */
static int
grepbuf(char *beg,char *lim,char *NAME)
{
    int nlines, n;
    register char *p, *b;
    char *endp;

    nlines = 0;
    p = beg;
    while ( (b = (*execute)(p, lim - p, &endp)) != 0 )
    {
        /* Avoid matching the empty line at the end of the buffer. */
        if ( b == lim && ((b > beg && b[-1] == '\n') || b == beg) )
            break;
        if ( !out_invert )
        {
            prtext(b, endp, (int *) 0, NAME);
            nlines += 1;
        }
        else if ( p < b )
        {
            prtext(p, b, &n, NAME);
            nlines += n;
        }
        p = endp;
    }
    if ( out_invert && p < lim )
    {
        prtext(p, lim, &n, NAME);
        nlines += n;
    }
    return (nlines);
}

/* Search a given file.  Return a count of lines printed. */
static int
grep(int fd,char *NAME)
{
    int nlines, i;
    size_t residue, save;
    char *beg, *lim;

    reset(fd);

    totalcc = 0;
    lastout = 0;
    totalnl = 0;
    pending = 0;

    nlines = 0;
    residue = 0;
    save = 0;
    for ( ;; )
    {
        if ( fillbuf(save) < 0 )
        {
            error(filename, errno);
            return (nlines);
        }
        lastnl = bufbeg;
        if ( lastout )
            lastout = bufbeg;
        if ( (size_t) (buflim - bufbeg) == save )
            break;
        beg = bufbeg + save - residue;
        for ( lim = buflim; lim > beg && lim[-1] != '\n'; --lim )  ;	// notice the ;
        residue = buflim - lim;
        if ( beg < lim )
        {
            nlines += grepbuf(beg, lim,NAME);
            if ( pending )
            {
                prpending(lim, NAME);
            }
        }
        i = 0;
        beg = lim;
        while ( i < out_before && beg > bufbeg && beg != lastout )
        {
            ++i;
            do
                --beg;
            while ( beg > bufbeg && beg[-1] != '\n' );
        }
        if ( beg != lastout )
            lastout = 0;
        save = residue + lim - beg;
        totalcc += buflim - bufbeg - save;
        if ( out_line )
            nlscan(beg);
    }
    if ( residue )
    {
        nlines += grepbuf(bufbeg + save - residue, buflim,NAME);
        if ( pending )
        {
            prpending(buflim, NAME);
        }
    }
    return (nlines);
}

static char version[] = "GNU grep version 2.0d";

#define USAGE \
  "usage: %s [-[[AB] ]<num>] [-[CEFGLSVbchilnqsvwx?]] [-[ef]] <expr> [<files...>]\n" \
     "Win32 port by Tim Charron (http://www.interlog.com/~tcharron/grep.html)\n"

static void
usage()
{
    fprintf(stderr, USAGE, prog);
    exit(2);
}

/* Go through the matchers vector and look for the specified matcher.
	If we find it, install it in compile and execute, and return 1.  */
int
setmatcher(name)
char *name;
{
    int i;

    for ( i = 0; matchers[i].name; ++i )
        if ( strcmp(name, matchers[i].name) == 0 )
        {
            compile = matchers[i].compile;
            execute = matchers[i].execute;
            return (1);
        }
    return (0);
}

void help()
{
    printf("%s\n\n",version);
    printf("SYNOPOSIS\n");
    printf("grep [-[[AB] ]<num>] [-[CEFGLSVbchilnqsvwx?]] [-[ef]] <expr> [<files...>]\n\n");
    printf("DESCRIPTION\n\n");
    printf("Grep searches the named input files (or standard input if no files are named, or\n");
    printf("the file name - is given) for lines containing a match to the given pattern.\n");
    printf("By default, grep prints the matching lines.  There are three major variants of\n\n");
    printf("grep, controlled by the following options.\n");
    printf("-G Interpret pattern as a basic regular expression (see below).  This is\n");
    printf("   the default.\n");
    printf("-E Interpret pattern as an extended regular expression (see below).\n");
    printf("-F Interpret pattern as a list of fixed strings, separated by newlines,\n");
    printf("   any of which is to be matched.\n\n");
    printf("In addition, two variant programs egrep and fgrep are available.  Egrep is\n");
    printf("similiar (but not identical) to \"grep -E\", and is compatible with the\n");
    printf("historical Unix egrep.  Fgrep is the same as \"grep -F\".\n\n");
    printf("All variants of grep understand the following options:\n\n");
    printf("-num Matches will be printed with num lines of leading and trailing\n");
    printf("    context.  However, grep will never print any given line more than once.\n");
    printf("-A  \"num\" Print num lines of trailing context after matching lines.\n");
    printf("-B  \"num\" Print num lines of leading context before matching lines.\n");
    printf("-C  Equivalent to -2.\n");
    printf("-S  Search subdirectories.\n");
    printf("-V  Print the version number of grep to standard error.  This version\n");
    printf("    number should be included in all bug reports (see below).\n");
    printf("-b  Print the byte offset within the input file before each line of output.\n");
    printf("-c  Suppress normal output; instead print a count of matching lines for\n");
    printf("    each input file.  With the -v option (see below), count non-matching\n");
    printf("    lines.\n");
    printf("-e  \"pattern\" Use pattern as the pattern; useful to protect patterns\n");
    printf("    beginning with -.\n");
    printf("-f  \"file\" Obtain the pattern from file.\n");
    printf("-h  Suppress the prefixing of filenames on output when multiple files are\n");
    printf("    searched.\n");
    printf("-i  Ignore case distinctions in both the pattern and the input files.\n");
    printf("-L  Suppress normal output; instead print the name of each input file from\n");
    printf("    which no output would normally have been printed.\n");
    printf("-l  Suppress normal output; instead print the name of each input file from\n");
    printf("    which output would normally have been printed.\n");
    printf("-n  Prefix each line of output with the line number within its input file.\n");
    printf("-q  Quiet; suppress normal output.\n");
    printf("-s  Suppress error messages about nonexistent or unreadable files.\n");
    printf("-v  Invert the sense of matching, to select non-matching lines.\n");
    printf("-w  Select only those lines containing matches that form whole words.  The\n");
    printf("    test is that the matching substring must either be at the beginning of\n");
    printf("    the line, or preceded by a non-word constituent character.  Similarly,\n");
    printf("    it must be either at the end of the line or followed by a non-word\n");
    printf("    constituent character.  Word-constituent characters are letters, digits,\n");
    printf("    and the underscore.\n");
    printf("-x  Select only those matches that exactly match the whole line.\n");
    printf("-?  Displays this help.\n\n");
    printf("Win32 port with subdirectory search created by Tim Charron\n");
    printf("(full source available at http://www.interlog.com/~tcharron/grep.html)\n");

}

int recurse(pathin, wildcardin, suppress_errors, subdirs, count_matches)
char * pathin;
char * wildcardin;
int suppress_errors;
int subdirs;
int count_matches;
{
    int desc, count, status=1;

    WIN32_FIND_DATA fileinfo;
    int filewasfound=1;
    HANDLE findh;
    char pname[240];
    char filenamein[240];

    strcpy(filenamein, pathin);
    if ( subdirs )
    {
        // Search for pathin\*\wildcardin in subidrectories (not the current directory)
        strcat(filenamein, "\\*");
    }
    else
    {
        // Search in the pathin directory only for files matching wildcardin (no subdirectories)
        strcat(filenamein, "\\");
        strcat(filenamein, wildcardin);
    }

//printf("recurse entered with %s + %s / %d : %s\n",pathin, wildcardin, subdirs, filenamein);fflush(stdout);

    findh=FindFirstFile(filenamein, &fileinfo);
    if ( findh == INVALID_HANDLE_VALUE )
    {
        filewasfound=0;
        if ( !globalsubdirs )							 // avoid getting a list of all directories that don't have any matching filenames
            if ( !suppress_errors )
                error(filenamein,errno);
    }
    else
        while ( filewasfound )
        {
            char *P;
            filename=fileinfo.cFileName;
            if ( NULL != (P=strrchr(filenamein,'\\')) )
            {
                sprintf(pname,"%s",filenamein);
                P=strrchr(pname,'\\');
                strcpy(P+1,filename);
            }
            else
            {
                strcpy(pname, fileinfo.cFileName);
            }

            if ( !subdirs )
            {
                // Try to open this file
                desc=open(pname, O_RDONLY);
                if ( !(FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(pname) ) )
                    if ( desc<0 )
                        // Couldn't open, and it wasn't a directory.
                        if ( !suppress_errors )
                            error(filenamein, errno);
            }
            else
            {
                if ( FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(pname) )
                {
                    // This is a directory.
                    // Ignore "." and ".."
                    if ( (strcmp(pname+strlen(pname)-1, ".")!=0) &&
                            (strcmp(pname+strlen(pname)-2, "..")!=0) )
                    {

                        // Found a directory.  Search files in this directory, then it's subdirectories
                        //z this will output the progress ,but it make performance poor...
                        //z if ( !out_quiet ) {fprintf(stderr, "%-79s\r",pname);fflush(stderr);}	  // Show progress

                        if ( !recurse(pname, wildcardin, suppress_errors, 0, count_matches) ) status=0;
                        if ( !recurse(pname, wildcardin, suppress_errors, 1, count_matches) ) status=0;
                    }
                }
            }
            if ( subdirs || (desc<0) )
            {
                // Couldn't open file, or this is a subdirectory search only.
                filewasfound=FindNextFile(findh, &fileinfo);
                continue;
            }

            if ( globalsubdirs )   // Show the full path of this file...
            {
                // include subdir,eg:/log/log.c
                if(NULL != (P=(strchr(pname,'.'))))
                {
                    if(*(P+1) == '\\')
                    {
                        strcpy(pname,P+2);
                    }
                }

                count=grep(desc,pname);
            }
            else     // Show only the filename (no path)...
            {
                // only filename ,eg:file.c
                count=grep(desc,fileinfo.cFileName);
            }
            if ( count_matches )
            {
                if ( out_file )
                    if ( globalsubdirs )
                    {
                        printf("%s:", pname);
                    }
                    else
                    {
                        printf("%s:", fileinfo.cFileName);
                    }
                printf("%d\n", count);
            }
            if ( count )
            {
                status = 0;
                if ( list_files == 1 )
                    if ( globalsubdirs )
                    {
                        printf("%-12s\n", pname);
                    }
                    else
                    {
                        printf("%-12s\n", fileinfo.cFileName);
                    }
            }
            else
            {
                if ( list_files == -1 )
                    if ( globalsubdirs )
                    {
                        printf("%-12s\n", pname);
                    }
                    else
                    {
                        printf("%-12s\n", fileinfo.cFileName);
                    }
            }
            close(desc);
            filewasfound=FindNextFile(findh, &fileinfo);
        }
    FindClose(findh);
    return (status);
}

int
main(argc, argv)
int argc;
char *argv[];
{
    char *keys;
    size_t keycc, oldcc, keyalloc;
    int keyfound, count_matches, no_filenames, suppress_errors;
    int opt, cc, desc, count, status;
    FILE *fp;
    extern char *optarg;
    extern int optind;

    prog = argv[0];
    if ( prog && strrchr(prog, '/') )
        prog = strrchr(prog, '/') + 1;

    keys = NULL;
    keycc = 0;
    keyfound = 0;
    count_matches = 0;
    no_filenames = 0;
    list_files = 0;
    suppress_errors = 0;
    globalsubdirs = 0;
    matcher = NULL;

    while ( (opt = getopt(argc, argv, "0123456789A:B:C:EFGVSXR?:bce:f:hiLlnqrsvwxy"))
            != EOF )
        switch ( opt )
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            out_before = 10 * out_before + opt - '0';
            out_after = 10 * out_after + opt - '0';
            break;
        case 'A':
            out_after = atoi(optarg);
            if ( out_after < 0 )
                usage();
            break;
        case 'B':
            out_before = atoi(optarg);
            if ( out_before < 0 )
                usage();
            break;
        case 'C':
            out_before = out_after = atoi(optarg);
            if(out_after < 0 || out_before < 0)
            {
                usage();
            }
            break;
        case 'E':
            if ( matcher && strcmp(matcher, "egrep") != 0 )
                fatal("you may specify only one of -E, -F, or -G", 0);
            matcher = "posix-egrep";
            break;
        case 'F':
            if ( matcher && strcmp(matcher, "fgrep") != 0 )
                fatal("you may specify only one of -E, -F, or -G", 0);;
            matcher = "fgrep";
            break;
        case 'G':
            if ( matcher && strcmp(matcher, "grep") != 0 )
                fatal("you may specify only one of -E, -F, or -G", 0);
            matcher = "grep";
            break;
        case 'V':
            fprintf(stderr, "%s\n", version);
            break;
        case 'X':
            if ( matcher )
                fatal("matcher already specified", 0);
            matcher = optarg;
            break;
        case 'b':
            out_byte = 1;
            break;
        case 'c':
            out_quiet = 1;
            count_matches = 1;
            break;
        case 'e':
            cc = strlen(optarg);
            keys = xrealloc(keys, keycc + cc + 1);
            if ( keyfound )
                keys[keycc++] = '\n';
            strcpy(&keys[keycc], optarg);
            keycc += cc;
            keyfound = 1;
            break;
        case 'f':
            fp = strcmp(optarg, "-") != 0 ? fopen(optarg, "r") : stdin;
            if ( !fp )
                fatal(optarg, errno);
            for ( keyalloc = 1; keyalloc <= keycc; keyalloc *= 2 )
                ;
            keys = xrealloc(keys, keyalloc);
            oldcc = keycc;
            if ( keyfound )
                keys[keycc++] = '\n';
            while ( !feof(fp)
                    && (cc = fread(keys + keycc, 1, keyalloc - keycc, fp)) > 0 )
            {
                keycc += cc;
                if ( keycc == keyalloc )
                    keys = xrealloc(keys, keyalloc *= 2);
            }
            if ( fp != stdin )
                fclose(fp);
            /* Nuke the final newline to avoid matching a null string. */
            if ( keycc - oldcc > 0 && keys[keycc - 1] == '\n' )
                --keycc;
            keyfound = 1;
            break;
        case 'h':
            no_filenames = 1;
            break;
        case 'i':
        case 'y':											 /* For old-timers . . . */
            match_icase = 1;
            break;
        case 'L':
            /* Like -l, except list files that don't contain matches.
            	Inspired by the same option in Hume's grep. */
            out_quiet = 1;
            list_files = -1;
            break;
        case 'l':
            out_quiet = 1;
            list_files = 1;
            break;
        case 'n':
            out_line = 1;
            break;
        case 'q':
            out_quiet = 1;
            break;
        case 'R':
        case 'r':
        case 'S':
            globalsubdirs = 1;
            break;
        case 's':
            suppress_errors = 1;
            break;
        case 'v':
            out_invert = 1;
            break;
        case 'w':
            match_words = 1;
            break;
        case 'x':
            match_lines = 1;
            break;
        case '?':
            help();
            return (2);
            break;
        default:
            usage();
            break;
        }

    if ( !keyfound )
        if ( optind < argc )
        {
            keys = argv[optind++];
            keycc = strlen(keys);
        }
        else
            usage();

    if ( !matcher )
        matcher = prog;

    if ( !setmatcher(matcher) && !setmatcher("default") )
        abort();

    (*compile)(keys, keycc);

    if (!no_filenames)
    {
        if (argc-optind > 1) out_file=1;
        if (argc-optind > 0) if (NULL != strchr(argv[optind], '*')) out_file=1;
        if (argc-optind > 0) if (NULL != strchr(argv[optind], '?')) out_file=1;
    }

    status = 1;

    if ( optind < argc )
    {
        while ( optind < argc )
        {
            desc = strcmp(argv[optind], "-");
            if ( !desc )  									 // stdin
            {
                count=grep(0,"(standard input)");
                if ( count_matches )
                {
                    printf("%d\n",count);
                }
                if ( count )
                {
                    status=0;
                    if ( list_files == 1 ) printf("%s:\n","(standard input)");
                }
                else if ( list_files == -1 ) printf("%s:\n","(standard input)");
            }
            else
            {
                char *P;
                if ( NULL != (P=strrchr(argv[optind],'\\')) )
                {
                    // Path provided...
                    *P=0;
                    if ( !recurse(argv[optind],P+1, suppress_errors, 0, count_matches) )	status=0;	// Files that match spec only
                    if ( globalsubdirs )
                    {
                        if ( !recurse(argv[optind],P+1, suppress_errors, 1, count_matches) )	status=0;	// Subdirs only
                    }
                }
                else
                {
                    // No path provided...
                    if ( !recurse(".",argv[optind],suppress_errors,0, count_matches) ) status=0;	 // Files that match spec only
                    if ( globalsubdirs )
                    {
                        if ( !recurse(".", argv[optind], suppress_errors, 1, count_matches) ) status=0;	 // Subdirs only
                    }
                }
                if ( globalsubdirs && !out_quiet ) fprintf(stderr, "%-79s\r","");
            }
            ++optind;
        }
    }
    else
    {
        count = grep(0,"(standard input)");
        filename="(standard input)";
        if ( count_matches )
            printf("%d\n", count);
        if ( count )
        {
            status = 0;
            if ( list_files == 1 )
                printf("(standard input)\n");
        }
        else if ( list_files == -1 )
            printf("(standard input)\n");
    }

    return (errseen ? 2 : status);
}

