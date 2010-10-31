.sub "asdf"
  noop;
.end;

.sub "fib"
  $I1 = INT POP_ARG;
  $I2 = INT LOAD_IMM :2;
  $I3 = INT ISGT $I1, $I2;

  $I2 = INT LOAD_IMM :1;

  #INT SAY $I1;
  #INT SAY $I2;
  #INT SAY $I3;

  INT IF $I3 :cont;

  INT PUSH_RET $I2;
  : GOTO :end;

  cont:
  $P1 = INT BLOCK :1;
  $P2 = PMC NEW_CTX $P1;

  $I1 = INT SUB $I1, $I2;
  INT PUSH_ARG $P2, $I1;
  CALL $P2;
  $I3 = INT POP_RET $P2;

  $P2 = PMC NEW_CTX $P1;

  $I1 = INT SUB $I1, $I2;
  INT PUSH_ARG $P2, $I1;
  CALL $P2;
  $I4 = INT POP_RET $P2;

  $I5 = INT ADD $I3, $I4;
  INT PUSH_RET $I5;

  end: NOOP;
.end;

.sub 'main'
  start: NOOP;

  $S1 = STR LOAD_CONST :[plan];
  STR SAY $S1;

  $P1 = INT BLOCK :1;
  $P2 = PMC NEW_CTX $P1;

  $I1 = INT LOAD_IMM :20;
        INT PUSH_ARG $P2, $I1;
  CALL $P2;
  $I2 = INT POP_RET $P2;
  #$I3 = INT LOAD_IMM :1134903170;
  $I3 = INT LOAD_IMM :6765;

  $I4 = INT ISEQ $I2, $I3;
        INT IF $I4 :t1_ok;
  $S1 = STR LOAD_CONST :[nok];
  : GOTO :t1;
  t1_ok:
  $S1 = STR LOAD_CONST :[ok];
  t1:
  STR SAY $S1;

  INT SAY $I2;


  end: NOOP;
.end;

.const ''
  s0: "Hello World!";
  s1: " - ";
  s2: "Hello!";
  s3: "World!";

  plan: "1..4";
  ok:   "ok";
  nok:  "not ok";
  t1:   " 1 # output happens";
  t2:   " 2 # Say works";
  t3:   " 3 # write works";
  t3_1: " 3_1 # write does not have a \n";
  t4:   " 4 # we are now sane";
.end;
