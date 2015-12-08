/*
 * Writer.cpp
 *
 *  Created on: 2012. 12. 6.
 *      Author: chaerim
 */

#include "Writer.h"

Writer::Writer() {
}

Writer::~Writer() {
	fout.close();
}

Writer::Writer(string filename) {
	fout.open(filename.c_str());
}
