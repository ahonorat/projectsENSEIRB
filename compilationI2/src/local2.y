//cast : sitofp
//zext i1 true to i32

: IF '(' expression ')' statement{
  char* debut = label();
  char* fin = label();
  //char* cond = $<c>3.id;
  temp = $<c>3.s; //code générant la condition
  temp=concat(temp, strdup("br i1 %"));
  temp = concat(temp, $<c>3.r); //récupération de la condition
  temp = concat(temp, strdup(", label %"));
  temp = concat(temp, strdup(debut));
  temp = concat(temp, strdup(", label %"));
  temp = concat(temp, strdup(fin));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, debut);
  temp = concat(temp, strdup(":\n"));
  temp = concat(temp, $<c>5.s); //code à l'intérieur de la boucle
  temp = concat(temp, strdup("\n br label %"));
  temp = concat(temp, strdup(fin));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, fin);
  temp = concat(temp, strdup(":\n"));
  $<c>$.s = temp;
  }
| IF '(' expression ')' statement ELSE statement {
  char *si = label();
  char *sinon = label();
  char *fin = label();
  temp = $<c>3.s; //code générant la condition
  temp=concat(temp, strdup("br i1 %"));
  temp = concat(temp, $<c>3.r); //récupération de la condition
  temp = concat(temp, strdup(", label %"));
  temp = concat(temp, strdup(si));
  temp = concat(temp, strdup(", label %"));
  temp = concat(temp, strdup(sinon));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, si);
  temp = concat(temp, $<c>5.s); //code if
  temp = concat(temp, strdup("\n br label %"));
  temp = concat(temp, strdup(fin));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp; sinon);
  temp = concat(temp, $<c>7.s); //code else
  temp = concat(temp, strdup("\n br label %"));
  temp = concat(temp, strdup(fin));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, fin);
  temp = concat(temp, strdup(":\n"));
  $<c>$.s = temp;
  }

| FOR '(' expression_statement expression_statement expression ')' statement {
  char *test = label(); //test condition
  char *debut = label(); //début boucle
  char *fin = label(); //sortie de boucle
  temp = $<c>3.s; //initialisation
  temp = concat(temp, strdup("\n br label %"));//on va au label test
  temp = concat(temp, strdup(test));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup(test));//label test
  temp = concat(temp, strdup(":\n"));
  temp = concat(temp, $<c>4.s); //création de la condition
  temp = concat(temp, strdup("\n br i1 %")); //branchement conditionnel
  temp = concat(temp, $<c>4.r); //récupération condition
  temp = concat(temp, strdup(", label %"));
  temp = concat(temp, strdup(debut));
  temp = concat(temp, strdup(", label %"));
  temp = concat(temp, strdup(fin));
  temp = concat(temp, debut); //label début
  temp = concat(temp, strdup(":\n"));
  temp = concat(temp, $<c>7.s); //code de la boucle
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>5.s); //action fin de boucle
  temp = concat(temp, strdup("\n br label %")); //retour vers test
  temp = concat(temp, test);
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, fin); //fin de la boucle - label fin
  temp = concat(temp, strdup(":\n"));
  $<c>$.s = temp;
  }
;

iteration_statement
: WHILE '(' expression ')' statement {
  char* test = label(); //test condition
  char* debut = label(); //début de la boucle
  char* fin = label(); //fin de la boucle
  temp = strdup("br label %");// on va au label test
  temp = concat(temp, strdup(test));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup(test)); //label test
  temp = concat(temp, strdup(":\n"));
  temp = concat(temp, $<c>3.s); //création de la condition
  temp = concat(temp, strdup("\n br i1 %")); //branchement conditionnel
  temp = concat(temp, $<c>3.r); //récupération condition
  temp = concat(temp, strdup(", label %"));
  temp = concat(temp, strdup(debut));
  temp = concat(temp, strdup(", label %"));
  temp = concat(temp, strdup(fin));
  temp = concat(temp, debut); //label début
  temp = concat(temp, strdup(":\n"));
  temp = concat(temp, $<c>7.s); //code de la boucle
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup("br label %")); //retour vers test
  temp = concat(temp, test);
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, fin); //fin de la boucle - label fin
  temp = concat(temp, strdup(":\n"));
  $<c>$.s = temp;
  }
