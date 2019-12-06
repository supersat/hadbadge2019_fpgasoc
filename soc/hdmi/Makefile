SRC := clk_8_250_125_25.v hdmi-encoder.v tmds_encoder.v vga.v vga2dvid.v
SRC_SIM := ../misc/ram_sdp.v ../misc/fifo_sync_ram.v hdmi-test.v vga.v vga2dvid.v verilator_main.cpp

verilator: verilator-build/Vhdmi_test $(EXTRA_DEPEND)
	./verilator-build/Vhdmi_test $(VERILATED_ARG)

verilator-build/Vhdmi_test: $(SRC_SIM)
	verilator -CFLAGS "-ggdb -DVERILATOR_USE_FST=1" --assert --trace-fst-thread \
		--Mdir verilator-build -Wno-style -Wno-fatal -cc --top-module hdmi_test --exe $(SRC_SIM)
	make OPT_FAST="-Og -fno-stack-protector" -C verilator-build -f Vhdmi_test.mk

clean:
	rm -rf verilator-build

.PHONY: clean
