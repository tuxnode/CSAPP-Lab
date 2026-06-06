# CSAPP Attack Lab 实验要求

## 概述

Attack Lab 是 CS:APP 3rd Edition 的配套实验，旨在通过实际攻击演示两类常见的安全漏洞：

- **Part I — Code Injection (代码注入)**：利用缓冲区溢出覆盖返回地址，执行注入的代码。
- **Part II — Return-Oriented Programming (ROP, 返回导向编程)**：在栈不可执行且地址随机化的情况下，利用现有代码片段（gadgets）构造攻击。

本实例的 Cookie（唯一标识）：**`0x59b997fa`**

---

## 文件清单

| 文件 | 说明 |
|---|---|
| `README.txt` | 文件说明 |
| `attacklab.pdf` | CMU 官方实验文档 |
| `cookie.txt` | 本实例的 Cookie: `0x59b997fa` |
| `ctarget` | 代码注入目标程序（Phase 1-3） |
| `rtarget` | ROP 目标程序（Phase 4-5） |
| `hex2raw` | 十六进制字节串转原始字节的工具 |
| `farm.c` | gadget farm 的 C 源码（rtarget 用） |
| `pwn1.py` | Phase 1 的 pwntools 脚本 |

---

## 核心概念

### 缓冲区大小

`getbuf` 函数分配 **40 字节**栈空间：

```asm
sub $0x28, %rsp   ; 分配 40 字节
```

因此攻击载荷的前 40 字节填充缓冲区，第 41 字节起覆盖返回地址。

### 运行方式

```bash
# 必须加 -q 跳过自动评分服务器连接
./hex2raw < exploit.txt | ./ctarget -q
./hex2raw < exploit.txt | ./rtarget -q
```

`hex2raw` 是 Linux ELF 程序，macOS 上无法直接运行。可用 Python 脚本替代其功能。

### hex2raw 输入格式

```
# 每行用空格或换行分隔的十六进制字节对
30 30 30 30 30 30 30 30
c0 17 40 00 00 00 00 00   # touch1 地址（小端序）
```

---

## Part I: Code Injection (ctarget) — Phase 1~3

ctarget 的栈**可执行**且**地址不随机化**。

### Phase 1 — 调用 touch1

**目标**：让 `getbuf` 返回时不回到 `test`，而是跳转到 `touch1`。

**要求**：
- 覆盖 `getbuf` 的返回地址为 `touch1` 的地址

**touch1 地址**：`0x4017c0`

**攻击载荷布局**：
```
[40 bytes 填充] [touch1 地址 (8 bytes, 小端序)]
```

**touch1 行为**：设置 `vlevel=1`，调用 `validate(1)`。

---

### Phase 2 — 调用 touch2 并传入 Cookie

**目标**：让 `getbuf` 跳转到注入的代码，将 Cookie 值传入 `%rdi`，然后调用 `touch2`。

**要求**：
- 构造机器码写入栈缓冲区，功能为：
  1. `mov $0x59b997fa, %edi`（将 Cookie 传入第一个参数寄存器）
  2. `push $0x4017ec` + `ret`（跳转到 touch2）
- 覆盖返回地址为栈缓冲区起始地址

**touch2 地址**：`0x4017ec`

**touch2 行为**：比较 `%edi` 与 Cookie，相等则调用 `validate(2)`。

**注意**：需要借助 GDB 确定栈缓冲区地址。另一种思路是利用 `ret` 指令跳转，减小对栈地址的依赖。

---

### Phase 3 — 调用 touch3 并传入 Cookie 字符串指针

**目标**：让 `getbuf` 跳转到注入的代码，将指向 Cookie 字符串的指针传入 `%rdi`，然后调用 `touch3`。

**要求**：
- Cookie 的字符串表示为 `"59b997fa"`（8 个 ASCII 字符 + 1 个 `\0` = 9 字节）
- 字符串不能放在 `getbuf` 的 40 字节缓冲区内，因为 `hexmatch` 函数会在此区域分配临时空间，可能覆盖数据
- 应将字符串放在 `getbuf` 栈帧**之上的安全位置**（例如在注入代码之后、攻击载荷的高地址处）
- 注入代码需将 `%rdi` 指向该字符串地址

**touch3 地址**：`0x4018fa`

**touch3 行为**：调用 `hexmatch(cookie, str)` 比较字符串，匹配则调用 `validate(3)`。

**`hexmatch` 地址**：`0x40184c`

---

## Part II: Return-Oriented Programming (rtarget) — Phase 4~5

