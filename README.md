# Homework 3: Testing different caches

Tanmay Dubey

I used Catch2 as my testing framework. The .c file was used for testing C implementations, and the .cc file for C++ ones.

Test results for my cache:

Test name | Test function/description | Test status
--------- | ------------------------- | -----------
retrieve_single_value | SECTION( "Cache returns correct value and size for a key that was inserted" ) | :heavy_check_mark:
retrieve_single_size | SECTION( "Cache returns correct value and size for a key that was inserted" ) | :heavy_check_mark:
retrieve_modified_value | SECTION( "Cache returns correct value and size for a key that was inserted and modified" ) | :heavy_check_mark:
retrieve_modified_size | SECTION( "Cache returns correct value and size for a key that was inserted and modified" ) | :heavy_check_mark:
retrieve_not_present | SECTION( "Cache correctly returns NULL for a key that wasn't inserted" ) | :heavy_check_mark:
retrieve_deleted | SECTION( "Cache correctly returns NULL for a key that was inserted and deleted" ) | :heavy_check_mark:
retrieve_evicted | SECTION( "Cache correctly returns NULL for a key that was evicted" ) | :heavy_check_mark:
retrieve_same_hash_1st_key_value | SECTION( "Cache returns correct values and sizes for two keys that hash to the same location" ) | :heavy_check_mark:
retrieve_same_hash_1st_key_size | SECTION( "Cache returns correct values and sizes for two keys that hash to the same location" ) | :heavy_check_mark:
retrieve_same_hash_2nd_key_value | SECTION( "Cache returns correct values and sizes for two keys that hash to the same location" ) | :heavy_check_mark:
retrieve_same_hash_2nd_key_size | SECTION( "Cache returns correct values and sizes for two keys that hash to the same location" ) | :heavy_check_mark:

Test results for other caches I tested:

Cache no. 1 (Alyssa and Monica)

Test name | Test status
--------- | -----------
retrieve_single_value | :heavy_check_mark:
retrieve_single_size | :heavy_check_mark:
retrieve_modified_value | :heavy_check_mark:
retrieve_modified_size | :heavy_check_mark:
retrieve_not_present | :heavy_check_mark:
retrieve_deleted | :heavy_check_mark:
retrieve_evicted | :heavy_check_mark:
retrieve_same_hash_1st_key_value | :heavy_check_mark:
retrieve_same_hash_1st_key_size | :heavy_check_mark:
retrieve_same_hash_2nd_key_value | :heavy_check_mark:
retrieve_same_hash_2nd_key_size | :heavy_check_mark:

Cache no. 2 (David and Mike)

Test name | Test status
--------- | -----------
retrieve_single_value | :heavy_check_mark:
retrieve_single_size | :x:
retrieve_modified_value | :x:
retrieve_modified_size | :x:
retrieve_not_present | :heavy_check_mark:
retrieve_deleted | :heavy_check_mark:
retrieve_evicted | :heavy_check_mark:
retrieve_same_hash_1st_key_value | :heavy_check_mark:
retrieve_same_hash_1st_key_size | :x:
retrieve_same_hash_2nd_key_value | :heavy_check_mark:
retrieve_same_hash_2nd_key_size | :x:

Cache no. 3 (Ezra and Joe)

Test name | Test status
--------- | -----------
retrieve_single_value | :heavy_check_mark:
retrieve_single_size | :x:
retrieve_modified_value | :x:
retrieve_modified_size | :x:
retrieve_not_present | :heavy_check_mark:
retrieve_deleted | :heavy_check_mark:
retrieve_evicted | :x:
retrieve_same_hash_1st_key_value | :heavy_check_mark:
retrieve_same_hash_1st_key_size | :x:
retrieve_same_hash_2nd_key_value | :heavy_check_mark:
retrieve_same_hash_2nd_key_size | :x:
