#ifndef UPDATER_H
#define UPDATER_H

#include <stdbool.h>

#define CONFIG_PATH "/etc/omni_updater/config.json"
#define STATE_PATH "/var/local/omni_updater-state.json"

typedef struct {
    int current_manager_index;
    bool is_pending_resume;
} UpdateState;

void load_state(UpdateState* state);
void save_state(int index, bool pending_resume);
void clear_state(void);

bool is_reboot_required(void);
int run_command(const char* cmd);

#endif // UPDATER_H