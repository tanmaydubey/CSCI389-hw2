#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"

struct cache_elt {
    key_type elt_key;
    val_type value;
    index_type val_size;
    struct cache_elt *next;
};

typedef struct cache_elt *cache_entry;

typedef key_type (*impl_evictor_type)(cache_type);

struct cache_obj {
    index_type max_size;
    index_type current_size;
    index_type no_of_buckets;
    cache_entry *array;
    struct evictor *evictor;
    hash_func hasher;
};

float load_factor(cache_type cache);

void resize_cache(cache_type cache);

// Default hasher
index_type default_hasher(key_type key) {
    index_type result = 0;
    for (index_type i = 0; i < strlen(key); i++) {
        result += key[i];
    }
    return result;
}

/*
// Default evictor: evicts first item in array. Eviction check implemented in cache_set
key_type default_evictor(cache_type cache) {
    for (index_type i = 0; i < cache->no_of_buckets; i++) {
        if ((cache->array)[i] != NULL) {
            return (cache->array)[i]->elt_key;
        }
    }
    return "";
}
*/

// Generic evictor API

struct evictor {
    key_type (*pop)(struct evictor *evictor);
};

key_type evict_key(struct evictor *evictor) {
    return evictor->pop(evictor);
}


// FIFO evictor API


struct FIFO_node_struct {
    key_type key;
    struct FIFO_node_struct *next;
};

typedef struct FIFO_node_struct *FIFO_node;

struct FIFO_evictor {
    struct evictor FIFO_evictor;
    FIFO_node queue_head;
    FIFO_node queue_tail;
    void (*push)(struct FIFO_evictor *evictor, key_type key);
};

void FIFO_enqueue(struct FIFO_evictor *evictor, key_type key) {
    if (evictor->queue_tail == NULL) {
        evictor->queue_tail = (FIFO_node) malloc(sizeof(struct FIFO_node_struct));
        evictor->queue_tail->key = key;
        evictor->queue_tail->next = NULL;
        evictor->queue_head = evictor->queue_tail;
    }
    else {
        FIFO_node new_node = (FIFO_node) malloc(sizeof(struct FIFO_node_struct));
        new_node->key = key;
        new_node->next = NULL;
        evictor->queue_tail->next = new_node;
        evictor->queue_tail = new_node;
    }
}

key_type FIFO_dequeue(struct evictor *evictor) {
    struct FIFO_evictor *evictor_2 = (struct FIFO_evictor *) evictor;
    if (evictor_2->queue_head == NULL) {
        // printf("Cache is currently empty, cannot evict any items");
        return NULL;
    }
    else {
        key_type ret = evictor_2->queue_head->key;
        evictor_2->queue_head = evictor_2->queue_head->next;
        if(evictor_2->queue_head == NULL) {
            evictor_2->queue_tail = NULL;
        }
        return ret;
    }
}

struct FIFO_evictor *construct_FIFO_evictor() {
    struct FIFO_evictor *ret = (struct FIFO_evictor *) malloc(sizeof(struct FIFO_evictor));
    ret->queue_head = NULL;
    ret->queue_tail = NULL;
    ret->push = FIFO_enqueue;
    struct evictor *base = (struct evictor *) malloc(sizeof(struct evictor));
    base->pop = FIFO_dequeue;
    ret->FIFO_evictor = *base;
    return ret;
}


// Implementation of functions in cache.h


cache_type create_cache(index_type maxmem, evictor_type evictor, hash_func hasher) {

    if (hasher == NULL) {
        hasher = &default_hasher;
    }

    index_type default_starting_array_size = 8;
    cache_entry *array = (cache_entry *) malloc(default_starting_array_size * sizeof(cache_entry));
    if (array == NULL) {
        printf("Error trying to allocate memory for cache array");
        return NULL;
    }
    for (index_type i = 0; i < default_starting_array_size; i++) {
        array[i] = NULL;
    }

    // see if initializing using pointer to cache_obj (cache_type) works
    cache_type cache_ptr = (cache_type) malloc(sizeof(struct cache_obj));
    cache_ptr->max_size = maxmem;
    cache_ptr->current_size = 0;
    cache_ptr->no_of_buckets = default_starting_array_size;
    cache_ptr->array = array;
    if (evictor == NULL) {
        struct FIFO_evictor *evictor_2 = construct_FIFO_evictor();
        evictor = (struct FIFO_evictor *) evictor_2;
    }
    else {
        evictor = (struct evictor *) evictor;
    }
    cache_ptr->evictor = (struct evictor *) evictor;
    cache_ptr->hasher = hasher;
    return cache_ptr;
}

