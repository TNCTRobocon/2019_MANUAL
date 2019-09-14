#include "statemachine.h"

#include <stdlib.h>

#define HASHTABLE_SIZE 32

TransList::TransList():list(HASHTABLE_SIZE)
{
}

char* TransList::search(const char* condition){
    return (char*)list.search(condition);
}

int TransList::add(const char* condition,const char* next){
    return list.add(condition,next);
}

int TransList::remove(const char* condition){
    return list.remove(condition);
}

char* StateMachine::StateData::search_next(){
    return trans_list->search(get_condition());
}

StateMachine::StateMachine(const char* init_state):_init_state(init_state),state_table(HASHTABLE_SIZE)
{
    state=NULL;
}

int StateMachine::run(void* context){
    StateData* state_data=(StateData*)state_table.search(state);
    if(state_data==NULL){
        state=(char*)_init_state;
        state_data=(StateData*)state_table.search(state);
        if(state_data==NULL){
            return -1;
        }
        state_data->object->entry(context);
    }

    char* next=state_data->search_next();
    if(next!=NULL){
        state_data->object->exit(context);
        state=next;
        state_data=(StateData*)state_table.search(state);
        if(state_data==NULL){
            return -1;
        }
        state_data->object->entry(context);
        state_data->object->update(context);
        return 1;
    }

    state_data->object->update(context);

    return 0;
}

char* StateMachine::get(){
    return state;
}

int StateMachine::add(const char* name,const StateBase* object,const TransList* trans_list,const GET_CONDITION_t get_condition){
    if(state_table.search(name)!=NULL){
        return -1;
    }

    StateData* temp=new StateData;
    temp->object=(StateBase*)object;
    temp->trans_list=(TransList*)trans_list;
    temp->get_condition=(GET_CONDITION_t)get_condition;
    state_table.add(name,temp);

    return 1;
}

int StateMachine::remove(const char* name){
    StateData* temp=(StateData*)state_table.search(name);
    if(temp==NULL){
        return -1;
    }

    state_table.remove(name);
    delete temp;

    return 1;
}