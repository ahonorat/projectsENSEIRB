#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "concat.h"

struct tree_el {char *type; int flag_init; char *reg;};
struct tree {char c; struct tree *next; struct tree *level_down; struct tree_el *id;};

struct tree_el *whatId(struct tree *h, char *str){
  int i = 0;
  struct tree *temp;
  temp = h;
  if (str != NULL) {
    while (str[i] != '\0' && temp != NULL){
      if (str[i] == temp->c){
	if (str[i+1] == '\0')
	  return temp->id;
	else if (str[i] == temp->c) {
	  i++;
	  temp = temp->level_down;
	} else
	  temp = temp->next;
      } else
	temp = temp->next;
    }
  }
  return NULL;
}

int existId(struct tree *h, char *str){
  return (whatId(h, str) != NULL);
}

int addId(struct tree *h, char *str){
  int i = 0;
  struct tree *temp;
  temp = h;
  if (str != NULL) {
    while (str[i] != '\0' && temp != NULL){
      if (str[i] == temp->c){
	if (str[i+1] == '\0'){
	  if (temp->id == NULL) {
	    temp->id = malloc(sizeof(struct tree_el));
	    temp->id->type = NULL;
	    temp->id->flag_init = 0; // pas initialisé
	    temp->id->reg = pointeur(); //insérer la récupération d'un nouveau regi
	    return 1;
	  } else 
	    return 0;
	} else {
	  if (temp->level_down == NULL) {
	    temp->level_down = malloc(sizeof(struct tree));
	    temp = temp->level_down;
	    temp->next = NULL;
	    temp->id = NULL;
	    temp->level_down = NULL;
	    temp->c = str[i];
	    i++;
	  } else {
	    temp = temp->level_down;
	    i++;
	  }
	}
      } else {
	if (temp->next == NULL) {
	  temp->next = malloc(sizeof(struct tree));
	  temp = temp->next;
	  temp->next = NULL;
	  temp->id = NULL;
	  temp->level_down = NULL;
	  temp->c = str[i];
	} else
	  temp = temp->next;
      }
    }
  }
  return 0;
}

int modifyId(struct tree *h, char *str, char *type, int init){
  struct tree_el *temp = whatId(h, str);
  if (temp != NULL) {
    if (type != NULL){
      if (temp->type != NULL)
	free(temp->type);
      temp->type = type;
    }
    temp->flag_init = init;
    return 1;
  }
  return 0;
}

int modifyReg(struct tree *h, char *str, char *reg){
  struct tree_el *temp = whatId(h, str);
  if (temp != NULL) {
    if (reg != NULL){
      if (temp->type != NULL)
	free(temp->reg);
      temp->reg = reg;
    }
    return 1;
  }
  return 0;
}

char *typeId(struct tree *h, char *str){
  struct tree_el *temp = whatId(h, str);
  if (temp != NULL) {
    return temp->type;
  }
  return "erreur";
}

int initId(struct tree *h, char *str){
  struct tree_el *temp = whatId(h, str);
  if (temp != NULL) {
    return temp->flag_init;
  }
  return 0;
}

char *regId(struct tree *h, char *str){
  struct tree_el *temp = whatId(h, str);
  if (temp != NULL) {
    return temp->reg;
  }
  return "erreur";
}

