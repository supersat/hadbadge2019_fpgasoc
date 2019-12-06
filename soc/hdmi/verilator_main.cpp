/*
 * Copyright 2019 Karl Koscher <supersat@gmail.com>
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include "Vhdmi_test.h"
#include <verilated.h>
#include <verilated_fst_c.h>

int main(int argc, char **argv) {
	// Initialize Verilators variables
	Verilated::commandArgs(argc, argv);
	Verilated::traceEverOn(true);

	// Create an instance of our module under test
	Vhdmi_test *tb = new Vhdmi_test;
	
    //Create trace
	VerilatedFstC *trace = new VerilatedFstC;
	tb->trace(trace, 99);
	trace->open("hdmitrace.fst");

    uint64_t ts = 0;
	int doEval;
	int vblank = 0;

	tb->clk_8m = 0;
	tb->clk_25MHz = 0;
	tb->clk_250MHz = 0;
	tb->clk_locked = 1;

    tb->red = 0;
    tb->green = 0;
    tb->blue = 0;

    tb->audio_left = 0xAAA0;
    tb->audio_right = 0x5550;

	while(1) {
		doEval = 0;
		ts++;
		if (!(ts % 4)) {
			tb->clk_250MHz = !tb->clk_250MHz;
			doEval = 1;
		}
		if (!(ts % 40)) {
			tb->clk_25MHz = !tb->clk_25MHz;
			doEval = 1;
		}
		if (!(ts % 125)) {
			tb->clk_8m = !tb->clk_8m;
			if (!tb->clk_8m) {
				tb->audio_left = rand() & 0xffff;
				tb->audio_right = rand() & 0xffff;
			}
			doEval = 1;
		}
		if (doEval) {
			tb->eval();
			trace->dump(ts);
			if (tb->cy == 522) {
				vblank = 1;
			}
			if (tb->cy == 0 && vblank) {
				break;
			}
		}
	};

	trace->flush();
	trace->close();

	exit(EXIT_SUCCESS);
}