;

jump_statement
: RETURN ';'{
  $<c>$.s = strdup("ret void\n"); 
  }
| RETURN expression ';' {
  temp = $<c>2.s; //création expression
  temp = concat(temp, strdup("ret "));
  temp = concat(temp, $<c>2.t); //type de retour
  temp = concat(temp, $<c>2.r); //valeur retournée
  temp = concat(temp, strdup("\n"));
  $<c>$.s = temp;
  }
;

expression_statement
: ';' { 
  $<c>$.s = strdup("\n");
  if (store_name != NULL){
    free(store_name);
    store_name = NULL;
  }
  check_sem(sem_stack, strdup("&"));
 }
| expression ';' {
  $<c>1.s=concat($<c>1.s, strdup("\n")); 
  $<c>$.s = $<c>1.s;
  $<c<$.t = $<c>1.t;
  $<c>$.r = $<c>1.r;
  check_sem(sem_stack, strdup("%"));
  }
;

statement_list
: statement {
  $<c>$ = $<c>1;
  check_sem(sem_stack, strdup("9"));
 }
| statement_list statement {
  $<c>1.s=concat($<c>1.s, $<c>2.s); 
  $<c>$.s = $<c>1.s;
  check_sem(sem_stack, strdup("$"));
 }
;

statement
: compound_statement {
  $<c>$.s = $<c>1.s;
  check_sem(sem_stack, strdup("Z"));
}
| expression_statement  {
  $<c>$ = $<c>1;
  check_sem(sem_stack, strdup("w"));
 } 
| selection_statement {
check_sem(sem_stack, strdup("1"));
 $<c>$ = $<c>1;
}
| iteration_statement {
check_sem(sem_stack, strdup("2"));
 $<c>$ = $<c>1;
}
| jump_statement {
check_sem(sem_stack, strdup("3"));
 $<c>$ = $<c>1;
}
;

expression

