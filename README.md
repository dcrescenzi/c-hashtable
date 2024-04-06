# Hashtable implementation in C
## Description
I've always liked making data structures from scratch to remind myself to both appriciate the easy to use implementations already available in most modern languages and to remind myself of what goes on under the hood. 

I decided to make this hashtable out of interest but to also help out anyone needing one on the fly in C.  Feel free to use it anywhere (as long as it isn't breaching any academic integrity :) ).

I also tried my hand at an implementation that uses a homemade SSO to reduce memory allocs and frees for keys, which can be found in the **experimental** branch.  It's definitley more dangerous than the implementation on main, but is *very* fast for small keys (len <= 8). The main implementation is already quite performant, but if you need a little extra and are mostly dealing with small keys, try out the experimental branch.  To implement sso, I just encode the string within the char* itself as long as the key is 8 or less chars.  To see more, look at ```encode_key_as_char_ptr```

All in all, this mini project has taught me to really appreciate the standard library :)

## Testing and demoing
*I assume the user is on a linux machine with make and python installed.*

**TO TEST**: ```make test```

This generates tests with gen_tests.py, compiles them and runs them with output. To add your own test, define them under an existing or new suite in test/hashtable_test.h and implement it in test/hashtable_test.c.

**TO DEBUG**: ```make debug```

This does the same thing ```make test``` does, but doesn't run the tests and doesn't clean anything up.  You can set breakpoints within hashtable.c or test/hashtable_test.c and then debug test_exe to debug issues.

**TO DEMO**:  ```make demo```

Builds a short demo comparing my hashtable implementation and C++'s std::unordered_map. The executable is benchmark/hashtable_demo. Both maps are timed inserting an inputted amount of keys of inputted length into the hashmap, which could overlap (which then should increment the key's counter).