#include "messages.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int store_init(MessageStore *store, size_t initial_capacity)
{
    if (store == NULL)
    {
        return 0;
    }

    store->messages = NULL;
    store->count = 0;
    store->capacity = 0;

    if (initial_capacity == 0)
    {
        return 1;
    }
    if (initial_capacity > SIZE_MAX / sizeof(Message))
    {
        return 0;
    }

    store->messages = calloc(initial_capacity, sizeof(Message));
    if (store->messages == NULL)
    {
        return 0;
    }
    store->capacity = initial_capacity;
    return 1;
}

int message_init(Message *msg, int id, const char *text)
{
    size_t size;
    char *copy;

    if (msg == NULL || text == NULL || id <= 0)
    {
        return 0;
    }

    size = strlen(text) + 1;
    copy = malloc(size);
    if (copy == NULL)
    {
        return 0;
    }
    memcpy(copy, text, size);

    msg->id = id;
    msg->text = copy;
    return 1;
}

int store_reserve(Message **messages, size_t *capacity, size_t required)
{
    size_t new_capacity;
    size_t i;
    Message *bigger;

    if (messages == NULL || capacity == NULL)
    {
        return 0;
    }
    if (*capacity >= required)
    {
        return 1;
    }

    new_capacity = (*capacity == 0) ? 4 : *capacity;
    while (new_capacity < required)
    {
        if (new_capacity > SIZE_MAX / 2)
        {
            new_capacity = required;
        }
        else
        {
            new_capacity *= 2;
        }
    }
    if (new_capacity > SIZE_MAX / sizeof(Message))
    {
        return 0;
    }

    bigger = realloc(*messages, new_capacity * sizeof(Message));
    if (bigger == NULL)
    {
        return 0;
    }

    for (i = *capacity; i < new_capacity; i++)
    {
        bigger[i].id = 0;
        bigger[i].text = NULL;
    }

    *messages = bigger;
    *capacity = new_capacity;
    return 1;
}

int store_append(MessageStore *store, int id, const char *text)
{
    if (store == NULL || text == NULL || id <= 0)
    {
        return 0;
    }
    if (!store_reserve(&store->messages, &store->capacity, store->count + 1))
    {
        return 0;
    }
    if (!message_init(&store->messages[store->count], id, text))
    {
        return 0;
    }

    store->count++;
    return 1;
}

void message_free(Message *msg)
{
    if (msg == NULL)
    {
        return;
    }

    free(msg->text);
    msg->text = NULL;
    msg->id = 0;
}

void store_free(MessageStore *store)
{
    size_t i;

    if (store == NULL)
    {
        return;
    }

    for (i = 0; i < store->count; i++)
    {
        message_free(&store->messages[i]);
    }
    free(store->messages);

    store->messages = NULL;
    store->count = 0;
    store->capacity = 0;
}

int store_clone(MessageStore *dest, const MessageStore *source)
{
    size_t i;

    if (dest == NULL || source == NULL)
    {
        return 0;
    }
    if (!store_init(dest, source->count))
    {
        return 0;
    }

    for (i = 0; i < source->count; i++)
    {
        const Message *m = &source->messages[i];

        if (!store_append(dest, m->id, m->text))
        {
            store_free(dest);
            return 0;
        }
    }
    return 1;
}
