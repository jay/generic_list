/*
Copyright (C) 2014-2015 Jay Satiro <raysatiro@yahoo.com>
All rights reserved.

This file is part of generic_list.

https://github.com/jay/generic_list

generic_list is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

generic_list is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with generic_list. If not, see <http://www.gnu.org/licenses/>.
*/

/** Stresstest for generic_list
Expects a temporary ramdisk on drive T. Refer to drive variable in main().
*/

#include <Windows.h>
#include <stdio.h>

#include <fstream>
#include <iostream>
#include <list>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "strerror.hpp"
#include "util.hpp"

#include "generic_list.h"


using namespace std;


static void pause( void )
{
        system( "pause" );
        return;
}

void init()
{
    /* If the program is started in its own window then pause before exit
    (eg user clicks on program in explorer, or vs debugger initiated program)
    */
    {
        HANDLE hOutput = GetStdHandle( STD_OUTPUT_HANDLE );
        CONSOLE_SCREEN_BUFFER_INFO csbi;

        ZeroMemory( &csbi, sizeof( csbi ) );

        if( ( hOutput != INVALID_HANDLE_VALUE )
            && ( GetFileType( hOutput ) == FILE_TYPE_CHAR )
            && GetConsoleScreenBufferInfo( hOutput, &csbi )
            && !csbi.dwCursorPosition.X
            && !csbi.dwCursorPosition.Y
            && ( csbi.dwSize.X > 0 )
            && ( csbi.dwSize.Y > 0 )
            )
        {
            // increase console buffer size to 10000 lines and pause before exit

            if( csbi.dwSize.Y < 10000 )
                csbi.dwSize.Y = 10000;

            SetConsoleScreenBufferSize( hOutput, csbi.dwSize );
            atexit( pause );
        }
    }

    util_init();
}


struct my_list;
struct my_node {
    DECLARE_NODE_MEMBERS( my_node, my_list );
};
struct my_list {
    DECLARE_LIST_MEMBERS( my_node );
};


// true if list is sane
bool sanity_check_list( my_list *list )
{
    DEBUG_IF( !list, "Missing list" );

    if( !list->count )
    {
        DEBUG_IF( list->head,
            "list->count is 0 but head exists. list->head: 0x" << list->head );

        DEBUG_IF( list->tail,
            "list->count is 0 but tail exists. list->tail: 0x" << list->tail );

        return true;
    }

    DEBUG_IF( !list->head && !list->tail, "Missing head and tail. list->count: " << list->count );
    DEBUG_IF( !list->head, "Missing head. list->count: " << list->count );
    DEBUG_IF( !list->tail, "Missing tail. list->count: " << list->count );

    size_t current_count = 0;
    for( my_node *previous_node = NULL, *node = list->head;
        node;
        previous_node = node, node = node->next )
    {
        ++current_count;

        DEBUG_IF( current_count > list->count,
            "More nodes in list than expected."
            << " current_count: " << current_count
            << ", list: 0x" << list
            << ", list->count: " << list->count
            << ", node: 0x" << node
            << ", list->tail: 0x" << list->tail );

        DEBUG_IF( previous_node != node->prev,
            "current_count: " << current_count
            << ", list: 0x" << list
            << ", previous_node: 0x" << previous_node
            << ", node->prev: 0x" << node->prev );

        DEBUG_IF( list != node->parent,
            "current_count: " << current_count
            << ", list: 0x" << list
            << ", node->parent: 0x" << node->parent );

        if( !node->next )
        {
            DEBUG_IF( current_count != list->count,
                "Our node count is not the same as list->count."
                << " current_count: " << current_count
                << ", list: 0x" << list
                << ", list->count: " << list->count );

            DEBUG_IF( node != list->tail,
                "The end of the list is not at tail."
                << " list: 0x" << list
                << ", node: 0x" << node
                << ", list->tail: 0x" << list->tail );
        }
    }

    return true;
}


