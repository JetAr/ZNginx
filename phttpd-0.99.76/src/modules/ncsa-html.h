/*
** ncsa-html.h
**
** Copyright (c) 1995 Marcus E. Hennecke <marcush@leland.stanford.edu>
** Copyright (c) 1994-1995 Peter Eriksson <pen@signum.se>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef PHTTPD_NCSA_HTML_H
#define PHTTPD_NCSA_HTML_H

/* Defines the maximum inclusion level */
#define MAXINCLUDE 8


/* The segment structure contains all the necessary information for */
/* one piece of the parsed file. */
struct segment
{
    struct segment *next;	/* Pointer to next segment */
    char *block;		/* Pointer to data block */
    size_t size;		/* Size of data block */

    /* The following two are set of this segment is the last of a block */
    /* of memory that needs to be freed or unmapped. If free_start is */
    /* not NULL, the block starting at free_start needs to be freed or */
    /* unmapped. If free_size is not 0, the block is unmapped, otherwise */
    /* it is freed. */
    char *free_start;	/* Start of block to be freed or unmapped */
    size_t free_size;	/* Size of block to be freed or unmapped */
    fscentry_t *fep;	/* File info of mapped block */
};

/* Contains all the necessary information during parsing that needs to */
/* be passed on to the inclusion routines. */
struct includeinfo
{
    struct connectioninfo *cip;	/* The connection info */
    int level;			/* Inclusion level */
    unsigned int fsc_flags;     /* Fscache flags to use */
    fscentry_t *fep;		/* Cache entry of urrent file */
    time_t lastmod;		/* Last modification date. This may be */
    /* different from sb.st_mtime if a */
    /* file or information from a file is */
    /* included that is younger than the */
    /* current file. */
    size_t length;		/* Content length */
    char *errmsg;		/* Message to be printed if errors occur */
    char *timefmt;		/* Format for all time displays */
    char *sizefmt;		/* Format for all size displays */
};

/* List element for the attribute pairs. */
struct attr_pair
{
    struct attr_pair *next;	/* Points to next pair */
    char *name;			/* Attribute name */
    char *value;		/* Attribute value */
};

/* Defines command names, the corresponding function and a flag to */
/* indicate whether the parser should bother to parse the attributes. */
struct command
{
    const char *name;
    void (*function)(struct segment*, struct attr_pair*, struct includeinfo*);
    int has_attributes;
};

#endif
