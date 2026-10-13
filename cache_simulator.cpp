// Smart Cache Management & Replacement Simulator

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <iomanip>
using namespace std;

// Base class: every replacement policy follows this interface

class Cache {
protected:
    int capacity;
    int hits = 0, misses = 0, evictions = 0;
public:
    explicit Cache(int cap) : capacity(cap) {}
    virtual ~Cache() {}

    // returns true on hit, false on miss. evicted = page removed (or -1)
    virtual bool access(int page, int &evicted) = 0;
    virtual string show() const = 0;
    virtual string name() const = 0;

    int getHits() const { return hits; }
    int getMisses() const { return misses; }
    int getEvictions() const { return evictions; }
    double hitRatio() const {
        int total = hits + misses;
        return total == 0 ? 0.0 : 100.0 * hits / total;
    }
};


// LRU: Hash Table (page -> node) + Doubly Linked List (usage order)
// Head = most recently used, Tail = least recently used
// All operations are O(1)

class LRUCache : public Cache {
    struct Node {
        int page;
        Node *prev, *next;
        Node(int p) : page(p), prev(nullptr), next(nullptr) {}
    };
    unordered_map<int, Node*> table;
    Node *head = nullptr, *tail = nullptr;

    void addToFront(Node *n) {
        n->prev = nullptr;
        n->next = head;
        if (head) head->prev = n;
        head = n;
        if (!tail) tail = n;
    }
    void removeNode(Node *n) {
        if (n->prev) n->prev->next = n->next; else head = n->next;
        if (n->next) n->next->prev = n->prev; else tail = n->prev;
        n->prev = n->next = nullptr;
    }
public:
    explicit LRUCache(int cap) : Cache(cap) {}
    ~LRUCache() {
        Node *cur = head;
        while (cur) { Node *nx = cur->next; delete cur; cur = nx; }
    }
    string name() const override { return "LRU (Hash + Doubly Linked List)"; }

    bool access(int page, int &evicted) override {
        evicted = -1;
        auto it = table.find(page);
        if (it != table.end()) {            // HIT: move to front
            removeNode(it->second);
            addToFront(it->second);
            hits++;
            return true;
        }
        misses++;                           // MISS
        if ((int)table.size() == capacity) { // full: evict tail (least recent)
            Node *victim = tail;
            evicted = victim->page;
            removeNode(victim);
            table.erase(victim->page);
            delete victim;
            evictions++;
        }
        Node *n = new Node(page);
        addToFront(n);
        table[page] = n;
        return false;
    }
    string show() const override {
        string s = "[MRU] ";
        for (Node *c = head; c; c = c->next) s += to_string(c->page) + (c->next ? " <-> " : "");
        if (!head) s += "(empty)";
        return s + " [LRU]";
    }
};


// FIFO: Hash Set (fast lookup) + Queue (arrival order)
// Oldest page is at the front of the queue and is evicted first

class FIFOCache : public Cache {
    unordered_set<int> table;
    queue<int> order;
public:
    explicit FIFOCache(int cap) : Cache(cap) {}
    string name() const override { return "FIFO (Hash + Queue)"; }

    bool access(int page, int &evicted) override {
        evicted = -1;
        if (table.count(page)) { hits++; return true; }   // HIT (order unchanged)
        misses++;
        if ((int)table.size() == capacity) {              // full: dequeue oldest
            evicted = order.front();
            order.pop();
            table.erase(evicted);
            evictions++;
        }
        order.push(page);
        table.insert(page);
        return false;
    }
    string show() const override {
        if (order.empty()) return "[OLDEST] (empty) [NEWEST]";
        queue<int> copy = order;                          // copy to print safely
        string s = "[OLDEST] ";
        while (!copy.empty()) {
            s += to_string(copy.front());
            copy.pop();
            if (!copy.empty()) s += " -> ";
        }
        return s + " [NEWEST]";
    }
};

// Simulator: runs accesses and prints results

