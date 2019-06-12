%{
#include "tree.h"   
#include "concat.h"

#define TRUE 1
#define FALSE 0

  extern int yylineno;

  void Wvar(char *);
  int yylex ();
  int yyerror ();

  struct tree *htemp=NULL;
  struct tree *hglob=NULL;

  int nb_err = 0;
  int err = 0;

  char *current_type = NULL;
  char *store_name = NULL;
  int in_local_flag = FALSE;
  int func_dec_flag = FALSE;
  
  char *temp = NULL;
  char *code = NULL;
  char *ret_type = NULL;

%}

%union {
  struct att {
    char *s; //code principal
    char *c; //code auxiliaire
    char *r; //registre
    char *t; //type
    char *sem; //états visités
    char *n;//nom de l'identifiant
  } c;
 }


%token <c> IDENTIFIER 
%token <c> CONSTANTF 
%token <c> CONSTANTI
%token INC_OP DEC_OP LE_OP GE_OP EQ_OP NE_OP
%token SUB_ASSIGN MUL_ASSIGN ADD_ASSIGN
 //%token TYPE_NAME ?
%token INT FLOAT VOID
%token IF ELSE WHILE RETURN FOR
%start program
%%


 //==================
 //primary_expression
 //==================

primary_expression
: IDENTIFIER {
  code = strdup("\n");
  if (0 == existId(hglob, $1.s) && 0 == existId(htemp, $1.s)){
    temp = concat(strdup("Variable inconnue : "),strdup($1.s));
    yyerror(temp);
    free(temp);
  } else if (store_name == NULL){ // i.e. affectation de la variable
    store_name = strdup($1.s);    
  } else { // i.e. la variable est utilisée : est-elle initialisée ? dans le bon bloc
    if (initId(hglob, $1.s) == 0 && initId(htemp, $1.s) == 0){
      temp = concat(strdup("Variable non initialisée : "),strdup($1.s));
      yyerror(temp);
      free(temp);
    }
  }
  if (1 == existId(hglob, $1.s)) {
    $<c>$.r = strdup(regId(hglob, $1.s));
    $<c>$.t = strdup(typeId(hglob, $1.s));
  } else {
    $<c>$.r = strdup(regId(htemp, $1.s));
    $<c>$.t = strdup(typeId(htemp, $1.s));
  }  
  if ($<c>$.r[0] == 'r'){
    temp = strdup($<c>$.r+1);
    free($<c>$.r);
    $<c>$.r = temp;
    free($1.s);
    $<c>$.s = strdup(" "); 
    $<c>$.sem = strdup("w");  
  } else {
    $<c>$.s = $1.s; 
    $<c>$.sem = strdup("a");
  }
 }
| CONSTANTI {
  $<c>$.r = registre();
  $<c>$.t = strdup("i");
  $<c>$.s = strdup("%");
  $<c>$.s = concat($<c>$.s, strdup($<c>$.r));
  $<c>$.s = concat($<c>$.s, strdup(" = add i32 0, "));
  $<c>$.s = concat($<c>$.s, strdup($1.s));
  $<c>$.s = concat($<c>$.s, strdup("\n"));
  $<c>$.sem = strdup("b");
  }
| CONSTANTF {
  $<c>$.r = registre();
  $<c>$.t = strdup("f");
  $<c>$.s = strdup("%");
  $<c>$.s = concat($<c>$.s, strdup($<c>$.r));
  $<c>$.s = concat($<c>$.s, strdup(" = fadd float 0.0, "));
  $<c>$.s = concat($<c>$.s, strdup($<c>1.s));
  $<c>$.s = concat($<c>$.s, strdup("\n"));
  $<c>$.sem = strdup("c");
    }
| '(' expression ')' {
  $<c>$ = $<c>2;
  free($<c>$.sem);
  $<c>$.sem = strdup("d");
  }
| IDENTIFIER '(' ')' {
  $<c>$.r = registre();
  temp = strdup(" ");
  if (0 == existId(hglob, $1.s)){
    temp = concat(strdup("Fonction inconnue : "),strdup($1.s));
    yyerror(temp);
    free(temp);  
  } 
  $<c>$.t = strdup(typeId(hglob, $1.s));
  //ici le code de chargement
  char typeRetour = $<c>$.t[0];
  if (typeRetour != 'v'){
    temp = strdup("%");
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
  }
  temp = concat(temp, strdup("call "));
  if (typeRetour=='i')
    temp = concat(temp, strdup("i32 "));
  else if (typeRetour == 'f')
    temp = concat(temp, strdup("float "));
  else if (typeRetour == 'v')
    temp = concat(temp, strdup("void "));
  temp = concat(temp, strdup("@"));
  temp = concat(temp, strdup($<c>1.s));
  temp = concat(temp, strdup("("));
  temp = concat(temp, params_func($1.s));
  temp = concat(temp, strdup(")\n"));
  $<c>$.s = temp; 
  if (strcmp($<c>$.t+1,"v")!=0)
    yyerror("Erreur de typage de la fonction : attend paramètre void.");
  $<c>$.t[1] = '\0';
  temp = strdup($<c>$.t);
  free($<c>$.t);
  $<c>$.t = temp;
  $<c>$.sem = strdup("e");
  }
