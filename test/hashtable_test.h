/*
Author: Dante Crescenzi
Last Modif: 28 Mar 2024
Description: interface of unit tests for hashtable functionality
*/

#include "../hashtable.h"

//SUITE = hashtable_init_should
bool reject_empty_size();
bool reject_non_power_of_2_size();
bool properly_initialize_members();

//SUITE = hashtable_resize_should
bool properly_upsize();
bool properly_downsize();

//SUITE = hashtable_squash_should
bool squash_for_power_of_2_size();
bool squash_for_general_size();

//SUITE = hashtable_clear_should
bool properly_clear();

//SUITE = hashtable_swap_should
bool properly_swap();
bool handle_self_swap();

//SUITE = hashtable_merge_should
bool properly_merge();
bool handle_and_indicate_conflict();
bool do_nothing_on_self_merge();

//SUITE = hashtable_copy_should
bool properly_copy();

//SUITE = hashtable_insert_should
bool properly_insert();
bool automatically_resize();
bool return_insert_reference();
bool indicate_insert_failure();

//SUITE = hashtable_lookup_should
bool properly_lookup();
bool return_lookup_reference();
bool indicate_lookup_failure();

//SUITE = hashtable_delete_should
bool properly_delete();
bool indicate_deletion_failure();
bool allow_reinsert_after_delete();

//SUITE = combo_operations
bool squash_copy();
bool merge_squash();