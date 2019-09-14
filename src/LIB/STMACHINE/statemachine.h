#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include "hashtable/hashtable.h"

typedef char* (*GET_CONDITION_t)(void);

class StateBase{
public:
    virtual void entry(void* context)=0;
    virtual void update(void* context)=0;
    virtual void exit(void* context)=0;
};

class TransList{
private:
    HashTable list;
public:
    TransList();
    char* search(const char* condition);
    int add(const char* condition,const char* next);
    int remove(const char* condition);
};

class StateMachine{
private:
    class StateData{
    public:
        StateBase* object;
        TransList* trans_list;
        GET_CONDITION_t get_condition;
        char* search_next();
    };
    const char* _init_state;
    char* state;
    HashTable state_table;
public:
    StateMachine(const char* init_state);
    int run(void* context);
    char* get();
    int add(const char* name,const StateBase* object,const TransList* trans_list,const GET_CONDITION_t get_condition);
    int remove(const char* name);
};

#endif