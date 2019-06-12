#include "concat.h"

int nbLabels=0;
int nbReg=0;
int nbCond=0;
int nbPtr=0;

//concatène la str2 à str1, la mémoire de str1 est réallouée, celle de str2 détruite
//str1 et 2 doivent être différents et initialisés



char *concat(char *str1,char *str2){
  if (str1 != str2 && str1 != NULL && str2 != NULL){
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if (len1 == 0)
      str1 = str2;
    else {
      str1 = realloc(str1,len1+len2+1);
      str1 = strcat(str1,str2);
      free(str2);
    }
    return str1;
  }
  return NULL;
}

char * label(){
  nbLabels++;
  char *temp=strdup("l");
  char * num=malloc(12*sizeof(char));
  snprintf(num, 11, "%d", nbLabels);
  temp=concat(temp, num);
  return temp;
}

char * registre(){
  nbReg++;
  char *temp=strdup("r");
  char * num=malloc(12*sizeof(char));
  snprintf(num, 11, "%d", nbReg);
  temp=concat(temp, num);
  return temp;
}

char * condition(){
  nbCond++;
  char *temp=strdup("c");
  char * num=malloc(12*sizeof(char));
  snprintf(num, 11, "%d", nbCond);
  temp=concat(temp, num);
  return temp;
}

char * pointeur(){
  nbPtr++;
  char *temp=strdup("p");
  char * num=malloc(12*sizeof(char));
  snprintf(num, 11, "%d", nbPtr);
  temp=concat(temp, num);
  return temp;
}

char *entete_func(){
  char *temp;
  temp = strdup("; variables servant a definir plus loin les variables utilisees par le robot\n");
  temp =  concat(temp,strdup("%ctrl = getelementptr %struct.CarElt* %car, i32 0, i32 5\n"));
  temp =  concat(temp,strdup("%public_car = getelementptr %struct.CarElt* %car, i32 0, i32 2\n"));
  temp =  concat(temp,strdup("%private_car = getelementptr %struct.CarElt* %car, i32 0, i32 4\n"));
  temp =  concat(temp,strdup("%pos = getelementptr %struct.tPublicCar* %public_car, i32 0, i32 3\n"));
  temp =  concat(temp,strdup("%dyngc  = getelementptr %struct.tPublicCar* %public_car, i32 0, i32 0\n"));
  temp =  concat(temp,strdup("%seg.addr = getelementptr %struct.tTrkLocPos* %pos, i32 0, i32 0\n"));
  temp =  concat(temp,strdup("%seg = load %struct.trackSeg** %seg.addr\n"));
  temp =  concat(temp,strdup("%posd_pos = getelementptr %struct.tDynPt* %dyngc, i32 0, i32 0\n"));
  temp =  concat(temp,strdup("%posd_vel = getelementptr %struct.tDynPt* %dyngc, i32 0, i32 1\n"));
  temp =  concat(temp,strdup("%posd_acc = getelementptr %struct.tDynPt* %dyngc, i32 0, i32 2\n"));
  temp =  concat(temp,strdup("%$x = getelementptr %struct.tPosd* %posd_pos, i32 0, i32 0\n"));
  temp =  concat(temp,strdup("%$y = getelementptr %struct.tPosd* %posd_pos, i32 0, i32 1\n"));
  temp =  concat(temp,strdup("%$z = getelementptr %struct.tPosd* %posd_pos, i32 0, i32 2\n"));
  temp =  concat(temp,strdup("%$speedx = getelementptr %struct.tPosd* %posd_vel, i32 0, i32 0\n"));
  temp =  concat(temp,strdup("%$speedy = getelementptr %struct.tPosd* %posd_vel, i32 0, i32 1\n"));
  temp =  concat(temp,strdup("%$speedz = getelementptr %struct.tPosd* %posd_vel, i32 0, i32 2\n"));
  temp =  concat(temp,strdup("%$accelx = getelementptr %struct.tPosd* %posd_acc, i32 0, i32 0\n"));
  temp =  concat(temp,strdup("%$accely = getelementptr %struct.tPosd* %posd_acc, i32 0, i32 1\n"));
  temp =  concat(temp,strdup("%$accelz = getelementptr %struct.tPosd* %posd_acc, i32 0, i32 2\n"));
  temp =  concat(temp,strdup("%$rpm = getelementptr %struct.tPrivCar* %private_car, i32 8\n"));

  temp =  concat(temp,strdup("; variables utilisees en ECRITURE par le robot\n"));
  temp = concat(temp,strdup("%$steer = getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 0\n"));
  temp = concat(temp,strdup("%$accel = getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 1\n"));
  temp = concat(temp,strdup("%$brake = getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 2\n"));
  temp = concat(temp,strdup("%$clutch = getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 3\n"));
  temp = concat(temp,strdup("%$gear = getelementptr %struct.tCarCtrl* %ctrl, i32 0, i32 4\n"));
  return temp;
}

char *params_func(char *func){
  if (strcmp(func, "norm_pi_pi") == 0)
    return strdup(" ");
  else if (strcmp(func, "get_track_angle") == 0)
    return strdup("%struct.tTrkLocPos* %pos");
  else if (strcmp(func, "get_pos_to_right") == 0)
    return strdup("%struct.tTrkLocPos* %pos");
  else if (strcmp(func, "get_pos_to_middle") == 0)
    return strdup("%struct.tTrkLocPos* %pos");
  else if (strcmp(func, "get_pos_to_left") == 0)
    return strdup("%struct.tTrkLocPos* %pos");
  else if (strcmp(func, "get_pos_to_start") == 0)
    return strdup("%struct.tTrkLocPos* %pos");
  else if (strcmp(func, "get_track_seg_length") == 0)
    return strdup("%struct.trackSeg* %seg");
  else if (strcmp(func, "get_track_seg_width") == 0)
    return strdup("%struct.trackSeg* %seg");
  else if (strcmp(func, "get_track_seg_start_width") == 0)
    return strdup("%struct.trackSeg* %seg");
  else if (strcmp(func, "get_track_seg_end_width") == 0)
    return strdup("%struct.trackSeg* %seg");
  else if (strcmp(func, "get_track_seg_radius") == 0)
    return strdup("%struct.trackSeg* %seg");
  else if (strcmp(func, "get_track_seg_right_radius") == 0)
    return strdup("%struct.trackSeg* %seg");
  else if (strcmp(func, "get_track_seg_left_radius") == 0)
    return strdup("%struct.trackSeg* %seg");
  else if (strcmp(func, "get_track_seg_arc") == 0)
    return strdup("%struct.trackSeg* %seg");
  else if (strcmp(func, "get_car_yaw") == 0)
    return strdup("%struct.CarElt* %car");
  else 
    return strdup("i32 %index, %struct.CarElt* %car, %struct.Situation* %s");
}


