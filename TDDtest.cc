#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "cache.hh"

Cache::index_type my_hasher(Cache::key_type key) {
    Cache::index_type result = 0;
    for(int i = 0; i < key.length(); i++) {
        result += (Cache::index_type) key[i];
    }

    return result;
}

TEST_CASE( "Test cache insertion, deletion, eviction and resizing" ) {

    Cache cache1(10, [](){ return 0; }, my_hasher);
    Cache::index_type i = 0;
    Cache::index_type& size = i;

    SECTION( "Cache returns correct value and size for a key that was inserted" ) {

        float value1 = 3.1415926535;
        cache1.set("pi", &value1, sizeof(value1));
        const float* return1 = (const float*) cache1.get("pi", size);

        REQUIRE( return1 != NULL );

        SECTION( "Cache returns correct value" ) {

            REQUIRE( *return1 == value1 );
        }

        SECTION( "Cache returns correct size" ) {

            REQUIRE( size == sizeof(value1) );
        }
    }

    SECTION( "Cache returns correct value and size for a key that was inserted and modified") {

        long value2 = 1123581321;
        cache1.set("fibonacci", &value2, sizeof(value2));
        float value3 = 1.123581321;
        cache1.set("fibonacci", &value3, sizeof(value3));
        const long* return2 = (const long*) cache1.get("fibonacci", size);

        REQUIRE( return2 != NULL );

        SECTION( "Cache returns correct value" ) {

            REQUIRE( *return2 == value3 );
        }
        
        SECTION( "Cache returns correct size" ) {

            REQUIRE( size == sizeof(value3) );
        }
    }

    SECTION( "Cache correctly returns NULL for a key that wasn't inserted") {

        const Cache::index_type* return3 = (const Cache::index_type*) cache1.get("three", size);

        REQUIRE( return3 == NULL );
    }

    SECTION( "Cache correctly returns NULL for a key that was inserted and deleted" ) {

        Cache::index_type value4 = 4;
        cache1.set("four", &value4, sizeof(value4));
        cache1.del("four");
        const Cache::index_type* return4 = (const Cache::index_type*) cache1.get("four", size);

        REQUIRE( return4 == NULL );
    }

    SECTION(" Cache correctly returns NULL for a key that was evicted ") {

        float value1 = 3.1415926535;
        cache1.set("pi", &value1, sizeof(value1));
        long value2 = 1123581321;
        cache1.set("fibonacci", &value2, sizeof(value2));
        const long* return2 = (const long*) cache1.get("fibonacci", size);
        Cache::index_type value4 = 4;
        cache1.set("four", &value4, sizeof(value4));
        const Cache::index_type* return4 = (const Cache::index_type*) cache1.get("four", size);

        // The sizes of the values inserted above add up to 12, while the maximum size of 
        // the cache is 10, so "pi" should be evicted according to either FIFO or LRU.

        const float* return1 = (const float*) cache1.get("pi", size);

        REQUIRE ( return1 == NULL );
    }

    SECTION( "Cache returns correct values and sizes for two keys that hash to the same location" ) {

        float value1 = 3.1415926535;
        cache1.set("pi", &value1, sizeof(value1));
        Cache::index_type value4 = 1234;
        cache1.set("qh", &value4, sizeof(value4));

        // According to the hash function I passed into the cache constructor, "pi" and "qh" hash to the same bucket.

        const float* return1 = (const float*) cache1.get("pi", size);
        Cache::index_type j = 0;
        Cache::index_type& size4 = j;
        const Cache::index_type* return4 = (const Cache::index_type*) cache1.get("qh", size4);

        REQUIRE( return1 != NULL );
        REQUIRE( return4 != NULL );

        SECTION( "Cache returns correct value for first key" ) {

            REQUIRE( *return1 == value1 );
        }

        SECTION( "Cache returns correct size for first key" ) {

            REQUIRE( size == sizeof(value1) );
        }

        SECTION( "Cache returns correct value for second key" ) {

            REQUIRE( *return4 == value4 );
        }
        
        SECTION( "Cache returns correct size for second key" ) {

            REQUIRE( size4 == sizeof(value4) );
        }
    }
}