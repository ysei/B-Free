/*

B-Free Project の生成物は GNU Generic PUBLIC LICENSE に従います。

GNU GENERAL PUBLIC LICENSE
Version 2, June 1991

(C) B-Free Project.

*/

/* $Header: /cvsroot/bfree-info/B-Free/Program/btron-pc/kernel/ITRON/kernlib/sys_lowlib.c,v 1.1 2011/12/27 17:13:35 liu1 Exp $ */

static unsigned char rcsid[] = "$Id: sys_lowlib.c,v 1.1 2011/12/27 17:13:35 liu1 Exp $";


#include "../../ITRON/h/types.h"
#include "../../ITRON/h/itron.h"
#include "../../ITRON/h/syscall.h"
#include "../../ITRON/h/errno.h"
#include "../../ITRON/h/lowlib.h"


/* lowlib を特定のタスクに attach する
 */
ER
lod_low (ID task, B *name)
{
  ER erResult;
  struct a
    {
      ID task;
      B *name;
    } args;

  args.task = task;
  args.name = name;

  erResult = call_syscall (SYS_VSYS_MSC, 3, &args);

  return (erResult);
}


/* lowlib の情報を取得
 */
ER
sts_low (B *name, struct lowlib_info *infop, W *nlowlib)
{
  ER erResult;
  struct a
    {
      B				*name;
      struct lowlib_info	*infop;
      W				*nlowlib; /* lowlib の登録数 (name == */
					  /* NULL のとき) 	    */
    } args;

  args.name = name;
  args.infop = infop;
  args.nlowlib = nlowlib;
  
  erResult = call_syscall (SYS_VSYS_MSC, 5, &args);

  return (erResult);
}

