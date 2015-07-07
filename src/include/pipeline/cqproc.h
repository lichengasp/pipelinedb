/* Copyright (c) 2013-2015 PipelineDB */
/*-------------------------------------------------------------------------
 *
 * cqproc.h
 *	  prototypes for cqproc.c.
 *
 * src/include/pipeline/cqproc.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef CQPROC_H
#define CQPROC_H

#include "datatype/timestamp.h"
#include "nodes/parsenodes.h"
#include "postmaster/bgworker.h"
#include "signal.h"

#define NUM_WORKERS(entry) ((entry)->pg_size - 1)

typedef enum
{
	CQCombiner,
	CQWorker
} CQProcessType;

typedef struct
{
	BackgroundWorkerHandle handle;
	pid_t last_pid;
} CQBackgroundWorkerHandle;

typedef struct
{
	int id;
	int pg_size;
	sig_atomic_t active;
	CQBackgroundWorkerHandle combiner;
	CQBackgroundWorkerHandle *workers;
	char *shm_query;
} CQProcEntry;

extern bool ContinuousQueryCrashRecovery;

extern void InitCQProcState(void);

extern CQProcEntry* GetCQProcEntry(int id);
extern int GetProcessGroupSize(int id);
extern int GetProcessGroupSizeFromCatalog(RangeVar* rv);
extern void SetActiveFlag(int id, bool flag);
extern void MarkCombinerAsRunning(int id);
extern void MarkWorkerAsRunning(int id, int worker_id);

extern CQProcEntry* CQProcEntryCreate(int key, int pg_size);
extern void CQProcEntryRemove(int key);

/* IPC */
extern pid_t GetCombinerPid(int id);
extern pid_t *GetWorkerPids(int id);

/* Resource Management */
extern bool WaitForCQProcsToStart(int id);
extern void WaitForCQProcsToTerminate(int id);
extern void TerminateCQProcs(int id);
extern bool IsCombinerRunning(int id);
extern bool AreCQWorkersStopped(int id);
extern void EnableCQProcsRecovery(int id);
extern void DisableCQProcsRecovery(int id);

extern void RunCQProcs(const char *cvname, void *state, CQProcEntry *procentry);

#endif   /* CQPROC_H */
