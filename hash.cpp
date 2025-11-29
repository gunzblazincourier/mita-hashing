#include <bits/stdc++.h>

using namespace std;

enum Algorithms {L, Q, D, C}; // Added 'C' for chaining
enum Algorithms alg = L;

const int TABLE_SIZE = 10007;
struct Pair {
    int key;
    int value;
};

Pair* table[TABLE_SIZE];
vector<Pair*> table2[TABLE_SIZE];

long long collision_count = 0;
void flip_enum_alg(Algorithms a) {
    alg = a;
}

int hash_func(int key) {
    return key % TABLE_SIZE;
}

int hash_func2(int key) {
    return 983 - (key % 983);
}

// Helper to clear table between tests
void clear_table() {
    // 1. Clear Open Addressing Table
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table[i] != nullptr) {
            delete table[i]; 
            table[i] = nullptr;
        }
    }
    // 2. Clear Chaining Table
    for (int i = 0; i < TABLE_SIZE; i++) {
        for (Pair* p : table2[i]) {
            delete p; 
        }
        table2[i].clear(); 
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
            index = ((int)(index + pow(attempts, 2))) % TABLE_SIZE;
        } else if (alg == D) {
            index = ((int)(index + (attempts * hash_func2(p->key)))) % TABLE_SIZE;
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
            index = ((int)(index + pow(attempts, 2))) % TABLE_SIZE;
        } else if (alg == D) {
            index = ((int)(index + (attempts * hash_func2(key)))) % TABLE_SIZE;
        }
        
        if (attempts >= TABLE_SIZE) break;
    }
    return nullptr;
}

bool insert2(Pair* p) {
    if (p == nullptr) {
        return false;
    }
    int index = hash_func(p->key);

    // NOTE: Assumption that all indices will contain vectors (temporary)
    // // Insert vector in index if none
    // if (table2[index] == nullptr) {
    //     vector<Pair*> v;
    //     table2[index] = v;
    // }

    // Replace value in case of duplicate key
    for (int i = 0; i < table2[index].size(); i++) {
        if (table2[index][i]->key == p->key) {
            table2[index][i]->value = p->value;
            return true;
        }
    }

    // Else add Pair to vector at index position
    table2[index].push_back(p);
    return true;
}

Pair* search2(int key) {
    int index = hash_func(key);

    for (Pair* p : table2[index]) {
        if (p->key == key) {
            return p;
        }
        collision_count++;
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
void save_to_csv(string filename, string strategy_name, double load_factor, long long time_ns, long long collisions) {
    ofstream file;
    file.open(filename, ios::app);
    
    file.seekp(0, ios::end);
    if (file.tellp() == 0) {
        file << "Strategy,Load_Factor,Time_ns,Collisions\n";
    }
    
    file << strategy_name << "," << load_factor << "," << time_ns << "," << collisions << "\n";
    file.close();
}

// Test Logic (LOAD FACTOR STRESS TEST)
void run_load_factor_test() {
    // We will loop through 0,1,2,3 (L, Q, D, C)
    vector<string> names = {"Linear", "Quadratic", "Double", "Chaining"};
    vector<Algorithms> algs = {L, Q, D, C};

    for(int s = 0; s < 4; s++) {
        // Set the global strat
        alg = algs[s];
        string current_name = names[s];
        
        cout << "\n=== Testing Strategy: " << current_name << " ===" << endl;

        // Test from 5% to 95% full
        for (double load = 0.05; load <= 0.95; load += 0.05) {
            clear_table();
            
            int num_items = (int)(TABLE_SIZE * load);
            vector<int> keys = generate_data(num_items);

            // 1. Fill table
            for(int k : keys) {
                Pair* p = new Pair;
                p->key = k;
                p->value = k;
                
                // Switch Logic for Insert
                if (alg == C) insert2(p);
                else insert(p);
            }

            // 2. Measure Search Time
            collision_count = 0; 
            
            auto start = chrono::high_resolution_clock::now();
            
            for(int k : keys) {
                // Switch Logic for Search
                if (alg == C) search2(k);
                else search(k);
            }

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

            // 3. Save Results
            cout << "Load: " << load << " | Time: " << duration << "ns | Collisions: " << collision_count << endl;
            save_to_csv("hash_results.csv", current_name, load, duration, collision_count);
        }
    }
}

void run_search_test() {
    cout << "\n=== Starting Successful vs Unsuccessful Search Test ===" << endl;

    vector<string> names = {"Linear", "Quadratic", "Double", "Chaining"};
    vector<Algorithms> algs = {L, Q, D, C};
    
    // Fix load fact. at 70% for this experiment?
    double load = 0.70;
    int num_items = (int)(TABLE_SIZE * load);

    for(int s = 0; s < 4; s++) {
        alg = algs[s];
        clear_table();

        // 1. Gen the Data
        vector<int> present_keys; // Keys IN table
        vector<int> absent_keys;  // Keys NOT be in table

        // Gen even numbers for "Present" keys and odd for "Absent" keys to ensure no overlap
        for(int i=0; i<num_items; i++) {
            present_keys.push_back(i * 2);      // 0, 2, 4...
            absent_keys.push_back((i * 2) + 1); // 1, 3, 5...
        }

        // 2. Fill Table
        for(int k : present_keys) {
            Pair* p = new Pair;
            p->key = k;
            p->value = k;
            if (alg == C) insert2(p);
            else insert(p);
        }

        // 3. Test Successful Search
        collision_count = 0;
        auto start_succ = chrono::high_resolution_clock::now();
        for(int k : present_keys) {
            if (alg == C) search2(k);
            else search(k);
        }
        auto end_succ = chrono::high_resolution_clock::now();
        long long time_succ = chrono::duration_cast<chrono::nanoseconds>(end_succ - start_succ).count();
        
        save_to_csv("search_test_results.csv", names[s] + "_Success", load, time_succ, collision_count);
        cout << names[s] << " Success | Time: " << time_succ << " | Collisions: " << collision_count << endl;

        // 4. Test Unsuccessful Search
        collision_count = 0;
        auto start_fail = chrono::high_resolution_clock::now();
        for(int k : absent_keys) {
            if (alg == C) search2(k);
            else search(k);
        }
        auto end_fail = chrono::high_resolution_clock::now();
        long long time_fail = chrono::duration_cast<chrono::nanoseconds>(end_fail - start_fail).count();

        save_to_csv("search_test_results.csv", names[s] + "_Fail", load, time_fail, collision_count);
        cout << names[s] << " Fail    | Time: " << time_fail << " | Collisions: " << collision_count << endl;
    }
}

int main() {
    srand(time(0));
    
    // Init table to nulls
    for(int i=0; i<TABLE_SIZE; i++) table[i] = nullptr;

    run_load_factor_test();
    run_search_test();

    return 0;
}