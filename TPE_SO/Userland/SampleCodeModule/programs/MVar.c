
#include <sys_calls.h>
#include <userLibrary.h>

typedef struct {
	int value;
	uint16_t semLector;
	uint16_t semEscritor;
} mvar_t;

mvar_t mvar;

void openMVar() {
	mvar.value = 0;
	u_sys_sem_open(32, 0);
	mvar.semLector = 32;
	u_sys_sem_open(33, 1);
	mvar.semEscritor = 33;
}

void putMVar(int value) {
	u_sys_sem_wait(mvar.semEscritor);
	mvar.value = value;
	u_sys_sem_post(mvar.semLector);
}

int takeMVar() {
	u_sys_sem_wait(mvar.semLector);
	int value  = mvar.value;
	mvar.value = 0;
	u_sys_sem_post(mvar.semEscritor);
	return value;
}
