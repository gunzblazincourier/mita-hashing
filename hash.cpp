#include <bits/stdc++.h>
const int TABLE_SIZE = 1007

struct Pair {
    int key;
    int value;
};
Pair table[TABLE_SIZE];

int hash(int key) {
    return key % TABLE_SIZE;
}

bool insert(Pair p) {
    if (p == nullptr) {
        return false;
    }

    int index = hash(p.key);
    if (table[index] != nullptr) {
        return false;
    }
    table[index] = p;
    return true;
}

Pair search(int key) {
    int index = hash(key);
    if (table[index] != nullptr && table[index].key == key) {
        return table[index];
    } else {
        return nullptr
    }
}

Pair del(int key) {
    int index = hash(key);
    if (table[index] != nullptr && table[index].key == key) {
        Pair p = table[index];
        table[index] = nullptr;
        return p;
    } else {
        return nullptr
    }
}