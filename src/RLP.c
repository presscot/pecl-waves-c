/*
 * Copyright (c) 2016-2018 . All Rights Reserved.
 */

#include <stdlib.h>
#include <string.h>
#include "RLP.h"

#define SIZE_THRESHOLD 56
#define OFFSET_SHORT_LIST 0xc0
#define OFFSET_LONG_LIST 0xf7
#define OFFSET_LONG_ITEM  0xb7
#define OFFSET_SHORT_ITEM 0x80

int wallet_copy_rpl(uint8_t *source, uint8_t *destination, uint8_t size,
                    int copyPos) {
    int ret_val = copyPos;
    if (size != 0) {
        memcpy(source, destination, size);
        ret_val = ret_val + size;
    }
    return ret_val;
}

int wallet_encode_list(uint64_t *rawTx, uint32_t totalLength, list_node_t* list) {
    uint8_t *data;

    list = list_node_t* getFirstElementOfList(list_node_t *list);

    int copyPos;
    if (totalLength < SIZE_THRESHOLD) {
        data = malloc(1 + totalLength);
        data[0] = (int8_t) (OFFSET_SHORT_LIST + totalLength);
        copyPos = 1;
    } else {
        int tmpLength = totalLength;
        uint8_t byteNum = 0;
        while (tmpLength != 0) {
            ++byteNum;
            tmpLength = tmpLength >> 8;
        }
        tmpLength = totalLength;
        uint8_t *lenBytes;
        lenBytes = malloc(byteNum);
        int i;
        for (i = 0; i < byteNum; ++i) {
            lenBytes[byteNum - 1 - i] =
                    (uint8_t) ((tmpLength >> (8 * i)) & 0xFF);
        }
        data = malloc(1 + byteNum + totalLength);
        data[0] = (uint8_t) (OFFSET_LONG_LIST + byteNum);
        memcpy(data + 1, lenBytes, byteNum);
        copyPos = byteNum + 1;
        free(lenBytes);
    }

    while( NULL != list ){
        (list->element).pointer;
        (list->element).size;

        copyPos = wallet_copy_rpl(data + copyPos, (list->element).pointer, (list->element).size, copyPos);

        list = list->next;
    }

    memcpy(rawTx, data, copyPos);
    return copyPos;
}

void wallet_encode_element(pb_byte_t *bytes, pb_size_t size,
                           pb_byte_t *new_bytes, pb_size_t *new_size, bool remove_leading_zeros) {

    pb_byte_t *pbytes;
    pb_size_t psize;


    if (remove_leading_zeros) {
        int leading_count = 0;
        for (int j = 0; j < size; ++j) {
            pb_byte_t singleByte = bytes[j];
            if ((singleByte | 0x00) == 0) {
                leading_count = leading_count + 1;
            } else {
                break;
            }
        }
        if (leading_count > 0) {
            pbytes = malloc(size - leading_count);
            memcpy(pbytes, bytes + 1, (size - leading_count));
            psize = (pb_size_t) (size - leading_count);
        }else{
            pbytes = malloc(size);
            memcpy(pbytes, bytes, (size));
            psize = size;
        }
    }else{
        pbytes = malloc(size);
        memcpy(pbytes, bytes, (size));
        psize = size;
    }

    if (psize == 0) {
        *new_size = 1;
        new_bytes[0] = (pb_byte_t) OFFSET_SHORT_ITEM;
    } else if (psize == 1 && pbytes[0] == 0x00) {
        *new_size = 1;
        new_bytes[0] = pbytes[0];
    } else if (psize == 1 && ((pbytes[0] & 0xFF) == 0)) {
        *new_size = 1;
        new_bytes[0] = pbytes[0];
    } else if (psize == 1 && (pbytes[0] & 0xFF) < 0x80) {
        *new_size = 1;
        new_bytes[0] = pbytes[0];
    } else if (psize < SIZE_THRESHOLD) {
        pb_byte_t length = (pb_byte_t) (OFFSET_SHORT_ITEM + psize);
        new_bytes[0] = length;
        memcpy(new_bytes + 1, pbytes, psize);
        *new_size = psize + 1;
    } else {
        int tmpLength = psize;
        pb_byte_t lengthOfLength = (pb_byte_t) 0;
        while (tmpLength != 0) {
            ++lengthOfLength;
            tmpLength = tmpLength >> 8;
        }
        pb_byte_t *data = malloc(1 + lengthOfLength + psize);
        data[0] = (pb_byte_t) (OFFSET_LONG_ITEM + lengthOfLength);
        tmpLength = psize;
        int i;
        for (int i = lengthOfLength; i > 0; --i) {
            data[i] = (pb_byte_t) (tmpLength & 0xFF);
            tmpLength = tmpLength >> 8;
        }
        memcpy(data + 1 + lengthOfLength, pbytes, psize);
        memcpy(new_bytes, data, ((1 + lengthOfLength + psize)));
        *new_size = (1 + lengthOfLength + psize);
        free(pbytes);
        free(data);
    }
}

void wallet_encode_byte(pb_byte_t singleByte, pb_byte_t *new_bytes) {
    if ((singleByte & 0xFF) == 0) {
        new_bytes[0] = (pb_byte_t) OFFSET_SHORT_ITEM;
    } else if ((singleByte & 0xFF) <= 0x7F) {
        new_bytes[0] = (pb_byte_t) singleByte;
    } else {
        new_bytes[0] = (pb_byte_t) (OFFSET_SHORT_ITEM + 1);
        new_bytes[1] = singleByte;
    }
}

void wallet_encode_short(uint16_t singleShort, pb_byte_t *new_bytes) {
    if ((singleShort & 0xFF) == singleShort)
        wallet_encode_byte((pb_byte_t) singleShort, new_bytes);
    else {
        new_bytes[0] = (pb_byte_t) (OFFSET_SHORT_ITEM + 2);
        new_bytes[1] = (singleShort >> 8 & 0xFF);
        new_bytes[2] = (singleShort >> 0 & 0xFF);
    }
}

void wallet_encode_int(uint32_t singleInt, pb_byte_t *new_bytes) {
    if ((singleInt & 0xFF) == singleInt) {
        wallet_encode_byte((pb_byte_t) singleInt, new_bytes);
    } else if ((singleInt & 0xFFFF) == singleInt) {
        wallet_encode_short((uint16_t) singleInt, new_bytes);
    } else if ((singleInt & 0xFFFFFF) == singleInt) {
        new_bytes[0] = (pb_byte_t) (OFFSET_SHORT_ITEM + 3);
        new_bytes[1] = (pb_byte_t) (singleInt >> 16);
        new_bytes[2] = (pb_byte_t) (singleInt >> 8);
        new_bytes[3] = (pb_byte_t) (singleInt);

    } else {
        new_bytes[0] = (pb_byte_t) (OFFSET_SHORT_ITEM + 4);
        new_bytes[1] = (pb_byte_t) (singleInt >> 24);
        new_bytes[2] = (pb_byte_t) (singleInt >> 16);
        new_bytes[3] = (pb_byte_t) (singleInt >> 8);
        new_bytes[4] = (pb_byte_t) (singleInt);
    }

}
