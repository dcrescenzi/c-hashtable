/*
Author: Dante Crescenzi
Last Modif: 28 Mar 2024
Description: unit tests for hashtable functionality
*/

#include "hashtable_test.h"

//INIT TESTS (prefixed with hashtable_init_should)
bool reject_empty_size()
{
    hashtable_t* htb = hashtable_init(0);
    return htb == NULL;
}

bool reject_non_power_of_2_size()
{
    bool pass = true;
    hashtable_t* htb;

    htb = hashtable_init(3);
    pass &= htb == NULL;

    htb = hashtable_init(22);
    pass &= htb == NULL;

    htb = hashtable_init(11111);
    pass &= htb == NULL;

    return pass;
}

bool properly_initialize_members()
{
    bool pass = true;
    hashtable_t* htb;

    htb = hashtable_init(1 << 10);
    pass &= htb != NULL;
    pass &= htb->size == 0;
    pass &= htb->capacity == 1 << 10;
    
    hashtable_cleanup(htb);
    return pass;
}

//RESIZE TESTS (prefixed with hashtable_resize_should)
bool properly_upsize()
{
    bool pass = true;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);

    pass &= htb->capacity == 8;
    pass &= htb->size == 5;

    hashtable_resize(htb, 1 << 5);
    pass &= htb->capacity == 1 << 5;
    pass &= htb->size == 5;

    hashtable_cleanup(htb);
    return pass;
}

bool properly_downsize()
{
    bool pass = true;
    hashtable_t* htb = hashtable_init(1 << 10);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);

    pass &= htb->capacity == 1 << 10;
    pass &= htb->size == 5;

    hashtable_resize(htb, 1 << 3);
    pass &= htb->capacity == 1 << 3;
    pass &= htb->size == 5;

    hashtable_cleanup(htb);
    return pass;
}

//SQUASH TESTS (prefixed with hashtable_squash_should)
bool squash_for_power_of_2_size()
{
    bool pass = true;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    pass &= htb->size == 4;
    pass &= htb->capacity == 8;

    hashtable_squash(htb);
    pass &= htb->size == 4;
    pass &= htb->capacity == 4;

    hashtable_cleanup(htb);
    return pass;
}

bool squash_for_general_size()
{
    bool pass = true;
    hashtable_t* htb = hashtable_init(1 << 5);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    hashtable_insert(htb, "key6", 6);
    hashtable_insert(htb, "key7", 7);
    pass &= htb->size == 7;
    pass &= htb->capacity == 1 << 5;

    hashtable_squash(htb);
    pass &= htb->size == 7;
    pass &= htb->capacity == 8;

    hashtable_cleanup(htb);
    return pass;
}

//CLEAR TESTS (prefixed with hashtable_clear_should)
bool properly_clear()
{
    bool pass = true;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    hashtable_insert(htb, "key6", 6);
    hashtable_insert(htb, "key7", 7);
    pass &= htb->size == 7;
    pass &= htb->capacity == 16;

    hashtable_clear(htb);
    pass &= htb->size == 0;
    pass &= htb->capacity == 16;

    cell_info_t lookup = hashtable_lookup(htb, "key3");
    pass &= lookup.status == KEY_NOT_FOUND;

    hashtable_cleanup(htb);
    return pass;
}

//SWAP TESTS (prefixed with hashtable_swap_should)
bool properly_swap()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);
    hashtable_t* htb2 = hashtable_init(1 << 3);

    hashtable_insert(htb, "1key1", 1);
    hashtable_insert(htb, "1key2", 2);
    hashtable_insert(htb, "1key3", 3);

    hashtable_insert(htb2, "2key1", 1);
    hashtable_insert(htb2, "2key2", 2);
    hashtable_insert(htb2, "2key3", 3);
    hashtable_insert(htb2, "2key4", 4);
    hashtable_insert(htb2, "2key5", 5);
    
    pass &= htb->size == 3;
    pass &= htb2->size == 5;

    lookup = hashtable_lookup(htb, "1key3");
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb2, "2key2");
    pass &= lookup.status == OK;

    hashtable_swap(htb, htb2);

    pass &= htb2->size == 3;
    pass &= htb->size == 5;

    lookup = hashtable_lookup(htb2, "1key3");
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb, "2key2");
    pass &= lookup.status == OK;

    hashtable_cleanup(htb);
    hashtable_cleanup(htb2);
    return pass;
}

bool handle_self_swap()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    
    pass &= htb->size == 5;

    lookup = hashtable_lookup(htb, "key3");
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb, "key2");
    pass &= lookup.status == OK;

    hashtable_swap(htb, htb);

    pass &= htb->size == 5;

    lookup = hashtable_lookup(htb, "key3");
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb, "key2");
    pass &= lookup.status == OK;

    hashtable_cleanup(htb);
    return pass;
}

