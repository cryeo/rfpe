/*
 * Writer.h
 *
 *  Created on: 2012. 12. 6.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef WRITER_H_
#define WRITER_H_

class Writer {
	ofstream fout;
public:
	Writer();
	Writer(string filename);
	virtual ~Writer();
	template<typename T>
	void put(T val) {
		fout << val << endl;
	}
};

#endif /* WRITER_H_ */
