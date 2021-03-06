/*

B-Free Project の生成物は GNU Generic PUBLIC LICENSE に従います。

GNU GENERAL PUBLIC LICENSE
Version 2, June 1991

(C) B-Free Project.

*/
/*************************************************************************
 *
 *		2nd BOOT memory allocate/deallocate routine.
 *
 * $Header: /cvsroot/bfree-info/B-Free/Program/PC9801/src/boot/2nd/memory.c,v 1.1 2011/12/27 17:13:36 liu1 Exp $
 *
 * $Log: memory.c,v $
 * Revision 1.1  2011/12/27 17:13:36  liu1
 * Initial Version.
 *
 * Revision 1.5  1995-09-21 15:50:41  night
 * ソースファイルの先頭に Copyright notice 情報を追加。
 *
 * Revision 1.4  1995/09/20  15:32:20  night
 * malloc 用のメモリを 640K - 100K 分に変更。
 *
 * Revision 1.3  1995/06/28  14:11:07  night
 * メモリアロケートの管理領域を (640K - 100K) 〜 640K から (2M - 100K) 〜
 * 2M に変更した。
 *
 * Revision 1.2  1995/06/26  15:06:12  night
 * malloc 関数の追加。
 *
 * Revision 1.1  1993/10/11  21:29:33  btron
 * btron/386
 *
 * Revision 1.1.1.1  93/01/14  12:30:24  btron
 * BTRON SYSTEM 1.0
 * 
 * Revision 1.1.1.1  93/01/13  16:50:27  btron
 * BTRON SYSTEM 1.0
 * 
 */

static char	rcsid[] = "$Header: /cvsroot/bfree-info/B-Free/Program/PC9801/src/boot/2nd/memory.c,v 1.1 2011/12/27 17:13:36 liu1 Exp $";

#include "types.h"
#include "location.h"
#include "config.h"
#include "memory.h"


struct alloc_entry
{
  struct alloc_entry	*next;
  int			size;
  unsigned char		body[0];
};

static struct alloc_entry	*alloc_reg;


extern int	end;
static void	init_malloc (void *last, int size);
void		*last_addr;
unsigned int	base_mem, ext_mem, real_mem;

#define TRUE_SIZE(size)	(size + sizeof (struct alloc_entry))


/**************************************************************************
 * init_memory
 *
 *
 */
void
init_memory (void)
{
  volatile int	*p;

  last_addr = (void *)&end;
  outb (0x00f2, 0);	/* 1M 以上の領域を使用できるようにする。*/
  
  for (p = (int *)0x100000; (int)p < 0xf00000; (int)p += 0x100000)
    {
      *p = 0;
      *p = 0xAA;
      if (*p != 0xAA)
	break;
    }
  printf ("Extended Memory = %d K bytes\n", ((int)p - 0x100000) / 1000);
  printf ("USE Memory      = %d bytes\n", last_addr);

  ext_mem = ((int)p - 0x100000);
  base_mem = BASE_MEM;
  real_mem = ext_mem + BASE_MEM;

#ifdef notdef
  /* malloc 機構の初期化 */
  init_malloc ((void *)(2 * 1024 * 1024), MALLOC_SIZE);
#endif
  /* 640K バイトから下位 100 K バイトの領域を malloc 用に使用する */
  init_malloc ((void *)(640 * 1024), MALLOC_SIZE);
}

/*
 * boot でメモリを使用する機構の初期化。
 *
 * 第１引数で指定したメモリの最後から、第２引数で指定した分だけを 
 * malloc で使用する。
 *
 */
static void
init_malloc (void *last, int size)
{
  (char *)alloc_reg = (unsigned char *)last - size;
  alloc_reg->size = size;
  alloc_reg->next = alloc_reg;
  printf ("init_malloc: last = 0x%x\n", last);
  printf ("init_malloc: alloc_reg = 0x%x\n", alloc_reg);
  printf ("init_malloc: alloc_reg->size = %d\n", alloc_reg->size);
  printf ("init_malloc: alloc_reg->next = 0x%x\n", alloc_reg->next);
}

