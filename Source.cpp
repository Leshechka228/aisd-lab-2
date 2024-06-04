#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include <algorithm>

using namespace std;

template <typename K, typename T>
class MyHashMap {
private:
    struct Node {
        K key;
        T value;
    };

    vector<Node>* table;

    int size;

    int hash_function(K key) {
        return key % size;
    }

public:

    MyHashMap() : size(16){
        table = new vector<Node>[size];
    }

    void resizeTable(int newSize) {
        std::vector<Node>* newTable = new std::vector<Node>[newSize];
        for (int i = 0; i < size; ++i) {
            for (const auto& node : table[i]) {
                int newHash = hash_function(node.key) % newSize;
                newTable[newHash].push_back(node);
            }
        }
        delete[] table; 
        table = newTable;
        size = newSize;
    }

    MyHashMap(int random_values_count) : MyHashMap() {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 100);

        for (int i = 0; i < random_values_count; ++i) {
            K key = i;
            T value = dis(gen);
            insert(key, value);
        }
    }

    MyHashMap(const MyHashMap& other) {
        size = other.size;
        table = new vector<Node>[size];

        for (int i = 0; i < size; ++i) {
            table[i] = other.table[i];
        }
    }

    ~MyHashMap() {
        delete[] table;
    }

    MyHashMap& operator=(const MyHashMap& other) {
        if (this != &other) {
            for (int i = 0; i < size; ++i) {
                table[i] = other.table[i];
            }
        }
        return *this;
    }

    void print() {
        for (int i = 0; i < size; ++i) {
            cout << "Bucket " << i << ": ";
            for (const auto& pair : table[i]) {
                cout << "[" << pair.key << ", " << pair.value << "] ";
            }
            cout << endl;
        }
    }

    void insert(K key, T value) {
        int index = hash_function(key);
        table[index].push_back({ key, value });
    }

    void insert_or_assign(K key, T value) {
        int index = hash_function(key);
        for (auto& pair : table[index]) {
            if (pair.key == key) {
                pair.value = value;
                return;
            }
        }
        insert(key, value);
    }

    bool contains(const T& value) {
        for (int i = 0; i < size; ++i) {
            for (const auto& pair : table[i]) {
                if (pair.value == value) {
                    return true;
                }
            }
        }
        return false;
    }

    T* search(K key) {
        int index = hash_function(key);
        for (auto& pair : table[index]) {
            if (pair.key == key) {
                return &pair.value;
            }
        }
        return nullptr;
    }

    bool erase(K key) {
        int index = hash_function(key);
        auto& bucket = table[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->key == key) {
                bucket.erase(it);
                return true;
            }
        }
        return false;
    }

    int count(K key) {
        int index = hash_function(key);
        int count = 0;
        for (const auto& pair : table[index]) {
            if (pair.key == key) {
                count++;
            }
        }
        return count;
    }

    int count_collisions() {
        int collisions = 0;
        for (int i = 0; i < size; ++i) {
            if (table[i].size() > 1) {
                collisions += table[i].size() - 1;
            }
        }
        return collisions;
    }
};

void performExperiments() {
    const int elementCount = 20;
    const int experimentCount = 100;
    const int step = 50;

    default_random_engine generator(random_device{}());

    for (int tableSize = 25; tableSize <= 475; tableSize += step) {
        int collisionCount = 0;

        for (int i = 0; i < experimentCount; ++i) {
            MyHashMap<int, string> myMap;
            myMap.resizeTable(tableSize);

            uniform_int_distribution<int> distribution(1, 1000);

            for (int j = 0; j < elementCount; ++j) {
                int key = distribution(generator);
                myMap.insert(key, "Value");
            }

            // Подсчет количества коллизий
            collisionCount += (elementCount - myMap.count_collisions());
        }

        double collisionProbability = static_cast<double>(collisionCount) / (elementCount * experimentCount);
        cout << "Table size: " << tableSize << ", Collision probability: " << collisionProbability << endl;

        if (collisionProbability < 0.5) {
            cout << "Collision probability is less than 50% for table size: " << tableSize << endl;
            break;
        }
    }
}

int main() {

    performExperiments();

    MyHashMap<int, std::string> myMap;

    myMap.insert(1, "One");
    myMap.insert(11, "Eleven");
    myMap.insert(2, "Two");
    myMap.insert(3, "Three");
    myMap.insert(13, "Thirteen");

    myMap.print();

    myMap.insert_or_assign(2, "NewTwo");
    myMap.insert_or_assign(4, "Four");

    myMap.print();

    std::cout << "Contains 'Two': " << myMap.contains("Two") << std::endl;
    std::cout << "Contains 'Five': " << myMap.contains("Five") << std::endl;

    int key = 3;
    std::string* value = myMap.search(key);
    if (value) {
        std::cout << "Value for key " << key << ": " << *value << std::endl;
    }
    else {
        std::cout << "Key not found" << std::endl;
    }

    std::cout << "Erasing key 3: " << myMap.erase(3) << std::endl;
    std::cout << "Erasing key 5: " << myMap.erase(5) << std::endl;

    std::cout << "Count of key 2: " << myMap.count(2) << std::endl;

    return 0;
}