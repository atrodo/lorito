.sub 'block'
  $S0 = STR LOAD_CONST :[plan];
  STR SAY $S0;

  $S0 = STR LOAD_CONST :[ok];

  $S1 = STR LOAD_CONST :[t1];
  STR WRITE $S0;
  STR SAY   $S1;

  $S1 = STR LOAD_CONST :[t2];
  STR WRITE $S0;
  STR SAY   $S1;

  $S1 = STR LOAD_CONST :[t3];
  STR WRITE $S0;
  STR WRITE $S1;

  $S1 = STR LOAD_CONST :[t3_1];
  STR WRITE $S0;
  STR SAY   $S1;

  $S1 = STR LOAD_CONST :[t4];
  STR WRITE $S0;
  STR SAY   $S1;
.end;

.const ''
  plan: "1..4";
  ok:   "ok";
  nok:  "not ok";
  t1:   " 1 # output happens";
  t2:   " 2 # Say works";
  t3:   " 3 # write works";
  t3_1: " 3_1 # write does not have a \n";
  t4:   " 4 # we are now sane";
.end;
