#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "list.h"

struct List_node* list_node_new( char* text )
{
    struct List_node* lnode = calloc( 1, sizeof( struct List_node ) );
    lnode->text = text;
    lnode->counter = 0;
    lnode->prev = NULL;
    lnode->next = NULL;
    return lnode;
}

void list_node_free( struct List_node* lnode )
{
    free( lnode );
    lnode = NULL;
}


void list_node_print( char* id, void* data, void* arg )
{
    ( void ) data;
    ( void ) arg;
    printf( "%s\n", id );
}

struct List* list_new()
{
    struct List* list = calloc( 1, sizeof( struct List ) );
    list->head = NULL;
    list->tail = NULL;
    return list;
}

struct List_node* list_begin( struct List* list )
{
    return list->head;
}

struct List_node* list_end( struct List* list )
{
    return list->tail;
}
/*
void list_free( struct List* list )
{
    if( list == NULL )
    {
        return;
    }

    free( list );
    list = NULL;
}

void list_push_front( struct List* list, char* id, void* data )
{
    assert( list );
    struct List_node* lnode = list_node_new( id, data );
    lnode->next = list->head;
    lnode->prev = NULL;

    if( list->head != NULL )
    {
        list->head->prev = lnode;
    }

    list->head = lnode;

    if( list->tail == NULL )
    {
        list->tail = lnode;
    }
}

void list_sorted_insert( struct List* list, char* id, void* data )
{
    assert( list );

    if( list->head == NULL )
    {
        list_push_back( list, id, data );
        return;
    }

    struct List_node* tmp = list->head;

    int comparison;

    while( tmp != NULL && ( comparison = strcmp( tmp->id, id ) ) < 0 )
    {
        tmp = tmp->next;
    }

    if( comparison == 0 )
    {
        return;
    }

    if( tmp == NULL )
    {
        list_push_back( list, id, data );
        return;
    }

    struct List_node* lnode = list_node_new( id, data );

    lnode->next = tmp;

    lnode->prev = tmp->prev;

    if( tmp->prev != NULL )
    {
        tmp->prev->next = lnode;
    }

    tmp->prev = lnode;

    if( tmp == list->head )
    {
        list->head = lnode;
    }
}


void list_push_back( struct List* list, char* id, void* data )
{
    assert( list );
    struct List_node* newNode = list_node_new( id, data );
    newNode->next = NULL;
    newNode->prev = list->tail;

    if( list->tail != NULL )
    {
        list->tail->next = newNode;
    }

    list->tail = newNode;

    if( list->head == NULL )
    {
        list->head = newNode;
    }
}

char* list_pop_front( struct List* list, void** data )
{
    assert( list );

    if( list->head == NULL )
    {
        return NULL;
    }

    struct List_node* lnode = list->head;

    list->head = list->head->next;

    if( list->head != NULL )
    {
        list->head->prev = NULL;
    }

    char* id = lnode->id;

    if( data != NULL )
    {
        *data = lnode->data;
    }

    free( lnode );
    return id;
}

char* list_pop_back( struct List* list, void** data )
{
    assert( list );

    if( list->tail == NULL )
    {
        return NULL;
    }

    struct List_node* lnode = list->tail;

    list->tail = list->tail->prev;

    if( list->tail != NULL )
    {
        list->tail->next = NULL;
    }

    char* id = lnode->id;

    if( data != NULL )
    {
        *data = lnode->data;
    }

    free( lnode );
    return id;
}



bool list_node_exists( struct List* list, char const* id )
{
    assert( list );
    assert( id );
    struct List_node* lnode = list->head;

    while( lnode != NULL )
    {
        if( strcmp( lnode->id, id ) == 0 )
        {
            return true;
        }
        else
        {
            lnode = lnode->next;
        }
    }

    return false;
}

void list_del_node( struct List* list, char const* id )
{
    struct List_node* prev = NULL;
    struct List_node* node = list->head;

    while( node != NULL )
    {
        if( strcmp( node->id, id ) == 0 )
        {
            if( prev == NULL )
            {
                list->head = node->next;
            }
            else
            {
                prev->next = node->next;
            }

            list_node_free( node );
            break;
        }

        prev = node;
        node = node->next;
    }
}


void list_for_each( struct List* list, void( *func )( char*, void* ) )
{
    assert( list );
    struct List_node* tmp = list->head;

    while( tmp != NULL )
    {
        func( tmp->id, tmp->data );
        tmp = tmp->next;
    }
}
*/

