#include "hashtable.h"

#include <string.h>

HashTable::HashTable(int size):_size(size)
{
    table=new Node*[_size];
    for(int i=0;i<_size;i++){
        table[i]=NULL;
    }
}

int HashTable::hash(const char* key){
    if(key==NULL){
        return 0;
    }

    int tale=strlen(key)-1;
    
    int hash_value=(key[0]+key[tale/2]+key[tale])%_size;
    return hash_value;
}

HashTable::~HashTable(){
    for(int i=0;i<_size;i++){
        Node* p_node=table[i];
        while(p_node!=NULL){
            Node* next=p_node->next;
            delete p_node;
            p_node=next;
        }
    }

    delete table;
}

void* HashTable::search(const char* key){
    int hash_value=hash(key);
    Node* p_node=table[hash_value];

    while(p_node!=NULL){
        if(!strcmp(p_node->key,key)){
            return (void*)p_node->data;
        }
        p_node=p_node->next;
    }

    return NULL;
}

int HashTable::add(const char* key,const void* data){
    int hash_value=hash(key);
    Node* p_node=table[hash_value];
    Node* temp_node;

    while(p_node!=NULL){
        if(!strcmp(p_node->key,key)){
            return -1;
        }
        p_node=p_node->next;
    }

    temp_node=new Node;
    temp_node->key=key;
    temp_node->data=data;
    temp_node->next=table[hash_value];
    table[hash_value]=temp_node;

    return 1;
}

int HashTable::remove(const char* key){
    int hash_value=hash(key);
    Node* p_node=table[hash_value];
    Node** p_node_pointer=&table[hash_value];

    while(p_node!=NULL){
        if(!strcmp(p_node->key,key)){
            *p_node_pointer=p_node->next;
            delete p_node;
            return 1;
        }
        p_node_pointer=&p_node->next;
        p_node=p_node->next;
    }

    return -1;
}