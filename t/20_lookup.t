.sub "add"
  $P1 = CTX;
  $P2 = PMC POP_ARG $P1;

  $S2 = STR LOAD_CONST :[method_ok];
  $P5 = STR LOOKUP $P2, $S2;
  $P6 = PMC NEW_CTX $P5;
  CALL $P6;

  $S2 = STR LOAD_CONST :[t_add];
  STR SAY $S2;
.end;

.sub "sub"
  $P1 = CTX;
  $P2 = PMC POP_ARG $P1;

  $S2 = STR LOAD_CONST :[method_ok];
  $P5 = STR LOOKUP $P2, $S2;
  $P6 = PMC NEW_CTX $P5;
  CALL $P6;

  $S2 = STR LOAD_CONST :[t_sub];
  STR SAY $S2;
.end;

.sub "eq"
  $P1 = CTX;
  $P2 = PMC POP_ARG $P1;

  $S2 = STR LOAD_CONST :[method_ok];
  $P5 = STR LOOKUP $P2, $S2;
  $P6 = PMC NEW_CTX $P5;
  CALL $P6;

  $S2 = STR LOAD_CONST :[t_eq];
  STR SAY $S2;
.end;

.sub "ok"
  $S1 = STR LOAD_CONST :[ok];
  STR WRITE $S1;
.end;

.sub "lookup"
  $P1 = CTX;
  $S1 = STR POP_ARG $P1;

  N0:
  $S2 = STR LOAD_CONST :[method_add];
  $I1 = STR ISEQ $S1, $S2;
  INT IF $I1 :add;
  :GOTO :N1;
  add:
  $P2 = INT BLOCK :0;
  PMC PUSH_ARG $P1, $P2;
  :GOTO :end;

  N1:
  $S2 = STR LOAD_CONST :[method_sub];
  $I1 = STR ISEQ $S1, $S2;
  INT IF $I1 :sub;
  :GOTO :N2;
  sub:
  $P2 = INT BLOCK :1;
  PMC PUSH_ARG $P1, $P2;
  :GOTO :end;

  N2:
  $S2 = STR LOAD_CONST :[method_eq];
  $I1 = STR ISEQ $S1, $S2;
  INT IF $I1 :eq;
  :GOTO :N3;
  eq:
  $P2 = INT BLOCK :2;
  PMC PUSH_ARG $P1, $P2;
  :GOTO :end;

  N3:
  $S2 = STR LOAD_CONST :[method_ok];
  $I1 = STR ISEQ $S1, $S2;
  INT IF $I1 :ok;
  :GOTO :N4;
  ok:
  $P2 = INT BLOCK :3;
  PMC PUSH_ARG $P1, $P2;
  :GOTO :end;

  N4: noop;
  end: noop;
.end;

.main 'main'
  $S1 = STR LOAD_CONST :[plan];
  STR SAY $S1;

  $P1 = INT NEW :0;
  $S1 = STR LOAD_CONST :[method1];
  $P2 = STR LOOKUP $P1, $S1;
  $P3 = PMC NEW_CTX $P2;
  $P4 = INT BLOCK :4;

  PMC PUSH_ARG $P3, $P4;
  PMC PUSH_ARG $P3, $P0;
  PMC PUSH_ARG $P3, $P1;
  CALL $P3;

  $S2 = STR LOAD_CONST :[method_add];
  $P5 = STR LOOKUP $P1, $S2;
  $P6 = PMC NEW_CTX $P5;
  PMC PUSH_ARG $P6, $P1;
  CALL $P6;

  $S2 = STR LOAD_CONST :[method_sub];
  $P5 = STR LOOKUP $P1, $S2;
  $P6 = PMC NEW_CTX $P5;
  PMC PUSH_ARG $P6, $P1;
  CALL $P6;

  $S2 = STR LOAD_CONST :[method_eq];
  $P5 = STR LOOKUP $P1, $S2;
  $P6 = PMC NEW_CTX $P5;
  PMC PUSH_ARG $P6, $P1;
  CALL $P6;

  $P1 = INT NEW :20;
  $P2 = NEW_CTX;
  PMC PUSH_ARG $P2, $P1;
  $S1 = STR LOAD_CONST :[pmc_size];
  STR CALL $P2, $S1;
  $I1 = INT POP_ARG $P2;
  $I2 = INT LOAD_IMM :20;
  $I3 = INT ISEQ $I1, $I2;

  INT IF $I3 :t4_ok;
    $S2 = STR LOAD_CONST :[nok];
    STR WRITE $S2;
    :GOTO :t4;
  t4_ok:
    $S2 = STR LOAD_CONST :[ok];
    STR WRITE $S2;

  t4:
  $S2 = STR LOAD_CONST :[t_str];
  STR SAY $S2;

.end;

.const ''
  method1:    "SET_LOOKUP";
  method_add: "add";
  method_sub: "sub";
  method_eq:  "eq";
  method_ok:  "ok";

  pmc_size:   "size";

  plan:  "1..4";
  ok:    "ok";
  nok:   "nok";
  t_add: " 1 # We call 'add'";
  t_sub: " 2 # We call 'sub'";
  t_eq:  " 3 # We call 'eq'";
  t_str: " 4 # We called the STR version of CALL";
.end;
