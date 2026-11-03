// baseline_logic_unit_999
// -----------------------------------------------------------------------------
// Large self-contained stress test that composes the nine public case circuits
// into one design whose baseline Cyclone IV logic-cell count is exactly 999.
//
//   cases 01..05 : pmux_case01_balanced .. pmux_case05_nested   (parameterised)
//   cases 06..09 : test1, test2, test3, test4                   (unmodified)
//
// Measured at the defaults (PAD = 38, W1..W5 = 4, 3, 3, 3, 4):
//   baseline  (verification/reference/yosys.exe) : 999 logic / 803 dff
//   candidate (提交材料/yosys.exe)                : 925 logic / 800 dff  (-7.41%)
//
// Stress structure
//   * 16 self-starting 32-bit LFSRs give 512 bits of live pseudo-random stimulus,
//     so no DUT input is constant and no DUT can be constant-folded away.
//   * a 16-bit cycle counter and an 8-bit phase counter supply the structured
//     handshake signals the test* cases need (valid / cs / ena / ROM address).
//   * a 64-bit MISR consumes every DUT output, so no DUT logic can be dropped as
//     unobserved even though the design only has clk / rst / en inputs.
//   * PAD is a chain of "baseline logic units": one 3-input XOR (one LCELL) plus
//     one flop per stage, with the tail routed to the probe output so the bank
//     cannot be repacked into the observation logic.
//
// Reset style is asynchronous on purpose: the Cyclone IV flop primitive absorbs
// async reset and enable, so state costs ~0 LCELLs. Measured on this very flow, a
// synchronous reset plus enable costs exactly one LCELL per flop.
//
// Calibration: ABC LUT packing re-optimises globally, so the LCELL count moves by
// up to ~10 cells for a one-stage change (measured: PAD 0 -> 962, 10 -> 970,
// 20 -> 977, 30 -> 1003, 38 -> 999). Any change to the widths, to PAD, or to the
// order of the source files changes the count.
//
// Measured with tools/measure_baseline.py (competition-exact flow):
//   read_verilog -icells <the 9 case files, in the order listed in README.md>
//   synth_intel -family cycloneiv -top baseline_logic_unit_999 -run begin:map_cells
//   techmap -map +/intel/cycloneiv/cells_map.v ; clean ; check -noinit -assert
//   logic = cycloneiv_lcell_comb + $not  ==  999
//
// NOTE: W1..W5 must stay <= 7, otherwise a slot's operand field would run into
//       that slot's 16-bit control field.
// -----------------------------------------------------------------------------