: unary_expression assignment_operator comparison_expression {
  if (store_name != NULL) {
    free(store_name);
    store_name = NULL;
  }
  temp = $<c>3.s;
  temp = concat(temp, strdup("\n"));
  switch($<c>2.r){
  case "=": 
    temp = concat(temp, strdup("store "));
    if($<c>1.t=="i"){
      temp = concat(temp, strdup("i32 %"));
      temp = concat(temp, strdup($<c>3.r));
      temp = concat(temp, strdup(", i32* %"));
      temp = concat(temp, strdup($<c>1.r));
    }
    else if($<c>1.t=="f"){
      temp = concat(temp, strdup("float %"));
      temp = concat(temp, strdup($<c>3.r));
      temp = concat(temp, strdup(", float* %"));
      temp = concat(temp, strdup($<c>1.r));
    }
    $<c>$.t = $<c>1.t;
    $<c>$.r = $<c>1.r;
    $<c>$.s = temp;
    break;
  case "+=":
    temp = concat(temp, strdup($<c>1.s));
    temp = concat(temp, strdup("\n"));
    char * res = registre();
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(res));
    temp = concat(temp, strdup(" = load "));
    if($<c>1.t=="i"){
      temp = concat(temp, strdup("i32* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
      char *res2 = registre();
      temp = concat(temp, strdup("%"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(" = add i32 %"));
      temp = concat(temp, strdup(res));
      temp = concat(temp, strdup(", %"));
      temp = concat(temp, strdup($<c>3.r));
      temp = concat(temp, strdup("\n"));
      temp = concat(temp, strdup("store "));
      temp = concat(temp, strdup("i32 %"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(", i32* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
    }
    else if($<c>1.t == "f"){
      temp = concat(temp, strdup("float* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
      char *res2 = registre();
      temp = concat(temp, strdup("%"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(" = fadd float %"));
      temp = concat(temp, strdup(res));
      temp = concat(temp, strdup(", %"));
      temp = concat(temp, strdup($<c>3.r));
      temp = concat(temp, strdup("\n"));
      temp = concat(temp, strdup("store "));
      temp = concat(temp, strdup("float %"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(", float* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
    }
    $<c>$.t = $<c>1.t;
    $<c>$.r = $<c>1.r;
    $<c>$.s = temp;
    break;
  case "-=":
    temp = concat(temp, strdup($<c>1.s));
    temp = concat(temp, strdup("\n"));
    char * res = registre();
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(res));
    temp = concat(temp, strdup(" = load "));
    if($<c>1.t=="i"){
      temp = concat(temp, strdup("i32* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
      char *res2 = registre();
      temp = concat(temp, strdup("%"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(" = sub i32 %"));
      temp = concat(temp, strdup(res));
      temp = concat(temp, strdup(", %"));
      temp = concat(temp, strdup($<c>3.r));
      temp = concat(temp, strdup("\n"));
      temp = concat(temp, strdup("store "));
      temp = concat(temp, strdup("i32 %"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(", i32* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
    }
    else if($<c>1.t == "f"){
      temp = concat(temp, strdup("float* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
      char *res2 = registre();
      temp = concat(temp, strdup("%"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(" = fsub float %"));
      temp = concat(temp, strdup(res));
      temp = concat(temp, strdup(", %"));
      temp = concat(temp, strdup($<c>3.r));
      temp = concat(temp, strdup("\n"));
      temp = concat(temp, strdup("store "));
      temp = concat(temp, strdup("float %"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(", float* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
    }
    $<c>$.t = $<c>1.t;
    $<c>$.r = $<c>1.r;
    $<c>$.s = temp;
    break;
  case "*=":
    temp = concat(temp, strdup($<c>1.s));
    temp = concat(temp, strdup("\n"));
    char * res = registre();
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(res));
    temp = concat(temp, strdup(" = load "));
    if($<c>1.t=="i"){
      temp = concat(temp, strdup("i32* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
      char *res2 = registre();
      temp = concat(temp, strdup("%"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(" = mul i32 %"));
      temp = concat(temp, strdup(res));
      temp = concat(temp, strdup(", %"));
      temp = concat(temp, strdup($<c>3.r));
      temp = concat(temp, strdup("\n"));
      temp = concat(temp, strdup("store "));
      temp = concat(temp, strdup("i32 %"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(", i32* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
    }
    else if($<c>1.t == "f"){
      temp = concat(temp, strdup("float* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
      char *res2 = registre();
      temp = concat(temp, strdup("%"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(" = fmul float %"));
      temp = concat(temp, strdup(res));
      temp = concat(temp, strdup(", %"));
      temp = concat(temp, strdup($<c>3.r));
      temp = concat(temp, strdup("\n"));
      temp = concat(temp, strdup("store "));
      temp = concat(temp, strdup("float %"));
      temp = concat(temp, strdup(res2));
      temp = concat(temp, strdup(", float* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
    }
    $<c>$.t = $<c>1.t;
    $<c>$.r = $<c>1.r;
    $<c>$.s = temp;
    break;
  }
  check_sem(sem_stack, strdup("E"));
  }
| comparison_expression {
check_sem(sem_stack, strdup("F"));
 $<c>$ = $<c>1;
}
;

comparison_expression
: additive_expression {
  $<c>$ = $<c>1; 
  check_sem(sem_stack, strdup("x"));
}
| additive_expression '<' additive_expression {
  char * cond = condition();
  temp = $<c>1.s;
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>3.s);
  temp = concat(temp, strdup("\n"));
  
  if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp= concat(temp, strdup("icmp slt i32 %"));
  }
  else {
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp = concat(temp, strdup("fcmp olt float %"));
  } 
  
  temp = concat(temp, $<c>1.r);
  temp = concat(temp, strdup(", %"));
  temp = concat(temp, $<c>3.r);
  temp = concat(temp, strdup("\n"));
  $<c>$.r = cond; 
  $<c>$.t = "b";
  $<c>$.s = temp;
  check_sem(sem_stack, strdup("y"));
}
| additive_expression '>' additive_expression {
  char * cond = strdup(condition());
  temp = $<c>1.s;
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>3.s);
  temp = concat(temp, strdup("\n"));
  
  if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp= concat(temp, strdup("icmp sgt i32 %"));
  }
  else {
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp = concat(temp, strdup("fcmp ogt float %"));
  } 
  
  temp = concat(temp, $<c>1.r);
  temp = concat(temp, strdup(", %"));
  temp = concat(temp, $<c>3.r);
  temp = concat(temp, strdup("\n"));
  $<c>$.r = cond; 
  $<c>$.t = "b";
  $<c>$.s = temp;
  check_sem(sem_stack, strdup("z"));
}
| additive_expression LE_OP additive_expression {
  char * cond = strdup(condition());
  temp = $<c>1.s;
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>3.s);
  temp = concat(temp, strdup("\n"));
  
  if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp= concat(temp, strdup("icmp sge i32 %"));
  }
  else {
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp = concat(temp, strdup("fcmp oge float %"));
  } 
  
  temp = concat(temp, $<c>1.r);
  temp = concat(temp, strdup(", %"));
  temp = concat(temp, $<c>3.r);
  temp = concat(temp, strdup("\n"));
  $<c>$.r = cond; 
  $<c>$.t = "b";
  $<c>$.s = temp;
  check_sem(sem_stack, strdup("A"));
}
| additive_expression GE_OP additive_expression {
  char * cond = strdup(condition());
  temp = $<c>1.s;
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>3.s);
  temp = concat(temp, strdup("\n"));
  
  if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp= concat(temp, strdup("icmp sle i32 %"));
  }
  else{
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp = concat(temp, strdup("fcmp ole float %"));
  } 
  
  temp = concat(temp, $<c>1.r);
  temp = concat(temp, strdup(", %"));
  temp = concat(temp, $<c>3.r);
  temp = concat(temp, strdup("\n"));
  $<c>$.r = cond; 
  $<c>$.t = "b";
  $<c>$.s = temp;
  check_sem(sem_stack, strdup("B"));
}
| additive_expression EQ_OP additive_expression {
  char * cond = strdup(condition());
  temp = $<c>1.s;
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>3.s);
  temp = concat(temp, strdup("\n"));
  
  if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp= concat(temp, strdup("icmp eq i32 %"));
  }
  else{
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp = concat(temp, strdup("fcmp oeq float %"));
  } 
 
  temp = concat(temp, $<c>1.r);
  temp = concat(temp, strdup(", %"));
  temp = concat(temp, $<c>3.r);
  temp = concat(temp, strdup("\n"));
  $<c>$.r = cond; 
  $<c>$.t = "b";
  $<c>$.s = temp;
  check_sem(sem_stack, strdup("C"));
}
| additive_expression NE_OP additive_expression {
  char * cond = strdup(condition());
  temp = $<c>1.s;
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>3.s);
  temp = concat(temp, strdup("\n"));
  
  if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp= concat(temp, strdup("icmp ne i32 %"));
  }
  else{
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(cond));
    temp = concat(temp, strdup(" = "));
    temp = concat(temp, strdup("fcmp one float %"));
  } 
  
  temp = concat(temp, $<c>1.r);
  temp = concat(temp, strdup(", %"));
  temp = concat(temp, $<c>3.r);
  temp = concat(temp, strdup("\n"));
  $<c>$.r = cond; 
  $<c>$.t = "b";
  $<c>$.s = temp;
  check_sem(sem_stack, strdup("D"));
}
;

additive_expression
: multiplicative_expression {
  $<c>$ = $<c>1; 
  check_sem(sem_stack, strdup("t"));
}
| additive_expression '+' multiplicative_expression {
  temp = $<c>1.s;
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>3.s);
  temp = concat(temp, strdup("\n"));
  char* res = registre();
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(res));
  temp = concat(temp, strdup(" = "));
  if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
    temp = concat(temp, strdup("add i32 "));
    $<c>$.t = "i";
  }
  else{
    temp = concat(temp, strdup("fadd float "));
    $<c>$.t = "f";
  }
  temp = concat(temp, strdup("%"));
  temp = concat(temp, $<c>1.r);
  temp = concat(temp, strdup(", "));
  temp = concat(temp, strdup("%"));
  temp = concat(temp, $<c>3.r);
  temp = concat(temp, strdup("\n"));
  $<c>$.r = res;
  $<c>$.s = temp;
  check_sem(sem_stack, strdup("u"));  
 }
| additive_expression '-' multiplicative_expression {
  temp = $<c>1.s;
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>3.s);
  temp = concat(temp, strdup("\n"));
  char* res = registre();
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(res));
  temp = concat(temp, strdup(" = "));
  if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
    temp = concat(temp, strdup("sub i32 "));
    $<c>$.t = "i";
  }
  else{
    temp = concat(temp, strdup("fsub float "));
    $<c>$.t = "f";
  }
  temp = concat(temp, strdup("%"));
  temp = concat(temp, $<c>1.r);
  temp = concat(temp, strdup(", "));
  temp = concat(temp, strdup("%"));
  temp = concat(temp, $<c>3.r);
  temp = concat(temp, strdup("\n"));
  $<c>$.r = res;
  $<c>$.s = temp;
  check_sem(sem_stack, strdup("v"));
}
;

multiplicative_expression
: unary_expression {
  temp = $<c>1.s;
  char *reg = strdup(registre());
  temp = concat(temp, strdup("\n %"));
  temp = concat(temp, strdup(reg));
  temp = concat(temp, strdup(" = load "));
  if($<c>1.t == "i")
    temp = concat(temp, strdup("i32* %"));
  else if($<c>1.t == "f")
    temp = concat(temp, strdup("float* %"));
  temp = concat(temp, $<c>1.r);
  temp = concat(temp, strdup("\n"));
  $<c>$.s = temp;
  $<c>$.t = $<c>1.t;
  $<c>$.r = reg;
  free(reg);
  check_sem(sem_stack, strdup("q"));
  }
| multiplicative_expression '*' unary_expression {
  temp = $<c>1.s;
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>3.s);
  temp = concat(temp, strdup("\n"));
  char* res = strdup(registre());
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(res));
  temp = concat(temp, strdup(" = "));
  if($<c>1.t == "i" && $<c>3.t == "i"){
    temp = concat(temp, strdup("mul i32 "));
    $<c>$.t = "i";
  }
  if($<c>1.t == "f" && $<c>3.t == "f"){
    temp = concat(temp, strdup("fmul float "));
    $<c>$.t = "f";
  }
  temp = concat(temp, strdup("%"));
  temp = concat(temp, $<c>1.r);
  temp = concat(temp, strdup(", "));
  temp = concat(temp, strdup("%"));
  temp = concat(temp, $<c>3.r);
  temp = concat(temp, strdup("\n"));
  $<c>$.r = res;
  $<c>$.s = temp;
  check_sem(sem_stack, strdup("r"));
}
| multiplicative_expression '/' unary_expression {
  temp = $<c>1.s;
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>3.s);
  temp = concat(temp, strdup("\n"));
  char* res = registre();
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(res));
  temp = concat(temp, strdup(" = "));
  if($<c>1.t == "i" && $<c>3.t == "i"){
    temp = concat(temp, strdup("sdiv i32 "));
    $<c>$.t = "i";
  }
  if($<c>1.t == "f" && $<c>3.t == "f"){
    temp = concat(temp, strdup("fdiv float "));
    $<c>$.t = "f";
  }
  temp = concat(temp, strdup("%"));
  temp = concat(temp, $<c>1.r);
  temp = concat(temp, strdup(", "));
  temp = concat(temp, strdup("%"));
  temp = concat(temp, $<c>3.r);
  temp = concat(temp, strdup("\n"));
  $<c>$.r = res;
  $<c>$.s = temp;
  check_sem(sem_stack, strdup("s"));
}
 ;

unary_expression 
: postfix_expression {
  $<c>$ = $<c>1;
  check_sem(sem_stack, strdup("m"));
}
| INC_OP unary_expression {
  temp = $<c>2.s;
  char *res = strdup(registre());
  char *tmp = strdup(registre());
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(tmp));
  temp = concat(temp, strdup(" = load "));
  if($<c>2.t[0] == 'i')
    temp = concat(temp, strdup("i32* %"));
  else{
    temp = concat(temp, strdup("float* %"));
  temp = concat(temp, strdup($<c>2.r));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(res));
  temp = concat(temp, strdup(" = "));
  if($<c>2.t[0] =='i'){
    temp = concat(temp, strdup("add i32  1, "));
  }
  else{
    temp = concat(temp, strdup("fadd float 1.0, "));
  }
  temp = concat(temp, strdup("%"));
  temp = concat(temp, tmp);
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup("store "));
  if($<c>2.t[0] =='i'){
    temp = concat(temp, strdup("i32 %"));
    temp = concat(temp, res);
    temp = concat(temp, strdup(", i32* %"));
    temp = concat(temp, strdup($<c>2.r));
  }
  else{
    temp = concat(temp, strdup("float %"));
    temp = concat(temp, res);
    temp = concat(temp, strdup(", float *"));
    temp = concat(temp, strdup($<c>2.r));
  }
  $<c>$.s = temp;
  $<c>$.r = $<c>2.r;
  $<c>$.t = $<c>2.t;
  check_sem(sem_stack, strdup("n"));
}
| DEC_OP unary_expression {
  temp = $<c>2.s;
  char *res = strdup(registre());
  char *tmp = strdup(registre());
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(tmp));
  temp = concat(temp, strdup(" = load "));
  if($<c>2.t == "i")
    temp = concat(temp, strdup("i32* %"));
  else if($<c>2.t == "f")
    temp = concat(temp, strdup("float* %"));
  temp = concat(temp, strdup($<c>2.r));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(res));
  temp = concat(temp, strdup(" = "));
  if($<c>2.t[0] =='i'){
    $<c>$.t = "i";
    temp = concat(temp, strdup("sub i32 "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(tmp));
    temp = concat(temp, strdup(", 1"));
  }
  else{
    $<c>$.t = "f";
    temp = concat(temp, strdup("fsub float "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(tmp));
    temp = concat(temp, strdup(", 1.0"));
  }
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup("store "));
  if($<c>2.t[0] =='i'){
    temp = concat(temp, strdup("i32 %"));
    temp = concat(temp, res);
    temp = concat(temp, strdup(", i32* %"));
    temp = concat(temp, strdup($<c>2.r));
  }
  else{
    temp = concat(temp, strdup("float %"));
    temp = concat(temp, res);
    temp = concat(temp, strdup(", float *"));
    temp = concat(temp, strdup($<c>2.r));
  }
  $<c>$.s = temp;
  $<c>$.r = $<c>2.r;
  $<c>$.t = $<c>2.t;
  check_sem(sem_stack, strdup("o"));
  }
  
| unary_operator unary_expression {
  
    temp = $<c>2.s;
    char *res = registre(); 
  char *tmp = registre();
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(tmp));
  temp = concat(temp, strdup(" = load "));
  if($<c>2.t == "i")
    temp = concat(temp, strdup("i32* %"));
  else if($<c>2.t == "f")
    temp = concat(temp, strdup("float* %"));
  temp = concat(temp, strdup($<c>2.r));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(res));
  temp = concat(temp, strdup(" = "));
  if($<c>2.t =="i"){
    //$<c>$.t = "i";
    temp = concat(temp, strdup("mul -1, "));
  }
  else if($<c>2.t =="f"){
    //$<c>$.t = "f";
    temp = concat(temp, strdup("fmul float -1.0, "));
  }
  temp = concat(temp, strdup("%"));
  temp = concat(temp, tmp);
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup("store "));
  if($<c>2.t =="i"){
    temp = concat(temp, strdup("i32 %"));
    temp = concat(temp, res);
    temp = concat(temp, strdup(", i32* %"));
    temp = concat(temp, strdup($<c>2.r));
  }
  else if($<c>2.t =="f"){
    temp = concat(temp, strdup("float %"));
    temp = concat(temp, res);
    temp = concat(temp, strdup(", float *"));
    temp = concat(temp, strdup($<c>2.r));
  }
  $<c>$.s = temp;
  $<c>$.r = $<c>2.s;
  $<c>$.t = $<c>2.t;
  check_sem(sem_stack, strdup("p"));
}
;

postfix_expression
: primary_expression  {
  $<c>$ = $<c>1; 
  check_sem(sem_stack, strdup("i"));
}
| postfix_expression '[' expression ']' {
  temp = $<c>1.s;
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, $<c>3.s);
  temp = concat(temp, strdup("\n"))
  char *addr = pointeur();
  char *res = registre();
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(addr));
  if($<c>1.t == "it"){
    temp = concat(temp, strdup(" = getelementptr i32* %"));
    temp = concat(temp, $<c>1.r); 
    temp = concat(temp, strdup(", i32 %"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(res));
    temp = concat(temp, strdup(" = load i32* %"));
    temp = concat(temp, addr);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    $<c>$.r = res;
    $<c>$.t = "i"; 
  }
  else if($<c>1.t == "ft"){
    temp = concat(temp, strdup(" = getelementptr float* %"));
    temp = concat(temp, $<c>1.r); 
    temp = concat(temp, strdup(", i32 %"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup(res));
    temp = concat(temp, strdup(" = load float* %"));
    temp = concat(temp, addr);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    $<c>$.r = res;
    $<c>$.t = "f"; 
  }
  check_sem(sem_stack, strdup("j"));
}
;

primary_expression
: IDENTIFIER { //pas possible de faire le chargement ici(cas affectation)
  }
| CONSTANTI {
  code = concat(code, strdup("%"));
  code = concat(code, strdup($<c>$.r));
  code = concat(code, strdup(" = add i32 0, "));
  code = concat(code, strdup("??????")); //comment recup valeur ?
  code = concat(code, strdup("\n"));
  }
| CONSTANTF{
  code = concat(code, strdup("%"));
  code = concat(code, strdup($<c>$.r));
  code = concat(code, strdup(" = fadd float 0.0, "));
  code = concat(code, strdup("??????")); //comment recup valeur ?
  code = concat(code, strdup("\n"));
  }
| IDENTIFIER '(' ')' {
  temp = strdup("%");
  temp = concat(temp, strdup($<c>$.r));
  temp = concat(temp, strdup(" = call "));
  char typeRetour = $<c>1.t[0];
  if (typeRetour=='i')
    temp = concat(temp, strdup("i32 "));
  else if (typeRetour == 'f')
    temp = concat(temp, strdup("float "));
  temp = concat(temp, ??);//nom de la fonction
  temp = concat(temp, strdup("()\n"));
  $<c>$.s = temp;
  }
| IDENTIFIER '(' argument_expression_list ')' {
  temp = strdup("%");
  temp = concat(temp, strdup($<c>$.r));
  temp = concat(temp, strdup(" = call "));
  char typeRetour = $<c>1.t[0];
  if (typeRetour=='i')
    temp = concat(temp, strdup("i32 "));
  else if (typeRetour == 'f')
    temp = concat(temp, strdup("float "));
  temp = concat(temp, ??);//nom de la fonction
  temp = concat(temp, strdup("( "));
  temp = concat(temp, strdup($<c>3.r)); //faire gestion nécessaire dans arg_exp
  temp = concat(temp, strdup(")\n"));
  $<c>$.s = temp;
  }
		  
  
  }
| IDENTIFIER INC_OP  { 
  //recopoer code pré-incrémentation (unary) ? Différence ?
  }
| IDENTIFIER DEC_OP{
  //idem
  };


assignment_operator
: '=' {
  $<c>$.sem = strdup("G");
  $<c>$.r = "=";
  }
| MUL_ASSIGN {
  $<c>$.sem = strdup("H");
  $<c>2.r = "*=";
  }
| ADD_ASSIGN {
  $<c>$.sem = strdup("I");
  $<c>2.r = "+=";
  }
| SUB_ASSIGN {
  $<c>$.sem = strdup("J");
  $<c>$.r = "-=";
}
;
