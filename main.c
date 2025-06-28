
#include "memhack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#define PROCESS_NAME "SkyrimSE"
#define MODULE_NAME "libnvidia-gpucomp.so.575.64"
#define MODULE_OFFSET 0x0018C720
#define OFFSET1 0x0
#define OFFSET2 0x2A0
#define OFFSET3 0x184

int main() {
    pid_t pid = find_pid(PROCESS_NAME);
    if (pid < 0) { printf("Process not found\n"); return 1; }
    uintptr_t base = get_module_base(pid, MODULE_NAME);
    if (!base) { printf("Module not found\n"); return 1; }
    uintptr_t first_addr = base + MODULE_OFFSET;
    uintptr_t offsets[] = {OFFSET1, OFFSET2, OFFSET3};

    char mem_path[256];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);
    int mem_fd = open(mem_path, O_RDWR);
    if (mem_fd < 0) { perror("open mem"); return 1; }

    bool write_enabled = true;
    float value = 0.0f, new_value = 0.0f;

    while (1) {
        uintptr_t target = resolve_pointer(mem_fd, first_addr, offsets, 3);
        if (!target) { printf("Failed to resolve pointer\n"); break; }

        if (!read_mem(mem_fd, target, &value, sizeof(value))) {
            printf("Read failed\n"); break;
        }
        printf("Current value: %f\n", value);

        if (write_enabled) {
            if (!write_mem(mem_fd, target, &new_value, sizeof(new_value))) {
                printf("Write failed\n"); break;
            }
            printf("Wrote value: %f\n", new_value);
        }

        // Toggle write_enabled externally or add input handling here
        sleep(1);
    }

    close(mem_fd);
    return 0;
}
