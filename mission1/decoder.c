#include "decoder.h"

#include <stdio.h>
#include <string.h>

int decode_message(const char *encoded, int shift, char *decoded, size_t capacity)
{
    size_t len = 0;
    size_t i;

    if (encoded == NULL || decoded == NULL || shift < 0 || shift > 25) {
        return 0;
    }

    while (encoded[len] != '\0') {
        char c = encoded[len];
        if (!((c >= 'A' && c <= 'Z') || c == '_')) {
            return 0;
        }
        len++;
    }

    if (capacity < len + 1) {
        return 0;
    }

    for (i = 0; i < len; i++) {
        char c = encoded[len - 1 - i];
        if (c == '_') {
            decoded[i] = '_';
        } else {
            decoded[i] = (char)('A' + (c - 'A' - shift + 26) % 26);
        }
    }

    decoded[len] = '\0';
    return 1;
}

int calculate_checksum(const char *message)
{

    if (message == NULL) {
        return -1;
    }

    size_t i = 0;
    size_t sum = 0;

    while(message[i] != '\0') {
        sum += message[i];
        i++;
    }

    return (sum % 97);
}

static int print_invalid(void)
{
    puts("[INVALID]");
    return 0;
}

static int is_valid_chars(const char *s)
{
    size_t i;

    for (i = 0; s[i] != '\0'; i++) {
        if (!((s[i] >= 'A' && s[i] <= 'Z') || s[i] == '_')) {
            return 0;
        }
    }
    return 1;
}

static int parse_number(const char *s, int max_value, int *out)
{
    size_t i;
    int value = 0;

    if (s[0] == '\0') {
        return 0;
    }
    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') {
            return 0;
        }
        value = value * 10 + (s[i] - '0');
        if (value > max_value) {
            return 0;
        }
    }
    *out = value;
    return 1;
}

int process_transmission(const char *line)
{
    char buffer[201];
    char decoded[161];
    char *sender;
    char *key_str;
    char *checksum_str;
    char *message;
    size_t len;
    size_t i;
    int pipes = 0;
    int key;
    int checksum;

    if (line == NULL) {
        return print_invalid();
    }

    len = strlen(line);

    for (i = 0; i < len; i++) {
        if (line[i] == '|') {
            pipes++;
        } else if (line[i] == ' ') {
            return print_invalid();
        }
    }
    if (pipes != 3) {
        return print_invalid();
    }

    strcpy(buffer, line);
    sender = strtok(buffer, "|");
    key_str = strtok(NULL, "|");
    checksum_str = strtok(NULL, "|");
    message = strtok(NULL, "|");
    if (sender == NULL || key_str == NULL || checksum_str == NULL || message == NULL) {
        return print_invalid();
    }

    len = strlen(sender);
    if (len < 1 || len > 24 || !is_valid_chars(sender)) {
        return print_invalid();
    }

    if (!parse_number(key_str, 25, &key)) {
        return print_invalid();
    }

    if (strlen(checksum_str) != 2 || !parse_number(checksum_str, 96, &checksum)) {
        return print_invalid();
    }

    len = strlen(message);
    if (len < 1 || len > 160 || !is_valid_chars(message)) {
        return print_invalid();
    }

    if (!decode_message(message, key, decoded, sizeof(decoded))) {
        return print_invalid();
    }

    if (calculate_checksum(decoded) != checksum) {
        return print_invalid();
    }

    printf("[%s] %s\n", sender, decoded);
    return 1;
}

int decode_advanced(const char *encoded, const int *keys, size_t key_count, char *decoded, size_t capacity)
{
    /* Optional TODO. */
    (void)encoded;
    (void)keys;
    (void)key_count;
    (void)decoded;
    (void)capacity;
    return 0;
}

