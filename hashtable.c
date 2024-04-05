/*
Author: Dante Crescenzi
Last Modif: 28 Mar 2024
Description: impl of hashtable methods outlined in hashtable.h
*/

#include "hashtable.h"

typedef enum
{
    INFO,
    WARN,
    ERROR
} LOG_TYPE;

void hashtable_log(LOG_TYPE type, const char* location, const char* fmt, ...)
{
    const char* RED = "\e[1;31m";
    const char* YELLOW = "\e[1;33m";
    const char* WHITE = "\e[1;37m";
    const char* RESET = "\x1b[0m";

    char color[16];
    switch(type)
    {
        case WARN:
            strcpy(color, YELLOW);
            break;
        case ERROR:
            strcpy(color, RED);
            break;
        case INFO:
        default:
            strcpy(color, WHITE);
            break;
    }

    fprintf(stdout, "%s[HASHTABLE::%s]%s ", color, location, RESET);
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    fprintf(stdout, "\n");
}

hashtable_t* hashtable_init(uint32_t capacity)
{
    bool capacity_is_not_power_of_2 = capacity & (capacity - 1);
    if(capacity == 0 || capacity_is_not_power_of_2)
    {
        if(hashtable_logs) hashtable_log(ERROR, "hashtable_init", "capacity %u must be nonzero and a power of two, aborting", capacity);
        return NULL;
    }

    hashtable_t* hashtable = (hashtable_t*)malloc(sizeof(hashtable_t));

    hashtable->capacity = capacity;
    hashtable->size = 0;
    hashtable->data = (cell_t*)malloc(sizeof(cell_t) * capacity);
    for(uint32_t i = 0; i < hashtable->capacity; i++) hashtable->data[i].key = NULL;

    if(hashtable_logs) hashtable_log(INFO, "hashtable_init", "created and initialized hashtable of capacity %u", capacity);
    return hashtable;
}

void hashtable_cleanup(hashtable_t* hashtable)
{
    if(hashtable_logs) hashtable_log(INFO, "hashtable_cleanup", "destroying hashtable of capacity %u with %u elements", hashtable->capacity, hashtable->size);
    for(uint32_t i = 0; i < hashtable->capacity; i++)
    {
        free(hashtable->data[i].key);
        hashtable->data[i].key = NULL;
        //<customize> cleanup any resources tied to value
    }
    free(hashtable->data);
    hashtable->data = NULL;
    free(hashtable);
}

uint32_t hashtable_resize(hashtable_t* hashtable, uint32_t new_capacity)
{
    if(new_capacity == hashtable->capacity) return new_capacity;
    if(new_capacity == 1 << 31)
    {
        if(hashtable_logs) hashtable_log(WARN, "hashtable_resize", "resized to max capacity");
        return new_capacity;
    }
    if(new_capacity < hashtable->size)
    {
        if(hashtable_logs) hashtable_log(ERROR, "hashtable_resize", "new capacity %u too small to hold current elements (%u), aborting", new_capacity, hashtable->size);
        return hashtable->capacity;
    }
    bool capacity_is_not_power_of_2 = new_capacity & (new_capacity - 1);
    if(capacity_is_not_power_of_2)
    {
        if(hashtable_logs) hashtable_log(ERROR, "hashtable_resize", "new capacity %u is not a power of 2, aborting", new_capacity);
        return hashtable->capacity;
    }

    hashtable_t* tmp_hashtable = hashtable_init(new_capacity);

    for(uint32_t i = 0; i < hashtable->capacity; i++)
    {
        cell_t cell = hashtable->data[i];
        if(cell.key == NULL) continue;
        hashtable_insert_(tmp_hashtable, cell.key, cell.value, /*resize*/ false, /*move*/ true);
        hashtable->data[i].key = NULL;
        //<customize> handle the fact that value may have been moved (prevent double free)
    }

    hashtable_swap(hashtable, tmp_hashtable);
    hashtable_cleanup(tmp_hashtable);

    if(hashtable_logs) hashtable_log(INFO, "hashtable_resize", "resized hashtable to new capacity %u", new_capacity);
    return new_capacity;
}

