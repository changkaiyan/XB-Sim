#ifndef _STAGE_LINEAR_1
#define _STAGE_LINEAR_1

#include "crossbar.h"
#include "systemc.h"

using namespace std;

SC_MODULE(stage_linear_1) {
	sc_in<float> input[INPUT_LINEAR_1];
	sc_out<float> output[INPUT_LINEAR_2];
	sc_in<int> signal_in;
	sc_out<int> signal_out;

	CROSSBAR cb;

	// read crossbar data from file
	void init_crossbar() {
		float* cell = new float[CROSSBAR_L*CROSSBAR_W];
		for (int i = 0; i < CROSSBAR_L; i++) {
			for (int j = 0; j < CROSSBAR_W; j++) {
				cell[i*CROSSBAR_W + j] = i * CROSSBAR_W + j;
			}
		}
		cb.init(cell, CROSSBAR_L, CROSSBAR_W);
		delete[] cell;
	}

	// activation function default relu
	void activation(float tmp_input[]) {
		for (int i = 0; i < INPUT_LINEAR_2; i++)
			if (tmp_input[i] < 0.0)
				tmp_input[i] = 0.0;
	}

	// run matrix multiply
	void stage_linear_run() {
		
		float tmp_input[CROSSBAR_L] = { 0.0 };
		float tmp_output[CROSSBAR_W] = { 0.0 };
		// read data from former layer
		for (int i = 0; i < INPUT_LINEAR_1; i++) {
			tmp_input[CROSSBAR_L-INPUT_LINEAR_1+i] = input[i].read();
		}
		cb.run(tmp_input, tmp_output);
		activation(tmp_output);
		for (int i = 0; i < INPUT_LINEAR_2; i++) {
			output[i].write(tmp_output[i]);
		}
		signal_out.write(signal_in.read());
	}

	SC_CTOR(stage_linear_1) {
		init_crossbar();

		SC_METHOD(stage_linear_run);
		sensitive << signal_in;
		dont_initialize();
	}

	~stage_linear_1() {
		cb.free_space();
	}
};

#endif // !_STAGE_LINEAR_1