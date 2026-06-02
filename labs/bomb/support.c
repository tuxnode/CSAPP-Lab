#include "support.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
00000000004012a0 <sig_handler>:
  4012a0:	48 83 ec 08          	sub    rsp,0x8
  4012a4:	bf c0 24 40 00       	mov    edi,0x4024c0
  4012a9:	e8 62 f8 ff ff       	call   400b10 <puts@plt>
  4012ae:	bf 03 00 00 00       	mov    edi,0x3
  4012b3:	e8 98 f9 ff ff       	call   400c50 <sleep@plt>
  4012b8:	be 82 25 40 00       	mov    esi,0x402582
  4012bd:	bf 01 00 00 00       	mov    edi,0x1
  4012c2:	b8 00 00 00 00       	mov    eax,0x0
  4012c7:	e8 34 f9 ff ff       	call   400c00 <__printf_chk@plt>
  4012cc:	48 8b 3d 6d 24 20 00 	mov    rdi,QWORD PTR [rip+0x20246d]        # 603740 <stdout@GLIBC_2.2.5>
  4012d3:	e8 08 f9 ff ff       	call   400be0 <fflush@plt>
  4012d8:	bf 01 00 00 00       	mov    edi,0x1
  4012dd:	e8 6e f9 ff ff       	call   400c50 <sleep@plt>
  4012e2:	bf 8a 25 40 00       	mov    edi,0x40258a
  4012e7:	e8 24 f8 ff ff       	call   400b10 <puts@plt>
  4012ec:	bf 10 00 00 00       	mov    edi,0x10
  4012f1:	e8 2a f9 ff ff       	call   400c20 <exit@plt>
*/
const char *so_you = "So you think you can stop the bomb with ctrl-c, do you?";
const char *well = "Well";
const char *ok = "...OK";
void sig_handler_impl(int x) {
  puts(so_you);
  sleep(3);
  printf("%s", well);
  fflush(stdout);
  sleep(1);
  puts(ok);
  exit(0x10);
}

void initialize_bomb() {
  sig_handler = sig_handler_impl;
  signal(2, sig_handler);
}
