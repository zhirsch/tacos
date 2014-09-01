#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait3(int* status, int options, struct rusage* rusage) {
  if (rusage != NULL) {
    memset(rusage, 0, sizeof(*rusage));
  }
  return waitpid(-1, status, options);
}
