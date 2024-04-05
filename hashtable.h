/*
Author: Dante Crescenzi
Last Modif: 28 Mar 2024
Description: string -> any hashtable interface

This header describes the interface to create, interact with and cleanup a string -> any
hashtable. The user can specify whatever value type needed in the hashmap via the typedef
below, simply use it to typedef the desired type to 'value_type'.

NOTE: the stock hashmap was developed without memory management of value in mind. However, if
it is needed (ex if value_type is char*), ctrl+f for "<customize>" in hashtable.c and add in 
whatever resource management is needed. Keys are memory managed automatically - please leave
all memory mamangement of keys (and eventually values, once customized) to the provided
functions.

The user can specify the max load factor to reach before automatic resizing (default 0.75),
and whether or not logs should be printed.  If logs are off performance is not affected,
as with a high enough -O level the compiler will optimize all log calls and checks away
if logs are off.

The unit tests only work with int value_type but can be converted to use any.  To run them,
run 'make test' or just 'make'.
*/

#ifndef INCLUDE_HASHTABLE_H
#define INCLUDE_HASHTABLE_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

//TODO RECOMMENT AND CLEAN ALL THIS!!!!!!

typedef /*value type here ->*/ int /*<-*/ value_type;

//specify max load factor, and logging
#define MAX_LOAD_FACTOR 0.75
#define hashtable_logs false

//struct to represent a cell of the hashtable.
//assuming int value, size stays at 16 (sso_len removes 1 byte of padding)
//-1 sso_len means key treated as normal string. 0 is no key, anything else sso.
typedef struct
{
    char* key;
    value_type value;
    int8_t sso_len;
} cell_t;

//struct to represent a hashtable.
typedef struct
{
    uint32_t capacity;
    uint32_t size;
    cell_t* data;
} hashtable_t;

//possible results of insert/lookup/delete
typedef enum
{
    OK,
    DUPLICATE_KEY,
    KEY_NOT_FOUND,
    HASHTABLE_FULL,
    EMPTY_KEY
} STATUS;

//iterator-like struct to return insert/lookup/delete info
typedef struct
{
    cell_t* cell;
    STATUS status;
} cell_info_t;

//initialize a hashtable with passed capacity, which must be a power of 2.
//returns a pointer to the new hashtable
hashtable_t* hashtable_init(uint32_t capacity);

//cleanup the passed hashtable.
//NOTE: needs customization if value_type requires special management.
void hashtable_cleanup(hashtable_t* hashtable);

//resize the given hashtable to new_capacity, if possible.
//returns the new capacity of the hashtable. 
uint32_t hashtable_resize(hashtable_t* hashtable, uint32_t new_capacity);

//squash the given hashtable to it's smallest possible memory footprint.
//returns the new capacity of the hashtable.
uint32_t hashtable_squash(hashtable_t* hashtable);

//clear the given hashtable, making it empty.
//returns the number of deleted items.
//NOTE: needs customization if value_type requires special management.
uint32_t hashtable_clear(hashtable_t* hashtable);

//swap the 2 passed hashtables.
void hashtable_swap(hashtable_t* rhs, hashtable_t* lhs);

//merge the src hashtable into dest, leaving src unchanged
//if there is a key conflict, the value in dest takes precedence.
//returns true if there was a key conflict found, false otherwise.
bool hashtable_merge(hashtable_t* dest, hashtable_t* src);

//perform a deep copy of the passed hashtable.
//returns a pointer to the copy.
hashtable_t* hashtable_copy(hashtable_t* hashtable);

//insert a key value pair into the passed hashtable, with flags to control automatic resizing and
//moving keys/values behavior.
//returns a cell_info_t, with status and pointer to cell if insertion succeeded (NULL otherwise).
//NOTE: needs customization if value_type requires special management.
cell_info_t hashtable_insert_(hashtable_t* hashtable, char* key, value_type value, bool auto_resize, bool move, uint8_t sso_len);

//insert a key value pair into the passed hashtable, and automatically resize if need be.
//automatically copies key over, if moving preferred use above function.
//returns a cell_info_t, with status and pointer to cell if insertion succeeded (NULL otherwise).
cell_info_t hashtable_insert(hashtable_t* hashtable, char* key, value_type value);

//lookup a key value pair in the passed hashtable
//returns a cell_info_t, with status and pointer to cell if lookup succeeded (NULL otherwise).
cell_info_t hashtable_lookup(hashtable_t* hashtable, char* key);

//lookup a key value pair in the passed hashtable, where the key arg is an sso
//note this doesnt mean if key arg *can* be. It means only use this variant of lookup is key *is* represented in sso.
//returns a cell_info_t, with status and pointer to cell if lookup succeeded (NULL otherwise).
cell_info_t hashtable_lookup_sso(hashtable_t* hashtable, char* key, uint8_t sso_len);

//delete a key value pair in the passed hashtable
//returns a cell_info_t, with status of deletion (cell pointer always NULL)
//NOTE: needs customization if value_type requires special management.
cell_info_t hashtable_delete(hashtable_t* hashtable, char* key);

//delete a key value pair in the passed hashtable, where key arg is sso
//note this doesnt mean if key arg *can* be. It means only use this variant of delete is key *is* represented in sso.
//returns a cell_info_t, with status of deletion (cell pointer always NULL)
//NOTE: needs customization if value_type requires special management.
cell_info_t hashtable_delete_sso(hashtable_t* hashtable, char* key, uint8_t sso_len);

#endif //INCLUDE_HASHTABLE_H