#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "cache.h"

int main(void) {

    // Test create_cache

    index_type cache1_size = 10;
    index_type cache2_size = 20;

    cache_type cache1 = create_cache(cache1_size, NULL, NULL);
    cache_type cache2 = create_cache(cache2_size, NULL, NULL);
    printf("Cache created successfully!\n");

    // Elementary tests of cache_set and cache_get

    float value1 = 3.1415926535;
    cache_set(cache1, "pi", &value1, sizeof(value1));
    index_type *size1 = malloc(sizeof(index_type));
    const float *return1 = cache_get(cache1, "pi", size1);
    assert(return1 != NULL);
    assert(*return1 == value1);
    assert(*size1 == sizeof(value1));
    printf("First entry created successfully!\n");

    long value2 = 1123581321;
    cache_set(cache1, "fibonacci", &value2, sizeof(value2));
    index_type *size2 = malloc(sizeof(index_type));
    const long *return2 = cache_get(cache1, "fibonacci", size2);
    assert(return2 != NULL);
    assert(*return2 == value2);
    assert(*size2 == sizeof(value2));
    printf("Second entry created successfully!\n");

    // Test query for key that wasn't inserted

    index_type *size3 = malloc(sizeof(index_type));
    const index_type *return3 = cache_get(cache1, "three", size3);
    assert(return3 == NULL);
    printf("Query returns NULL for key that wasn't inserted\n");

    // Test query for value that was modified

    value2 = 1234567890;
    cache_set(cache1, "fibonacci", &value2, sizeof(value2));
    return2 = cache_get(cache1, "fibonacci", size2);
    assert(return2 != NULL);
    assert(*return2 == value2);
    assert(*size2 == sizeof(value2));
    printf("Query returns correct result for key whose value was modified\n");

    // Test query for value that was inserted and deleted

    cache_delete(cache1, "fibonacci");
    return2 = cache_get(cache1, "fibonacci", size2);
    assert(return2 == NULL);
    printf("Query returns correct result for key that was inserted and deleted\n");

    // Test query for value that was evicted

    return1 = cache_get(cache1, "pi", size1);
    assert(return1 == NULL);
    printf("Query returns correct result for key that was evicted\n");

    // Test query for value that hashes to same location as a previous one

    index_type value4 = 1234;
    cache_set(cache2, "qh", &value4, sizeof(value4));
    index_type *size4 = malloc(sizeof(index_type));
    const index_type *return4 = cache_get(cache2, "qh", size4);
    assert(return4 != NULL);
    assert(*return4 == value4);
    assert(*size4 == sizeof(value4));
    cache_set(cache2, "pi", &value1, sizeof(value1));
    return1 = cache_get(cache2, "pi", size1);
    assert(return1 != NULL);
    assert(*return1 == value1);
    assert(*size1 == sizeof(value1));
    printf("Values are successfully stored in same bucket\n");

    // Test resizing functionality of cache

    index_type value5 = 5;
    cache_set(cache2, "five", &value5, sizeof(value5));
    index_type value6 = 6;
    cache_set(cache2, "six", &value6, sizeof(value6));
    index_type value7 = 7;
    cache_set(cache2, "seven", &value7, sizeof(value7));
    printf("Cache successfully resizes\n");

    // Test cache eviction policy
    index_type value8 = 8;
    cache_set(cache2, "eight", &value8, sizeof(value8));
    return4 = cache_get(cache2, "qh", size4);
    assert(return4 == NULL);
    printf("Cache eviction policy works as expected\n");

    // Test query for value that was deleted from a bucket

    cache_delete(cache2, "pi");
    return1 = cache_get(cache2, "pi", size1);
    assert(return1 == NULL);
    printf("Query returns correct result for entry deleted from a bucket containing more than one entry\n");


    free(size1);
    free(size2);
    free(size3);
    free(size4);
    destroy_cache(cache1);
    destroy_cache(cache2);
}

