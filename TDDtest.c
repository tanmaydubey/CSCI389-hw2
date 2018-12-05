#define CATCH_CONFIG_MAIN

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

    index_type cache1_size = 10;
    cache_type cache1 = create_cache(cache1_size, NULL, &hasher);
    index_type *size = (index_type *) malloc(sizeof(index_type));

    REQUIRE( cache1 != NULL );

    SECTION( "Cache returns correct value and size for a key that was inserted" ) {

        float value1 = 3.1415926535;
        cache_set(cache1, "pi", &value1, sizeof(value1));
        const float *return1 = (const float *) cache_get(cache1, "pi", size);

        REQUIRE( return1 != NULL );

        SECTION( "Cache returns correct value" ) {

            REQUIRE( *return1 == value1 );
        }

        SECTION( "Cache returns correct size" ) {

            REQUIRE( *size == sizeof(value1) );
        }
    }

    SECTION( "Cache returns correct value and size for a key that was inserted and modified") {

        long value2 = 1123581321;
        cache_set(cache1, "fibonacci", &value2, sizeof(value2));
        float value3 = 1.123581321;
        cache_set(cache1, "fibonacci", &value3, sizeof(value3));
        const float *return2 = (const float *) cache_get(cache1, "fibonacci", size);

        REQUIRE( return2 != NULL );

        SECTION( "Cache returns correct value" ) {

            REQUIRE( *return2 == value3 );
        }
        
        SECTION( "Cache returns correct size" ) {

            REQUIRE( *size == sizeof(value3) );
        }
    }

    SECTION( "Cache correctly returns NULL for a key that wasn't inserted") {

        const index_type *return3 = (const index_type *) cache_get(cache1, "three", size);

        REQUIRE( return3 == NULL );
    }

    SECTION( "Cache correctly returns NULL for a key that was inserted and deleted" ) {

        index_type value4 = 4;
        cache_set(cache1, "four", &value4, sizeof(value4));
        cache_delete(cache1, "four");
        const index_type *return4 = (const index_type *) cache_get(cache1, "four", size);

        REQUIRE( return4 == NULL );
    }

    SECTION(" Cache correctly returns NULL for a key that was evicted ") {

        float value1 = 3.1415926535;
        cache_set(cache1, "pi", &value1, sizeof(value1));
        long value2 = 1123581321;
        cache_set(cache1, "fibonacci", &value2, sizeof(value2));
        const long *return2 = (const long *) cache_get(cache1, "fibonacci", size);
        index_type value4 = 4;
        cache_set(cache1, "four", &value4, sizeof(value4));
        const index_type *return4 = (const index_type *) cache_get(cache1, "four", size);

        // The sizes of the values inserted above add up to 12, while the maximum size of 
        // the cache is 10, so "pi" should be evicted according to either FIFO or LRU.

        const float *return1 = (const float *) cache_get(cache1, "pi", size);

        REQUIRE ( return1 == NULL );
    }

    SECTION( "Cache returns correct values and sizes for two keys that hash to the same location" ) {

        float value1 = 3.1415926535;
        cache_set(cache1, "pi", &value1, sizeof(value1));
        index_type value4 = 1234;
        cache_set(cache1, "qh", &value4, sizeof(value4));

        // According to the hash function I passed into the cache constructor, "pi" and "qh" hash to the same bucket.

        const float *return1 = (const float *) cache_get(cache1, "pi", size);
        index_type *size4 = (index_type *) malloc(sizeof(index_type));
        const index_type *return4 = (const index_type *) cache_get(cache1, "qh", size4);

        REQUIRE( return1 != NULL );
        REQUIRE( return4 != NULL );

        SECTION( "Cache returns correct value for first key" ) {

            REQUIRE( *return1 == value1 );
        }

        SECTION( "Cache returns correct size for first key" ) {

            REQUIRE( *size == sizeof(value1) );
        }

        SECTION( "Cache returns correct value for second key" ) {

            REQUIRE( *return4 == value4 );
        }
        
        SECTION( "Cache returns correct size for second key" ) {

            REQUIRE( *size4 == sizeof(value4) );
        }
    }
}