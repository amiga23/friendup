/*©mit**************************************************************************
*                                                                              *
* This file is part of FRIEND UNIFYING PLATFORM.                               *
* Copyright (c) Friend Software Labs AS. All rights reserved.                  *
*                                                                              *
* Licensed under the Source EULA. Please refer to the copy of the MIT License, *
* found in the file license_mit.txt.                                           *
*                                                                              *
*****************************************************************************©*/

//
// buffered string
//

#ifndef __BUFFERED_STRING_H__
#define __BUFFERED_STRING_H__

#define BUF_STRING_MAX 1024 * 12

//
// BufferString structure
//

typedef struct BufString
{
	unsigned int             bs_Size;        // current data size
	unsigned int             bs_Bufsize;     // buffer size
	char           *bs_Buffer;      // pointer to buffer
	unsigned int  buffer_increments;
	unsigned int previous_increment;
} BufString;


/**
 * Creates new buffered string object with a default buffer size
 * @return pointer to BufString object or NULL on failure
 */
BufString *BufStringNew(void);

/**
 * Creates new buffered string object with a desired buffer size
 * @param initial_size desired initial size
 * @return pointer to BufString object
 */
BufString *BufStringNewSize(unsigned int initial_size);

/**
 * Deallocated a buffered string object and its buffers
 * @param bs pointer to a buffered string object
 */
void BufStringDelete(BufString *bs);

/**
 * Appends a string to an existing buffered string object.
 * @param bs pointer to BufString object
 * @param string_to_append string to be appended
 * @return 0 on success, 1 on failure, string in buffer is always null-terminated
 */
unsigned int BufStringAdd(BufString *bs, const char *string_to_append);

/**
 * Appends a string  with a defined length to an existing buffered string object.
 * @param bs pointer to BufString object
 * @param string_to_append string to be appended
 * @param string_to_append_length how many bytes from the string should be added
 * @return 0 on success, 1 on failure, string in buffer is always null-terminated
 */
unsigned int BufStringAddSize(BufString *bs, const char *string_to_append, unsigned int string_to_append_length);

#endif //__BUFFERED_STRING_H__
