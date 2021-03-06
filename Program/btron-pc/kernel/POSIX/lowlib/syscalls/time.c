/*

B-Free Project の生成物は GNU Generic PUBLIC LICENSE に従います。

GNU GENERAL PUBLIC LICENSE
Version 2, June 1991

(C) B-Free Project.

*/
/* $Header: /cvsroot/bfree-info/B-Free/Program/btron-pc/kernel/POSIX/lowlib/syscalls/time.c,v 1.1 2011/12/27 17:13:35 liu1 Exp $ */
static char rcsid[] = "@(#)$Header: /cvsroot/bfree-info/B-Free/Program/btron-pc/kernel/POSIX/lowlib/syscalls/time.c,v 1.1 2011/12/27 17:13:35 liu1 Exp $";

/*
 * $Log: time.c,v $
 * Revision 1.1  2011/12/27 17:13:35  liu1
 * Initial Version.
 *
 * Revision 1.3  1999-03-21 00:58:21  monaka
 * Major fix. Many system calls runs correctly (...perhaps ;-). But signal and process functions are not yet.
 *
 * Revision 1.2  1999/02/19 10:05:04  monaka
 * Addition or fix for parameter structure.
 *
 * Revision 1.1  1997/08/31 13:10:50  night
 * 最初の登録
 *
 *
 *
 */


#include "../lowlib.h"




int
psys_time (void *argp)
{
  ER			error;
  struct posix_request	req;
  struct posix_response	res;
  struct psc_time *args = (struct psc_time *)argp;

  req.param.par_time.tm = args->tm;

  error = _make_connection(PSC_TIME, &req, &res);
  if (error != E_OK)
    {
      /* What should I do? */
    }

  else if (res.errno)
    {
      ERRNO = res.errno;
      return (-1);
    }

  return (res.status);
}