rtarget 启用了两项防护：

1. **栈随机化（Stack Randomization）**：每次运行时栈地址不同
2. **栈不可执行（NX bit）**：无法在栈上执行代码

因此只能通过在栈上布置地址和小片已有代码（gadgets）来构造攻击链。

### Gadget Farm

gadget farm 位于 `start_farm` 和 `end_farm` 之间，是 rtarget 中专门为实验准备的可用 gadget 区域。

**关键 Gadgets**：

| Gadget | 地址 | 机器码 |
|---|---|---|
| `popq %rax; nop; ret` | `0x4019ab` | `58 90 c3` |
| `movq %rax, %rdi; ret` | `0x4019a2` | `48 89 c7 c3` |
| `movq %rsp, %rax; nop; ret` | `0x401aad` | `48 89 e0 90 c3` |
| `lea (%rdi,%rsi,1), %rax; ret` (add_xy) | `0x4019d6` | `48 8d 04 37 c3` |
| `movl %eax, %edx; nop; ret` | `0x4019dd` | `89 c2 90 c3` |
| `movl %edx, %ecx; nop; nop; ret` | `0x4019e3` | `89 d1 90 90 c3` |
| `movl %ecx, %esi; nop; nop; ret` | `0x401a13` | `89 ce 90 90 c3` |
| `movq %rsp, %rax; xor %eax, %ecx; ret` | `0x401a5c` | `48 89 e0 91 c3` |

---

### Phase 4 — ROP 调用 touch2

**目标**：使用 ROP gadgets 将 Cookie 传入 `%rdi`，然后调用 `touch2`。

**要求**：
- 利用 `popq %rax; ret` 将 Cookie 从栈上弹出到 `%rax`
- 利用 `movq %rax, %rdi; ret` 将值从 `%rax` 移到 `%rdi`
- 最后跳转到 `touch2`

**攻击载荷布局**：
```
[40 bytes 填充]
[popq %rax 地址]       # 0x4019ab
[Cookie 值]           # 0x59b997fa
[movq %rax→%rdi 地址]  # 0x4019a2
[touch2 地址]         # 0x4017ec
```

---

### Phase 5 — ROP 调用 touch3

**目标**：使用 ROP gadgets 将 Cookie 字符串的指针传入 `%rdi`，然后调用 `touch3`。

**要求**：
- 由于栈地址随机化，不能硬编码字符串地址
- 需要用 `movq %rsp, %rax` 获取栈指针
- 将栈指针加上一个偏移量，使其指向攻击载荷末尾存放的 Cookie 字符串
- 将计算结果传入 `%rdi`
- 跳转到 `touch3`

**核心思路**：
1. `movq %rsp, %rax; ret` — 获取栈指针
2. 通过 `add_xy`（`lea (%rdi,%rsi,1), %rax`）将 `%rsp` + 偏移量 → `%rax`
3. 将 `%rax` 的值移至 `%rdi`
4. 跳转到 `touch3`
5. 在攻击载荷末尾放置 Cookie 字符串 `"59b997fa"`

将值移入 `%rsi` 需要多条 gadget 组合：`%rax` → `%edx` → `%ecx` → `%esi`。

---

## 重要函数地址汇总

| 函数 | 含义 | ctarget 地址 | rtarget 地址 |
|---|---|---|---|
| `getbuf` | 漏洞函数 | `0x4017a8` | `0x4017a8` |
| `touch1` | Phase 1 目标 | `0x4017c0` | `0x4017c0` |
| `touch2` | Phase 2 目标 | `0x4017ec` | `0x4017ec` |
| `touch3` | Phase 3 目标 | `0x4018fa` | `0x4018fa` |
| `hexmatch` | 比较函数 | `0x40184c` | `0x40184c` |
| `test` | getbuf 的调用者 | `0x401968` | `0x401968` |
| `validate` | 验证函数 | `0x401c8d` | `0x401dad` |
| `global cookie` | 全局 Cookie | `0x6044e4` | `0x6054e4` |
| `vlevel` | 全局等级变量 | `0x6044dc` | `0x6054dc` |

---

## 验证机制

- 每个 `touchN` 设置 `vlevel = N` 后调用 `validate(N)`
- `validate` 检查 `vlevel` 与阶段号匹配
- 通过则输出 **PASS**，否则输出 **FAIL**
- 段错误（segmentation fault）表示攻击破坏了栈结构
- 出现 `MISALIGNED` 表示 `vlevel` 不匹配
