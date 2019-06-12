#ifndef __TREE_H_
#define __TREE_H_

struct tree;

int existId(struct tree *h, char *str);
int addId(struct tree *h, char *str);
int modifyId(struct tree *h, char *str, char *type, int init);
int modifyReg(struct tree *h, char *str, char *reg);
char *typeId(struct tree *h, char *str);
int init_var(struct tree *h);
int init_func(struct tree *h);
int initId(struct tree *h, char *str);
char *regId(struct tree *h, char *str);


struct tree *init_tree();
void delete_tree(struct tree *h);

#endif
