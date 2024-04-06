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

/// @brief Initialize a hashtable with given size. Must be a power of 2.
/// @param capacity the hashtable's capacity, must be power of 2.
/// @return Pointer to the new hashtable.
hashtable_t* hashtable_init(uint32_t capacity);

/// @brief Cleanup the given hashtable.
/// @param hashtable the hashtable to cleanup.
/// @return Nothing.
void hashtable_cleanup(hashtable_t* hashtable);

/// @brief Resize a hashtable to a new size, larger or smaller.
/// @param hashtable the hashtable to resize.
/// @param new_capacity the new size of the hashtable.
/// @return The size after the resize operation.  Might not equal new_capacity.
uint32_t hashtable_resize(hashtable_t* hashtable, uint32_t new_capacity);

/// @brief Squash a hashtable to it's smallest possible memory footprint.
/// @param hashtable the hashtable to squash.
/// @return The new size of the hashtable.
uint32_t hashtable_squash(hashtable_t* hashtable);

/// @brief Clear a hashtable of all elements.
/// @param hashtable the hashtable to clear.
/// @return The number of items deleted from the table.
uint32_t hashtable_clear(hashtable_t* hashtable);

/// @brief Swap 2 hashtables.
/// @param rhs rhs hashtable to swap.
/// @param lhs lhs hashtable to swap.
/// @return Nothing.
void hashtable_swap(hashtable_t* rhs, hashtable_t* lhs);

/// @brief Merge the src hashtable into the dest hashtable and indicate any conflict. dest values preferred if conflict.
/// @param dest The hashtable that will accept new elements.
/// @param src The hashtable to be merged into dest.
/// @return True if there were any key conflicts, false otherwise.
bool hashtable_merge(hashtable_t* dest, hashtable_t* src);

/// @brief Make a deep copy of the passed hashtable.
/// @param hashtable The hashtable to copy.
/// @return The deep copy of the passed hashtable.
hashtable_t* hashtable_copy(hashtable_t* hashtable);

/// @brief Insert a key and value into the hashtable with extended options.
/// @param hashtable The hashtable to insert into.
/// @param key The key to insert.
/// @param value The corresponding value to insert.
/// @param auto_resize Whether or not this insertion should be allowed to trigger a resize.
/// @param move Whether or not to move in key opposed to copying.
/// @param sso_len length of sso key if key in sso form. -1 if normal string key.
/// @return Cell info with insertion status. If success, cell pointer points to inserted cell. NULL otherwise.
cell_info_t hashtable_insert_(hashtable_t* hashtable, char* key, value_type value, bool auto_resize, bool move, uint8_t sso_len);

/// @brief Insert a key and value into the hashtable with a key in normal form.
/// @param hashtable The hashtable to insert into.
/// @param key The key to insert.
/// @param value The corresponding value to insert.
/// @return Cell info with insertion status. If success, cell pointer points to inserted cell. NULL otherwise.
cell_info_t hashtable_insert(hashtable_t* hashtable, char* key, value_type value);

/// @brief Lookup from the hashtable with a key in normal form.
/// @param hashtable The hashtable to lookup in.
/// @param key The key to search.
/// @return Cell info with lookup status. If success, cell pointer points to found cell. NULL otherwise.
cell_info_t hashtable_lookup(hashtable_t* hashtable, char* key);

/// @brief Lookup from the hashtable with a key in sso form.
/// @param hashtable The hashtable to lookup in.
/// @param key The key to search, in sso form.
/// @param sso_len The length of the sso key.
/// @return Cell info with lookup status. If success, cell pointer points to found cell. NULL otherwise.
cell_info_t hashtable_lookup_sso(hashtable_t* hashtable, char* key, uint8_t sso_len);

/// @brief Delete from the hashtable with a key in normal form.
/// @param hashtable The hashtable to delete from.
/// @param key The key to delete.
/// @return Cell info with deletion status. Cell pointer will always be NULL
cell_info_t hashtable_delete(hashtable_t* hashtable, char* key);

/// @brief Delete from the hashtable with a key in sso form.
/// @param hashtable The hashtable to delete from.
/// @param key The key to delete, in sso form.
/// @param sso_len The length of the sso key.
/// @return Cell info with deletion status. Cell pointer will always be NULL
cell_info_t hashtable_delete_sso(hashtable_t* hashtable, char* key, uint8_t sso_len);

/// @brief Given a cell, check whether the key is in sso form.
/// @param cell The cell in question.
/// @return True if key is in sso form, false otherwise.
bool key_is_sso(cell_t cell);

/// @brief Converts a char* from sso representation to a true string.  User responsible for freeing.
/// @param sso The sso key in question.
/// @param sso_len The length of the sso string.
/// @return True string representation of the sso string.
char* decode_sso_key(char* sso, uint8_t sso_len);

#endif //INCLUDE_HASHTABLE_H