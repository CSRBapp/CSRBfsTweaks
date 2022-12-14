#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#define INTERCEPT_RENAMEAT

static int (*original_rename)(const char *oldpath, const char *newpath) = NULL;

#ifdef INTERCEPT_RENAMEAT
static int (*original_renameat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) = NULL;
static int (*original_renameat2)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags) = NULL;
#endif

unsigned CSRBfsTweaksInitDebugEnabled;

#define DEBUG_FORCE(...) fprintf(stderr, "*** [CSRBfsTweaks]: " __VA_ARGS__)

#define DEBUG(...) \
	if(CSRBfsTweaksInitDebugEnabled) \
	{ \
		DEBUG_FORCE(__VA_ARGS__); \
	}


extern char **environ;

__attribute__((constructor)) static void CSRBfsTweaksInit(void)
{
	static int initialised = 0;
	if(initialised)
	{
		return;
	}
	initialised = 1;

#if 0
	char ** env;
	env = environ;
	fprintf(stderr, "---\n");
	for(; env && *env ; ++env)
	{
		fprintf(stderr, "%s\n", *env);
	}
	fprintf(stderr, "---\n");
#endif

	if(getenv("CSRBFSTWEAKSDEBUG"))
	{
		CSRBfsTweaksInitDebugEnabled = 1;
		fprintf(stderr, "CSRBfsTweaks ENABLED\n");
	}

	original_rename = dlsym(RTLD_NEXT, "rename");
	original_renameat = dlsym(RTLD_NEXT, "renameat");
	original_renameat2 = dlsym(RTLD_NEXT, "renameat2");

	/* TODO: asset the backedup symbols */

	DEBUG("INIT COMPLETE\n");
}

int rename(const char *oldpath, const char *newpath)
{
	int ret;
	struct stat oldpathStat;

	DEBUG("rename(%s, %s)\n", oldpath, newpath);

	/* check if we are renameing a directory */
	ret = stat(oldpath, &oldpathStat);
	if(ret)
	{
		/* stat failed so pass back the error */
		return ret;
	}

	if(S_ISDIR(oldpathStat.st_mode))
	{
		/* we are, so redirect to "mv" */
		DEBUG_FORCE("REROUTING DIRECTORY rename(%s, %s) to mv()\n", oldpath, newpath);

		char cmd[32768];
		snprintf(cmd, sizeof(cmd), "/usr/bin/mv '%s' '%s'", oldpath, newpath);
		ret = system(cmd);
	}
	else
	{
		ret = original_rename(oldpath, newpath);
	}

	return ret;
}

#ifdef INTERCEPT_RENAMEAT

int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath)
{
	int ret;

	DEBUG("renameat(%d, %s, %d, %s)\n", olddirfd, oldpath, newdirfd, newpath);

	/* TODO: add assert */
	ret = original_renameat(olddirfd, oldpath, newdirfd, newpath);

	return ret;
}

int renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags)
{
	int ret;

	DEBUG("renameat2(%d, %s, %d, %s, %u)\n", olddirfd, oldpath, newdirfd, newpath, flags);

	ret = original_renameat2(olddirfd, oldpath, newdirfd, newpath, flags);

	return ret;
}

#endif // INTERCEPT_RENAMEAT
 