uint32_t hashtable_squash(hashtable_t* hashtable)
{
    uint32_t cur_size = hashtable->size;
    bool size_is_power_of_2 = !(cur_size & (cur_size - 1));
    if(size_is_power_of_2)
    {
        hashtable_resize(hashtable, cur_size);
        if(hashtable_logs) hashtable_log(INFO, "hashtable_squash", "squashed hashtable to min capacity %u", hashtable->capacity);
        return hashtable->capacity;
    }

    int idx = 0;
    while (cur_size >>= 1) idx++;

    if(idx == 31)
    {
        if(hashtable_logs) hashtable_log(WARN, "hashtable_squash", "unable to squash, max capacity needed");
        return hashtable->capacity;
    }
    
    hashtable_resize(hashtable, 1 << (++idx));
    if(hashtable_logs) hashtable_log(INFO, "hashtable_squash", "squashed hashtable to min capacity %u", hashtable->capacity);
    return hashtable->capacity;
}

uint32_t hashtable_clear(hashtable_t* hashtable)
{
    uint32_t num_deletions = 0;
    for(uint32_t i = 0; i < hashtable->capacity; i++)
    {
        num_deletions += (hashtable->data[i].key == NULL ? 0 : 1);
        free(hashtable->data[i].key);
        hashtable->data[i].key = NULL;
        //<customize> cleanup any resources tied to value
    }

    hashtable->size = 0;
    if(hashtable_logs) hashtable_log(INFO, "hashtable_clear", "cleared %u elements from hashtable", num_deletions);
    return num_deletions;
}

void hashtable_swap(hashtable_t* rhs, hashtable_t* lhs)
{
    if(rhs == lhs) return;
    hashtable_t tmp = *rhs;
    *rhs = *lhs;
    *lhs = tmp;
    if(hashtable_logs) hashtable_log(INFO, "hashtable_swap", "swap performed");
}

bool hashtable_merge(hashtable_t* dest, hashtable_t* src)
{
    if(dest == src)
    {
        if(hashtable_logs) hashtable_log(WARN, "hashtable_merge", "merge called on the same hashtable");
        return false;
    }

    bool conflict = false;
    for(uint32_t i = 0; i < src->capacity; i++)
    {
        cell_t cell = src->data[i];
        if(cell.key == NULL) continue;
        cell_info_t info = hashtable_insert_(dest, cell.key, cell.value, /*resize*/ true, /*move*/ false);
        if(hashtable_logs && info.status != OK) hashtable_log(WARN, "hashtable_merge", "found conflicting key '%s' during merge", cell.key);
        conflict |= info.status != OK;
    }
    if(hashtable_logs) hashtable_log(INFO, "hashtable_merge", "finished merge - new size = %u, conflicts = %s", dest->size, conflict ? "Y" : "N");
    return conflict;
}

hashtable_t* hashtable_copy(hashtable_t* hashtable)
{
    hashtable_t* copy = hashtable_init(hashtable->capacity);
    for(uint32_t i = 0; i < hashtable->capacity; i++)
    {
        cell_t cell = hashtable->data[i];
        if(cell.key == NULL) continue;
        hashtable_insert_(copy, cell.key, cell.value, /*resize*/ false, /*move*/ false);
    }
    if(hashtable_logs) hashtable_log(INFO, "hashtable_copy", "copied hashtable of size %u, capacity %u", hashtable->size, hashtable->capacity);
    return copy;
}

uint32_t hash(char* key) //local utility
{
    uint32_t val = 5381;
    int c;

    while(c = *key++) val = ((val << 5) + val) + c;
    return val;
}

uint32_t mod(uint32_t n, uint32_t d) //local utility
{
    return n & (d - 1);
}

