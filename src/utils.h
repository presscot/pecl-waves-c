/*
 * Copyright (c) 2016-2018 . All Rights Reserved.
 */

#ifndef RLP_UTILS_H
#define RLP_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

typedef uint32_t pb_size_t;
typedef uint_least8_t pb_byte_t;

int size_of_bytes(int str_len);
uint8_t strtohex(char c);

typedef struct list_element {
    void *pointer;
    pb_size_t size;
} list_element_t;
typedef struct list_node {
    list_element element;
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

int hex2byte_arr(char *buf, int len, uint8_t *out, int outbuf_size);
void int8_to_char(uint8_t *buffer, int len, char *out);

list_node_t* createElement(void * pointer, pb_size_t size );
list_node_t* addElementToList(list_node_t *head, void * pointer, pb_size_t size);
void removeElementFromList(list_node_t *element, void * pointer, pb_size_t size);
void clearLeftOfElement(list_node_t *element);
void clearRightOfElement(list_node_t *element);
void clearList(list_node_t **element);


#endif //RLP_UTILS_H
