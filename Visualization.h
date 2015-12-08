/*
 * Visualization.h
 *
 *  Created on: 2012. 12. 5.
 *      Author: chaerim
 */

#ifndef COMMON_H_
#include "Common.h"
#endif

#ifndef BELIEFDIVISION_H_
#include "BeliefDivision.h"
#endif

#ifndef VISUALIZATION_H_
#define VISUALIZATION_H_

#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-ps.h>

class RepeatedGame;

PreciseNumber convert(PreciseNumber t, PreciseNumber scale);
void drawInfo(RepeatedGame &rg, BeliefDivision &original, BeliefDivision &target, vector<Belief> &normalCondition, int loop, string filename, string des);
pair<PreciseNumber, PreciseNumber> transform(PreciseNumber a, PreciseNumber b);

#endif /* VISUALIZATION_H_ */
