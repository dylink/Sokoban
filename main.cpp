#include "plateau.hpp"




int main(int argc, char * argv[]){
  if(argc < 2){
    printf("Usage : %s SOK_FILE\n", argv[0]);
    return 0;
  }
  Plateau board;
  board.loadPlateau(argv[1]);
  board.affichePlateau();
  //board.jeu();

  //board.move_L();
  /*board.affichePlateau();
  printf("%d\n", board.can_move_U());*/
  //board.DFS(6);
  /*vector<vector<uint>> plateau;
  plateau = board.plateau;
  board.moves = up;
  board.play(board.moves);
  board.affichePlateau();
  board.unplay(plateau);
  board.affichePlateau();*/
  return 0;
}
