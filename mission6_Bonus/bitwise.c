#include "bitwise.h"

static int digit_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

int parse_base_number(const char *text, unsigned base, uint32_t *out)
{
    uint32_t value = 0;
    size_t i = 0;

    if (text == NULL || out == NULL)
        return 0;
    if (base != 2 && base != 10 && base != 16)
        return 0;

    if (text[0] == '0') {
        if (base == 2 && (text[1] == 'b' || text[1] == 'B'))
            i = 2;
        else if (base == 16 && (text[1] == 'x' || text[1] == 'X'))
            i = 2;
    }
    if (text[i] == '\0')
        return 0;

    for (; text[i] != '\0'; i++) {
        int digit = digit_value(text[i]);

        if (digit < 0 || (unsigned)digit >= base)
            return 0;
        if (value > (UINT32_MAX - (uint32_t)digit) / base)
            return 0;
        value = value * base + (uint32_t)digit;
    }

    *out = value;
    return 1;
}

int format_binary(uint32_t value, unsigned width, char *out, size_t capacity)
{
    unsigned i;

    if (out != NULL && capacity > 0)
        out[0] = '\0';
    if (out == NULL || width < 1 || width > 32 || capacity < (size_t)width + 1)
        return 0;
    if (width < 32 && (value >> width) != 0)
        return 0;

    for (i = 0; i < width; i++)
        out[i] = ((value >> (width - 1 - i)) & 1u) ? '1' : '0';
    out[width] = '\0';
    return 1;
}

int decode_twos_complement(uint32_t raw, unsigned bits, int32_t *out)
{
    if (out == NULL || bits < 2 || bits > 16)
        return 0;
    if ((raw >> bits) != 0)
        return 0;

    if (raw & (1u << (bits - 1)))
        *out = (int32_t)raw - (int32_t)(1u << bits);
    else
        *out = (int32_t)raw;
    return 1;
}

int encode_twos_complement(int32_t value, unsigned bits, uint32_t *out)
{
    int32_t limit;

    if (out == NULL || bits < 2 || bits > 16)
        return 0;

    limit = (int32_t)1 << (bits - 1);
    if (value < -limit || value >= limit)
        return 0;

    if (value < 0)
        *out = (uint32_t)(value + 2 * limit);
    else
        *out = (uint32_t)value;
    return 1;
}

int unpack_packet(uint16_t word, PacketFields *out)
{
    if (out == NULL)
        return 0;

    out->type = (uint8_t)(word >> 12);
    out->index = (uint8_t)((word >> 8) & 0xF);
    out->payload = (uint8_t)(word & 0xFF);
    return 1;
}

int pack_packet(const PacketFields *fields, uint16_t *out)
{
    if (fields == NULL || out == NULL)
        return 0;
    if (fields->type > 15 || fields->index > 15)
        return 0;

    *out = (uint16_t)((fields->type << 12) | (fields->index << 8) | fields->payload);
    return 1;
}

int decode_channel(const PacketFields *packets, size_t count,
                   uint8_t type, uint8_t key,
                   char *out, size_t capacity)
{
    uint8_t chars[16] = {0};
    int seen[16] = {0};
    size_t length = 0;
    size_t i;

    if (out != NULL && capacity > 0)
        out[0] = '\0';
    if (out == NULL || capacity == 0 || (packets == NULL && count != 0))
        return 0;

    for (i = 0; i < count; i++) {
        if (packets[i].type != type)
            continue;
        if (packets[i].index > 15 || seen[packets[i].index])
            return 0;
        seen[packets[i].index] = 1;
        chars[packets[i].index] = (uint8_t)(packets[i].payload ^ key);
        length++;
    }

    if (length < 2 || capacity < length)
        return 0;
    for (i = 0; i < 16; i++) {
        if (seen[i] != (i < length))
            return 0;
    }
    if (chars[length - 1] != 0)
        return 0;
    for (i = 0; i + 1 < length; i++) {
        if (chars[i] < 0x20 || chars[i] > 0x7E)
            return 0;
    }

    for (i = 0; i + 1 < length; i++)
        out[i] = (char)chars[i];
    out[length - 1] = '\0';
    return 1;
}

int parse_meeting_record(const uint8_t raw[10], MeetingRecord *out)
{
    MeetingRecord record;
    MeetingRecord empty = {0, 0, 0, 0, 0, 0};
    uint8_t checksum = 0;
    int32_t adjust;
    int final_minute;
    int i;

    if (out == NULL)
        return 0;
    *out = empty;
    if (raw == NULL)
        return 0;

    for (i = 0; i < 9; i++)
        checksum ^= raw[i];
    if (checksum != raw[9] || raw[7] != 0)
        return 0;

    record.meeting_id = (uint16_t)(raw[0] | (raw[1] << 8));
    record.base_minute = (uint16_t)(raw[2] | (raw[3] << 8));
    record.location_code = (uint16_t)(raw[4] | (raw[5] << 8));
    record.flags = raw[6];
    if (!decode_twos_complement(raw[8], 8, &adjust))
        return 0;
    record.minute_adjust = (int16_t)adjust;

    if (record.meeting_id == 0 || record.location_code == 0 ||
        record.base_minute > 1439)
        return 0;
    if ((record.flags & 0xFC) != 0 || (record.flags & 3) == 3)
        return 0;

    final_minute = (int)record.base_minute + record.minute_adjust;
    if (final_minute < 0 || final_minute > 1439)
        return 0;
    record.final_minute = (uint16_t)final_minute;

    *out = record;
    return 1;
}

int find_confirmed_meeting(FILE *input, MeetingRecord *out)
{
    MeetingRecord empty = {0, 0, 0, 0, 0, 0};
    MeetingRecord found = {0, 0, 0, 0, 0, 0};
    MeetingRecord record;
    uint8_t raw[10];
    size_t bytes;
    int matches = 0;

    if (out == NULL)
        return 0;
    *out = empty;
    if (input == NULL)
        return 0;

    while ((bytes = fread(raw, 1, 10, input)) == 10) {
        if (parse_meeting_record(raw, &record) &&
            (record.flags & 1) && !(record.flags & 2)) {
            found = record;
            matches++;
        }
    }
    if (bytes != 0 || ferror(input) || matches != 1)
        return 0;

    *out = found;
    return 1;
}

int resolve_location(const PacketFields *packets, size_t count,
                     uint8_t key, uint16_t location_code,
                     char *out, size_t capacity)
{
    char message[16] = {0};
    uint32_t code = 0;
    size_t length = 0;
    size_t i;

    if (out != NULL && capacity > 0)
        out[0] = '\0';
    if (out == NULL || capacity == 0)
        return 0;
    if (!decode_channel(packets, count, 14, key, message, sizeof(message)))
        return 0;

    if (message[0] != 'L' || message[1] != 'O' || message[2] != 'C' ||
        message[3] != ':' || message[8] != '=')
        return 0;

    for (i = 4; i < 8; i++) {
        int digit = digit_value(message[i]);

        if (digit < 0)
            return 0;
        code = code * 16 + (uint32_t)digit;
    }
    if (code != location_code)
        return 0;

    while (message[9 + length] != '\0') {
        if (message[9 + length] < 'A' || message[9 + length] > 'Z')
            return 0;
        length++;
    }
    if (length == 0 || capacity < length + 1)
        return 0;

    for (i = 0; i < length; i++)
        out[i] = message[9 + i];
    out[length] = '\0';
    return 1;
}
