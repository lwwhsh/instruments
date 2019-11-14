# 1 "../sncProgram.st"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "../sncProgram.st"
# 1 "./../sncExample.stt" 1
program sncExample
double v;
assign v to "{user}:xxxExample";
monitor v;

ss ss1
{
 state low
 {
     when(v>5.0)
     {
  printf("changing to high\n");
     } state high
     when(delay(.1))
     {
     } state low
 }
 state high
 {
     when(v<=5.0)
     {
  printf("changing to low\n");
     } state low
     when(delay(.1))
     {
     } state high
 }
}
# 1 "../sncProgram.st" 2
