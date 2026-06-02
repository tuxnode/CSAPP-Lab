#ifndef SUPPORT
#define SUPPORT

#include <signal.h>

void sig_handler_impl(int x);
void initialize_bomb();
void phase_1(const char *input);
void phase_2(const char *input);
void phase_3(const char *input);
void explode_bomb();
void read_six_num(const char *, int *);

#endif