// Generate a list and randomly modify its contents
bool generate_and_modify_list()
{
    my_list *list = NULL;
    DEBUG_IF( !( list = (my_list *)calloc( 1, sizeof my_list ) ), "Out of memory" );
    sanity_check_list( list );

    unsigned max_loop_count = getrand<unsigned>(0, 10);

    for( unsigned i = 0; i < max_loop_count; ++i )
    {
        my_node *node = NULL, *position_node = NULL;
        unsigned pos_node = 0, pos_position_node = 0;

        enum e_link { UNLINK, FIRST, LAST, BEFORE, AFTER };
        char *e_link_names[] = { "UNLINK", "FIRST", "LAST", "BEFORE", "AFTER" };
        // If there's no head there are no nodes so we can't link BEFORE or AFTER
        e_link link = list->head
            ? e_link( getrand<int>( (int)e_link( UNLINK ), (int)e_link( AFTER ) ) )
            : e_link( getrand<int>( (int)e_link( UNLINK ), (int)e_link( LAST ) ) );

        bool use_position_node = ( link == BEFORE || link == AFTER );
        if( use_position_node )
        {
            position_node = list->head;
            pos_position_node = getrand<unsigned>( 0, (unsigned)list->count - 1 );
            for( unsigned j = 0; position_node && j < pos_position_node; ++j )
            {
                position_node = position_node->next;
            }
            DEBUG_IF( !position_node, "Out of bounds. sanity_check_list should have caught this." );
        }

        enum e_which { NEW_NODE, EXISTING_NODE };
        e_which which = ( list->head && getrand<bool>() ) ? EXISTING_NODE : NEW_NODE;
        if( which == EXISTING_NODE )
        {
            node = list->head;
            if(node)
            {
                pos_node = getrand<unsigned>( 0, (unsigned)list->count - 1 );
                for( unsigned j = 0; j < pos_node; ++j )
                {
                    node = node->next;
                }
            }
        }
        else // NEW_NODE
        {
            DEBUG_IF( !( node = (my_node *)calloc( 1, sizeof my_node ) ),
                "Out of memory" );
        }

        switch(link)
        {
        case UNLINK:
            UNLINK_NODE( node );
            DEBUG_IF( node->next || node->prev || node->parent,
                "node was not properly unlinked from the list."
                << " list: 0x" << list
                << ", node: 0x" << node
                << ", node->next: 0x" << node->next
                << ", node->prev: 0x" << node->prev
                << ", node->parent: 0x" << node->parent
                << " (" << ( ( which == EXISTING_NODE ) ? "EXISTING_NODE" : "NEW_NODE" )
                << ", " << "UNLINK" << ")"
                );
            break;
        case FIRST:
            LINK_NODE_FIRST( node, list );
            DEBUG_IF( list != node->parent || list->head != node
                    || ( list->count == 1 && list->tail != node ),
                "node was not properly linked to the list."
                << " list: 0x" << list
                << ", list->head: 0x" << list->head
                << ", list->tail: 0x" << list->tail
                << ", node: 0x" << node
                << ", node->parent: 0x" << node->parent
                << " (" << ( ( which == EXISTING_NODE ) ? "EXISTING_NODE" : "NEW_NODE" )
                << ", " << "FIRST" << ")"
                );
            break;
        case LAST:
            LINK_NODE_LAST( node, list );
            DEBUG_IF( list != node->parent || list->tail != node
                    || ( list->count == 1 && list->head != node ),
                "node was not properly linked to the list."
                << " list: 0x" << list
                << ", list->head: 0x" << list->head
                << ", list->tail: 0x" << list->tail
                << ", node: 0x" << node
                << ", node->parent: 0x" << node->parent
                << " (" << ( ( which == EXISTING_NODE ) ? "EXISTING_NODE" : "NEW_NODE" )
                << ", " << "LAST" << ")"
                );
            break;
        case BEFORE:
            LINK_NODE_BEFORE( node, position_node );
            DEBUG_IF( position_node != node
                    && ( ( position_node->prev != node || node->next != position_node )
                        || ( list->count == 1 && ( list->head != node || list->tail != node ) )
                        || list != node->parent ),
                "node was not properly linked to the list."
                << " list: 0x" << list
                << ", node: 0x" << node
                << ", node->next: 0x" << node->next
                << ", position_node: 0x" << position_node
                << ", position_node->prev: 0x" << position_node->prev
                << " (" << ( ( which == EXISTING_NODE ) ? "EXISTING_NODE" : "NEW_NODE" )
                << ", " << "BEFORE" << ")"
                );
            break;
        case AFTER:
            LINK_NODE_AFTER( node, position_node );
            DEBUG_IF( position_node != node
                    && ( ( position_node->next != node || node->prev != position_node )
                        || ( list->count == 1 && ( list->head != node || list->tail != node ) )
                        || list != node->parent ),
                "node was not properly linked to the list."
                << " list: 0x" << list
                << ", node: 0x" << node
                << ", node->prev: 0x" << node->prev
                << ", position_node: 0x" << position_node
                << ", position_node->next: 0x" << position_node->next
                << " (" << ( ( which == EXISTING_NODE ) ? "EXISTING_NODE" : "NEW_NODE" )
                << ", " << "AFTER" << ")"
                );
            break;
        }

        bool found = false;
        for( my_node *p = list->head; p && !found; p = p->next )
        {
            if( p == node )
                found = true;
        }

        DEBUG_IF( !found && link != UNLINK,
            "node should have been linked but could not be found in the list."
            << " list: 0x" << list
            << ", node: 0x" << node
            << " (" << ( ( which == EXISTING_NODE ) ? "EXISTING_NODE" : "NEW_NODE" )
            << ", " << e_link_names[static_cast<int>(link)] << ")"
            );

        DEBUG_IF( found && link == UNLINK,
            "node should have been unlinked but was found in the list."
            << " list: 0x" << list
            << ", node: 0x" << node
            << " (" << ( ( which == EXISTING_NODE ) ? "EXISTING_NODE" : "NEW_NODE" )
            << ", " << "UNLINK" << ")"
            );

        if( link == UNLINK )
        {
            free( node );
            node = NULL;
        }

        sanity_check_list( list );
    }

    for( my_node *p = list->head; p; /**/ )
    {
        my_node *temp = p->next;
        UNLINK_NODE( p );
        free( p );
        p = temp;
    }

    free( list );

    return true;
}


