#pragma once

#include <csignal>

static inline int signal_number(const std::string sig)
{
#ifdef SIGHUP
	if (sig == "HUP")
		return SIGHUP;
#endif
#ifdef SIGINT
	if (sig == "INT")
		return SIGINT;
#endif
#ifdef SIGQUIT
	if (sig == "QUIT")
		return SIGQUIT;
#endif
#ifdef SIGILL
	if (sig == "ILL")
		return SIGILL;
#endif
#ifdef SIGTRAP
	if (sig == "TRAP")
		return SIGTRAP;
#endif
#ifdef SIGABRT
	if (sig == "ABRT")
		return SIGABRT;
#endif
#ifdef SIGIOT
	if (sig == "IOT")
		return SIGIOT;
#endif
#ifdef SIGBUS
	if (sig == "BUS")
		return SIGBUS;
#endif
#ifdef SIGFPE
	if (sig == "FPE")
		return SIGFPE;
#endif
#ifdef SIGKILL
	if (sig == "KILL")
		return SIGKILL;
#endif
#ifdef SIGUSR1
	if (sig == "USR1")
		return SIGUSR1;
#endif
#ifdef SIGSEGV
	if (sig == "SEGV")
		return SIGSEGV;
#endif
#ifdef SIGUSR2
	if (sig == "USR2")
		return SIGUSR2;
#endif
#ifdef SIGPIPE
	if (sig == "PIPE")
		return SIGPIPE;
#endif
#ifdef SIGALRM
	if (sig == "ALRM")
		return SIGALRM;
#endif
#ifdef SIGTERM
	if (sig == "TERM")
		return SIGTERM;
#endif
#ifdef SIGSTKFLT
	if (sig == "STKFLT")
		return SIGSTKFLT;
#endif
#ifdef SIGCHLD
	if (sig == "CHLD")
		return SIGCHLD;
#endif
#ifdef SIGCONT
	if (sig == "CONT")
		return SIGCONT;
#endif
#ifdef SIGSTOP
	if (sig == "STOP")
		return SIGSTOP;
#endif
#ifdef SIGTSTP
	if (sig == "TSTP")
		return SIGTSTP;
#endif
#ifdef SIGTTIN
	if (sig == "TTIN")
		return SIGTTIN;
#endif
#ifdef SIGTTOU
	if (sig == "TTOU")
		return SIGTTOU;
#endif
#ifdef SIGURG
	if (sig == "URG")
		return SIGURG;
#endif
#ifdef SIGXCPU
	if (sig == "XCPU")
		return SIGXCPU;
#endif
#ifdef SIGXFSZ
	if (sig == "XFSZ")
		return SIGXFSZ;
#endif
#ifdef SIGVTALRM
	if (sig == "VTALRM")
		return SIGVTALRM;
#endif
#ifdef SIGPROF
	if (sig == "PROF")
		return SIGPROF;
#endif
#ifdef SIGWINCH
	if (sig == "WINCH")
		return SIGWINCH;
#endif
#ifdef SIGIO
	if (sig == "IO")
		return SIGIO;
#endif
#ifdef SIGPOLL
	if (sig == "POLL")
		return SIGPOLL;
#endif
#ifdef SIGPWR
	if (sig == "PWR")
		return SIGPWR;
#endif
#ifdef SIGSYS
	if (sig == "SYS")
		return SIGSYS;
#endif
#ifdef SIGUNUSED
	if (sig == "UNUSED")
		return SIGUNUSED;
#endif
#ifdef SIGRTMIN
	if (sig == "RTMIN")
		return SIGRTMIN;
#endif
#ifdef SIGRTMAX
	if (sig == "RTMAX")
		return SIGRTMAX;
#endif

	return 0;
}