/*
 * malloc --- 指定したサイズのメモリを取得する
 *
 * alloc_reg には、アロケートできるフリーメモリのエントリがつながって
 * いる。これらの中から最初にアロケートできるサイズをもったものを取り
 * だす。
 */
void *
malloc (int size)
{
  int			true_size;	/* 管理用領域も含んだ"真の"アロケート */
					/* サイズ */
  struct alloc_entry	*p, *prev;
  struct alloc_entry	*alloced;


  if (alloc_reg == NULL)	/* 取得すべきフリーメモリがない */
    {
      return (NULL);
    }

  true_size = TRUE_SIZE (size);

/*  printf ("alloc_reg = 0x%x, alloc_reg->next = 0x%x\n", alloc_reg, */
/*  alloc_reg->next); */

  for (prev = alloc_reg, p = alloc_reg->next;
       p->size < true_size;
       prev = p, p = p->next)
    {
      if (p == alloc_reg)	/* 取得できるエントリがなかった */
	{
	  return (NULL);
	}
    }

  if (p->size == true_size)	/* 丁度のサイズだった */
    {
      if (p->next == p)	/* つながっているエントリはひとつだけだった */
	{
	  alloc_reg == NULL;
	  p->next = NULL;
	}
      else
	{
	  /* フリーリストから p で指定されているエントリを外す */
	  prev->next = p->next;
	}
      
      return ((void *)(p->body));	/* 取得したメモリへのポインタ */
					/* を返す。取得した領域は、 */
					/* alloc_reg 構造体の body 要 */
					/* 素であることに注意 */
    }

  (char *)alloced = ((char *)p) + (p->size - true_size);
  p->size -= true_size;
  alloced->size = true_size;
  return (alloced->body);
}

/*
 * 引数に渡されたポインタが指す領域をフリーリストに入れる。
 * もし、領域が以前フリーリストにないものだったら、なにもしない。
 */
void
free (void *ptr)
{
  struct alloc_entry	*current, *prev;
  struct alloc_entry	*new_entry;

  new_entry = (struct alloc_entry *)((char *)ptr - sizeof (struct alloc_entry));

  /* 挿入を指定された領域は malloc で管理している領域ではない. */
  if (((char *)new_entry < (char *)(void *)ext_mem 
                            + (1024 * 1024) - MALLOC_SIZE) 
      || ((char *)new_entry > (char *)(void *)ext_mem + (1024 * 1024)))
    {
      return;
    }

  /* フリーリストにエントリはひとつしかない */
  if (alloc_reg == alloc_reg->next)
    {
      if (((char *)new_entry + new_entry->size) == (char *)alloc_reg)
	{
	  new_entry->size += alloc_reg->size;
	  new_entry->next = new_entry;
	  alloc_reg = new_entry;
	}
      else if (((char *)alloc_reg + alloc_reg->size) == (char *)new_entry)
	{
	  alloc_reg->size += new_entry->size;
	}
      else
	{
	  alloc_reg->next = new_entry;
	  new_entry->next = alloc_reg;
	}
      return;
    }

  /* フリーリストを先頭から辿り、リストに挿入するポイントを決める */
  /* フリーリストはアドレス順になっており、current のアドレスが追加す */
  /* る領域の最後よりも大きくなったら、その前に挿入する。*/
  for (prev = alloc_reg, current = alloc_reg->next;
       current != alloc_reg;
       prev = current, current = current->next)
    {
      /* current に隣接している。current と領域をつなげる */
      if ((char *)current == ((char *)new_entry + new_entry->size))
	{
	  new_entry->size += current->size;
	  new_entry->next = current->next;
	  current = new_entry;
	  prev->next = current;
	}
      else
	{
	  /* current には隣接していない。リストに挿入する */
	  new_entry->next = current;
	  prev->next = new_entry;
	}
      
      /* prev に隣接している場合。prev のサイズを増やす。*/
      /* このとき、current と併合していようが、単に挿入していようが、 */
      /* どちらにしても prev->next は、new_entryを指していることに注意 */
      if ((char *)new_entry == ((char *)prev + prev->size))
	{
	  prev->size += new_entry->size;
	  prev->next = new_entry->next;
	}
    }
}