| IDENTIFIER '(' argument_expression_list ')' {
  $<c>$.r = registre();
  if (0 == existId(hglob, $1.s)){
    temp = concat(strdup("Fonction inconnue : "),strdup($1.s));
    yyerror(temp);
    free(temp);   
  } 
  $<c>$.t = strdup(typeId(hglob, $1.s));
  //rajouter ici le code de chargement
  char typeRetour = $<c>$.t[0];
  temp = $<c>3.c;
  if (typeRetour != 'v'){
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
  }
  temp = concat(temp, strdup("call "));
  if (typeRetour=='i')
    temp = concat(temp, strdup("i32 "));
  else if (typeRetour == 'f')
    temp = concat(temp, strdup("float "));
  else if (typeRetour == 'v')
    temp = concat(temp, strdup("void "));
  temp = concat(temp, strdup("@"));
  temp = concat(temp, strdup($1.s));
  temp = concat(temp, strdup("("));
  temp = concat(temp, $<c>3.s);
  if (strcmp($1.s, "norm_pi_pi") != 0)
    temp = concat(temp,strdup(", i32 %index, %struct.CarElt* %car, %struct.Situation* %s"));
  temp = concat(temp, strdup(")\n"));
  $<c>$.s = temp;
  if (strcmp($<c>$.t+1,$<c>3.t)!=0)
    yyerror("Erreur de typage de la fonction : paramètres invalides.");
  $<c>$.t[1] = '\0';
  temp = strdup($<c>$.t);
  free($<c>$.t);
  $<c>$.t = temp;
  free($1.s);
  free($<c>3.sem);
  free($<c>3.t);
  $<c>$.sem = strdup("f");
  }
| IDENTIFIER INC_OP  {
  if (0 == existId(hglob, $1.s) && 0 == existId(htemp, $1.s)){
    temp = concat(strdup("Variable inconnue : "),strdup($1.s));
    yyerror(temp);
    free(temp);
  } else if (store_name == NULL){ // i.e. affectation de la variable
    store_name = strdup($1.s);    
  } else { // i.e. la variable est utilisée : est-elle initialisée ? dans le bon bloc
    if (initId(hglob, $1.s) == 0 && initId(htemp, $1.s) == 0){
      temp = concat(strdup("Variable non initialisée : "),strdup($1.s));
      yyerror(temp);
      free(temp);
    }
  }
  if (strcmp(typeId(htemp,$1.s),"f") != 0 && strcmp(typeId(hglob,$1.s),"f") != 0
   && strcmp(typeId(htemp,$1.s),"i") != 0 && strcmp(typeId(hglob,$1.s),"i") != 0)
    yyerror("Incrémentation sur un mauvais type.");
  if (1 == existId(hglob, $1.s)) {
    $<c>$.r = strdup(regId(hglob, $1.s));
    $<c>$.t = strdup(typeId(hglob, $1.s));
  } else {
    $<c>$.r = strdup(regId(htemp, $1.s));
    $<c>$.t = strdup(typeId(htemp, $1.s));
  }  
  $<c>$.s = $1.s; 
  $<c>$.sem = strdup("g");
  }
| IDENTIFIER DEC_OP  {
  if (0 == existId(hglob, $1.s) && 0 == existId(htemp, $1.s)){
    temp = concat(strdup("Variable inconnue : "),strdup($1.s));
    yyerror(temp);
    free(temp);
  } else if (store_name == NULL){ // i.e. affectation de la variable
    store_name = strdup($1.s);    
  } else { // i.e. la variable est utilisée : est-elle initialisée ? dans le bon bloc
    if (initId(hglob, $1.s) == 0 && initId(htemp, $1.s) == 0){
      temp = concat(strdup("Variable non initialisée : "),strdup($1.s));
      yyerror(temp);
      free(temp);
    }
  }
  if (strcmp(typeId(htemp,$1.s),"f") != 0 && strcmp(typeId(hglob,$1.s),"f") != 0
   && strcmp(typeId(htemp,$1.s),"i") != 0 && strcmp(typeId(hglob,$1.s),"i") != 0)
    yyerror("Décrémentation sur un mauvais type."); 
  if (1 == existId(hglob, $1.s)) {
    $<c>$.r = strdup(regId(hglob, $1.s));
    $<c>$.t = strdup(typeId(hglob, $1.s));
  } else {
    $<c>$.r = strdup(regId(htemp, $1.s));
    $<c>$.t = strdup(typeId(htemp, $1.s));
  }  
  $<c>$.s = $1.s;
  $<c>$.sem = strdup("h");
  }
;

 //==================
 //postfix_expression
 //==================


postfix_expression
: primary_expression  {
  $<c>$ = $<c>1; 
  
 }
