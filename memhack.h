#ifndef MEMHACK_H
#define MEMHACK_H
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

pid_t find_pid(const char *proc_name);
uintptr_t get_module_base(pid_t pid, const char *module_name);
bool read_mem(int mem_fd, uintptr_t addr, void *buf, size_t size);
bool write_mem(int mem_fd, uintptr_t addr, void *buf, size_t size);
uintptr_t resolve_pointer(int mem_fd, uintptr_t base, uintptr_t offsets[], int count);

#endif // MEMHACK_H