//MERGE TESTS (prefixed with hashtable_merge_should)
bool properly_merge()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);
    hashtable_t* htb2 = hashtable_init(1 << 3);

    hashtable_insert(htb, "1key1", 1);
    hashtable_insert(htb, "1key2", 2);
    hashtable_insert(htb, "1key3", 3);

    hashtable_insert(htb2, "2key1", 1);
    hashtable_insert(htb2, "2key2", 2);
    hashtable_insert(htb2, "2key3", 3);
    hashtable_insert(htb2, "2key4", 4);
    hashtable_insert(htb2, "2key5", 5);
    
    pass &= htb->size == 3;
    pass &= htb2->size == 5;

    hashtable_merge(htb, htb2);

    pass &= htb->size == 8;
    pass &= htb->capacity == 16; //should have resized

    lookup = hashtable_lookup(htb, "1key3");
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb, "2key2");
    pass &= lookup.status == OK;

    hashtable_cleanup(htb);
    hashtable_cleanup(htb2);
    return pass;
}

bool handle_and_indicate_conflict()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);
    hashtable_t* htb2 = hashtable_init(1 << 3);

    hashtable_insert(htb, "1key1", 1);
    hashtable_insert(htb, "1key2", 2);
    hashtable_insert(htb, "1key3", 3);

    hashtable_insert(htb2, "1key1", 7);
    hashtable_insert(htb2, "2key2", 2);
    hashtable_insert(htb2, "2key3", 3);
    hashtable_insert(htb2, "2key4", 4);
    hashtable_insert(htb2, "2key5", 5);
    
    pass &= htb->size == 3;
    pass &= htb2->size == 5;

    pass &= hashtable_merge(htb, htb2);

    pass &= htb->size == 7;
    pass &= htb->capacity == 16; //should have resized

    lookup = hashtable_lookup(htb, "1key1");
    pass &= lookup.status == OK;
    pass &= lookup.cell->value == 1;

    lookup = hashtable_lookup(htb, "2key2");
    pass &= lookup.status == OK;

    hashtable_cleanup(htb);
    hashtable_cleanup(htb2);
    return pass;
}

bool do_nothing_on_self_merge()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 7);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    
    pass &= htb->size == 5;
    pass &= htb->capacity == 8;

    hashtable_merge(htb, htb);

    pass &= htb->size == 5;
    pass &= htb->capacity == 8; //should have resized

    lookup = hashtable_lookup(htb, "key1");
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb, "key2");
    pass &= lookup.status == OK;

    hashtable_cleanup(htb);
    return pass;
}

//COPY TESTS (prefixed with hashtable_copy_should)
bool properly_copy()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    
    pass &= htb->size == 5;
    pass &= htb->capacity == 8;

    lookup = hashtable_lookup(htb, "key3");
    pass &= lookup.cell->value == 3;
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb, "key2");
    pass &= lookup.cell->value == 2;
    pass &= lookup.status == OK;

    hashtable_t* htb2 = hashtable_copy(htb);

    pass &= htb2->size == 5;
    pass &= htb2->capacity == 8;

    lookup = hashtable_lookup(htb2, "key3");
    pass &= lookup.cell->value == 3;
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb2, "key2");
    pass &= lookup.cell->value == 2;
    pass &= lookup.status == OK;

    hashtable_cleanup(htb);
    hashtable_cleanup(htb2);
    return pass;
}

//INSERT TESTS (prefixed with hashtable_insert_should)
bool properly_insert()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    
    pass &= htb->size == 5;

    lookup = hashtable_lookup(htb, "key3");
    pass &= lookup.cell->value == 3;
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb, "key2");
    pass &= lookup.cell->value == 2;
    pass &= lookup.status == OK;

    hashtable_cleanup(htb);
    return pass;
}

bool automatically_resize()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    hashtable_insert(htb, "key6", 6);
    hashtable_insert(htb, "key7", 7);
    hashtable_insert(htb, "key8", 8);
    
    pass &= htb->size == 8;
    pass &= htb->capacity == 16;

    hashtable_cleanup(htb);
    return pass;
}

bool return_insert_reference()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    lookup = hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    
    pass &= htb->size == 5;

    lookup.cell->value++;
    lookup = hashtable_lookup(htb, "key3");
    pass &= lookup.cell->value == 4;
    pass &= lookup.status == OK;

    hashtable_cleanup(htb);
    return pass;
}