int init_var(struct tree *h){
  if (h != NULL){
    addId(h,"$x");
    modifyId(h, "$x", strdup("f"), 1);
    modifyReg(h, "$x", strdup("$x"));
    addId(h,"$y");
    modifyId(h, "$y", strdup("f"), 1);
    modifyReg(h, "$y", strdup("$y"));
    addId(h,"$z");
    modifyId(h, "$z", strdup("f"), 1);
    modifyReg(h, "$z", strdup("$z"));
    addId(h,"$accel");
    modifyId(h, "$accel", strdup("f"), 1);
    modifyReg(h, "$accel", strdup("$accel"));
    addId(h,"$accelx");
    modifyId(h, "$accelx", strdup("f"), 1);
    modifyReg(h, "$accelx", strdup("$accelx"));
    addId(h,"$accely");
    modifyId(h, "$accely", strdup("f"), 1);
    modifyReg(h, "$accely", strdup("$accely"));
    addId(h,"$accelz");
    modifyId(h, "$accelz", strdup("f"), 1);
    modifyReg(h, "$accelz", strdup("$accelz"));
    addId(h,"$speedx");
    modifyId(h, "$speedx", strdup("f"), 1);
    modifyReg(h, "$speedx", strdup("$speedx"));
    addId(h,"$speedy");
    modifyId(h, "$speedy", strdup("f"), 1);
    modifyReg(h, "$speedy", strdup("$speedy"));
    addId(h,"$speedz");
    modifyId(h, "$speedz", strdup("f"), 1);
    modifyReg(h, "$speedz", strdup("$speedz"));
    addId(h,"$rpm");
    modifyId(h, "$rpm", strdup("f"), 1);
    modifyReg(h, "$rpm", strdup("$rpm"));
    addId(h,"$steer");
    modifyId(h, "$steer", strdup("f"), 1);
    modifyReg(h, "$steer", strdup("$steer"));
    addId(h,"$brake");
    modifyId(h, "$brake", strdup("f"), 1);
    modifyReg(h, "$brake", strdup("$brake"));
    addId(h,"$clutch");
    modifyId(h, "$clutch", strdup("f"), 1);
    modifyReg(h, "$clutch", strdup("$clutch"));
    addId(h,"$gear");
    modifyId(h, "$gear", strdup("i"), 1);
    modifyReg(h, "$gear", strdup("$gear"));
    return 1;
  }
  return 0;
}

int init_func(struct tree *h){
  if (h != NULL){
    addId(h,"norm_pi_pi");
    modifyId(h, "norm_pi_pi", strdup("ff"), 1);
    addId(h,"get_track_angle");
    modifyId(h, "get_track_angle", strdup("fv"), 1);
    addId(h,"get_pos_to_right");
    modifyId(h, "get_pos_to_right", strdup("fv"), 1);
    addId(h,"get_pos_to_middle");
    modifyId(h, "get_pos_to_middle", strdup("fv"), 1);
    addId(h,"get_pos_to_left");
    modifyId(h, "get_pos_to_left", strdup("fv"), 1);
    addId(h,"get_pos_to_start");
    modifyId(h, "get_pos_to_start", strdup("fv"), 1);
    addId(h,"get_track_seg_length");
    modifyId(h, "get_track_seg_length", strdup("fv"), 1);
    addId(h,"get_track_seg_width");
    modifyId(h, "get_track_seg_width", strdup("fv"), 1);
    addId(h,"get_track_seg_start_width");
    modifyId(h, "get_track_seg_start_width", strdup("fv"), 1);
    addId(h,"get_track_seg_end_width");
    modifyId(h, "get_track_seg_end_width", strdup("fv"), 1);
    addId(h,"get_track_seg_radius");
    modifyId(h, "get_track_seg_radius", strdup("fv"), 1);
    addId(h,"get_track_seg_right_radius");
    modifyId(h, "get_track_seg_right_radius", strdup("fv"), 1);
    addId(h,"get_track_seg_left_radius");
    modifyId(h, "get_track_seg_left_radius", strdup("fv"), 1);
    addId(h,"get_track_seg_arc");
    modifyId(h, "get_track_seg_arc", strdup("fv"), 1);
    addId(h,"get_car_yaw");
    modifyId(h, "get_car_yaw", strdup("fv"), 1);

    //addId(h,"get_track_seg_next");
    //modifyId(h, "get_track_seg_next", strdup("???"), 1);

    return 1;
  }
  return 0;
}

struct tree *init_tree(){
  struct tree *temp = malloc(sizeof(struct tree));
  temp->next = NULL;
  temp->id = NULL;
  temp->level_down = NULL;
  temp->c = '$';
  return temp;
}


void delete_tree(struct tree *h){
  if (h->level_down != NULL)
    delete_tree(h->level_down);
  if (h->next != NULL)
    delete_tree(h->next);
  if (h->id != NULL){
    if (h->id->type != NULL)
      free(h->id->type);
    if (h->id->reg != NULL)
      free(h->id->reg);
    free(h->id);
  }
  free(h);
}
