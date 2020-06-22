/*
 * Copyright (c) 2016-2018 . All Rights Reserved.
 */

#include "utils.h"

int size_of_bytes(int str_len){
    int out_len = (str_len & 1) ? (str_len + 1) / 2 : str_len/2;
    return out_len;
}

uint8_t strtohex(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    if(c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if(c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 255;
}

int hex2byte_arr(char *buf, int len, uint8_t *out, int outbuf_size) {
    int i = len - 1;
    int out_len = (len & 1) ? (len + 1) / 2 : len/2;
    int j = out_len - 1;

    if (j > outbuf_size)
        return -1; /* Output buffer is smaller than need */

    while(i >= 0) {
        out[j] = strtohex(buf[i--]);
        if (i >= 0) {
            out[j--] |= strtohex(buf[i--]) << 4;
        }
    }

    return out_len;
}

void int8_to_char(uint8_t *buffer, int len, char *out) {
    const char hex[] = "0123456789abcdef";
    int max = 2 * len;
    int i = 0;
    int j = 0;
    while (j < len) {
        out[i++] = hex[(buffer[j] >> 4) & 0xF];
        out[i++] = hex[buffer[j] & 0xF];
        j++;
    }
    out[i] = '\0';
}

list_node_t* createElement(void * pointer, pb_size_t size ){
    list_node_t *head = NULL;

    head = (list_node_t *)malloc(sizeof(list_node_t));
    (head->element).pointer = pointer;
    (head->element).size = size;
    head->next = NULL;
    head->prev = NULL;

    return head;
}

list_node_t* addElementToList(list_node_t *head, void * pointer, pb_size_t size){
    list_node_t *new_element = NULL;

    new_element = createElement(pointer,size);

    if(NULL == head){
        return new_element;
    }

    while(NULL != head->next){
        head = head->next;
    }

    head->next = new_element;
    new_element->prev = head;

    return new_element;
}

void removeElementFromList(list_node_t *element, void * pointer, pb_size_t size){
     list_node_t *next = NULL, *prev = NULL;

     next = element->next;
     prev = element->prev;

     if( NULL != next  ){
        next->prev = prev;
     }

     if( NULL != prev  ){
        next->next = next;
     }

     free(element);

     return;
}

void clearLeftOfElement(list_node_t *element){
    list_node_t *prev = NULL, *tmp = NULL;
    prev = element->prev;
    element->prev = NULL;

    while(NULL != prev){
        tmp = prev;
        prev = prev->prev;
        free(tmp);
        tmp = NULL;
    }
}

void clearRightOfElement(list_node_t *element){
    list_node_t *next = NULL, *tmp = NULL;
    next = element->next;
    element->next = NULL;

    while(NULL != next){
        tmp = next;
        next = next->next;
        free(tmp);
        tmp = NULL;
    }
}

void clearList(list_node_t **element){
    clearLeftOfElement(&element);
    clearRightOfElement(&element);
    free(&element);
    element = NULL;
}