bool indicate_insert_failure()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    lookup = hashtable_insert(htb, "key4", 5);
    
    pass &= htb->size == 4;
    pass &= lookup.status == DUPLICATE_KEY;

    hashtable_cleanup(htb);
    return pass;
}

//LOOKUP TESTS (prefixed with hashtable_lookup_should)
bool properly_lookup()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    
    pass &= htb->size == 5;

    lookup = hashtable_lookup(htb, "key3");
    pass &= lookup.cell->value == 3;
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb, "key2");
    pass &= lookup.cell->value == 2;
    pass &= lookup.status == OK;

    hashtable_cleanup(htb);
    return pass;
}

bool return_lookup_reference()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    
    pass &= htb->size == 5;

    lookup = hashtable_lookup(htb, "key3");
    pass &= lookup.cell->value == 3;
    lookup.cell->value++;
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb, "key3");
    pass &= lookup.cell->value == 4;
    pass &= lookup.status == OK;

    hashtable_cleanup(htb);
    return pass;
}

bool indicate_lookup_failure()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    
    pass &= htb->size == 5;

    lookup = hashtable_lookup(htb, "bingus");
    pass &= lookup.status == KEY_NOT_FOUND;

    hashtable_cleanup(htb);
    return pass;
}

//DELETE TESTS (prefixed with hashtable_delete_should)
bool properly_delete()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    
    pass &= htb->size == 5;

    hashtable_delete(htb, "key4");
    pass &= htb->size == 4;

    lookup = hashtable_lookup(htb, "key4");
    pass &= lookup.status == KEY_NOT_FOUND;

    hashtable_cleanup(htb);
    return pass;
}

bool indicate_deletion_failure()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    
    pass &= htb->size == 5;

    lookup = hashtable_delete(htb, "bingus");
    pass &= lookup.status == KEY_NOT_FOUND;
    pass &= htb->size == 5;

    hashtable_cleanup(htb);
    return pass;
}

bool allow_reinsert_after_delete()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 3);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    
    pass &= htb->size == 5;

    lookup = hashtable_lookup(htb, "key1");
    pass &= lookup.status == OK;
    pass &= lookup.cell->value == 1; 

    hashtable_delete(htb, "key1");
    pass &= htb->size == 4;

    lookup = hashtable_lookup(htb, "key1");
    pass &= lookup.status == KEY_NOT_FOUND;  

    lookup = hashtable_insert(htb, "key1", 7);  
    pass &= lookup.status == OK;

    lookup = hashtable_lookup(htb, "key1");
    pass &= lookup.status == OK;
    pass &= lookup.cell->value == 7;  

    hashtable_cleanup(htb);
    return pass;
}

//COMBO OPERATIONS
bool squash_copy()
{
    bool pass = true;
    hashtable_t* htb = hashtable_init(1 << 5);

    hashtable_insert(htb, "key1", 1);
    hashtable_insert(htb, "key2", 2);
    hashtable_insert(htb, "key3", 3);
    hashtable_insert(htb, "key4", 4);
    hashtable_insert(htb, "key5", 5);
    hashtable_insert(htb, "key6", 6);
    hashtable_insert(htb, "key7", 7);
    pass &= htb->size == 7;
    pass &= htb->capacity == 1 << 5;

    hashtable_squash(htb);
    pass &= htb->size == 7;
    pass &= htb->capacity == 8;

    hashtable_t* htb2 = hashtable_copy(htb);
    pass &= htb2->size == 7;
    pass &= htb2->capacity == 8;

    hashtable_cleanup(htb);
    hashtable_cleanup(htb2);
    return pass;
}

bool merge_squash()
{
    bool pass = true;
    cell_info_t lookup;
    hashtable_t* htb = hashtable_init(1 << 5);
    hashtable_t* htb2 = hashtable_init(1 << 3);

    hashtable_insert(htb, "1key1", 1);
    hashtable_insert(htb, "1key2", 2);
    hashtable_insert(htb, "1key3", 3);

    hashtable_insert(htb2, "2key1", 1);
    hashtable_insert(htb2, "2key2", 2);
    hashtable_insert(htb2, "2key3", 3);
    hashtable_insert(htb2, "2key4", 4);
    hashtable_insert(htb2, "2key5", 5);
    
    pass &= htb->size == 3;
    pass &= htb2->size == 5;

    hashtable_merge(htb, htb2);
    hashtable_squash(htb);

    pass &= htb->size == 8;
    pass &= htb->capacity == 8;

    hashtable_cleanup(htb);
    hashtable_cleanup(htb2);
    return pass;
}

bool char_ptr_is_8_bytes()
{
    return sizeof(char*) == 8;
}