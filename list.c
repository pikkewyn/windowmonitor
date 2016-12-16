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
		lnode->counter = 1;
    lnode->prev = NULL;
    lnode->next = NULL;
    return lnode;
}

void list_node_free( struct List_node* lnode )
{
    free( lnode );
    lnode = NULL;
}


void list_node_print( int counter, char* text )
{
    if( text == NULL )
    {
        printf( "NULL: %d\n", counter );
    }
    else {
      printf( "%s: %d\n", text, counter );
    }
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

void list_free( struct List* list )
{
    if( list == NULL || list->head == NULL )
    {
        return;
    }

    struct List_node* curr = list->head;

    while( curr != NULL )
    {
        struct List_node* next = curr->next;
        
        free( curr->text );
        free( curr );

        curr = next;
    }
    
    free( list );
    list = NULL;
}

void list_push_front( struct List* list, char* text )
{
    assert( list );
    struct List_node* lnode = list_node_new( text );
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

void list_accumulated_insert( struct List* list, char* text, bool ( *comparator )( char const*, char const*, int ), int factor )
{
    assert( list );

    if( text == NULL )
    {
        return;
    }

    if( list->head == NULL )
    {
        list_push_back( list, text );
        return;
    }

    struct List_node* tmp = list->head;

    while( tmp != NULL )
    {
				if( comparator( tmp->text, text, factor ) )
				{
        		tmp->counter++;
						return;
				}
				
				tmp = tmp->next;
    }

    list_push_back( list, text );
    return;
}


void list_push_back( struct List* list, char* text )
{
    assert( list );
    struct List_node* newNode = list_node_new( text );
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
/*    assert( list );

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
        *data = lnode->text;
    }

    free( lnode );
    return id;*/
}

char* list_pop_back( struct List* list, void** data )
{
/*    assert( list );

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
        *data = lnode->text;
    }

    free( lnode );
    return id;*/
}

bool list_node_exists( struct List* list, char const* id )
{
/*    assert( list );
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

    return false;*/
}

void list_del_node( struct List* list, char const* id )
{
/*    struct List_node* prev = NULL;
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
    }*/
}


void list_for_each( struct List* list, void( *func )( int, char* ) )
{
    assert( list );
    struct List_node* tmp = list->head;

    while( tmp != NULL )
    {
        func( tmp->counter, tmp->text );
        tmp = tmp->next;
    }
}


struct List_node* List_merge( struct List_node* first, struct List_node* second )
{
    if (!first)
        return second;
 
    if (!second)
        return first;
 

    if( first->counter < second->counter )
    {
        first->next = List_merge( first->next,second );
        first->next->prev = first;
        first->prev = NULL;
        return first;
    }
    else
    {
        second->next = List_merge( first,second->next );
        second->next->prev = second;
        second->prev = NULL;
        return second;
    }
}


struct List_node* List_split( struct List_node* head )
{
    struct List_node* fast = head, *slow = head;
    while( fast->next && fast->next->next )
    {
        fast = fast->next->next;
        slow = slow->next;
    }

    struct List_node *temp = slow->next;
    slow->next = NULL;
    return temp;
}
 
struct List_node* List_merge_sort( struct List_node *head )
{
    if( !head || !head->next )
        return head;
    struct List_node* second = List_split( head );
 
    head = List_merge_sort( head );
    second = List_merge_sort( second );
 
    return List_merge( head, second );
}



