/* Ultimate Othello 1678
 * Copyright (C) 2002  Florent Boudet <flobo@ifrance.com>
 * iOS Software <http://www.ios-software.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *
 */

#include "IosException.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Exception::Exception(const char *exception)
{
	message = (char *) malloc(strlen(exception) + 1);
	strcpy(message, exception);
}

Exception::~Exception()
{
	free(message);
}

const char *Exception::getMessage()
{
    return message;
}

void Exception::printMessage()
{
	fprintf(stderr, "Exception thrown: %s\n", message);
}

