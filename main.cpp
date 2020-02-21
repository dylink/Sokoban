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
  //board.DFS();
  board.IDS();
  return 0;
}
