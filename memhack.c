#include "memhack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

pid_t find_pid(const char *proc_name) {
    DIR *dir = opendir("/proc");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            pid_t pid = atoi(entry->d_name);
            if (pid > 0) {
                char path[256], cmdline[4096];
                snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
                FILE *f = fopen(path, "r");
                if (f) {
                    size_t len = fread(cmdline, 1, sizeof(cmdline) - 1, f);
                    fclose(f);
                    if (len > 0) {
                        cmdline[len] = '\0';
                        if (strstr(cmdline, proc_name)) {
                            closedir(dir);
                            return pid;
                        }
                    }
                }
            }
        }
    }
    closedir(dir);
    return -1;
}

uintptr_t get_module_base(pid_t pid, const char *module_name) {
    char maps_path[256], line[512];
    snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid);
    FILE *maps = fopen(maps_path, "r");
    if (!maps) return 0;
    while (fgets(line, sizeof(line), maps)) {
        if (strstr(line, module_name)) {
            uintptr_t base, dummy;
            sscanf(line, "%lx-%lx", &base, &dummy);
            fclose(maps);
            return base;
        }
    }
    fclose(maps);
    return 0;
}

bool read_mem(int mem_fd, uintptr_t addr, void *buf, size_t size) {
    return pread(mem_fd, buf, size, addr) == size;
}

bool write_mem(int mem_fd, uintptr_t addr, void *buf, size_t size) {
    return pwrite(mem_fd, buf, size, addr) == size;
}

uintptr_t resolve_pointer(int mem_fd, uintptr_t base, uintptr_t offsets[], int count) {
    uintptr_t addr = base;
    for (int i = 0; i < count; ++i) {
        if (!read_mem(mem_fd, addr, &addr, sizeof(addr))) return 0;
        addr += offsets[i];
    }
    return addr;
}
