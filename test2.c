#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "cache.h"

TEST_CASE( "Elementary tests of inserting and returning values" ) {

    index_type cache1_size = 10;
    cache_type cache1 = create_cache(cache1_size, NULL, NULL);
    index_type *size = malloc(sizeof(index_type));

    REQUIRE( cache1 != NULL )

    SECTION( "Cache returns correct value and size for a key that was inserted" ) {

        float value1 = 3.1415926535;
        cache_set(cache1, "pi", &value1, sizeof(value1));
        const float *return1 = cache_get(cache1, "pi", size);

        REQUIRE( return1 != NULL );
        REQUIRE( *return1 == value1 );
        REQUIRE( *size == sizeof(value1) );
    }

    SECTION( "Cache correctly returns NULL for a key that wasn't inserted") {

        const index_type *return2 = cache_get(cache1, "three", size);

        REQUIRE( return2 == NULL );
    }

    SECTION( "Cache returns correct value and size for a key that was inserted and modified") {

        long value2 = 1123581321;
        cache_set(cache1, "fibonacci", &value2, sizeof(value2));
        
    }
}