cell_info_t hashtable_insert_(hashtable_t* hashtable, char* key, value_type value, bool auto_resize, bool move)
{
    cell_info_t insertion_result;
    insertion_result.cell = NULL;

    if(hashtable->size == hashtable->capacity)
    {
        if(hashtable_logs) hashtable_log(WARN, "hashtable_insert", "insertion of key '%s' failed, size has reached capacity %u", key, hashtable->capacity);
        insertion_result.status = HASHTABLE_FULL;
        return insertion_result;
    }

    uint32_t base_idx = hash(key);
    int probe = 0;

    do
    {
        uint32_t unmod_idx = base_idx + ((probe*(probe+1)) >> 1);
        uint32_t idx = mod(unmod_idx, hashtable->capacity);

        cell_t cell = hashtable->data[idx];
        if(!cell.key) //empty slot
        {
            if(move) //move in key and value
            {
                hashtable->data[idx].key = key;
                //<customize> properly handle resources while moving passed value to cell value
                hashtable->data[idx].value = value;
            }
            else //copy over key and value
            {
                size_t key_len = strlen(key);
                hashtable->data[idx].key = (char*)malloc((sizeof(char)*key_len) + 1);
                strcpy(hashtable->data[idx].key, key);
                //<customize> properly handle resources while assigning passed value to cell value
                hashtable->data[idx].value = value;
            }

            insertion_result.status = OK;
            insertion_result.cell = &hashtable->data[idx];
            hashtable->size++;
            if(hashtable_logs) hashtable_log(INFO, "hashtable_insert", "insertion of key '%s' succeeded", key);
            break;
        }
        else if(*key == *cell.key && strcmp(cell.key, key) == 0) //duplicate key
        {
            insertion_result.status = DUPLICATE_KEY;
            insertion_result.cell = &hashtable->data[idx];
            if(hashtable_logs) hashtable_log(WARN, "hashtable_insert", "insertion of key '%s' failed, duplicate key found", key);
            break;
        }
        probe++;
    } while(true); //ok since passed first check

    double load_factor = (double)hashtable->size / hashtable->capacity;
    if(auto_resize &&
       load_factor > MAX_LOAD_FACTOR && 
       insertion_result.status == OK && 
       hashtable->capacity < 1 << 31)
    {
        if(hashtable_logs) hashtable_log(INFO, "hashtable_insert", "insertion of key '%s' triggered resize to %u", key, hashtable->capacity << 1);
        hashtable_resize(hashtable, hashtable->capacity << 1);
        insertion_result = hashtable_lookup(hashtable, key);
    }

    return insertion_result;
}

cell_info_t hashtable_insert(hashtable_t* hashtable, char* key, value_type value)
{
    return hashtable_insert_(hashtable, key, value, /*resize*/ true, /*move*/ false);
}

cell_info_t hashtable_lookup(hashtable_t* hashtable, char* key)
{
    cell_info_t lookup_result;
    lookup_result.cell = NULL;

    uint32_t base_idx = hash(key);
    int probe = 0;

    do
    {
        uint32_t unmod_idx = base_idx + ((probe*(probe+1)) >> 1);
        uint32_t idx = mod(unmod_idx, hashtable->capacity);

        if(probe && idx == mod(base_idx, hashtable->capacity)) //full table cycle case
        {
            lookup_result.status = KEY_NOT_FOUND;
            if(hashtable_logs) hashtable_log(INFO, "hashtable_lookup", "lookup of key '%s' failed, not found", key);
            break; 
        }

        cell_t cell = hashtable->data[idx];
        if(!cell.key) //empty slot
        {
            lookup_result.status = KEY_NOT_FOUND;
            if(hashtable_logs) hashtable_log(INFO, "hashtable_lookup", "lookup of key '%s' failed, not found", key);
            break;
        }
        else if(strcmp(cell.key, key) == 0) //key found
        {
            lookup_result.status = OK;
            if(hashtable_logs) hashtable_log(INFO, "hashtable_lookup", "lookup of key '%s' succeeded", key);
            lookup_result.cell = &hashtable->data[idx];
            break;
        }
        probe++;
    } while(true); //ok since passed first check

    return lookup_result;
}

cell_info_t hashtable_delete(hashtable_t* hashtable, char* key)
{
    cell_info_t lookup_result = hashtable_lookup(hashtable, key);
    if(lookup_result.status == KEY_NOT_FOUND)
    {
        if(hashtable_logs) hashtable_log(WARN, "hashtable_delete", "deletion of key '%s' failed, not found", key);
        return lookup_result;
    }

    free(lookup_result.cell->key);
    lookup_result.cell->key = NULL;
    //<customize> properly delete resources while deleting cell value
    hashtable->size--;

    lookup_result.cell = NULL;
    if(hashtable_logs) hashtable_log(INFO, "hashtable_delete", "deletion of key '%s' succeeded", key);
    return lookup_result;
}