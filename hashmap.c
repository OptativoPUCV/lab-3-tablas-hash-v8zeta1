#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    long pos = hash(key, map->capacity);
    while (map->buckets[pos] != NULL && map->buckets[pos]->key != NULL) {
        if (is_equal(map->buckets[pos]->key, key)) {
            return;
        }
        pos = (pos + 1) % map->capacity;
    }
    if (map->buckets[pos] == NULL) {
        map->buckets[pos] = createPair(strdup(key), value);
    }
    else {
        map->buckets[pos]->key = strdup(key);
        map->buckets[pos]->value = value;
    }
    map->current = pos;
    map->size++;     
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    Pair ** old_buckets = map->buckets;
    long old_capacity = map->capacity;
    map->capacity *= 2;
    map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *));
    if(map->buckets == NULL) {
        map->buckets = old_buckets;
        map->capacity = old_capacity;
        return;
    }
    map->size = 0;
    map->current = -1;
    for(long k = 0 ; k < old_capacity ; k++) {
        if(old_buckets[k] != NULL && old_buckets[k]->key != NULL) {
            insertMap(map, old_buckets[k]->key, old_buckets[k]->value);
        }
    }
    free(old_buckets);
}


HashMap * createMap(long capacity) {
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if(map == NULL) return NULL;
    map->buckets = (Pair **)calloc(capacity, sizeof(Pair *));
    if(map->buckets == NULL) {
        free(map);
        return NULL;
    }
    map->capacity = capacity;
    map->size = 0;
    map->current = -1;
    return map;
}

void eraseMap(HashMap * map,  char * key) {    
    long pos = hash(key, map->capacity);
    while(map->buckets[pos] != NULL) {
        if(map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)) {
            map->buckets[pos]->key = NULL;
            map->size--;
            return;
        }
        pos = (pos+1) % map->capacity;
    }
}

Pair * searchMap(HashMap * map,  char * key) {   
    long pos = hash(key, map->capacity);
    while(map->buckets[pos] != NULL) {
        if(map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)) {
            map->current = pos;
            return map->buckets[pos];
        }
        pos = (pos+1) % map->capacity;
    }
    return NULL;
}

Pair * firstMap(HashMap * map) {
    for(long k = 0 ; k < map->capacity ; k++) {
        if(map->buckets[k] != NULL && map->buckets[k]->key != NULL) {
            map->current = k;
            return map->buckets[k];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {
    for(long k = map->current+1 ; k < map->capacity ; k++) {
        if(map->buckets[k] != NULL && map->buckets[k]->key != NULL) {
            map->current = k;
            return map->buckets[k];
        }
    }
    return NULL;
}
