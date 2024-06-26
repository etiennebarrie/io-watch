#include "monitor/monitor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	DEBUG = 0,
};

int path_prefix(const char *path, const char *prefix) {
	size_t path_size = strlen(path);
	size_t prefix_size = strlen(prefix);
	
	if (path_size < prefix_size) {
		return 0;
	}
	
	return strncmp(path, prefix, prefix_size) == 0;
}

ssize_t IO_Monitor_find(struct IO_Monitor *monitor, const char *path) {
	size_t index = 0;
	
	while (index < monitor->size) {
		if (path_prefix(path, monitor->paths[index])) {
			return index;
		}
		
		index += 1;
	}
	
	return -1;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <directory_to_watch> [<directory_to_watch> ...]\n", argv[0]);
		return 1;
	}

	struct IO_Monitor monitor;
	monitor.latency = 0.1;
	monitor.size = argc - 1;
	monitor.paths = (const char **) &argv[1];
	
	char * latency = getenv("IO_MONITOR_LATENCY");
	if (latency != NULL) {
		monitor.latency = atof(latency);
	}
	
	for (size_t i = 0; i < monitor.size; i++) {
		char *real_path = realpath(monitor.paths[i], NULL);
		if (real_path == NULL) {
			fprintf(stderr, "Error: realpath failed for %s\n", monitor.paths[i]);
			return 1;
		} else {
			if (DEBUG) fprintf(stderr, "Watching: %s\n", real_path);
			monitor.paths[i] = real_path;
		}
	}

	IO_Monitor_watch(&monitor);

	for (size_t i = 0; i < monitor.size; i++) {
		free((void *) monitor.paths[i]);
	}

	return 0;
}