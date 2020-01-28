#pragma once

#include <signal.h>
#include <string.h>

static inline int signal_number(const char *sig)
{
#ifdef SIGHUP
	if (!strcmp("HUP", sig))
		return SIGHUP;
#endif
#ifdef SIGINT
	if (!strcmp("INT", sig))
		return SIGINT;
#endif
#ifdef SIGQUIT
	if (!strcmp("QUIT", sig))
		return SIGQUIT;
#endif
#ifdef SIGILL
	if (!strcmp("ILL", sig))
		return SIGILL;
#endif
#ifdef SIGTRAP
	if (!strcmp("TRAP", sig))
		return SIGTRAP;
#endif
#ifdef SIGABRT
	if (!strcmp("ABRT", sig))
		return SIGABRT;
#endif
#ifdef SIGIOT
	if (!strcmp("IOT", sig))
		return SIGIOT;
#endif
#ifdef SIGBUS
	if (!strcmp("BUS", sig))
		return SIGBUS;
#endif
#ifdef SIGFPE
	if (!strcmp("FPE", sig))
		return SIGFPE;
#endif
#ifdef SIGKILL
	if (!strcmp("KILL", sig))
		return SIGKILL;
#endif
#ifdef SIGUSR1
	if (!strcmp("USR1", sig))
		return SIGUSR1;
#endif
#ifdef SIGSEGV
	if (!strcmp("SEGV", sig))
		return SIGSEGV;
#endif
#ifdef SIGUSR2
	if (!strcmp("USR2", sig))
		return SIGUSR2;
#endif
#ifdef SIGPIPE
	if (!strcmp("PIPE", sig))
		return SIGPIPE;
#endif
#ifdef SIGALRM
	if (!strcmp("ALRM", sig))
		return SIGALRM;
#endif
#ifdef SIGTERM
	if (!strcmp("TERM", sig))
		return SIGTERM;
#endif
#ifdef SIGSTKFLT
	if (!strcmp("STKFLT", sig))
		return SIGSTKFLT;
#endif
#ifdef SIGCHLD
	if (!strcmp("CHLD", sig))
		return SIGCHLD;
#endif
#ifdef SIGCONT
	if (!strcmp("CONT", sig))
		return SIGCONT;
#endif
#ifdef SIGSTOP
	if (!strcmp("STOP", sig))
		return SIGSTOP;
#endif
#ifdef SIGTSTP
	if (!strcmp("TSTP", sig))
		return SIGTSTP;
#endif
#ifdef SIGTTIN
	if (!strcmp("TTIN", sig))
		return SIGTTIN;
#endif
#ifdef SIGTTOU
	if (!strcmp("TTOU", sig))
		return SIGTTOU;
#endif
#ifdef SIGURG
	if (!strcmp("URG", sig))
		return SIGURG;
#endif
#ifdef SIGXCPU
	if (!strcmp("XCPU", sig))
		return SIGXCPU;
#endif
#ifdef SIGXFSZ
	if (!strcmp("XFSZ", sig))
		return SIGXFSZ;
#endif
#ifdef SIGVTALRM
	if (!strcmp("VTALRM", sig))
		return SIGVTALRM;
#endif
#ifdef SIGPROF
	if (!strcmp("PROF", sig))
		return SIGPROF;
#endif
#ifdef SIGWINCH
	if (!strcmp("WINCH", sig))
		return SIGWINCH;
#endif
#ifdef SIGIO
	if (!strcmp("IO", sig))
		return SIGIO;
#endif
#ifdef SIGPOLL
	if (!strcmp("POLL", sig))
		return SIGPOLL;
#endif
#ifdef SIGPWR
	if (!strcmp("PWR", sig))
		return SIGPWR;
#endif
#ifdef SIGSYS
	if (!strcmp("SYS", sig))
		return SIGSYS;
#endif
#ifdef SIGUNUSED
	if (!strcmp("UNUSED", sig))
		return SIGUNUSED;
#endif
#ifdef SIGRTMIN
	if (!strcmp("RTMIN", sig))
		return SIGRTMIN;
#endif
#ifdef SIGRTMAX
	if (!strcmp("RTMAX", sig))
		return SIGRTMAX;
#endif

	return 0;
}