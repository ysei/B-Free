#include <stdio.h>

void
main ()
{
  char *p = L"���������ɤǤν��ϡ���";

  while (*p != '\0')
    {
      putchar (*p | 0x80);
      p++;
    }
}
