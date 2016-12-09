#ifndef USER_LIST_H
#define USER_LIST_H

#include <stdbool.h>

struct List_node
{
    char* text;
    int counter;
    struct List_node* prev;
    struct List_node* next;
};

struct List_node* list_node_new( char const* text );
void list_node_free( struct List_node* node );
//void list_node_print( char*, void*, void* );

struct List
{
    struct List_node* head;
    struct List_node* tail;
};

struct List* list_new();
struct List_node* list_begin( struct List* list );
struct List_node* list_end( struct List* list );
void list_free( struct List* );
void list_push_front( struct List*, char* );
/*void list_push_back( struct List*, char*, void* );
void list_sorted_insert( struct List*, char*, void* );
char* list_pop_front( struct List*, void** );
char* list_pop_back( struct List*, void** );
void list_del_node( struct List*, char const* );
bool list_node_exists( struct List*, char const* );
void list_for_each( struct List* list, void( *func )( char*, void* ) );*/
#endif
