/*
networkTDDtest.c: same test code as TDDtest.c, except modified to work with the RESTful API.
Cache size has changed so that the tests still function.
*/

#define CATCH_CONFIG_MAIN

#include <string.h>

#include "catch.hpp"
#include "cache.h"

index_type hasher(key_type key) {
    index_type result = 0;
    for (index_type i = 0; i < strlen(key); i++) {
        result += (index_type) key[i];
    }
    return result;
}

TEST_CASE( "Test cache insertion, deletion, eviction and resizing" ) {

    index_type cache1_size = 24;
    cache_type cache1 = create_cache(cache1_size, &hasher);
    index_type *size = (index_type *) malloc(sizeof(index_type));

    REQUIRE( cache1 != NULL );

    SECTION( "Cache returns correct value and size for a key that was inserted" ) {

        char *value1 = "3.1415926535";
        cache_set(cache1, "pi", value1, strlen(value1)+1);
        const char *return1 = (const char *) cache_get(cache1, "pi", size);

        REQUIRE( return1 != NULL );

        SECTION( "Cache returns correct value" ) {

            REQUIRE( strcmp(return1, value1) == 0 );
        }

        SECTION( "Cache returns correct size" ) {

            REQUIRE( *size == strlen(value1)+1 );
        }
    }

    SECTION( "Cache returns correct value and size for a key that was inserted and modified") {

        char *value2 = "1123581321";
        cache_set(cache1, "fibonacci", value2, strlen(value2)+1);
        char *value3 = "1.123581321";
        cache_set(cache1, "fibonacci", value3, strlen(value3)+1);
        const char *return2 = (const char *) cache_get(cache1, "fibonacci", size);

        REQUIRE( return2 != NULL );

        SECTION( "Cache returns correct value" ) {

            REQUIRE( strcmp(return2, value3) == 0 );
        }
        
        SECTION( "Cache returns correct size" ) {

            REQUIRE( *size == strlen(value3)+1 );
        }
    }

    SECTION( "Cache correctly returns NULL for a key that wasn't inserted") {

        const char *return3 = (const char *) cache_get(cache1, "three", size);

        REQUIRE( return3 == NULL );
    }

    SECTION( "Cache correctly returns NULL for a key that was inserted and deleted" ) {

        char *value4 = "4";
        cache_set(cache1, "four", value4, strlen(value4)+1);
        cache_delete(cache1, "four");
        const char *return4 = (const char *) cache_get(cache1, "four", size);

        REQUIRE( return4 == NULL );
    }

    SECTION(" Cache correctly returns NULL for a key that was evicted ") {

        char *value1 = "3.1415926535";
        cache_set(cache1, "pi", value1, strlen(value1)+1);
        char *value2 = "1123581321";
        cache_set(cache1, "fibonacci", value2, strlen(value2)+1);
        const char *return2 = (const char *) cache_get(cache1, "fibonacci", size);
        char *value4 = "0004";
        cache_set(cache1, "four", value4, strlen(value4)+1);
        const char *return4 = (const char *) cache_get(cache1, "four", size);

        // The sizes of the values inserted above add up to 12, while the maximum size of 
        // the cache is 10, so "pi" should be evicted according to either FIFO or LRU.

        const char *return1 = (const char *) cache_get(cache1, "pi", size);

        REQUIRE ( return1 == NULL );
    }

    SECTION( "Cache returns correct values and sizes for two keys that hash to the same location" ) {

        char *value1 = "3.1415926535";
        cache_set(cache1, "pi", value1, strlen(value1)+1);
        char *value4 = "1234";
        cache_set(cache1, "qh", value4, strlen(value4)+1);

        // According to the hash function I passed into the cache constructor, "pi" and "qh" hash to the same bucket.

        const char *return1 = (const char *) cache_get(cache1, "pi", size);
        index_type *size4 = (index_type *) malloc(sizeof(index_type));
        const char *return4 = (const char *) cache_get(cache1, "qh", size4);

        REQUIRE( return1 != NULL );
        REQUIRE( return4 != NULL );

        SECTION( "Cache returns correct value for first key" ) {

            REQUIRE( strcmp(return1, value1) == 0 );
        }

        SECTION( "Cache returns correct size for first key" ) {

            REQUIRE( *size == strlen(value1)+1 );
        }

        SECTION( "Cache returns correct value for second key" ) {

            REQUIRE( strcmp(return4, value4) == 0 );
        }
        
        SECTION( "Cache returns correct size for second key" ) {

            REQUIRE( *size4 == strlen(value4)+1 );
        }
    } 
}