void printAccess(const Cache &c, int page, bool hit, int evicted) {
    cout << "  Access " << setw(3) << page << " -> " << (hit ? "HIT " : "MISS");
    if (evicted != -1) cout << "  (evicted " << evicted << ")";
    cout << "\n    " << c.show() << "\n";
}

void printStats(const Cache &c) {
    cout << "  " << c.name() << "\n"
         << "    Hits: " << c.getHits() << "  Misses: " << c.getMisses()
         << "  Evictions: " << c.getEvictions()
         << "  Hit ratio: " << fixed << setprecision(1) << c.hitRatio() << "%\n";
}

vector<int> readSequence() {
    cout << "Enter page numbers separated by spaces (e.g. 1 2 3 1 4 2 5): ";
    string line;
    getline(cin, line);
    istringstream iss(line);
    vector<int> seq;
    int x;
    while (iss >> x) seq.push_back(x);
    return seq;
}

int readInt(const string &prompt, int minVal, int maxVal) {
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);
        try {
            int v = stoi(line);
            if (v >= minVal && v <= maxVal) return v;
        } catch (...) {}
        cout << "  Invalid input. Enter a number from " << minVal << " to " << maxVal << ".\n";
    }
}

unique_ptr<Cache> makeCache(int policy, int cap) {
    if (policy == 1) return unique_ptr<Cache>(new LRUCache(cap));
    return unique_ptr<Cache>(new FIFOCache(cap));
}

void compareMode(int cap) {
    vector<int> seq = readSequence();
    if (seq.empty()) { cout << "  No pages entered.\n"; return; }
    LRUCache lru(cap);
    FIFOCache fifo(cap);
    int ev;
    for (int p : seq) { lru.access(p, ev); fifo.access(p, ev); }
    cout << "\n=== Comparison (capacity " << cap << ", " << seq.size() << " accesses) ===\n";
    printStats(lru);
    printStats(fifo);
    if (lru.hitRatio() > fifo.hitRatio()) cout << "  Winner: LRU\n";
    else if (fifo.hitRatio() > lru.hitRatio()) cout << "  Winner: FIFO\n";
    else cout << "  Result: tie\n";
}

int main() {
    cout<< "  Smart Cache Management & Replacement Simulator\n";
       

    int cap = readInt("Enter cache capacity (1-20): ", 1, 20);
    int policy = readInt("Choose policy  1) LRU   2) FIFO : ", 1, 2);
    unique_ptr<Cache> cache = makeCache(policy, cap);

    while (true) {
        cout << "\n----------------- MENU -----------------\n"
             << " 1. Access one page\n"
             << " 2. Run a sequence of pages\n"
             << " 3. Show cache contents\n"
             << " 4. Show statistics\n"
             << " 5. Compare LRU vs FIFO on a sequence\n"
             << " 6. Reset / change capacity and policy\n"
             << " 0. Exit\n";
        int choice = readInt("Choice: ", 0, 6);
        int ev;

        if (choice == 0) {
            cout << "Goodbye!\n";
            break;
        } else if (choice == 1) {
            int p = readInt("Page number: ", 0, 100000);
            bool hit = cache->access(p, ev);
            printAccess(*cache, p, hit, ev);
        } else if (choice == 2) {
            for (int p : readSequence()) {
                bool hit = cache->access(p, ev);
                printAccess(*cache, p, hit, ev);
            }
            printStats(*cache);
        } else if (choice == 3) {
            cout << "  " << cache->show() << "\n";
        } else if (choice == 4) {
            printStats(*cache);
        } else if (choice == 5) {
            compareMode(cap);
        } else if (choice == 6) {
            cap = readInt("New cache capacity (1-20): ", 1, 20);
            policy = readInt("Choose policy  1) LRU   2) FIFO : ", 1, 2);
            cache = makeCache(policy, cap);
            cout << "  Cache reset.\n";
        }
    }
    return 0;
}
