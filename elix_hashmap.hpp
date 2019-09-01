#ifndef ELIX_HASHMAP_HPP
#define ELIX_HASHMAP_HPP
//#define ELIX_HASHMAP_IMPLEMENTATION

typedef void* data_pointer;

#define ELIX_HASHMAP_POOL_SIZE 32

struct elix_hashmap_pool {
	uint8_t used = 0;
	uint32_t keys[ELIX_HASHMAP_POOL_SIZE];
	void * values[ELIX_HASHMAP_POOL_SIZE];
};

struct elix_hashmap {
	elix_hashmap_pool active;
	elix_hashmap * next = nullptr;
};

elix_hashmap * elix_hashmap_create();
bool elix_hashmap_destroy(elix_hashmap ** hm, void (*delete_func)(data_pointer*) = nullptr);

void elix_hashmap_insert_hash(elix_hashmap * hm, uint32_t key, data_pointer value);
data_pointer elix_hashmap_value_hash(elix_hashmap * hm, uint32_t key);
void elix_hashmap_remove_hash(elix_hashmap * hm, uint32_t key, void (*delete_func)(data_pointer*) = nullptr);

void elix_hashmap_insert(elix_hashmap * hm, const char * key, data_pointer value);
void elix_hashmap_remove(elix_hashmap * hm, const char * key, void (*delete_func)(data_pointer*) = nullptr);
data_pointer elix_hashmap_value(elix_hashmap * hm, const char * key);

inline uint32_t elix_hash( const char * str, size_t len ) {
	//Jenkins One-at-a-time hash
	uint32_t hash = 0;
	size_t i;

	for (i = 0; i < len; i++) {
		hash += str[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}
inline size_t elix_cstring_length(const char * string, uint8_t include_terminator = 0 ) {
	if (string) {
		size_t c = 0;
		while(*string++) {
			++c;
		}
		return c + include_terminator;
	}
	return 0;
}
#endif // ELIX_HASHMAP_HPP

#ifdef ELIX_HASHMAP_IMPLEMENTATION

elix_hashmap * elix_hashmap_create() {
	elix_hashmap * hm = new elix_hashmap();
	return hm;
}

void elix_hashmap_insert_hash(elix_hashmap * hm, uint32_t key, data_pointer value) {
	if ( hm->active.used < ELIX_HASHMAP_POOL_SIZE ) {
		uint32_t * current = &hm->active.keys[0];
		for (size_t c = 0; c< ELIX_HASHMAP_POOL_SIZE; c++, current++ ) {
			if ( *current == 0) {
				if ( hm->active.values[c] == nullptr ) {
					*current = key;
					hm->active.values[c] = value;
					hm->active.used++;
					return;
				} else {
					
				}
			}
		}
	}
	if ( !hm->next ) {
		hm->next = new elix_hashmap();
	}
	if ( !hm->next ) {
		return;
	}
	elix_hashmap_insert_hash(hm->next, key, value);
}

data_pointer elix_hashmap_value_hash(elix_hashmap * hm, uint32_t key) {
	if ( !hm || !hm->active.used) {
		return nullptr;
	}

	uint32_t * current = &hm->active.keys[0];
	for (size_t c = 0; c< ELIX_HASHMAP_POOL_SIZE; c++, current++ ) {
		if ( *current == key) {
			if ( hm->active.values[c] == nullptr ) {
				
			}
			return hm->active.values[c];
		}
	}
	if ( hm->next ) {
		return elix_hashmap_value_hash(hm->next, key);
	}

	return nullptr;
}

void elix_hashmap_remove_hash(elix_hashmap * hm, uint32_t key, void (*delete_func)(data_pointer*)) {
	if ( !hm->active.used && !hm->next ) {
		return;
	}

	uint32_t * current = &hm->active.keys[0];
	for (size_t c = 0; c< ELIX_HASHMAP_POOL_SIZE; c++, current++ ) {
		if ( *current == key) {
			*current = 0;
			if ( delete_func ) {
				delete_func(&hm->active.values[c]);
			} else {
				hm->active.values[c] = nullptr;
			}
			return;
		}
	}
	if ( hm->next ) {
		return elix_hashmap_remove_hash(hm->next, key, delete_func);
	}
	return;
}

void elix_hashmap_clear(elix_hashmap * hm, void (*delete_func)(data_pointer*)) {
	if ( hm->next ) {
		elix_hashmap_clear(hm->next,delete_func);
		delete hm->next;
		hm->next = nullptr;

	}
	if ( delete_func ) {
		data_pointer * current = &hm->active.values[0];
		for (size_t c = 0; c< ELIX_HASHMAP_POOL_SIZE; c++, current++ ) {
			delete_func(current);
		}
	}
	hm->active.used = 0;
	memset(&hm->active.keys, 0 , sizeof(uint32_t)*ELIX_HASHMAP_POOL_SIZE);
	memset(&hm->active.values, 0 , sizeof(uintptr_t)*ELIX_HASHMAP_POOL_SIZE);
}

bool elix_hashmap_destroy(elix_hashmap ** hm, void (*delete_func)(data_pointer*)) {
	elix_hashmap_clear((*hm), delete_func);
	delete (*hm);
	(*hm) = nullptr;
	return false;
}

void elix_hashmap_insert(elix_hashmap * hm, const char * key, data_pointer value) {
	uint32_t hash = elix_hash(key, elix_cstring_length(key));
	elix_hashmap_insert_hash( hm, hash, value);
}

void elix_hashmap_remove(elix_hashmap * hm, const char * key, void (*delete_func)(data_pointer*)) {
	uint32_t hash = elix_hash(key, elix_cstring_length(key));
	return elix_hashmap_remove_hash(hm, hash, delete_func);
}

data_pointer elix_hashmap_value(elix_hashmap * hm, const char * key) {
	uint32_t hash = elix_hash(key, elix_cstring_length(key));
	return elix_hashmap_value_hash(hm, hash);
}

#endif // ELIX_HASHMAP_IMPLEMENTATION


