#ifndef TESTSOK_H
#define TESTSOK_H
#include "plateau.hpp"

Plateau board;

/* position : ...#
 *            ...@
 * can move_U() return false */
bool test_p001() {
  board.loadPlateau("Maps/sok-01.txt");
  return !board.can_move_U();
}

/* position : ...#
 *            ...a
 * can move_U() return false */
bool test_p002() {
  board.loadPlateau("Maps/sok-02.txt");
  return !board.can_move_U();
}

/* position : ...#
 *            ...
 *            ...@
 * move_U() modifie la position : ...#
 *                                ...@
 *                                ...
 * can move_U() return true */
bool test_p003() {
  board.loadPlateau("Maps/sok-03.txt");
  if(!board.can_move_U()) return false;
  board.move_U();
  if(board.plateau[1][0]!=MAN_ON_FREE) return false;
  if(board.plateau[2][0]!=FREE) return false;
  if(board.man_pos[0]!=1) return false;
  return true;
}

/* position : 04U
              ...#
 *            ...
 *            ...a
 move_U() modifie la position : ...#
 *                              ...@
 *                              ...
 * can move_U() return true */
 bool test_p004() {
   board.loadPlateau("Maps/sok-04.txt");
   if(!board.can_move_U()) return false;
   board.move_U();
   if(board.plateau[1][0]!=MAN_ON_FREE) return false;
   if(board.plateau[2][0]!=TARGET) return false;
   if(board.man_pos[0]!=1) return false;
   return true;

 }

/* position : 05U
              ...
              ...$
              ...@
  move_U() modifie la position : ...$
  *                              ...@
  *                              ...
* can move_U() return  true*/
bool test_p005() {
  board.loadPlateau("Maps/sok-05.txt");
  if(!board.can_move_U()) return false;
  board.move_U();
  if(board.plateau[0][0]!=CRATE_ON_FREE) return false;
  if(board.plateau[1][0]!=MAN_ON_FREE) return false;
  if(board.plateau[2][0]!=FREE) return false;
  if(board.man_pos[0]!=1) return false;
  return true;
}

/* position : 06U
              ...
              ...$
              ...a
  move_U() modifie la position : ...$
  *                              ...@
  *                              ...
* can move_U() return true*/
bool test_p006() {
  board.loadPlateau("Maps/sok-06.txt");
  if(!board.can_move_U()) return false;
  board.move_U();
  if(board.plateau[0][0]!=CRATE_ON_FREE) return false;
  if(board.plateau[1][0]!=MAN_ON_FREE) return false;
  if(board.plateau[2][0]!=TARGET) return false;
  if(board.man_pos[0]!=1) return false;
  return true;
}

/* position : 07U
              ...
              ...*
              ...@
  move_U() modifie la position : ...$
  *                              ...a
  *                              ...
* can move_U() return  true*/
bool test_p007() {
  board.loadPlateau("Maps/sok-07.txt");
  if(!board.can_move_U()) return false;
  board.move_U();
  if(board.plateau[0][0]!=CRATE_ON_FREE) return false;
  if(board.plateau[1][0]!=MAN_ON_TARGET) return false;
  if(board.plateau[2][0]!=FREE) return false;
  if(board.man_pos[0]!=1) return false;
  return true;
}

/* position : 08U
              ...
              ...*
              ...a
move_U() modifie la position : ...$
*                              ...a
*                              ...
* can move_U() return true*/
bool test_p008() {
  board.loadPlateau("Maps/sok-08.txt");
  if(!board.can_move_U()) return false;
  board.move_U();
  if(board.plateau[0][0]!=CRATE_ON_FREE) return false;
  if(board.plateau[1][0]!=MAN_ON_TARGET) return false;
  if(board.plateau[2][0]!=TARGET) return false;
  if(board.man_pos[0]!=1) return false;
  return true;
}

/* position : 09U
              ...#
              ...$
              ...@
* can move_U() return false */
bool test_p009() {
  board.loadPlateau("Maps/sok-09.txt");
  return (!board.can_move_U());
}

/* position : 010U
              ...#
              ...$
              ...a
* can move_U() return false */
bool test_p010() {
  board.loadPlateau("Maps/sok-10.txt");
  return (!board.can_move_U());
}

/* position : 011U
              ...#
              ...*
              ...@
* can move_U() return false */
bool test_p011() {
  board.loadPlateau("Maps/sok-11.txt");
  return (!board.can_move_U());
}

/* position : 012U
              ...#
              ...*
              ...a
* can move_U() return false */
bool test_p012() {
  board.loadPlateau("Maps/sok-12.txt");
  return (!board.can_move_U());
}

#endif

int main(int _ac, char** _av) {
  int nb_test_ok = 0;
  if(!test_p001()) printf("test_p001 FAILED\n");
  else nb_test_ok++;
  //...
  if(!test_p002()) printf("test_p002 FAILED\n");
  else nb_test_ok++;
  //...
  if(!test_p003()) printf("test_p003 FAILED\n");
  else nb_test_ok++;
  //...
  if(!test_p004()) printf("test_p004 FAILED\n");
  else nb_test_ok++;
  //...
  if(!test_p005()) printf("test_p005 FAILED\n");
  else nb_test_ok++;
  //...
  if(!test_p006()) printf("test_p006 FAILED\n");
  else nb_test_ok++;
  //...
  if(!test_p007()) printf("test_p007 FAILED\n");
  else nb_test_ok++;
  //...
  if(!test_p008()) printf("test_p008 FAILED\n");
  else nb_test_ok++;
  //...
  if(!test_p009()) printf("test_p009 FAILED\n");
  else nb_test_ok++;
  //...
  if(!test_p010()) printf("test_p010 FAILED\n");
  else nb_test_ok++;
  //...
  if(!test_p011()) printf("test_p011 FAILED\n");
  else nb_test_ok++;
  //...
  if(!test_p012()) printf("test_p012 FAILED\n");
  else nb_test_ok++;
  //...
  printf("nb_test_ok: %d\n", nb_test_ok);
  return 0;
}
