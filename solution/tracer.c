
#include <stdio.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <err.h>
#include <stdlib.h>
#include <sys/user.h>
#include <signal.h>

void alarm_clock(int sig) {
	_exit(1);
}

int
main() {
        int pipes[2];
        pid_t pid;

        if (pipe(pipes) == -1)
                err(1, "pipe");

        pid = fork();
        if (pid == -1)
                err(1, "fork");
        if (pid == 0) {
                // child
                close(pipes[0]);
                if (ptrace(PTRACE_TRACEME, 0, 0, 0) == -1)
                        err(1, "ptrace(traceme)");
                close(pipes[1]);
                execl("./a.out", "./a.out", NULL);
                err(1, "execl");
        } else {
                // parent
		struct user_regs_struct regs;
                char c;
                close(pipes[1]);
                c = read(pipes[0], &c, sizeof(c));
//                if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1) {
//                        err(1, "ptrace(attach)");
//                }

                for (;;) {
                        int wstatus;
                        if (wait(&wstatus) == -1)
                                err(1, "wait");
                        if (WIFEXITED(wstatus))
                                exit(WEXITSTATUS(wstatus));
                        if (WIFSIGNALED(wstatus)) {
                                if (WTERMSIG(wstatus) == SIGSEGV) {
                                        printf("sigsegv\n");
                                }
                                printf("termsig\n");
                                exit(1);
                        }
                        if (WIFSTOPPED(wstatus)) {
				int sig = WSTOPSIG(wstatus);
				ptrace(PTRACE_GETREGS, pid, 0, &regs);
				if (sig == SIGSEGV) {
					// money!
					printf("I crashed with rip=%llx\n", regs.rip);
					printf("Good enough for me... here's your key:\n");
					printf("adopt your next pet from your local pet shelter\n");
					exit(1);
				}
                                ptrace(PTRACE_CONT, pid, 0, 0);
                        }
                }
        }
}
