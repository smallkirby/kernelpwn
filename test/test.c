#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <poll.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/un.h>
#include <sys/xattr.h>
#include <sys/klog.h>
#include <assert.h>

#define WAIT getc(stdin);

int main(int argc, char *argv[]) {
	int fd = open("/dev/hoge", O_RDONLY);
	WAIT;
	ioctl(fd, 1, 0);

	int qid;
	if ((qid = msgget(123, 0666|IPC_CREAT)) == -1) {
		perror("msgget");
		exit(1);
	}
	struct {
		long mtype;
		char mtext[0x80 - 0x30];
	} msgbuf;
	msgbuf.mtype = 1;

	WAIT;

	for(int i = 0; i < 0x12; i++) {
		if (msgsnd(qid, &msgbuf, sizeof(msgbuf.mtext), 0) == -1) {
		perror("msgsnd");
		return 1;
		}
	}

	return 0;
}
