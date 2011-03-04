.main 'main'
  $S1 = STR LOAD_CONST :[rpa];
  $P1 = STR LOADLIB $S1;

  $S10 = STR LOAD_CONST :[plan];
  STR SAY $S10;

  $S1 = STR LOAD_CONST :[file_get_method];
  $P2 = STR LOOKUP $P1, $S1;
  $P2 = PMC NEW_CTX $P2;

  $S1 = STR LOAD_CONST :[rpa_new];
  PMC PUSH_ARG $P2, $P1;
  STR PUSH_ARG $P2, $S1;
  CALL $P2;

  # P2 is now the 'new' method in rpa.
  $P2 = POP_ARG $P2;

  $P3 = PMC NEW_CTX $P2;
  CALL $P3;
  $P3 = POP_ARG $P3;
  $S1 = STR LOAD_CONST :[rpa_length];
  $P4 = STR LOOKUP $P3, $S1;
  $P4 = PMC NEW_CTX $P4;

  PMC PUSH_ARG $P4, $P3;
  CALL $P4;
  $I1 = INT POP_ARG $P4;

  $I4 = INT ISEQ $I1, $I0;
        INT IF $I4 :t1_ok;
  $S1 = STR LOAD_CONST :[nok];
  : GOTO :t1;
  t1_ok:
  $S1 = STR LOAD_CONST :[ok];
  t1:
  STR SAY $S1;
  $S1 = STR LOAD_CONST :[t1_tst];

.end;

.const ''
  rpa: "pmc/resizablepmcarray";
  rpa_new: "new";

  rpa_length : "length";
  rpa_shift  : "shift";
  rpa_unshift: "unshift";
  rpa_push   : "push";
  rpa_pop    : "pop";
  rpa_resize : "resize";
  rpa_get    : "get";
  rpa_put    : "put";
  rpa_splice : "splice";

  file_get_method: "GET_METHOD";
  
  pmc_size:   "size";

  plan:  "1..4";
  ok:    "ok";
  nok:   "nok";
  t1_tst: " 1 # We call 'add'";
  t2_tst: " 2 # We call 'sub'";
  t3_tst: " 3 # We call 'eq'";
  t4_tst: " 4 # We called the STR version of CALL";
.end;