void cache_set(cache_type cache, key_type key, val_type val, index_type val_size) {
    
    if (val_size > cache->max_size) {
        printf("Value is too big for cache\n");
        return;
    }
    index_type *size = (index_type *) malloc(sizeof(index_type));
    while (cache_space_used(cache) + val_size > cache->max_size) {
        key_type key_to_evict = "";

        //The below loop may only have to be called once if using a different implementation of FIFO or some other eviction policy
        while (cache_get(cache, key_to_evict, size) == NULL) {
            key_to_evict = evict_key(cache->evictor);
        }
        cache_delete(cache, key_to_evict);
    }
    free(size);
    
    index_type entry_index = (*(cache->hasher))(key);
    entry_index = entry_index % (cache->no_of_buckets);
    cache_entry p = (cache->array)[entry_index];
    cache_entry q = NULL;
    while (p != NULL) {
        if ((strcmp(p->elt_key, key) == 0)) {
            memcpy((void *) p->value, val, val_size);
            p->val_size = val_size;
            return;
        }
        q = p;
        p = p->next;
    }
    if (p == NULL) {
        cache_entry new_entry = (cache_entry) malloc(sizeof(struct cache_elt));
        new_entry->elt_key = (key_type) calloc(strlen(key)+1, sizeof(key));
        // new_entry->elt_key = malloc(sizeof(key));
        new_entry->value = malloc(val_size);
        if ((new_entry->elt_key == NULL) || (new_entry->value == NULL)) {
            printf("Error trying to allocate memory for key %s\n", key);
            return;
        }
        strcpy((char *) new_entry->elt_key, key); // fixed SIGSEGV at this line!
        memcpy((void *) new_entry->value, val, val_size);
        new_entry->val_size = val_size;
        new_entry->next = NULL;
        if (q == NULL) {
            (cache->array)[entry_index] = new_entry;
        }
        else {
            q->next = new_entry;
        }
        cache->current_size += 1;
        
        // This line should be removed and replaced with the appropriate line if FIFO is not being used for eviction
        struct FIFO_evictor *evictor_2 = (struct FIFO_evictor *) cache->evictor;
        evictor_2->push(evictor_2, key);
    }

    if (load_factor(cache) > 0.5) {
        resize_cache(cache);
    }
    /*
    // Check if key already exists in cache

    for (index_type i = 0; i < cache->no_of_buckets; i++) {
        if ((cache->array)[i] == NULL) {
            empty_entry = i;
            continue;
        }
        if ((strcmp((cache->array)[i]->elt_key, key) == 0)) {
            cache_entry array_elt = (cache->array)[i];
            memcpy(array_elt->value, val, val_size);
            array_elt->val_size = val_size;
            return;
        }
    }

    // Find first unused spot if key doesn't already exist
    if (empty_entry < cache->no_of_buckets) {
        cache_entry new_entry = malloc(sizeof(struct cache_elt));
        new_entry->elt_key = malloc(sizeof(key));
        new_entry->value = malloc(val_size);
        strcpy(new_entry->elt_key, key); // fixed SIGSEGV at this line!
        memcpy(new_entry->value, val, val_size);
        new_entry->val_size = val_size;
        (cache->array)[empty_entry] = new_entry;
        cache->current_size += 1;
    }
    */
}

