#ifndef SUPPORT
#define SUPPORT

#include <signal.h>

void (*sig_handler)(int);
void sig_handler_impl(int x);

void initialize_bomb();

#endif
