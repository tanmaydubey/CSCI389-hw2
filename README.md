# Homework 4: Caching over the network

Tanmay Dubey

After setting up my cache server, I ran basic tests of all the functions through manual queries from the terminal using the nc command. These passed without too much difficulty, and once they did I wrote the testing client code. I then linked the client code with the unit tests we wrote in Homework 3; I had to slightly change the tests to account for the fact that we were setting and getting strings instead of numbers, due to the RESTful nature of the API.

I faced quite some difficulty in initially getting the tests to compile, and then making them pass. Parsing the response in C is quite difficult; the only easy way to do it is with the strtok function, and determining exactly the right number of times to call it go extract the response body from the message took a little bit of tinkering.

Test results for my cache:

Test name | Test function/description | Test status
--------- | ------------------------- | -----------
retrieve_single_value | SECTION( "Cache returns correct value and size for a key that was inserted" ) | :heavy_check_mark:
retrieve_single_size | SECTION( "Cache returns correct value and size for a key that was inserted" ) | :heavy_check_mark:
retrieve_modified_value | SECTION( "Cache returns correct value and size for a key that was inserted and modified" ) | :heavy_check_mark:
retrieve_modified_size | SECTION( "Cache returns correct value and size for a key that was inserted and modified" ) | :heavy_check_mark:
retrieve_not_present | SECTION( "Cache correctly returns NULL for a key that wasn't inserted" ) | :heavy_check_mark:
retrieve_deleted | SECTION( "Cache correctly returns NULL for a key that was inserted and deleted" ) | :heavy_check_mark:
retrieve_evicted | SECTION( "Cache correctly returns NULL for a key that was evicted" ) | :x:
retrieve_same_hash_1st_key_value | SECTION( "Cache returns correct values and sizes for two keys that hash to the same location" ) | :heavy_check_mark:
retrieve_same_hash_1st_key_size | SECTION( "Cache returns correct values and sizes for two keys that hash to the same location" ) | :heavy_check_mark:
retrieve_same_hash_2nd_key_value | SECTION( "Cache returns correct values and sizes for two keys that hash to the same location" ) | :heavy_check_mark:
retrieve_same_hash_2nd_key_size | SECTION( "Cache returns correct values and sizes for two keys that hash to the same location" ) | :heavy_check_mark:
