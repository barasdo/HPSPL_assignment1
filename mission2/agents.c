#include "agents.h"

Agent *find_agent(Agent *agents, size_t count, int id)
{
    Agent *end;

    if (agents == NULL || count == 0)
    {
        return NULL;
    }

    end = agents + count;
    for (; agents < end; agents++)
    {
        if (agents->id == id)
        {
            return agents;
        }
    }
    return NULL;
}

int copy_agents(Agent *dest, size_t capacity, const Agent *source, size_t count)
{
    size_t i;

    if (count == 0)
    {
        return 1;
    }
    if (dest == NULL || source == NULL || capacity < count)
    {
        return 0;
    }

    for (i = 0; i < count; i++)
    {
        const Agent *s = &source[i];
        Agent *d = &dest[i];
        *d = *s;
    }
    return 1;
}

int update_agent(Agent *agent, AgentPatch patch)
{
    Agent copy;

    if (agent == NULL || patch.agent_id != agent->id)
    {
        return 0;
    }
    if (patch.new_clearance < 0 || patch.new_clearance > 5)
    {
        return 0;
    }
    if (patch.new_active != 0 && patch.new_active != 1)
    {
        return 0;
    }

    copy = *agent;
    copy.clearance = patch.new_clearance;
    copy.active = patch.new_active;
    *agent = copy;
    return 1;
}

int apply_patch(Agent *agents, size_t count, const AgentPatch *patch)
{
    Agent *agent;

    if (agents == NULL || patch == NULL)
    {
        return 0;
    }
    if (patch->new_clearance < 0 || patch->new_clearance > 5)
    {
        return 0;
    }
    if (patch->new_active != 0 && patch->new_active != 1)
    {
        return 0;
    }

    agent = find_agent(agents, count, patch->agent_id);
    if (agent == NULL)
    {
        return 0;
    }

    return update_agent(agent, *patch);
}

int classify_access(const Agent *agents, size_t count, const AccessEvent *event)
{
    const Agent *p;
    const Agent *end;
    int authorized = 0;

    if (event == NULL || (agents == NULL && count != 0))
    {
        return ACCESS_INVALID;
    }
    if (event->agent_id <= 0)
    {
        return ACCESS_INVALID;
    }
    if (event->resource_id != BGU_CAMERAS &&
        event->resource_id != CONTROL_SYSTEM &&
        event->resource_id != AIR_SUPPORT &&
        event->resource_id != EVIL_MORTY_PRIVATE_COMMS &&
        event->resource_id != SERVER_BACKUP &&
        event->resource_id != PORT_8080)
    {
        return ACCESS_INVALID;
    }
    if (event->required_clearance < 0 || event->required_clearance > 5)
    {
        return ACCESS_INVALID;
    }
    if (event->granted != 0 && event->granted != 1)
    {
        return ACCESS_INVALID;
    }
    if (event->sequence < -1 || event->sequence > 7)
    {
        return ACCESS_INVALID;
    }
    if (event->encoded_symbol < -1 || event->encoded_symbol > 25)
    {
        return ACCESS_INVALID;
    }

    end = agents + count;
    for (p = agents; p < end; p++)
    {
        if (p->id == event->agent_id)
        {
            if (p->active && p->clearance >= event->required_clearance)
            {
                authorized = 1;
            }
            break;
        }
    }

    if (authorized)
    {
        return event->granted ? ACCESS_OK : ACCESS_UNEXPECTED_DENIAL;
    }
    return event->granted ? ACCESS_UNAUTHORIZED_GRANTED : ACCESS_DENIED;
}

int recover_code(const Agent *agents, size_t agent_count, const AccessEvent *events, size_t event_count, char *code, size_t capacity)
{
    char letters[8];
    int found[8] = {0};
    size_t i;
    int pos;

    if (code != NULL && capacity > 0)
        code[0] = '\0';

    if (code == NULL || capacity < 9 || events == NULL ||
        (agents == NULL && agent_count != 0))
        return 0;

    for (i = 0; i < event_count; i++)
    {
        const AccessEvent *e = &events[i];

        if (e->agent_id != 67 ||
            classify_access(agents, agent_count, e) != ACCESS_UNAUTHORIZED_GRANTED ||
            e->sequence < 0 || e->sequence > 7 ||
            e->encoded_symbol < 0 || e->encoded_symbol > 25)
            continue;

        if (found[e->sequence])
            return 0;

        found[e->sequence] = 1;
        letters[e->sequence] = (char)('A' + ((e->encoded_symbol - e->sequence + 26) % 26));
    }

    for (pos = 0; pos < 8; pos++)
    {
        if (!found[pos])
            return 0;
    }

    for (pos = 0; pos < 8; pos++)
        code[pos] = letters[pos];
    code[8] = '\0';
    return 1;
}
