#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <cjson/cJSON.h>

#include "updater.h"

void load_state(UpdateState* state) {
    state->current_manager_index = 0;
    state->is_pending_resume = false;

    FILE* fp = fopen(STATE_PATH, "r");
    if (!fp) return;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = malloc(size + 1);
    if (!buffer) { fclose(fp); return; }

    size_t read_bytes = fread(buffer, 1, size, fp);
    buffer[read_bytes] = '\0';
    fclose(fp);

    cJSON* json = cJSON_Parse(buffer);
    if (json) {
        cJSON* idx = cJSON_GetObjectItemCaseSensitive(json, "current_index");
        cJSON* resume = cJSON_GetObjectItemCaseSensitive(json, "is_pending_resume");
        if (cJSON_IsNumber(idx)) state->current_manager_index = idx->valueint;
        if (cJSON_IsBool(resume)) state->is_pending_resume = cJSON_IsTrue(resume);
        cJSON_Delete(json);
    }
    free(buffer);
}

void save_state(int index, bool pending_resume) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "current_index", index);
    cJSON_AddBoolToObject(json, "is_pending_resume", pending_resume);

    char* string = cJSON_Print(json);
    FILE* fp = fopen(STATE_PATH, "w");
    if (fp) {
        fputs(string, fp);
        fclose(fp);
    }
    cJSON_Delete(json);
    free(string);
}

void clear_state(void) {
    unlink(STATE_PATH);
}

bool is_reboot_required(void) {
    if (access("/var/run/reboot-required", F_OK) == 0) {
        return true;
    }
    return false;
}

int run_command(const char* cmd) {
    if (!cmd || strlen(cmd) == 0) return 0;
    printf("[EXECUTING] %s\n", cmd);
    int status = system(cmd);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}