| postfix_expression '[' expression ']' {
  $<c>$ = $<c>1; 
  if (strcmp($<c>3.t,"i") != 0)
    yyerror("L'accès au tableau doit se faire par un entier.");
  if (strcmp($<c>1.sem,"j") == 0)
    yyerror("Pas de tableau à plusieurs dimensions.");
  else if (strcmp($<c>1.t+1,"t") != 0)
    yyerror("La variable n'est pas un tableau !");
  free($<c>1.s);
  temp = $<c>3.s;
  char *addr = pointeur();
  temp = concat(temp, strdup("%"));
  temp = concat(temp, strdup(addr));
  if(strcmp($<c>1.t,"it")==0){
    temp = concat(temp, strdup(" = getelementptr i32* %"));
    temp = concat(temp, $<c>1.r); 
    temp = concat(temp, strdup(", i32 %"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    $<c>$.r = addr;
    $<c>$.t = strdup("i"); 
  }
  else {
    temp = concat(temp, strdup(" = getelementptr float* %"));
    temp = concat(temp, $<c>1.r); 
    temp = concat(temp, strdup(", i32 %"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    $<c>$.r = addr;
    $<c>$.t = strdup("f"); 
  }
  free($<c>1.t);
  free($<c>3.sem);
  free($<c>3.t);
  free($<c>1.sem);
  $<c>$.sem = strdup("j");
  }
;

 //========================
 //argument_expression_list
 //========================

argument_expression_list
: expression {
  $<c>$ = $<c>1;
  char typeArg = $<c>1.t[0];
  if (typeArg == 'i'){
    temp = strdup("i32 %");
  } else {
    temp = strdup("float %");
  }
  temp = concat(temp,strdup($<c>1.r));
  $<c>$.s = temp;
  $<c>$.c = $<c>1.s;
  free($<c>1.sem);  
  $<c>$.sem = strdup("k");
  }
| argument_expression_list ',' expression {
  char typeArg = $<c>3.t[0];
  if (typeArg == 'i'){
    temp = strdup("i32 %");
  } else {
    temp = strdup("float %");
  }
  temp = concat(temp,strdup($<c>3.r));
  $<c>$ = $<c>1;
  $<c>$.s = concat($<c>$.s, strdup(", "));
  $<c>$.s = concat($<c>$.s, temp);
  $<c>$.r = concat($<c>$.r,$<c>3.r);
  $<c>$.t = concat($<c>$.t,$<c>3.t);
  $<c>$.c = concat($<c>1.c,$<c>3.s);
  free($<c>3.sem);
  free($<c>1.sem);
  $<c>$.sem = strdup("l");
  }
;

 //================
 //unary_expression
 //================

unary_expression 
: postfix_expression {
    $<c>$ = $<c>1;
    
  }
| INC_OP unary_expression {
  if (strcmp($<c>2.sem,"a") != 0 && strcmp($<c>2.sem,"o") != 0)
    yyerror("Incrémentation sur un mauvais type.");
  $<c>$ = $<c>2; //TODO code incrémentation
  $<c>$.sem = strdup("m");
  free($<c>2.sem);
 }
| DEC_OP unary_expression {
  if (strcmp($<c>2.sem,"a") != 0 && strcmp($<c>2.sem,"o") != 0)
    yyerror("Décrémentation sur un mauvais type.");
  $<c>$ = $<c>2; //TODO code incrémentation
  $<c>$.sem = strdup("n");
  free($<c>2.sem);
  }
| unary_operator unary_expression {
  $<c>$ = $<c>2; //TODO code incrémentation
  $<c>$.sem = strdup("o");
  free($<c>2.sem);
 }
;

 //==============
 //unary_operator
 //==============

unary_operator
: '-'
;

 //=========================
 //multiplicative_expression
 //=========================

multiplicative_expression
: unary_expression {
  $<c>$ = $<c>1;
  //ptr ou reg ?
  if(strcmp($<c>1.sem,"m") == 0 || strcmp($<c>1.sem,"n") == 0 ||
     strcmp($<c>1.sem,"j") == 0 || strcmp($<c>1.sem,"a") == 0 ||
     strcmp($<c>1.sem,"g") == 0 || strcmp($<c>1.sem,"h") == 0){
    if(strcmp($<c>1.sem,"j") == 0)
      temp = $<c>1.s;
    else
      temp = strdup(" ");
    //c'est un pointeur
    $<c>$.r = registre();
    temp = concat(temp,strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = load "));
    if($<c>1.t[0] == 'i')
      temp = concat(temp, strdup("i32* %"));
    else if($<c>1.t[0] == 'f')
      temp = concat(temp, strdup("float* %"));
    temp = concat(temp, $<c>1.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    $<c>$.t = $<c>1.t;
  } else {
    //c'est un registre
  }
 }
| multiplicative_expression '*' unary_expression {
    $<c>$ = $<c>1;
    temp = $<c>1.s;
    //temp = concat(temp,$<c>3.s);
    //ptr ou reg ?
    if(strcmp($<c>3.sem,"m") == 0 || strcmp($<c>3.sem,"n") == 0 ||
       strcmp($<c>3.sem,"j") == 0 || strcmp($<c>3.sem,"a") == 0 ||
       strcmp($<c>3.sem,"g") == 0 || strcmp($<c>3.sem,"h") == 0){
      if(strcmp($<c>3.sem,"j") == 0)
	temp = concat(temp,$<c>3.s);
      //c'est un pointeur
      char* reg = registre();
      temp = concat(temp, strdup("%"));
      temp = concat(temp, strdup(reg));
      temp = concat(temp, strdup(" = load "));
      if($<c>3.t[0] == 'i')
	temp = concat(temp, strdup("i32* %"));
      else if($<c>3.t[0] == 'f')
	temp = concat(temp, strdup("float* %"));
      temp = concat(temp, $<c>3.r);
      temp = concat(temp, strdup("\n"));
      $<c>3.r = reg;
    } else {
      //c'est un registre
      temp = concat(temp,$<c>3.s);
    }
    //nouveau registre
    $<c>$.r = registre();
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
    if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
      temp = concat(temp, strdup("mul i32 "));
      $<c>$.t = strdup("i");
    }
    else{
      temp = concat(temp, strdup("fmul float "));
      $<c>$.t = strdup("f");
    }
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>1.r);
    temp = concat(temp, strdup(", "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    if (strcmp($<c>1.t,$<c>3.t) != 0)
      yyerror("Types incompatibles.");
    free($<c>3.t);
    free($<c>3.sem);
  }
| multiplicative_expression '/' unary_expression {
    $<c>$ = $<c>1;
    temp = $<c>1.s;
    //ptr ou reg ?
    if(strcmp($<c>3.sem,"m") == 0 || strcmp($<c>3.sem,"n") == 0 ||
       strcmp($<c>3.sem,"j") == 0 || strcmp($<c>3.sem,"a") == 0 ||
       strcmp($<c>3.sem,"g") == 0 || strcmp($<c>3.sem,"h") == 0){
      if(strcmp($<c>3.sem,"j") == 0)
	temp = concat(temp,$<c>3.s);
      //c'est un pointeur
      char* reg = registre();
      temp = concat(temp, strdup("%"));
      temp = concat(temp, strdup(reg));
      temp = concat(temp, strdup(" = load "));
      if($<c>3.t[0] == 'i')
	temp = concat(temp, strdup("i32* %"));
      else if($<c>3.t[0] == 'f')
	temp = concat(temp, strdup("float* %"));
      temp = concat(temp, $<c>3.r);
      temp = concat(temp, strdup("\n"));
      $<c>3.r = reg;
    } else {
	temp = concat(temp,$<c>3.s);
	//c'est un registre
    }
    //nouveau registre
    $<c>$.r = registre();
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
    if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
      temp = concat(temp, strdup("sdiv i32 "));
      $<c>$.t = strdup("i");
    }
    else{
      temp = concat(temp, strdup("fdiv float "));
      $<c>$.t = strdup("f");
    }
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>1.r);
    temp = concat(temp, strdup(", "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    if (strcmp($<c>1.t,$<c>3.t) != 0)
      yyerror("Types incompatibles.");
    free($<c>3.t);
    free($<c>3.sem);
  }
;

 //===================
 //additive_expression
 //===================

additive_expression
: multiplicative_expression {
    $<c>$ = $<c>1; 
  }
| additive_expression '+' multiplicative_expression {
    $<c>$ = $<c>1;
    temp = $<c>1.s;
    $<c>$.r = registre();//nouveau registre
    temp = concat(temp,$<c>3.s);
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
    if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
      temp = concat(temp, strdup("add i32 "));
      $<c>$.t = strdup("i");
    }
    else{
      temp = concat(temp, strdup("fadd float "));
      $<c>$.t = strdup("f");
    }
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>1.r);
    temp = concat(temp, strdup(", "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    if (strcmp($<c>1.t,$<c>3.t) != 0)
      yyerror("Types incompatibles.");
    free($<c>3.t);
    free($<c>3.sem);
  }
| additive_expression '-' multiplicative_expression {
    $<c>$ = $<c>1;
    temp = $<c>1.s;
    $<c>$.r = registre();//nouveau registre
    temp = concat(temp,$<c>3.s);
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
    if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
      temp = concat(temp, strdup("sub i32 "));
      $<c>$.t = strdup("i");
    }
    else{
      temp = concat(temp, strdup("fsub float "));
      $<c>$.t = strdup("f");
    }
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>1.r);
    temp = concat(temp, strdup(", "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    if (strcmp($<c>1.t,$<c>3.t) != 0)
      yyerror("Types incompatibles.");
    free($<c>3.t);
    free($<c>3.sem);
  }
;

 //=====================
 //comparison_expression
 //=====================


comparison_expression
: additive_expression {
    $<c>$ = $<c>1;
    $<c>$.sem = strdup("x");
    free($<c>1.sem);
  }
| additive_expression '<' additive_expression {
    $<c>$ = $<c>1;
    //$<c>$.t = strdup("b");
    temp = $<c>1.s;
    $<c>$.r = condition();//nouveau registre
    temp = concat(temp,$<c>3.s);
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
    if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
      temp = concat(temp, strdup("icmp slt i32 "));
    }
    else{
      temp = concat(temp, strdup("fcmp olt float "));
    }
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>1.r);
    temp = concat(temp, strdup(", "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    if (strcmp($<c>1.t,$<c>3.t) != 0)
      yyerror("Types incompatibles.");
    
    free($<c>3.t);
    free($<c>3.sem);
    free($<c>$.sem);
    $<c>$.sem = strdup("F");
  }
| additive_expression '>' additive_expression {
    $<c>$ = $<c>1;
    temp = $<c>1.s;
    $<c>$.r = condition();
    temp = concat(temp,$<c>3.s);
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
    if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
      temp = concat(temp, strdup("icmp sgt i32 "));
    }
    else{
      temp = concat(temp, strdup("fcmp ogt float "));
    }
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>1.r);
    temp = concat(temp, strdup(", "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    if (strcmp($<c>1.t,$<c>3.t) != 0)
      yyerror("Types incompatibles.");
    free($<c>3.t);
    free($<c>3.sem);
    free($<c>$.sem);
    $<c>$.sem = strdup("F");
  }
| additive_expression LE_OP additive_expression {
    $<c>$ = $<c>1;
    temp = $<c>1.s;
    $<c>$.r = condition();//nouveau registre
    temp = concat(temp,$<c>3.s);
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
    if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
      temp = concat(temp, strdup("icmp sle i32 "));
    }
    else{
      temp = concat(temp, strdup("fcmp ole float "));
    }
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>1.r);
    temp = concat(temp, strdup(", "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    if (strcmp($<c>1.t,$<c>3.t) != 0)
      yyerror("Types incompatibles.");
    free($<c>3.t);
    free($<c>3.sem);
    free($<c>$.sem);
    $<c>$.sem = strdup("F");
  }
| additive_expression GE_OP additive_expression {
    $<c>$ = $<c>1;
    temp = $<c>1.s;
    $<c>$.r = condition();//nouveau registre
    temp = concat(temp,$<c>3.s);
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
    if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
      temp = concat(temp, strdup("icmp sge i32 "));
    }
    else{
      temp = concat(temp, strdup("fcmp oge float "));
    }
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>1.r);
    temp = concat(temp, strdup(", "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    if (strcmp($<c>1.t,$<c>3.t) != 0)
      yyerror("Types incompatibles.");
    free($<c>3.t);
    free($<c>3.sem);
    free($<c>$.sem);
    $<c>$.sem = strdup("F");
  }
| additive_expression EQ_OP additive_expression {
    $<c>$ = $<c>1;
    temp = $<c>1.s;
    $<c>$.r = condition();//nouveau registre
    temp = concat(temp,$<c>3.s);
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
    if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
      temp = concat(temp, strdup("icmp eq i32 "));
    }
    else{
      temp = concat(temp, strdup("fcmp oeq float "));
    }
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>1.r);
    temp = concat(temp, strdup(", "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    if (strcmp($<c>1.t,$<c>3.t) != 0)
      yyerror("Types incompatibles.");
    free($<c>3.t);
    free($<c>3.sem);
    free($<c>$.sem);
    $<c>$.sem = strdup("F");
  }
| additive_expression NE_OP additive_expression {
    $<c>$ = $<c>1;
    temp = $<c>1.s;
    $<c>$.r = condition();//nouveau registre
    temp = concat(temp,$<c>3.s);
    temp = concat(temp, strdup("%"));
    temp = concat(temp, strdup($<c>$.r));
    temp = concat(temp, strdup(" = "));
    if($<c>1.t[0] == 'i' && $<c>3.t[0] == 'i'){
      temp = concat(temp, strdup("icmp ne i32 "));
    }
    else{
      temp = concat(temp, strdup("fcmp one float "));
    }
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>1.r);
    temp = concat(temp, strdup(", "));
    temp = concat(temp, strdup("%"));
    temp = concat(temp, $<c>3.r);
    temp = concat(temp, strdup("\n"));
    $<c>$.s = temp;
    if (strcmp($<c>1.t,$<c>3.t) != 0)
      yyerror("Types incompatibles.");
    free($<c>3.t);
    free($<c>3.sem);
    free($<c>$.sem);
    $<c>$.sem = strdup("F");
  }
;

 //==========
 //expression
 //==========

expression
: unary_expression assignment_operator comparison_expression {
  $<c>$ = $<c>1;
  Wvar(store_name); //on peut l'affecter ? initialise au besoin
  if (store_name != NULL) {
    free(store_name);
    store_name = NULL;
  }
  code = strdup(" ");
  if (strcmp($<c>3.sem,"F") == 0){
    //on change le i1 en un i32
    char *res = registre();
    code = concat(code,strdup("%"));
    code = concat(code,strdup(res));
    code = concat(code,strdup(" = zext i1 %"));
    code = concat(code,$<c>3.r);
    code = concat(code, strdup(" to i32\n"));
    $<c>3.r = res;
  }
  //on change un peu le code, registre, toutca ...
  //différentes opérations suivant le type d'assignation
  if (strcmp($<c>2.sem, "G") == 0){
    temp = strdup("store ");
    if(strcmp($<c>1.t,"i")==0){
      temp = concat(temp, strdup("i32 %"));
      temp = concat(temp, strdup($<c>3.r));
      temp = concat(temp, strdup(", i32* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp,strdup("\n"));
    }
    else{
      temp = concat(temp, strdup("float %"));
      temp = concat(temp, strdup($<c>3.r));
      temp = concat(temp, strdup(", float* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp,strdup("\n"));
    }
    free($<c>$.r);
    $<c>$.r = strdup($<c>3.r);
  } else if (strcmp($<c>2.sem, "H") == 0){
    char * res = registre();
    char *res2 = registre();
    temp = strdup("%");
    temp = concat(temp, strdup(res));
    temp = concat(temp, strdup(" = load "));
    if(strcmp($<c>1.t,"i")==0){
      temp = concat(temp, strdup("i32* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
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
    } else {
      temp = concat(temp, strdup("float* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
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
    free($<c>$.r);
    $<c>$.r = res2;
    free(res);
  } else if (strcmp($<c>2.sem, "I") == 0){
    char * res = registre();
    char *res2 = registre();
    temp = strdup("%");
    temp = concat(temp, strdup(res));
    temp = concat(temp, strdup(" = load "));
    if(strcmp($<c>1.t,"i")==0){
      temp = concat(temp, strdup("i32* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
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
    } else {
      temp = concat(temp, strdup("float* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
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
    free($<c>$.r);
    $<c>$.r = res2;
    free(res);
  } else if (strcmp($<c>2.sem, "J") == 0){
    char * res = registre();
    char *res2 = registre();
    temp = strdup("%");
    temp = concat(temp, strdup(res));
    temp = concat(temp, strdup(" = load "));
    if(strcmp($<c>1.t,"i")==0){
      temp = concat(temp, strdup("i32* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
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
    } else {
      temp = concat(temp, strdup("float* %"));
      temp = concat(temp, strdup($<c>1.r));
      temp = concat(temp, strdup("\n"));
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
    free($<c>$.r);
    $<c>$.r = res2;
    free(res);
  }
  if (strcmp($<c>1.sem,"b") == 0 || strcmp($<c>1.sem,"c") == 0)
    yyerror("Vous ne pouvez pas affecter une constante.");
  else if (strcmp($<c>1.sem,"d") == 0 || strcmp($<c>1.sem,"e") == 0 || strcmp($<c>1.sem,"f") == 0)
    yyerror("Vous ne pouvez pas affecter un calcul.");
  else if (strcmp($<c>1.t,$<c>3.t) != 0)
    yyerror("Types incompatibles.");
  else if (strcmp($<c>1.sem,"a") != 0 && strcmp($<c>1.sem,"j") != 0)
    yyerror("Pas d'opérations unaires lors d'une affectation !");
  free($<c>2.sem);
  if (strcmp($<c>1.sem, "j") == 0){
      $<c>$.s = concat($<c>$.s,$<c>3.s);
  } else {
    free($<c>1.s);
    $<c>$.s = $<c>3.s; 
  }
  $<c>$.s = concat($<c>$.s,code);
  $<c>$.s = concat($<c>$.s,temp);
  free($<c>1.sem);
  $<c>$.sem = $<c>3.sem;
  free($<c>3.r);
  free($<c>3.t);
 }
| comparison_expression {
    $<c>$ = $<c>1;
    if (store_name != NULL) {
      free(store_name);
      store_name = NULL;
    }
  }
;

 //===================
 //assignment_operator
 //===================

assignment_operator
: '=' {
  if (1 == existId(hglob, store_name))
    err = modifyId(hglob, store_name, NULL, 1);
  else
    err = modifyId(htemp, store_name, NULL, 1);
  $<c>$.sem = strdup("G");
  }
| MUL_ASSIGN {
  if (1 == existId(hglob, store_name))
    err = modifyId(hglob, store_name, NULL, 1);
  else
    err = modifyId(htemp, store_name, NULL, 1);
  $<c>$.sem = strdup("H");
  }
| ADD_ASSIGN {
  if (1 == existId(hglob, store_name))
    err = modifyId(hglob, store_name, NULL, 1);
  else
    err = modifyId(htemp, store_name, NULL, 1);
  $<c>$.sem = strdup("I");
  }
| SUB_ASSIGN {
  if (1 == existId(hglob, store_name))
    err = modifyId(hglob, store_name, NULL, 1);
  else
    err = modifyId(htemp, store_name, NULL, 1);
  $<c>$.sem = strdup("J");
}
;

 //===========
 //declaration
 //===========

declaration
: type_name declarator_list ';' {
    $<c>$ = $<c>2;
  }
;

 //===============
 //declarator_list
 //===============


declarator_list
: declarator {
    $<c>$ = $<c>1;
  }
| declarator_list ',' declarator {
  free($<c>3.r);
  free($<c>3.sem);
  free($<c>3.t);
  $<c>$ = $<c>1;
  $<c>$.s = concat($<c>1.s, $<c>3.s);
  }
;

 //=========
 //type_name
 //=========

type_name
: VOID  {
    if (current_type != NULL)
      free(current_type);
    current_type = strdup("v");
    if (ret_type == NULL)
      ret_type = strdup("v");
 }
| INT   {
    if (current_type != NULL)
      free(current_type);
    current_type = strdup("i");
    if (ret_type == NULL)
      ret_type = strdup("i");
  }
| FLOAT {
    if (current_type != NULL)
      free(current_type);
    current_type = strdup("f");
    if (ret_type == NULL)
      ret_type = strdup("f");
  }
;

 //==========
 //declarator
 //==========

declarator
: IDENTIFIER  {
  if (in_local_flag == TRUE){
    /* ajoute le nom et le type donnés */
    if (1 == existId(hglob, $1.s) || 1 == existId(htemp, $1.s)){
      temp = concat(strdup("Redéfinition impossible de : "),strdup($1.s));
      yyerror(temp);
      free(temp);
    } else {
      err = addId(htemp, $1.s);
      err = modifyId(htemp, $1.s, strdup(current_type), 0);
    }
  } else if (in_local_flag == FALSE) {
    /* ajoute le nom et le type donnés */
    if (existId(hglob, $1.s) == 1){
      temp = concat(strdup("Redéfinition impossible de : "),strdup($1.s));
      yyerror(temp);
      free(temp);
    } else {
      if (func_dec_flag == FALSE){
	err = addId(hglob, $1.s);
	err = modifyId(hglob, $1.s, strdup(current_type), 0);
      } else {
	err = addId(htemp, $1.s);
    	err = modifyId(htemp, $1.s, strdup(current_type), 1);
      }
    }
  }
  $<c>$.t = strdup(current_type);
  //on transmet le nom de la variable
  $<c>$.r = $1.s; 
  //on transmet le code de chargement
  if (1 == existId(hglob, $1.s)){
    temp = strdup("@");
    temp = concat(temp, strdup(regId(hglob,$1.s)));
  } else {
    temp = strdup("%");
    temp = concat(temp, strdup(regId(htemp,$1.s)));
  } 
  temp = concat(temp,strdup(" = alloca "));
  if (strcmp(current_type,"i") == 0)
    temp = concat(temp, strdup("i32\n"));
  else 
    temp = concat(temp, strdup("float\n"));
  $<c>$.s = temp;
  $<c>$.sem = strdup("Q");
 }
| '(' declarator ')' {
  $<c>$ = $<c>2; 
  //????
  free($<c>$.sem);
  $<c>$.sem = strdup("R");
  }
| declarator '[' CONSTANTI ']' {
  $<c>$ = $<c>1;
  //on crée du code à partir de la constante
  //on modifie le type en tableau
  if (in_local_flag == FALSE){
    temp = strdup(typeId(hglob, $<c>1.r));
    temp = concat(temp,"t");
    modifyId(hglob, $<c>1.r, temp, 1);
  } else {
    temp = strdup(typeId(htemp, $<c>1.r));
    temp = concat(temp,strdup("t"));
    modifyId(htemp, $<c>1.r, temp, 1);
  } 
  $<c>$.t = strdup(current_type); 
  //on transmet le code de chargement
  if (1 == existId(hglob, $<c>1.r)){
    temp = strdup("@");
    temp = concat(temp, strdup(regId(hglob,$<c>1.r)));
  } else {
    temp = strdup("%");
    temp = concat(temp, strdup(regId(htemp,$<c>1.r)));
  } 
  temp = concat(temp,strdup(" = alloca "));
  if (strcmp(current_type,"i") == 0)
    temp = concat(temp, strdup("i32, i32 "));
  else 
    temp = concat(temp, strdup("float, i32 "));
  temp = concat(temp, $3.s);
  temp = concat(temp,strdup("\n"));
  free($<c>$.s);
  $<c>$.s = temp;
  if (strcmp($<c>1.sem, "Q") != 0)
    yyerror("Syntaxe incorrecte");
  free($<c>$.sem);
  $<c>$.sem = strdup("S");
}
| declarator '[' ']' {
  $<c>$ = $<c>1;
  //?????
  free($<c>$.sem);
  $<c>$.sem = strdup("T");
}
| declarator '(' {func_dec_flag = TRUE;} parameter_list ')' {
  func_dec_flag = FALSE;
  $<c>$ = $<c>1;
  //on a transmis le nom
  //on modifie le type en fonction à partir de parameter_list
  if (in_local_flag == FALSE){
    temp = strdup(ret_type);
    temp = concat(temp,$<c>4.t);
    modifyId(hglob, $<c>1.r, temp, 1);
  } else {
    yyerror("Définition impossible d'une fonction dans une autre fonction.");
    modifyId(hglob, $<c>1.r, strdup("erreur"), 1);
    free($<c>4.t);
  }
  //on crée du code 
  temp = strdup("@");
  temp = concat(temp, strdup($<c>1.r));
  temp = concat(temp, strdup("("));
  temp = concat(temp, $<c>4.s);
  temp = concat(temp, strdup(", i32 %index, %struct.CarElt* %car, %struct.Situation* %s"));
  temp = concat(temp, strdup(")"));
  $<c>$.s = temp;
  free($<c>4.r);
  free($<c>4.sem);
  if (strcmp($<c>1.sem, "Q") != 0)
    yyerror("Syntaxe incorrecte");
  free($<c>$.sem);
  $<c>$.sem = strdup("U");
  }
| declarator '(' ')' {
  $<c>$ = $<c>1;
  //on a transmis le nom
  if (in_local_flag == FALSE){
    modifyId(hglob, $<c>1.r, concat(strdup(ret_type),strdup("v")), 1);
  } else {
    yyerror("Définition impossible d'une fonction dans une autre fonction.");
    modifyId(hglob, $<c>1.r, strdup("erreur"), 1);
  }
  free($<c>1.s);
  //on crée du code 
  temp = strdup("@");
  temp = concat(temp, strdup($<c>1.r));
  temp = concat(temp, strdup("("));
  temp = concat(temp, strdup("i32 %index, %struct.CarElt* %car, %struct.Situation* %s"));
  temp = concat(temp,strdup(")"));
  $<c>$.s = temp;
  if (strcmp($<c>1.sem, "Q") != 0)
    yyerror("Syntaxe incorrecte");
  free($<c>$.sem);
  $<c>$.sem = strdup("V");
  }
;

 //==============
 //parameter_list
 //==============

parameter_list
: parameter_declaration {
  $<c>$ = $<c>1;
 }
| parameter_list ',' parameter_declaration {
  $<c>$ = $<c>1;
  $<c>$.s = concat($<c>$.s, strdup(", "));
  $<c>$.s = concat($<c>$.s, $<c>3.s);
  $<c>$.t = concat($<c>$.t, $<c>3.t);
  free($<c>3.sem);
  free($<c>3.r);
  }
;

 //=====================
 //parameter_declaration
 //=====================

parameter_declaration
: type_name declarator {
  $<c>$ = $<c>2;
  free($<c>2.t);
  $<c>$.t = strdup(current_type);
  if (strcmp($<c>$.sem,"Q") != 0)
    yyerror("Erreur de syntaxe d'un des paramètres.");
  if (strcmp(current_type,"i") == 0)
    code = strdup("i32 ");
  else if (strcmp(current_type,"f") == 0)
    code = strdup("float ");
  code=concat(code, strdup("%"));
  code=concat(code, strdup($<c>2.r));
  temp = concat(strdup("r"),strdup($<c>2.r));
  modifyReg(htemp, $<c>2.r, temp);
  $<c>$.s = code;
  free($<c>2.s);
}
;

 //=========
 //statement
 //=========

statement
: compound_statement {
  $<c>$ = $<c>1;
  }
| expression_statement  {
  $<c>$ = $<c>1;
 } 
| selection_statement {
  $<c>$ = $<c>1;
  }
| iteration_statement {
  $<c>$ = $<c>1;
}
| jump_statement {
  $<c>$ = $<c>1;
}
;

 //==================
 //compound_statement
 //==================

compound_statement
: '{' '}' {
  $<c>$.s = strdup("\n");
  $<c>$.sem = strdup("\n");
  $<c>$.r = strdup("\n");
  $<c>$.t = strdup("\n");
 }
| '{' statement_list '}' { 
  $<c>$ = $<c>2;
  }
| '{' declaration_list statement_list '}' {
    $<c>$ = $<c>2;
    $<c>$.s = concat($<c>2.s, $<c>3.s);
    free($<c>3.r);
    free($<c>3.t);
    $<c>$.sem = $<c>3.sem;
    free($<c>2.sem);
}
;

 //================
 //declaration_list
 //================

declaration_list
: declaration {
  $<c>$ = $<c>1;
}
| declaration_list declaration {
  $<c>$ = $<c>1;
  $<c>$.s = concat($<c>$.s, $<c>2.s);
  free($<c>2.r);
  free($<c>2.t);
  free($<c>2.sem);
}
;

 //==============
 //statement_list
 //==============

statement_list
: statement {
  $<c>$ = $<c>1;
 }
| statement_list statement {
  free($<c>2.r);
  free($<c>1.sem);
  free($<c>2.t);
  $<c>$ = $<c>1;
  $<c>$.s = concat($<c>$.s, $<c>2.s);
  $<c>$.sem = $<c>2.sem;
 }
;

 //====================
 //expression_statement
 //====================

expression_statement
: ';' { 
  $<c>$.s = strdup("\n");
  $<c>$.sem = strdup("\n");
  $<c>$.r = strdup("\n");
  $<c>$.t = strdup("\n");
  if (store_name != NULL){
    free(store_name);
    store_name = NULL;
  }
 }
| expression ';' {
  $<c>1.s=concat($<c>1.s, strdup("\n")); 
  $<c>$ = $<c>1;
  }
;

 //===================
 //selection_statement
 //===================

selection_statement
: IF '(' expression ')' statement {
  if (strcmp($<c>3.sem,"F") != 0)
      yyerror("L'expression donnée n'est pas une condition");
  $<c>$ = $<c>5;
  char* debut = label();
  char* fin = label();
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
  free($<c>3.sem);
  free($<c>3.t);
}
| IF '(' expression ')' statement ELSE statement {
  if (strcmp($<c>3.sem,"F") != 0)
      yyerror("L'expression donnée n'est pas une condition");
  $<c>$ = $<c>5;
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
  temp = concat(temp, strdup(":\n"));
  temp = concat(temp, $<c>5.s); //code if
  temp = concat(temp, strdup("br label %"));
  temp = concat(temp, strdup(fin));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, sinon);
  temp = concat(temp, strdup(":\n"));
  temp = concat(temp, $<c>7.s); //code else
  temp = concat(temp, strdup("br label %"));
  temp = concat(temp, strdup(fin));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, fin);
  temp = concat(temp, strdup(":\n"));
  $<c>$.s = temp;
  free($<c>3.sem);
  free($<c>3.t);  
  free($<c>7.sem);
  free($<c>7.r);
  free($<c>7.t);  
}
| FOR '(' expression_statement expression_statement expression ')' statement {
    if (strcmp($<c>4.sem,"F") != 0)
      yyerror("L'expression donnée n'est pas une condition");
  $<c>$ = $<c>5;
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
  temp = concat(temp, strdup("br i1 %")); //branchement conditionnel
  temp = concat(temp, $<c>4.r); //récupération condition
  temp = concat(temp, strdup(", label %"));
  temp = concat(temp, strdup(debut));
  temp = concat(temp, strdup(", label %"));
  temp = concat(temp, strdup(fin));
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, debut); //label début
  temp = concat(temp, strdup(":\n"));
  temp = concat(temp, $<c>7.s); //code de la boucle
  temp = concat(temp, $<c>5.s); //action fin de boucle
  temp = concat(temp, strdup("br label %")); //retour vers test
  temp = concat(temp, test);
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, fin); //fin de la boucle - label fin
  temp = concat(temp, strdup(":\n"));
  $<c>$.s = temp;
  free($<c>3.sem);
  free($<c>3.r);
  free($<c>3.t);  
  free($<c>4.sem);
  free($<c>4.t); 
  free($<c>7.sem);
  free($<c>7.r);
  free($<c>7.t);   
}
;

 //===================
 //iteration_statement
 //===================

iteration_statement
: WHILE '(' expression ')' statement {
  if (strcmp($<c>3.sem,"F") != 0)
    yyerror("L'expression donnée n'est pas une condition.");
  $<c>$ = $<c>5;
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
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, debut); //label début
  temp = concat(temp, strdup(":\n"));
  temp = concat(temp, $<c>5.s); //code de la boucle
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, strdup("br label %")); //retour vers test
  temp = concat(temp, test);
  temp = concat(temp, strdup("\n"));
  temp = concat(temp, fin); //fin de la boucle - label fin
  temp = concat(temp, strdup(":\n"));
  $<c>$.s = temp;
  free($<c>3.t);
  free($<c>3.sem);
 }
;

 //==============
 //jump_statement
 //==============

jump_statement
: RETURN ';'{
  $<c>$.sem = strdup("(");
  if (strcmp(ret_type,"v") != 0)
    yyerror("Type de retour incorrect.");
  $<c>$.s = strdup("ret void\n");
  $<c>$.r = strdup("\n");
  $<c>$.t = strdup("\n");
 }
| RETURN expression ';' {
  $<c>$ = $<c>2;
  temp = $<c>2.s; //création expression
  temp = concat(temp, strdup("ret "));
  if (strcmp($<c>2.t,"i")==0)//type de retour
    temp = concat(temp, strdup("i32 %")); 
  else 
    temp = concat(temp, strdup("float %"));
  temp = concat(temp, strdup($<c>2.r)); //valeur retournée
  temp = concat(temp, strdup("\n"));
  $<c>$.s = temp;
  free($<c>$.sem);
  $<c>$.sem = strdup(")");
  if (strcmp(ret_type,$<c>$.t) != 0)
    yyerror("Type de retour incorrect.");
 }
;

 //=======
 //program
 //=======

program
: external_declaration {
}
| program external_declaration {
}
;

 //====================
 //external_declaration
 //====================

external_declaration
: function_definition {
  printf("%s", $<c>1.s);
  free($<c>1.s);
  free($<c>1.r);
  free($<c>1.t);
  free($<c>1.sem);
  delete_tree(htemp);
  htemp = init_tree();
  init_var(htemp);
 }
| declaration {
  free($<c>1.s);
  free($<c>1.r);
  free($<c>1.t);
  free($<c>1.sem);
  }
;

 //===================
 //function_definition
 //===================

function_definition 
: type_name declarator {in_local_flag = TRUE;} compound_statement {
  $<c>$ = $<c>4;
  code = strdup("define "); 
  temp = strdup(typeId(hglob,$<c>2.r));
  temp[1] = '\0';
  if (strcmp(temp,"v") == 0)
    code = concat(code, strdup("void "));
  else if (strcmp(temp,"i") == 0)
    code = concat(code, strdup("i32 "));
  else if (strcmp(temp,"f") == 0)
    code = concat(code, strdup("float "));
  free(temp);
  code = concat(code, $<c>2.s);
  code = concat(code, strdup("{\n"));
  code = concat(code, entete_func());
  code = concat(code, $<c>4.s);
  in_local_flag = FALSE;
  if (strcmp(ret_type,"v") != 0 && strcmp($<c>4.sem, ")") != 0)
    yyerror("Manque une valeur de retour.");
  if (strcmp(ret_type,"v") == 0)
    code = concat(code, strdup("ret void\n"));
  $<c>$.s = concat(code, strdup("}\n"));
  free(ret_type);
  free($<c>2.sem);
  free($<c>2.r);
  free($<c>2.t);
  ret_type = NULL;
 }
;

%%
#include <stdio.h>
#include <string.h>

extern int column;
extern int yylineno;
extern FILE *yyin;

char *file_name = NULL;

void Wvar(char *s){
  if (s != NULL){
    if (strcmp(s,"$steer") == 0){
    } else if (strcmp(s,"$brake") == 0){
    } else if (strcmp(s,"$accel") == 0){
    } else if (strcmp(s,"$clutch") == 0){
    } else if (strcmp(s,"$gear") == 0){
    } else if (s[0] == '$'){
      temp = concat(strdup("Variable non modifiable : "),strdup(s));
      yyerror(temp);
      free(temp);
    } 
  }
}


int yyerror (char *s) {
  nb_err++;
  fprintf (stderr, "\033[31m%s:%d:%d:\n %s\n\033[0m", file_name, yylineno, column, s);
  return 0;
}


int main (int argc, char *argv[]) {
    FILE *input = NULL;
    if (argc==2) {
	input = fopen(argv[1], "r");
	file_name = strdup(argv[1]);
	if (input) {
	    yyin = input;
	}
	else {
	  fprintf (stderr, "%s: Could not open %s\n", *argv, argv[1]);
	    return 1;
	}
    }
    else {
	fprintf (stderr, "%s: error: no input file\n", *argv);
	return 1;
    }

    in_local_flag = 0;
    htemp = init_tree();
    hglob = init_tree();
    init_var(htemp);
    init_func(hglob);
    yyparse();


    if (existId(hglob,"drive") != TRUE || strcmp(typeId(hglob,"drive"),"vv") != 0)
      yyerror("Définition incorrecte/manquante de void drive().");

    if (current_type != NULL)
      free(current_type);
    delete_tree(htemp);
    delete_tree(hglob);
    free(file_name);   

    return 0;
}
