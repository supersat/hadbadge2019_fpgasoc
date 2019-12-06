module hdmi_test (
	input clk_8m,
    input clk_250MHz,
    input clk_25MHz,
    input clk_locked,
	output [3:0] tmds_test_out,
	
	output pixelclk,
	output fetch_next,
	input [7:0] red,
	input [7:0] green,
	input [7:0] blue,
	output next_line,
	output next_field,

	output [9:0] cx,
	output [9:0] cy,

	input [15:0] audio_left,
	input [15:0] audio_right
);

	parameter C_ddr = 1'b0; // 0:SDR 1:DDR
	
	// shift clock choice SDR/DDR
	wire clk_pixel, clk_shift;
	assign clk_pixel = clk_25MHz;
	assign clk_shift = clk_250MHz;

	// VGA signal generator
	wire [7:0] vga_r, vga_g, vga_b;
	wire vga_hsync, vga_vsync, vga_blank;
	wire [9:0] CounterX, CounterY;
	vga vga_instance (
		.clk_pixel(clk_pixel),
		.test_picture(1'b0), // enable test picture generation
		.vga_r(vga_r),
		.vga_g(vga_g),
		.vga_b(vga_b),
		.fetch_next(fetch_next),
		.red_byte(red),
		.green_byte(green),
		.blue_byte(blue),
		.next_line(next_line),
		.next_field(next_field),
		.vga_hsync(vga_hsync),
		.vga_vsync(vga_vsync),
		.vga_blank(vga_blank),
		.CounterX(CounterX),
		.CounterY(CounterY)
	);

	// VGA to digital video converter
	wire [1:0] tmds[3:0];
	vga2dvid #(
		.C_ddr(C_ddr)
	) vga2dvid_instance (
		.rst(!clk_locked), // TODO: Is this right?
		.clk_pixel(clk_pixel),
		.clk_shift(clk_shift),
		.in_red(vga_r),
		.in_green(vga_g),
		.in_blue(vga_b),
		.in_hsync(vga_hsync),
		.in_vsync(vga_vsync),
		.in_blank(vga_blank),
		.in_audio_left(audio_left),
		.in_audio_right(audio_right),
		.CounterX(CounterX),
		.CounterY(CounterY),
		.out_clock(tmds[3]),
		.out_red(tmds[2]),
		.out_green(tmds[1]),
		.out_blue(tmds[0])
	);

    assign tmds_test_out = { tmds[3][0], tmds[2][0], tmds[1][0], tmds[0][0] };
	assign cx = CounterX;
	assign cy = CounterY;

endmodule
