/**
 *  @file	hash.h
 *  @brief	Contains the hash function useful for \ref directory.h
 *
 */


#ifndef _HASH_H_
#define _HASH_H_

#include <stdint.h>

/**
 * \brief	Hash function (converts string to hash number)
 *
 * @param 	str
 *
 * @return 		Hash number
 */
extern uint32_t hash(const char *str);

#endif /* _HASH_H_ */
