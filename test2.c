#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <ncurses.h>

void monitor_activity() {
    struct sysinfo info;
    int refresh_rate = 1; // Refresh rate in seconds

    while (1) {
        sysinfo(&info);

        // Collect CPU stats
        double cpu_usage = 100.0 - ((double)info.loads[0] / (double)info.loads[2]) * 100.0;

        // Collect memory stats
        long total_memory = info.totalram / 1024;
        long free_memory = info.freeram / 1024;
        long used_memory = total_memory - free_memory;

        // Collect disk stats
        long total_disk = info.totalswap / 1024;
        long free_disk = info.freeswap / 1024;
        long used_disk = total_disk - free_disk;

        // Clear the screen
        clear();

        // Print CPU stats
        mvprintw(0, 0, "CPU Usage: %.2f%%", cpu_usage);

        // Print memory stats
        mvprintw(1, 0, "Memory Total: %ld KB", total_memory);
        mvprintw(2, 0, "Memory Free: %ld KB", free_memory);
        mvprintw(3, 0, "Memory Used: %ld KB", used_memory);

        // Print disk stats
        mvprintw(4, 0, "Disk Total: %ld KB", total_disk);
        mvprintw(5, 0, "Disk Free: %ld KB", free_disk);
        mvprintw(6, 0, "Disk Used: %ld KB", used_disk);

        // Refresh the screen
        refresh();

        // Sleep for the specified refresh rate
        sleep(refresh_rate);
    }
}

int main() {
    // Initialize ncurses
    initscr();

    // Start the system activity monitor
    monitor_activity();

    // End ncurses
    endwin();

    return 0;
}
