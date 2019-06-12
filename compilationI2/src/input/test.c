//Code de départ


void drive(){
  $accel = 1.;
}


//Tests de l'incrémentation :

/*
int essai(){
  return 0;
}

void test(){
  int a[2],b;
  b = 0;
  //b++ ++; //erreur détectée par la syntaxe
  //a++ ++; //erreur détectée par la syntaxe
  --a++; //erreur détectée par moi
  -- --a; //erreur détectée par moi
  --a[2]; //erreur détectée par moi
  -- ++b; //erreur détectée par moi
  --b++; //erreur détectée par moi
  --b;
  b++;
  //a[2]++; //erreur détectée par la syntaxe
  --essai(); //erreur détectée par moi
  //essai()++; //erreur détectée par la syntaxe
  -essai();
  ---b;
  -a[2];
  -essai() = 0; // pas d'affectation de calcul et pas d'opération unaire lors d'une affectation
  --b = 2; // pas d'affectation de calcul et pas d'opération unaire lors d'une affectation
  b++ = 2; // pas d'affectation de calcul et pas d'opération unaire lors d'une affectation
  -b = 2; // pas d'affectation de calcul et pas d'opération unaire lors d'une affectation
  -a[2] = 2; // pas d'affectation de calcul et pas d'opération unaire lors d'une affectation
  b = -b++ * a[1];
}
*/

//Test variables/fonctions non déclarées :

/*
void test(){
  a = 1;
  b();
  c[2];
  d(e);
}
*/

//Test variables non initialisée :

/* 
void test(int a, int b){}


void test(){
  int a,b,c;
  a = b;
  int(b,a); // ne produit pas d'erreur
  int(b,c); // ne produit une erreur que pour c 
  //dans une fonction, l'initialisation du premier paramètre n'est jamais vérifié
}
*/

//Test erreur de typage de la fonction

/*
void test1(){}

void test2(int b){}

void test(){
  int a,c;
  a = test1(a);
  test1(a);
  test1();
  test2(a);
  test2();
}
*/

//Test accès au tableau par un entier

/*
float test1(){}

void test(){
  int a[2];
  a[3+4];
  a[test1()];
  a[2];
}
*/

//Test la variable est-elle un tableau

/*
void test(){
  int c[2];
  int a[10],b;
  b[2];
  a[2];
  c[2];
}
*/

//Test Pas de post-décrémentation d'un tableau

/*
void test(){
  int a[1];
  a++;
  a++[2]; //utile à bloquer ? en tout cas ça l'est
  test--;
}
*/

//Test Pas de tableaux à plusieurs dimensions

/*
void test(){
  int a[10];
  a[2][2];
}
*/

//Types incompatibles

/*
void test(){
  int a;
  float b;
  a = 1.;
  a < b;
  a > b;
  a <= b;
  a >= b;
  a == b;
  a != b;
  a = b;
  a += b;
  a -= b;
  a *= b;
  a - b ;
  a + b;
  a * b;
  a / b;
}
*/

// Pas d'affectation de constante

/*
void test(){
  int a;
  0 = a; // ne produit pas d'erreur d'initialisation de a, normal, car le 0 ne donne pas lieu au remplissage de store_name
}
*/

// Pas d'affectation de calcul

/*
int test1(){}

void test(){
  int a;
  test1() = a;
}
*/

// Une condition est requise ...

/*
void test(){
  int i;
  for(i = 0; i = 2; i++);
  for(i = 0; i; i++);
  for(i = 0;i<0; i++); //indication de la ligne d'erreur parfois décalée
  if(i++);
  if(12);else;
  while(i+4);
}
*/

// Redéfinition impossible

/*
void test(){}

void test(){
  int a,a;
}
*/

// Syntaxe incorrecte (plusieurs) -- bizarre, aussi repérées par yacc, mais pas à la bonne ligne

/*
void test(){
  int a[][3]; // erreur aussi gérée par yacc !
  int b[3][]; // idem
  int v()[3]; 
  int vo(int c)[];
  int d[][];
  int (f)[];
}
*/

//Interdit de définir une fonction dans une autre --bizarre, aussi repérées par yacc, mais pas à la bonne ligne

/*
void test(){
  int a(); //idem tests précédents
  int c(int b);
}
*/

// Erreur syntaxe des paramètres : n'autorise qu'entier ou flottants

/*
void test1(int a){}
void test2(int a[]){}
void test3(int a()){}
*/

// Type de retour incorrect

/*
void test1(){
  int a;
  a = 0;
  return a;
}

int test2(){
  int a;
  return test1();
  return a;
}
*/


// Type de retour manquant
/*
int test(){
}
*/