module baseline_logic_unit_999 #(
    parameter integer PAD = 38,    // calibration baseline logic units (LCELL each)
    parameter integer W1  = 4,    // case01 lane width
    parameter integer W2  = 3,    // case02 lane width
    parameter integer W3  = 3,    // case03 lane width
    parameter integer W4  = 3,    // case04 lane width
    parameter integer W5  = 4     // case05 lane width
)(
    input wire clk,
    input wire rst,
    input wire en,
    output wire [63:0] signature,
    output wire [31:0] probe
);

    // ------------------------------------------------------------------ stimulus
    // Feedback is inverted so the all-zero state is not a lock-up; the one-hot
    // reset seed keeps the 16 generators on distinct phases from the first cycle.
    reg [511:0] lfsr_q;
    reg [15:0]  cyc;
    reg [7:0]   phase;

    function [31:0] lfsr_step;
        input [31:0] s;
        input integer mode;
        reg fb;
        begin
            case (mode)
                0: fb = s[31] ^ s[21] ^ s[1] ^ s[0];
                1: fb = s[31] ^ s[6] ^ s[4] ^ s[2] ^ s[1] ^ s[0];
                2: fb = s[31] ^ s[27] ^ s[25] ^ s[22] ^ s[20] ^ s[18]
                      ^ s[16] ^ s[14] ^ s[12] ^ s[10] ^ s[8] ^ s[6] ^ s[4] ^ s[2] ^ s[0];
                default: fb = s[31] ^ s[30] ^ s[28] ^ s[26] ^ s[24] ^ s[22]
                      ^ s[20] ^ s[18] ^ s[16] ^ s[14] ^ s[12] ^ s[10]
                      ^ s[8] ^ s[6] ^ s[4] ^ s[2] ^ s[0];
            endcase
            lfsr_step = {s[30:0], ~fb};
        end
    endfunction

    genvar gl;
    generate for (gl = 0; gl < 16; gl = gl + 1) begin: g_lfsr
        always @(posedge clk or posedge rst) begin
            if (rst)
                lfsr_q[gl*32 +: 32] <= 32'h1 << gl;
            else if (en)
                lfsr_q[gl*32 +: 32] <= lfsr_step(lfsr_q[gl*32 +: 32], gl % 4);
        end
    end endgenerate

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            cyc   <= 16'd0;
            phase <= 8'd0;
        end else begin
            cyc   <= cyc + 1'b1;
            phase <= phase + 1'b1;
        end
    end

    // 536-bit stimulus bus: [511:0] LFSR bank, [527:512] cycle, [535:528] phase
    wire [535:0] mix = {phase, cyc, lfsr_q};

    wire t1_valid = (phase[3:0] == 4'd0);

    // ------------------------------------------------------- cases 01..05 (pmux)
    wire [W1-1:0] c1_out;
    wire [W2-1:0] c2_out;
    wire [W3-1:0] c3_out;
    wire [W4-1:0] c4_out;
    wire [W5-1:0] c5_out;

    pmux_case01_balanced #(.LANES(1), .WIDTH(W1)) u_case01 (
        .clk(clk), .rst(rst), .en(en),
        .data_in(mix[0 +: 4*W1]), .ctrl_in(mix[32 +: 16]), .data_out(c1_out));

    pmux_case02_pipeline #(.LANES(1), .WIDTH(W2)) u_case02 (
        .clk(clk), .rst(rst), .en(en),
        .data_in(mix[48 +: 4*W2]), .ctrl_in(mix[80 +: 16]), .data_out(c2_out));

    pmux_case03_crossbar #(.LANES(1), .WIDTH(W3)) u_case03 (
        .clk(clk), .rst(rst), .en(en),
        .data_in(mix[96 +: 4*W3]), .ctrl_in(mix[128 +: 16]), .data_out(c3_out));

    pmux_case04_feedback #(.LANES(1), .WIDTH(W4)) u_case04 (
        .clk(clk), .rst(rst), .en(en),
        .data_in(mix[144 +: 4*W4]), .ctrl_in(mix[176 +: 16]), .data_out(c4_out));

    pmux_case05_nested #(.LANES(1), .WIDTH(W5)) u_case05 (
        .clk(clk), .rst(rst), .en(en),
        .data_in(mix[192 +: 4*W5]), .ctrl_in(mix[224 +: 16]), .data_out(c5_out));

    // ------------------------------------------------------------ case 06: test1
    // test1 only consumes I/Q bits [15:4], so 12 live bits per operand are enough.
    wire [15:0] t1_I0 = {4'h0, mix[240 +: 12]};
    wire [15:0] t1_I1 = {4'h0, mix[252 +: 12]};
    wire [15:0] t1_I2 = {4'h0, mix[264 +: 12]};
    wire [15:0] t1_I3 = {4'h0, mix[276 +: 12]};
    wire [15:0] t1_I4 = {4'h0, mix[288 +: 12]};
    wire [15:0] t1_I5 = {4'h0, mix[300 +: 12]};
    wire [15:0] t1_I6 = {4'h0, mix[312 +: 12]};
    wire [15:0] t1_I7 = {4'h0, mix[324 +: 12]};
    wire [15:0] t1_Q0 = {4'h0, mix[336 +: 12]};
    wire [15:0] t1_Q1 = {4'h0, mix[348 +: 12]};
    wire [15:0] t1_Q2 = {4'h0, mix[360 +: 12]};
    wire [15:0] t1_Q3 = {4'h0, mix[372 +: 12]};
    wire [15:0] t1_Q4 = {4'h0, mix[384 +: 12]};
    wire [15:0] t1_Q5 = {4'h0, mix[396 +: 12]};
    wire [15:0] t1_Q6 = {4'h0, mix[408 +: 12]};
    wire [15:0] t1_Q7 = {4'h0, mix[420 +: 12]};

    wire t1_por6, t1_i0007;
    wire [11:0] t1_i0008, t1_i0009;

    test1 u_test1 (
        .clock(clk), .reset(rst), .valid(t1_valid),
        .I0(t1_I0), .I1(t1_I1), .I2(t1_I2), .I3(t1_I3),
        .I4(t1_I4), .I5(t1_I5), .I6(t1_I6), .I7(t1_I7),
        .Q0(t1_Q0), .Q1(t1_Q1), .Q2(t1_Q2), .Q3(t1_Q3),
        .Q4(t1_Q4), .Q5(t1_Q5), .Q6(t1_Q6), .Q7(t1_Q7),
        .por6_out(t1_por6), .inter0007_out(t1_i0007),
        .inter0008_out(t1_i0008), .inter0009_out(t1_i0009));

    // ------------------------------------------------------------ case 07: test2
    wire [23:0] t2_stim = mix[432 +: 24];
    wire [4:0]  t2_out;

    test2 u_test2 (
        .clk(clk),
        .a(t2_stim[20]), .b(t2_stim[21]), .c(t2_stim[22]), .d(t2_stim[23]),
        .dataa(t2_stim[4:0]), .datab(t2_stim[9:5]),
        .datac(t2_stim[14:10]), .datad(t2_stim[19:15]),
        .out(t2_out));

    // ------------------------------------------------------------ case 08: test3
    wire [7:0] t3_gpio;
    wire       t3_sr;

    test3 u_test3 (
        .clk(clk), .cs(cyc[2]), .sr_in(mix[456]),
        .gpioout(t3_gpio), .sr_out(t3_sr));

    // ------------------------------------------------------------ case 09: test4
    // test4 decodes a constant table and would otherwise become a ROM. The address
    // is deliberately built from a combination of two stimulus fields so that no
    // flop sits directly on the ROM address: memory_dff must not turn the table
    // into a synchronous ROM, which this flow would try to map onto an M9K block.
    wire [4:0]  t4_datain = mix[457 +: 5] ^ mix[251 +: 5];
    wire [5:0]  t4_dataout;
    wire        t4_only5b, t4_5bis7, t4_dp, t4_dn;

    test4 u_test4 (
        .clk(clk), .reset_n(~rst), .ena(phase[1]), .datain(t4_datain),
        .dataout(t4_dataout), .only_5b(t4_only5b), .datain_5bis7(t4_5bis7),
        .data_dp(t4_dp), .data_dn(t4_dn));

    // --------------------------------------------- calibration baseline logic units
    // A chain of independent 3-input XOR "logic units". Each stage consumes the
    // previous stage plus one cycle bit and one phase bit, so
    //   * every stage is exactly one LCELL and one flip-flop,
    //   * the (cycle, phase) pair is unique per stage, so no two stages share a LUT,
    //   * no stimulus bit changes observability, so PAD is a clean 1-cell knob.
    // Only the chain tail is observed, which keeps the whole bank alive.
    wire pad_obs;
    reg [ (PAD > 0 ? PAD : 1) - 1 : 0] pad_q;

    generate
        if (PAD > 0) begin: g_pad
            genvar gp;
            for (gp = 0; gp < PAD; gp = gp + 1) begin: g_bit
                always @(posedge clk or posedge rst) begin
                    if (rst)
                        pad_q[gp] <= (gp == 0);
                    else if (en)
                        pad_q[gp] <= pad_q[(gp + PAD - 1) % PAD]
                                     ^ cyc[gp % 16] ^ phase[(gp / 16) % 8];
                end
            end
            assign pad_obs = pad_q[PAD-1];
        end else begin: g_no_pad
            always @* pad_q[0] = 1'b0;
            assign pad_obs = pad_q[0];
        end
    endgenerate

    // ------------------------------------------------ observation and signature
    localparam integer OBS_FIXED = 26 + 5 + 9 + 10;   // test1..test4 exposed bits
    localparam integer OBS_W = W1 + W2 + W3 + W4 + W5 + OBS_FIXED;

    wire [OBS_W-1:0] obs = {
        c1_out, c2_out, c3_out, c4_out, c5_out,
        t1_por6, t1_i0007, t1_i0008, t1_i0009,
        t2_out,
        t3_gpio, t3_sr,
        t4_dataout, t4_only5b, t4_5bis7, t4_dp, t4_dn
    };

    // Fold the observation bus into 64 lanes; the 0x3E7 constant keeps the 999 tag
    // in the signature path at no LCELL cost.
    reg [63:0] obs64;
    integer k;
    always @* begin
        obs64 = 64'h0000_0000_0000_03E7;
        for (k = 0; k < OBS_W; k = k + 1)
            obs64[k % 64] = obs64[k % 64] ^ obs[k];
    end

    // 64-bit MISR: x^64 + x^63 + x^61 + x^60 + 1
    wire sig_fb = signature_q[63] ^ signature_q[62] ^ signature_q[60] ^ signature_q[59];
    reg [63:0] signature_q;

    always @(posedge clk or posedge rst) begin
        if (rst)
            signature_q <= 64'd0;
        else if (en)
            signature_q <= {signature_q[62:0], sig_fb} ^ obs64;
    end

    assign signature = signature_q;
    // pad_obs goes straight to a primary output, so it cannot be repacked into the
    // observation LUTs: every PAD stage stays exactly one LCELL.
    assign probe     = {cyc, phase, 6'b011001, pad_obs, 1'b0};

endmodule
