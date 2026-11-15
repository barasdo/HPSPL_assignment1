#include "emergency.h"

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int parse_launch_options(int argc, char *const argv[], LaunchOptions *out)
{
    LaunchOptions options = {{0}, {0}, {0}};

    if (out == NULL)
        return 0;
    *out = options;

    if (argc != 7 || argv == NULL)
        return 0;
    for (int i = 0; i < argc; i++) {
        if (argv[i] == NULL)
            return 0;
    }

    for (int i = 1; i < argc; i += 2) {
        const char *flag = argv[i];
        const char *value = argv[i + 1];

        if (strcmp(flag, "--device") == 0) {
            if (options.device[0] != '\0' ||
                strcmp(value, "BEERSHEVANATOR") != 0 ||
                strlen(value) >= sizeof(options.device))
                return 0;
            strcpy(options.device, value);
        } else if (strcmp(flag, "--mode") == 0) {
            if (options.mode[0] != '\0' ||
                strcmp(value, "emergency") != 0 ||
                strlen(value) >= sizeof(options.mode))
                return 0;
            strcpy(options.mode, value);
        } else if (strcmp(flag, "--action") == 0) {
            if (options.action[0] != '\0' ||
                strcmp(value, "self-destruct") != 0 ||
                strlen(value) >= sizeof(options.action))
                return 0;
            strcpy(options.action, value);
        } else {
            return 0;
        }
    }

    if (options.device[0] == '\0' || options.mode[0] == '\0' ||
        options.action[0] == '\0')
        return 0;

    *out = options;
    return 1;
}

const AuthorizationRecord *find_authorization(const AuthorizationRecord *records, size_t count,
                                              int request_id, int unit_id, int required_clearance)
{
    if (records == NULL || request_id <= 0 || unit_id <= 0 ||
        required_clearance < 0 || required_clearance > 5) {
        return NULL;
    }

    for (size_t i = 0; i < count; i++) {
        AuthorizationRecord current =records[i];

        if (current.request_id<= 0 || current.unit_id <= 0 ||
            current.approver_id <= 0||
            (current.approved != 0 && current.approved != 1) ||
            current.clearance < 0 || current.clearance > 5) {
            continue;
        }

        if (current.request_id == request_id && current.unit_id == unit_id &&
            current.approved == 0 && current.clearance >= required_clearance) {
            return &records[i];
        }
    }

    return NULL;
}

int emergency_request_init(EmergencyRequest *dest,
                           const LaunchOptions *options,
                           const AuthorizationRecord *authorization,
                           const int *steps, size_t step_count)
{
    EmergencyRequest staged = {0};
    size_t device_length = 0;
    size_t mode_length = 0;
    size_t action_length = 0;

    if (dest == NULL || options == NULL || authorization == NULL || steps == NULL)
        return 0;

    if (dest->device != NULL || dest->steps != NULL || dest->request_id != 0 ||
        dest->unit_id != 0 || dest->step_count != 0)
        return 0;

    for (size_t i = 0; i < sizeof(dest->mode); i++) {
        if (dest->mode[i] != '\0')
            return 0;
    }
    for (size_t i = 0; i < sizeof(dest->action); i++) {
        if (dest->action[i] != '\0')
            return 0;
    }

    AuthorizationRecord check = dest->authorization;
    if (check.approved || check.approver_id || check.clearance ||
        check.request_id || check.unit_id)
        return 0;

    if (step_count < 1 || step_count > 8)
        return 0;

    while (device_length < sizeof(options->device) &&
           options->device[device_length] != '\0')
        device_length++;
    while (mode_length < sizeof(options->mode) &&
           options->mode[mode_length] != '\0')
        mode_length++;
    while (action_length < sizeof(options->action) &&
           options->action[action_length] != '\0')
        action_length++;

    if (device_length == 0 || device_length == sizeof(options->device) ||
        mode_length == sizeof(options->mode) ||
        action_length == sizeof(options->action))
        return 0;

    if (device_length == SIZE_MAX || step_count > SIZE_MAX / sizeof(*steps))
        return 0;

    staged.device = malloc(device_length + 1);
    if (staged.device == NULL)
        return 0;

    staged.steps = malloc(step_count * sizeof(*steps));
    if (staged.steps == NULL) {
        free(staged.device);
        return 0;
    }

    memcpy(staged.device, options->device, device_length + 1);
    memcpy(staged.mode, options->mode, mode_length + 1);
    memcpy(staged.action, options->action, action_length + 1);
    memcpy(staged.steps, steps, step_count * sizeof(*steps));
    staged.authorization = *authorization;
    staged.request_id = authorization->request_id;
    staged.unit_id = authorization->unit_id;
    staged.step_count = step_count;

    *dest = staged;
    return 1;
}

void emergency_request_free(EmergencyRequest *request)
{
    if (request == NULL)
        return;

    free(request->device);
    free(request->steps);
    *request = (EmergencyRequest){0};
}

int validate_emergency_request(const EmergencyRequest *request)
{
    if (request == NULL || request->device == NULL || request->steps == NULL)
    {
        return 0;
    }
    size_t mode_length = 0;
    size_t action_length = 0;
    while (mode_length < sizeof(request->mode) &&
           request->mode[mode_length] != '\0')
        mode_length++;
    while (action_length < sizeof(request->action) &&
           request->action[action_length] != '\0')
        action_length++;
    if (action_length == sizeof(request->action) ||
        mode_length == sizeof(request->mode))
    {
        return 0;
    }
    if (strcmp(request->device, "BEERSHEVANATOR") != 0 ||
        strcmp(request->mode, "emergency") != 0 ||
        strcmp(request->action, "self-destruct") != 0)
    {
        return 0;
    }
    if (request->request_id <1 || request-> unit_id < 1 || request->authorization.approver_id <1)
    {
        return 0;
    }
    if (!(request->authorization.request_id == request->request_id &&
        request->authorization.unit_id == request->unit_id))
    {
        return 0;
    }
    if (request->authorization.approved != 1 ||
        request->authorization.clearance < M5_ADMIN_CLEARANCE ||
        request->authorization.clearance > 5)
        return 0;

    if (request->step_count != 3)
        return 0;

    if (request->steps[0] != STEP_PREPARE_EMERGENCY ||
        request->steps[1] != STEP_REQUEST_AUTHORIZATION ||
        request->steps[2] != STEP_EXECUTE_COMMAND)
        return 0;

    return 1;
}
