#include <bits/stdc++.h>

using namespace std;

enum Algorithms {L, Q};
enum Algorithms alg = L;
const int TABLE_SIZE = 10007;

struct Pair {
    int key;
    int value;
};

Pair* table[TABLE_SIZE]; 

long long collision_count = 0;

void flip_enum() {
    if (alg == L) {
        alg = Q;
    } else if (alg == Q) {
        alg = L;
    }
}

int hash_func(int key) {
    return key % TABLE_SIZE;
}

// Helper to clear table between tests
void clear_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table[i] != nullptr) {
            delete table[i]; // Clean up mem
            table[i] = nullptr;
        }
    }
    collision_count = 0;
}

bool insert(Pair* p) {
    if (p == nullptr) {
        return false;
    }

    int index = hash_func(p->key);
    int original_index = index;
    int attempts = 0;

    // Modified to use Linear or Quadratic probing instead of just returning false
    while (table[index] != nullptr) {
        if (table[index]->key == p->key) {  //assume no duplicate keys allowed. 
            table[index]->value = p->value;
            return true;
        }

        collision_count++; // Track this metric
        attempts++;

        // Lin probe: go to next index
        if (alg == L) {
            index = (index + 1) % TABLE_SIZE;
        // Quadratic probe: change index to index + attempts^2
        } else if (alg == Q) {
            index = (index + pow(attempts, 2)) % TABLE_SIZE;
        }

        if (attempts >= TABLE_SIZE) {
            return false;   //avoid infinite loop if table is full
        }
    }

    table[index] = p;
    return true;
}

Pair* search(int key) {
    int index = hash_func(key);
    int attempts = 0;

    while (table[index] != nullptr) {
        if (table[index]->key == key) {
            return table[index];
        }
        
        collision_count++;
        attempts++;

        // Lin or quad probe to find it
        if (alg == L) {
            index = (index + 1) % TABLE_SIZE;   
        } else if (alg == Q) {
            index = (index + pow(attempts, 2)) % TABLE_SIZE;
        }
        
        if (attempts >= TABLE_SIZE) break;
    }
    return nullptr;
}


// Data Gen
vector<int> generate_data(int count) {
    vector<int> data;
    // Simple random num gen
    for(int i = 0; i < count; i++) {
        data.push_back(rand() % 100000); 
    }
    return data;
}

// CSV Writing
void save_to_csv(string filename, double load_factor, long long time_ns, long long collisions) {
    ofstream file;
    file.open(filename, ios::app);
    
    file.seekp(0, ios::end);
    if (file.tellp() == 0) {
        file << "Load_Factor,Time_ns,Collisions\n";
    }
    
    file << load_factor << "," << time_ns << "," << collisions << "\n";
    file.close();
}

// Test Logic (LOAD FACTOR STRESS TEST)
void run_load_factor_test() {
    cout << "Starting Load Factor Stress Test..." << endl;
    
    // Test from 5% to 95% full
    for (double load = 0.05; load <= 0.95; load += 0.05) {
        clear_table();
        
        int num_items = (int)(TABLE_SIZE * load);
        vector<int> keys = generate_data(num_items);

        // 1. Fill table
        for(int k : keys) {
            Pair* p = new Pair;
            p->key = k;
            p->value = k; // using key = value for now? Can make this 0...
            insert(p);
        }

        // 2. Measure Search Time
        collision_count = 0; // Reset to count collisions during search
        
        auto start = chrono::high_resolution_clock::now(); //start clock
        
        // Search for every key we inserted (Successful search test)
        for(int k : keys) {
            search(k);
        }

        auto end = chrono::high_resolution_clock::now(); //end clock
        auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

        // 3. Save Results
        cout << "Load: " << load << " | Time: " << duration << "ns | Collisions: " << collision_count << endl;
        save_to_csv("results_linear.csv", load, duration, collision_count);
    }
}

int main() {
    srand(time(0));
    
    // Init table to nulls
    for(int i=0; i<TABLE_SIZE; i++) table[i] = nullptr;

    run_load_factor_test();

    return 0;
}