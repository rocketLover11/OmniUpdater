#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <cjson/cJSON.h>

#include "updater.h"

int main(void) {
    if (getuid() != 0) {
        fprintf(stderr, "Error: This multi-manager system utility requires root permissions.\n");
        return 1;
    }

    UpdateState state;
    load_state(&state);

    FILE* fp = fopen(CONFIG_PATH, "r");
    if (!fp) {
        perror("Failed to open config file");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* config_data = malloc(size + 1);
    if (!config_data) { fclose(fp); return 1; }

    size_t read_bytes = fread(config_data, 1, size, fp);
    config_data[read_bytes] = '\0';
    fclose(fp);

    cJSON* json = cJSON_Parse(config_data);
    if (!json) {
        fprintf(stderr, "Invalid configuration format inside config.json.\n");
        free(config_data);
        return 1;
    }

    cJSON* managers = cJSON_GetObjectItemCaseSensitive(json, "package_managers");
    int managers_count = cJSON_GetArraySize(managers);

    if (state.is_pending_resume) {
        printf("--- Resuming pending system update task pipeline ---\n");
    }

    for (int i = state.current_manager_index; i < managers_count; i++) {
        cJSON* mgr = cJSON_GetArrayItem(managers, i);
        cJSON* enabled = cJSON_GetObjectItemCaseSensitive(mgr, "enabled");

        if (cJSON_IsTrue(enabled)) {
            cJSON* name = cJSON_GetObjectItemCaseSensitive(mgr, "name");
            cJSON* update_cmd = cJSON_GetObjectItemCaseSensitive(mgr, "update_cmd");
            cJSON* upgrade_cmd = cJSON_GetObjectItemCaseSensitive(mgr, "upgrade_cmd");

            printf("\n=> Upgrading Component System: %s\n", name->valuestring);

            if (cJSON_IsString(update_cmd)) run_command(update_cmd->valuestring);
            if (cJSON_IsString(upgrade_cmd)) run_command(upgrade_cmd->valuestring);

            if (is_reboot_required()) {
                printf("[WARNING] Kernel upgrade caught. Readying system reboot cycle.\n");

                save_state(i + 1, true);
                run_command("loginctl lock-sessions || true");
                sync();
                run_command("systemctl reboot");

                cJSON_Delete(json);
                free(config_data);
                return 0;
            }
        }
    }

    printf("\nAll system package tracking profiles updated!\n");
    clear_state();
    cJSON_Delete(json);
    free(config_data);
    return 0;
}