val_type cache_get(cache_type cache, key_type key, index_type *val_size) {
    index_type entry_index = (*(cache->hasher))(key);
    entry_index = entry_index % (cache->no_of_buckets);
    cache_entry p = (cache->array)[entry_index];
    while (p != NULL) {
        if (strcmp(p->elt_key, key) == 0) {
            *val_size = p->val_size;
            return p->value;
        }
        p = p->next;
    }
    return NULL;

    /*
    for (index_type i = 0; i < cache->no_of_buckets; i++) {
        if (((cache->array)[i] != NULL) && (strcmp((cache->array)[i]->elt_key, key) == 0)) {
            val_type value_ptr = (cache->array)[i]->value;
            // This is the best solution I could come up with for measuring size of *value_ptr, since value_ptr is a void pointer and thus cannot
            // be dereferenced directly
            int value_size = (cache->array)[i]->val_size;
            *val_size = value_size;
            return value_ptr;
        }
    }
    return NULL;
    */
}

void cache_delete(cache_type cache, key_type key) {
    index_type entry_index = (*(cache->hasher))(key);
    entry_index = entry_index % (cache->no_of_buckets);
    cache_entry p = (cache->array)[entry_index];
    cache_entry q = NULL;
    while (p != NULL) {
        if (strcmp(p->elt_key, key) == 0) {
            if (q == NULL) {
                (cache->array)[entry_index] = p->next;
            }
            else {
                q->next = p->next;
            }
            free((void *) p->value);
            free((void *) p->elt_key);
            free(p);
            cache->current_size -= 1;
            break;
        }
        else {
            q = p;
            p = p->next;
        }       
    }

    /*
    for (index_type i = 0; i < cache->no_of_buckets; i++) {
        if ((cache->array)[i] != NULL && (strcmp((cache->array)[i]->elt_key, key) == 0)) {
            free((cache->array)[i]->value);
            free((cache->array)[i]->elt_key);
            free((cache->array)[i]);
            (cache->array)[i] = NULL;
            break;
        }
    }
    */
}

index_type cache_space_used(cache_type cache) {
    index_type space_used = 0;
    cache_entry p = NULL;
    for (index_type i = 0; i < cache->no_of_buckets; i++) {
        p = (cache->array)[i];
        while (p != NULL) {
            space_used += p->val_size;
            p = p->next;
        }
    }
    return space_used;
}

void destroy_cache(cache_type cache) {
    for (index_type i = 0; i < cache->no_of_buckets; i++) {
        while ((cache->array)[i] != NULL) {
            cache_delete(cache, (cache->array)[i]->elt_key);
        }
    }
    free(cache->evictor);
    // free(cache->hasher);
    free(cache->array);
    free(cache);
}

float load_factor(cache_type cache) {
    index_type no_of_entries = cache->current_size;
    float result = (no_of_entries * 1.0) / (cache->no_of_buckets);
    return result;
}

void resize_cache(cache_type cache) {

    index_type new_buckets = (cache->no_of_buckets)*2;
    cache_entry *old_array = cache->array;
    cache_entry *new_array = (cache_entry *) malloc(new_buckets * sizeof(cache_entry));
    if (new_array == NULL) {
        printf("Error trying to allocate memory for new array during resizing");
        return;
    }
    for (index_type i = 0; i < new_buckets; i++) {
        new_array[i] = NULL;
    }

    struct cache_elt placeholder_array[cache->current_size];
    key_type key = "";
    val_type value = NULL;
    index_type *size = (index_type *) malloc(sizeof(index_type));
    for (index_type i = 0; i < cache->current_size; i++) {
        key = cache->evictor->pop(cache->evictor);
        value = cache_get(cache, key, size);
        struct cache_elt temp = {key, value, *size, NULL};
        placeholder_array[i] = temp;
    }

    index_type size_snapshot = cache->current_size;
    for (index_type i = 0; i < size_snapshot; i++) {
        cache_delete(cache, placeholder_array[i].elt_key);
    }
    cache->array = new_array;
    cache->no_of_buckets = new_buckets;

    for(index_type i = 0; i < cache->current_size; i++) {
        cache_set(cache, placeholder_array[i].elt_key, placeholder_array[i].value, placeholder_array[i].val_size);
    }

    free(size);
    free(old_array);
}