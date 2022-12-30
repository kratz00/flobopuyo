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

#ifndef _IOSVECTOR
#define _IOSVECTOR

class IosVector {
public:
	IosVector();
	IosVector(const int size);
	~IosVector();
	void addElement(void *element);
	void * getElementAt(const int index) const;
	void removeElementAt(const int index);
	void removeElement(void *element);
	void removeAllElements();
	int getSize() const;
	int getCapacity() const;
	void dumpVector() const;
private:
	void **vectorData;
	static const int vectorInitialSize = 10;
	static const int vectorSizeIncrement = 10;
	int vectorCapacity;
	int vectorSize;
	
	void increaseVectorSize();
};

#endif // _IOSVECTOR
