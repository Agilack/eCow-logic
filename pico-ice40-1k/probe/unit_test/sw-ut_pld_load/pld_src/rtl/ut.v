module ut_top (
  output led_0,
  output led_1,
  output led_2,
  output led_3,
  output led_4,
  output led_5,
  output led_6,
  output led_7,
  input  but_1,
  input  but_2,
  input  but_3,
  input  but_4
);

wire [7:0] leds = { led_7, led_6, led_5, led_4, led_3, led_2, led_1, led_0 };

assign leds = (but_1 == 1'b0) ? 8'hAA :
              (but_2 == 1'b0) ? 8'hFF :
              (but_3 == 1'b0) ? 8'h00 :
              (but_4 == 1'b0) ? 8'hA5 : 8'h55;

endmodule
