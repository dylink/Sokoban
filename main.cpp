#include "plateau.hpp"


int main(int argc, char * argv[]){
  if(argc < 2){
    printf("Usage : %s SOK_FILE\n", argv[0]);
    return 0;
  }
  Plateau board;
  board.loadPlateau(argv[1]);
  board.affichePlateau();
  /*vector<vector<uint>> plateau;
  plateau = board.plateau;
  move_t moves;
  moves = up;
  //printf("%d && %d\n", board.man_pos[0], board.man_pos[1]);
  board.play(moves);
  //board.move_R();
  printf("%d && %d\n", board.man_pos[0], board.man_pos[1]);
  //printf("%d && %d\n", board.man_pos[0], board.man_pos[1]);
  board.unplay(moves);
  printf("%d && %d\n", board.man_pos[0], board.man_pos[1]);*/
  //board.jeu();
  //board.move_L();
  /*board.affichePlateau();
  printf("%d\n", board.can_move_U());*/
  /*move_t t;
  t = none;
  board.DFS(12, t);
  cout << board.count << "\n";
  move_t move = none;
  while(!board.finJeu()){
    board.play(board.bestMove(move));
    cout << board.bestMove(move) << endl;
    move = board.bestMove(move);
  }*/
  move_t t = none;
  cout << "Trouvé!\n";
  printf("%d\n", board.bestMove(none));
  board.play(board.bestMove(none));
  printf("%d\n", board.bestMove(none));
  board.play(board.bestMove(none));
  printf("%d\n", board.bestMove(none));
  board.play(board.bestMove(none));
  printf("%d\n", board.bestMove(none));
  board.play(board.bestMove(none));
  printf("%d\n", board.bestMove(none));
  board.play(board.bestMove(none));
  printf("%d\n", board.bestMove(none));
  //cout << board.count << endl;
  /*vector<vector<uint>> plateau;
  plateau = board.plateau;
  board.moves = up;
  board.play(board.moves);
  board.affichePlateau();
  board.unplay(plateau);
  board.affichePlateau();*/
  return 0;
}
