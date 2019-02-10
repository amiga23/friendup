/* 
 * Copyright (c) 2016 Lammert Bies
 * Copyright (c) 2013-2016 the Civetweb developers
 * Copyright (c) 2004-2013 Sergey Lyubka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ============
 * Release: 2.0
 */

#include "httplib_main.h"

#if defined(_WIN32_WCE)

/*
 * time_t time( time_t *ptime );
 *
 * On WinCE systems not all of the common system functions are available. This
 * time() function provides an equivalent for time() based on time functions
 * which are available in the WinCE kernel.
 */

time_t time( time_t *ptime ) {

	time_t t;
	SYSTEMTIME st;
	FILETIME ft;

	GetSystemTime( & st );
	SystemTimeToFileTime( & st,  & ft );
	t = SYS2UNIX_TIME( ft.dwLowDateTime, ft.dwHighDateTime );

	if ( ptime != NULL ) *ptime = t;

	return t;

}  /* time */

#endif /* defined(_WIN32_WCE) */