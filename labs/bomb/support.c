#include "support.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void (*sig_handler)(int);

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

/*
0000000000400ee0 <phase_1>:
  400ee0:	48 83 ec 08          	sub    rsp,0x8
  400ee4:	be 00 24 40 00       	mov    esi,0x402400
  400ee9:	e8 4a 04 00 00       	call   401338 <strings_not_equal>
  400eee:	85 c0                	test   eax,eax
  400ef0:	74 05                	je     400ef7 <phase_1+0x17>
  400ef2:	e8 43 05 00 00       	call   40143a <explode_bomb>
  400ef7:	48 83 c4 08          	add    rsp,0x8
  400efb:	c3                   	ret
*/
void phase_1(const char *input) {
  int cmp = strcmp(input, "Border relations with Canada have never been better.");
  if (cmp == 0) {
    explode_bomb();
  }
}

/*
0000000000400efc <phase_2>:
  400efc:	55                   	push   rbp
  400efd:	53                   	push   rbx
  400efe:	48 83 ec 28          	sub    rsp,0x28
  400f02:	48 89 e6             	mov    rsi,rsp
  400f05:	e8 52 05 00 00       	call   40145c <read_six_numbers>
  400f0a:	83 3c 24 01          	cmp    DWORD PTR [rsp],0x1
  400f0e:	74 20                	je     400f30 <phase_2+0x34>
  400f10:	e8 25 05 00 00       	call   40143a <explode_bomb>
  400f15:	eb 19                	jmp    400f30 <phase_2+0x34>
  400f17:	8b 43 fc             	mov    eax,DWORD PTR [rbx-0x4]
  400f1a:	01 c0                	add    eax,eax
  400f1c:	39 03                	cmp    DWORD PTR [rbx],eax
  400f1e:	74 05                	je     400f25 <phase_2+0x29>
  400f20:	e8 15 05 00 00       	call   40143a <explode_bomb>
  400f25:	48 83 c3 04          	add    rbx,0x4
  400f29:	48 39 eb             	cmp    rbx,rbp
  400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
  400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40>
  400f30:	48 8d 5c 24 04       	lea    rbx,[rsp+0x4]
  400f35:	48 8d 6c 24 18       	lea    rbp,[rsp+0x18]
  400f3a:	eb db                	jmp    400f17 <phase_2+0x1b>
  400f3c:	48 83 c4 28          	add    rsp,0x28
  400f40:	5b                   	pop    rbx
  400f41:	5d                   	pop    rbp
  400f42:	c3                   	ret
*/
void phase_2(const char *input) {
} 

// 炸弹爆炸
void explode_bomb() {
  puts("BOOM!!!");
}
