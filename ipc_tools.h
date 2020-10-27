#pragma once
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*����� �������*/
#define PERM 0600
/*������������ ����� ���������*/
#define MAXLEN 50
#define ERR ((struct TData *) -1)
using namespace std;
/*��������� ��� semctl*/
union semun 
{
	int val;/* �������� ��� SETVAL */
	struct semid_ds *buf;/* ����� ��� IPC_STAT, IPC_SET */
	unsigned short *array; /* ������ ��� GETALL, SETALL */
};
struct TData
{
	char msg[MAXLEN + 1];
	pid_t pid;
};
/*��������� ������*/
void error(char *s)
{
	perror(s);
	getchar();
	exit(1);
}
/*������������� ��������*/
int init_sem(key_t semkey)
{
	int semid;
	union semun arg;
	/*��������� �������������� ��������*/
	if ((semid = semget(semkey, 1, PERM | IPC_CREAT)) == -1)
		error("semget error!");
	arg.val = 1;
	/*��������� �������� ��������� ��������*/
	if (semctl(semid, 0, SETVAL, arg) == -1)
		error("semctl error!");
	return semid;
}
/*wait*/
void p(int semid)
{
	struct sembuf p_buf;
	p_buf.sem_num = 0;
	p_buf.sem_op = -1;
	p_buf.sem_flg = SEM_UNDO;
	/*��������� �������� �� 1*/
	if (semop(semid, &p_buf, 1) == -1)
		error("p(semid) error");
}
/*signal*/
void v(int semid)
{
	struct sembuf v_buf;
	v_buf.sem_num = 0;
	v_buf.sem_op = 1;
	v_buf.sem_flg = SEM_UNDO;
	/*��������� �������� �� 1*/
	if (semop(semid, &v_buf, 1) == 1)
		error("v(semid) error!");
}
/*������������� ����������� ������*/
int init_shm(key_t shmkey)
{
	int shmid;
	if ((shmid = shmget(shmkey, sizeof(TData), PERM | IPC_CREAT)) == -1)
		error("shmget error");
	return  shmid;
}
/*������ ������ � ����������� ������*/
void write_shr_data(int shmid, TData &data)
{
	TData *p = (TData *)shmat(shmid, NULL, 0);
	strncpy(p->msg, data.msg, MAXLEN);
	p->pid = data.pid;
	shmdt(p);
}
/*������ ������ �� ����������� ������*/
void read_shr_data(int shmid, TData &data)
{
	TData *p;
	if ((p = (TData *)shmat(shmid, NULL, 0)) == ERR)
		error("shmat error");
	strncpy(data.msg, p->msg, MAXLEN);
	data.pid = p->pid;
	shmdt(p);
}
/*��������� ����������� ipc �����*/
key_t get_ipc_key(int id)
{
	close(creat("ipc_key", PERM));
	return ftok("ipc_key", id);
}