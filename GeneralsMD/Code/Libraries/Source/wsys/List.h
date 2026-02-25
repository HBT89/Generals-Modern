#pragma once
#ifndef WSYS_LIST_H
#define WSYS_LIST_H

// wsys List stub - proprietary library removed
// Minimal linked list declarations for compilation

#include <cstddef>

struct ListNode
{
    void* data;
    ListNode* next;
};

typedef ListNode* ListHandle;

inline ListHandle ListCreate(void)
{
    return NULL;
}

inline void ListDestroy(ListHandle list)
{
    (void)list;
}

inline void ListAppend(ListHandle list, void* data)
{
    (void)list; (void)data;
}

#endif // WSYS_LIST_H
