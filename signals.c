#include "metroid.h"

void handle_sigint(int sig) {
	if (sig == SIGINT) {
		g_signal_flag = 1;
	}
}

void setup_signal_handlers(void) {
	struct sigaction sa;

	bzero(&sa, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handle_sigint;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
}
