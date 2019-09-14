#ifndef HASHTABLE_H_
#define HASHTABLE_H_

class HashTable{
private:
    class Node{
    public:
        const char* key;
        const void* data;
        Node* next;
    };
    const int _size;
    Node** table;
    int hash(const char* key);
public:
    HashTable(const int size);
    ~HashTable();
    void* search(const char* key);
    int add(const char* key,const void* data);
    int remove(const char* key);
};

#endif