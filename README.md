# Smart Cache Management & Replacement Simulator

A C++ console project that shows how a **cache** works: it stores pages, and when it is full, it removes one page to make room. The program compares two replacement policies, **LRU** and **FIFO**.


##  Problem Statement
A cache is small and fast, so it can only hold a few items. When a new item arrives and the cache is full, we must decide **which old item to remove**. This project simulates that decision and measures how good each policy is.

##  Features
- Choose the cache capacity and the policy (LRU or FIFO)
- Access one page at a time, or run a whole sequence
- See the cache contents after every access
- Statistics: hits, misses, evictions, hit ratio
- Compare LRU vs FIFO on the same sequence

##  Data Structures Used

| Policy | Data Structures | Role |
|--------|-----------------|------|
| **LRU** | Hash Table + Doubly Linked List | Hash table finds a page instantly. List keeps usage order (head = most recent, tail = least recent). |
| **FIFO** | Hash Set + Queue | Hash set checks if a page exists. Queue keeps arrival order (front = oldest). |

##  How It Works
- **Hit:** the page is already in the cache.
  - LRU moves it to the front of the list.
  - FIFO leaves the order unchanged.
- **Miss:** the page is not in the cache. If the cache is full, one page is evicted, then the new page is added.
  - LRU evicts the **least recently used** page.
  - FIFO evicts the **oldest** page.

##  Time Complexity

| Operation | LRU | FIFO |
|-----------|-----|------|
| Lookup | O(1) | O(1) |
| Insert | O(1) | O(1) |
| Evict | O(1) | O(1) |

##  Future Improvements
- [ ] Add LFU (Least Frequently Used) policy
- [ ] Add a graphical interface
- [ ] Load access sequences from a file