size_t iteration;
stringstream mersenne_state_initial, mersenne_state_iteration, mersenne_state_iteration_prev;

int main( int argc, char *argv[] )
{
    init();

    if( argc > 1 )
    {
        string filename = argv[ 1 ];
        cout << "Restoring state from " << filename << endl;
        ifstream file( filename );
        if( !file.is_open() )
        {
            cerr << endl << "Failed to restore state " << endl;
            exit( 1 );
        }
        file >> mersenne;
        file.close();
        cout << endl;
    }

    // spec ios::left flag must be used on strinsgream before writing engine state
    mersenne_state_initial.setf( ios::left );
    mersenne_state_iteration.setf( ios::left );
    mersenne_state_iteration_prev.setf( ios::left );

    // copy the initial state of the PRNG
    mersenne_state_initial << mersenne;
    mersenne_state_iteration << mersenne;
    mersenne_state_iteration_prev << mersenne;


    // - create ramdisk
    // imdisk -a -s 10M -m T: -p "/fs:ntfs /q /y"
    //
    // - delete ramdisk
    // imdisk -d -m T:
    // or imdisk -D -m T: to force a removal.
    const string drive = "T:\\";

    // the max bytes to use on the ramdisk, might be a few bytes over
    // also i make buffer up to twice this size elsewhere
    const int max_ramdisk_size = 1048576;

    cout << "WARNING: The stresstest will read and write repeatedly to drive " << drive << endl
        << "It's highly preferable the drive be a RAM drive with >= "
        << ( max_ramdisk_size / 1048576 ) + 10 << " MB of free space." << endl << endl;

    SaveOutputToFile( drive + "\\state_initial.txt", mersenne_state_initial.str() );

    for( iteration = 1; iteration < SIZE_MAX; ++iteration )
    {
        mersenne_state_iteration_prev.swap( mersenne_state_iteration );
        stringstream().swap( mersenne_state_iteration );
        mersenne_state_iteration.setf( ios::left );
        mersenne_state_iteration << mersenne;

        cout << "Iteration " << FormatWithCommas<size_t>( iteration ) << endl;

        SaveOutputToFile( drive + "\\state_iteration.txt", mersenne_state_iteration.str() );
        SaveOutputToFile( drive + "\\state_iteration_prev.txt", mersenne_state_iteration_prev.str() );

        DEBUG_IF( !generate_and_modify_list(), "generate_and_modify_list() failed." );
    }

    return 0;
}
