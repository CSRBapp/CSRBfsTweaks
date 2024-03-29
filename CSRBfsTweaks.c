#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#define INTERCEPT_RENAMEAT

static int (*original_rename)(const char *oldpath, const char *newpath) = NULL;

#ifdef INTERCEPT_RENAMEAT
static int (*original_renameat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) = NULL;
static int (*original_renameat2)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags) = NULL;
#endif

static long (*original_pathconf)(const char *path, int name) = NULL;

unsigned CSRBfsTweaksInitDebugEnabled = 0;

#define DEBUG_FORCE(...) fprintf(stderr, "*** [CSRBfsTweaks]: " __VA_ARGS__)

#define DEBUG(...) \
	if(CSRBfsTweaksInitDebugEnabled) \
	{ \
		DEBUG_FORCE(__VA_ARGS__); \
	}

extern char **environ;

__attribute__((constructor)) static void CSRBfsTweaksInit(void)
{
	//DEBUG_FORCE("*** START\n");
	static int initialised = 0;
	if(initialised)
	{
		return;
	}
	initialised = 1;
	//DEBUG_FORCE("*** INITIALISING\n");

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
#ifdef INTERCEPT_RENAMEAT
	original_renameat = dlsym(RTLD_NEXT, "renameat");
	original_renameat2 = dlsym(RTLD_NEXT, "renameat2");
#endif

	original_pathconf = dlsym(RTLD_NEXT, "pathconf");

	/* TODO: assert the backedup symbols */

	DEBUG("INIT COMPLETE\n");
}

int rename(const char *oldpath, const char *newpath)
{
	int ret;

	DEBUG("rename(%s, %s)\n", oldpath, newpath);

	/* first try the original rename to check for permissions etc.
	 * CSRBfs will report EXDEV if it worked */

	ret = original_rename(oldpath, newpath);
	if(ret == 0)
	{
		/* success */
		return ret;
	}

	/* so, there was an error */

	if(errno != EXDEV)
	{
		/* unhandled error */
		return ret;
	}

	/* got EXDEV, so retry with "mv" instead */

	char cmd[32768];
	snprintf(cmd, sizeof(cmd), "/usr/bin/mv '%s' '%s'", oldpath, newpath);

	ret = system(cmd);
	if(ret == 0)
	{
		errno = 0;
	}
	else
	{
		errno = EIO;
	}

	return (ret ? -1 : 0);
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

long pathconf(const char *path, int name)
{
	long ret;
	char rpath[PATH_MAX];

	if(!realpath(path, rpath))
	{
		return 0;
	}

	DEBUG("pathconf(%p [%s][%s], %d)\n", path, path, rpath, name);

	switch(name)
	{
		case _PC_NAME_MAX: /* The maximum length of a filename in the directory path or fd that the process is allowed to create. */
			ret = 512 - strlen(rpath) + strlen("/mnt/CSRB/FS/00000000000000000000000000000000") - 1;
			break;
		case _PC_PATH_MAX: /* The maximum length of a relative pathname when path or fd is the current working directory. */
			ret = 512 - strlen(rpath) + strlen("/mnt/CSRB/FS/00000000000000000000000000000000");
			break;
		default:
			ret = original_pathconf(path, name);
			break;
	}

	DEBUG("pathconf ret:%ld\n", ret);

	return ret;
}

