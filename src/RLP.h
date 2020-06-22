#ifndef RLP_RLP_H
#define RLP_RLP_H

#include <stdbool.h>
#include <stdint.h>
#include "utils.h"


int rlp_encode_list(uint8_t *rawTx, uint32_t totalLength, list_node_t* list);
void rlp_encode_element(pb_byte_t *bytes, pb_size_t size,
                           pb_byte_t *new_bytes, pb_size_t *new_size, bool remove_leading_zeros);
void rlp_encode_int(uint32_t singleInt, pb_byte_t *new_bytes);
#endif //RLP_RLP_H
