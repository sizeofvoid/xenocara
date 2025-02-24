/*

Copyright (c) 1993, 1994, 1998 The Open Group.

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

*/

#ifdef HAVE_CONFIG_H
# include "makedepend-config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

/* http://clang.llvm.org/docs/LanguageExtensions.html#has-attribute */
#ifndef __has_attribute
# define __has_attribute(x) 0  /* Compatibility with non-clang compilers. */
#endif

#ifndef _X_ATTRIBUTE_PRINTF
#if __has_attribute(__format__) \
    || defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 203)
# define _X_ATTRIBUTE_PRINTF(x,y) __attribute__((__format__(__printf__,x,y)))
#else /* not gcc >= 2.3 */
# define _X_ATTRIBUTE_PRINTF(x,y)
#endif
#endif

#ifndef _X_NORETURN
#if __has_attribute(noreturn) \
    || (defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 205)) \
    || (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590))
# define _X_NORETURN __attribute((noreturn))
#else
# define _X_NORETURN
#endif /* GNUC  */
#endif

#ifndef HAVE_REALLOCARRAY
#define reallocarray(ptr, num, size)    realloc(ptr, (num) * (size))
#endif
#define mallocarray(num, size)          reallocarray(NULL, num, size)

#define MAXDEFINES	512
#define MAXFILES	2048
#define MAXINCFILES	128     /* "-include" files */
#define MAXDIRS		512     /* -I flags */
#define PATHMAX		4096    /* realpath */
#define SYMTABINC	10      /* must be > 1 for define() to work right */
#define	TRUE		1
#define	FALSE		0

/* the following must match the directives table in parse.c */
#define	IF		0
#define	IFDEF		1
#define	IFNDEF		2
#define	ELSE		3
#define	ENDIF		4
#define	DEFINE		5
#define	UNDEF		6
#define	INCLUDE		7
#define	LINE		8
#define	PRAGMA		9
#define ERROR           10
#define IDENT           11
#define SCCS            12
#define ELIF            13
#define EJECT           14
#define WARNING         15
#define INCLUDENEXT     16
#define IFFALSE         17      /* pseudo value --- never matched */
#define ELIFFALSE       18      /* pseudo value --- never matched */
#define INCLUDEDOT      19      /* pseudo value --- never matched */
#define IFGUESSFALSE    20      /* pseudo value --- never matched */
#define ELIFGUESSFALSE  21      /* pseudo value --- never matched */
#define INCLUDENEXTDOT  22      /* pseudo value --- never matched */

#ifdef DEBUG
extern int _debugmask;

/*
 * debug levels are:
 *
 *     0	show ifn*(def)*,endif
 *     1	trace defined/!defined
 *     2	show #include
 *     3	show #include SYMBOL
 *     4-6	unused
 */
#define debug(level,arg) do { if (_debugmask & (1 << level)) warning arg; } while(0)
#else
#define	debug(level,arg) do { /**/ } while (0)
#endif /* DEBUG */

typedef unsigned char boolean;

struct symtab {
    char *s_name;
    char *s_value;
};

/* possible i_flag */
#define DEFCHECKED	(1<<0)  /* whether defines have been checked */
#define NOTIFIED	(1<<1)  /* whether we have revealed includes */
#define MARKED		(1<<2)  /* whether it's in the makefile */
#define SEARCHED	(1<<3)  /* whether we have read this */
#define FINISHED	(1<<4)  /* whether we are done reading this */
#define INCLUDED_SYM	(1<<5)  /* whether #include SYMBOL was found
                                   Can't use i_list if TRUE */
struct inclist {
    char                *i_incstring;   /* string from #include line */
    char                *i_file;        /* path name of the include file */
    char                *i_realpath;    /* path name processed by realpath() */
    struct inclist      **i_list;       /* list of files it itself includes */
    struct symtab       **i_defs;       /* symbol table for this file and its
                                           children when merged */
    unsigned int        i_listlen;      /* length of i_list */
    int                 i_ndefs;        /* current # defines */
    boolean             *i_merged;      /* whether we have merged child
                                           defines */
    unsigned char       i_flags;
};

struct filepointer {
    const char  *f_name;
    char        *f_p;
    char        *f_base;
    char        *f_end;
    long        f_len;
    long        f_line;
    long        cmdinc_count;
    char        **cmdinc_list;
    long        cmdinc_line;
};

#include <stdlib.h>

/* main.c */
int match(const char *str, const char *const *list);
char *getnextline(struct filepointer *fp);
struct filepointer *getfile(const char *file);
void freefile(struct filepointer *fp);

/* include.c */
void included_by(struct inclist *ip, struct inclist *newfile);
struct inclist *newinclude(const char *newfile, const char *incstring,
                           const char *incpath);
void inc_clean(void);
struct inclist *inc_path(const char *file, const char *include, int type);

/* parse.c */
struct symtab **isdefined(const char *symbol, struct inclist *file,
                          struct inclist **srcfile);
void define2(const char *name, const char *val, struct inclist *file);
void define(char *def, struct inclist *file);
void undefine(const char *symbol, struct inclist *file);
int find_includes(struct filepointer *filep,
                  struct inclist *file, struct inclist *file_red,
                  int recursion, boolean failOK);

/* pr.c */
void recursive_pr_include(struct inclist *head,
                          const char *file, const char *base);
void add_include(struct filepointer *filep,
                 struct inclist *file, struct inclist *file_red,
                 const char *include, int type, boolean failOK);

/* cppsetup.c */
int cppsetup(const char *filename, const char *line,
             struct filepointer *filep, struct inclist *inc);

/* main.c */
extern void fatalerr(const char *, ...) _X_ATTRIBUTE_PRINTF(1, 2) _X_NORETURN;
extern void warning(const char *, ...)  _X_ATTRIBUTE_PRINTF(1, 2);
extern void warning1(const char *, ...) _X_ATTRIBUTE_PRINTF(1, 2);

extern struct inclist   inclist[MAXFILES];
extern struct inclist   *inclistp;
extern struct inclist   *inclistnext;
extern struct inclist   maininclist;
extern const char       *includedirs[];
extern const char       **includedirsnext;
extern char             *notdotdot[];

extern const char       *objprefix;
extern const char       *objsuffix;
extern int              width;
extern boolean          printed;
extern boolean          verbose;
extern boolean          show_where_not;
extern boolean          warn_multiple;
