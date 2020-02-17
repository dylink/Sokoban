#include <cstdio>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <list>
#include <termios.h>

static struct termios g_old_kbd_mode;

static void old_attr(void){
    tcsetattr(0, TCSANOW, &g_old_kbd_mode);
}

using namespace std;

///Nombre de lignes
#define L 100
///Nombre de colonnes
#define C 100

#define MOVE_U 0
#define MOVE_D 1
#define MOVE_L 2
#define MOVE_R 3

#define FREE 0
#define TARGET 1
#define WALL 2
#define CRATE_ON_FREE 3
#define CRATE_ON_TARGET 4
#define MAN_ON_FREE 5
#define MAN_ON_TARGET 6
#define EOL 7
enum move_t {up = MOVE_U, down = MOVE_D, l = MOVE_L, r = MOVE_R};

///Création d'un type "plateau" de taille C*L
typedef int plateau[L][C];

class Plateau {

public:
  vector<vector<uint>> plateau;
  uint man_pos[2];
  move_t moves;

  void loadPlateau(string file){
    this->plateau.clear();
    ifstream myfile (file, ifstream::in);
    char c = myfile.get();
    vector<uint> vect;
    if(!myfile.is_open()){
      cout << file << " : " << "This file does not exist or cannot be open.\n";
      exit(1);
    }
    while(myfile.good()){
      switch(c){
        case '\n': vect.push_back(EOL); this->plateau.push_back(vect); vect.clear(); break;
        case ' ': vect.push_back(FREE); break;
        case '#': vect.push_back(WALL); break;
        case '$': vect.push_back(CRATE_ON_FREE); break;
        case '.': vect.push_back(TARGET); break;
        case '*': vect.push_back(CRATE_ON_TARGET); break;
        case 'a': vect.push_back(MAN_ON_TARGET); this->man_pos[0] = this->plateau.size(); this->man_pos[1] = vect.size()-1; break;
        case '@': vect.push_back(MAN_ON_FREE); this->man_pos[0] = this->plateau.size(); this->man_pos[1] = vect.size()-1; break;
      }
      c = myfile.get();
    }
    myfile.close();
  }

  void affichePlateau(){
    for(uint i = 0; i<this->plateau.size(); i++){
      for(uint j = 0; j<this->plateau[i].size(); j++){
        switch(this->plateau[i][j]){
          case FREE: printf("  "); break;
          case WALL: printf("\033[38m☐ \033[0m"); break;
          case MAN_ON_FREE: printf("\033[33m☻ \033[0m"); break;
          case MAN_ON_TARGET: printf("\033[36m☻ \033[0m"); break;
          case CRATE_ON_FREE: printf("\033[31m☒ \033[0m"); break;
          case CRATE_ON_TARGET: printf("\033[32m☒ \033[0m"); break;
          case TARGET: printf("\033[32m☉ \033[0m"); break;
          case EOL: printf("\n"); break;
        }
      }
    }
  }

  bool can_move_U() {
    if(this->man_pos[0] <= 1) return false;
    if(this->plateau[this->man_pos[0]-1][this->man_pos[1]]==FREE || this->plateau[this->man_pos[0]-1][this->man_pos[1]]==TARGET) return true;
    if(this->man_pos[0] >= 1) {
      if(this->plateau[this->man_pos[0]-1][this->man_pos[1]]==CRATE_ON_FREE &&
         (this->plateau[this->man_pos[0]-2][this->man_pos[1]]==FREE || this->plateau[this->man_pos[0]-2][this->man_pos[1]]==TARGET)) return true;
      if(this->plateau[this->man_pos[0]-1][this->man_pos[1]]==CRATE_ON_TARGET &&
         (this->plateau[this->man_pos[0]-2][this->man_pos[1]]==FREE || this->plateau[this->man_pos[0]-2][this->man_pos[1]]==TARGET)) return true;
    }
    return false;
  }

  void move_U() {
    if(this->plateau[this->man_pos[0]-1][this->man_pos[1]]==FREE) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
      this->plateau[this->man_pos[0]-1][this->man_pos[1]] = MAN_ON_FREE;
      this->man_pos[0]-=1;
      return;
    }
    if(this->plateau[this->man_pos[0]-1][this->man_pos[1]]==TARGET) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
      this->plateau[this->man_pos[0]-1][this->man_pos[1]] = MAN_ON_TARGET;
      this->man_pos[0]-=1;
      return;
    }
    if(this->plateau[this->man_pos[0]-2][this->man_pos[1]]==FREE) {
      if(this->plateau[this->man_pos[0]-1][this->man_pos[1]]==CRATE_ON_FREE) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]-1][this->man_pos[1]]=MAN_ON_FREE;
        this->plateau[this->man_pos[0]-2][this->man_pos[1]]=CRATE_ON_FREE;
        this->man_pos[0]-=1;
        return;
      }
      if(this->plateau[this->man_pos[0]-1][this->man_pos[1]]==CRATE_ON_TARGET) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]-1][this->man_pos[1]]=MAN_ON_TARGET;
        this->plateau[this->man_pos[0]-2][this->man_pos[1]]=CRATE_ON_FREE;
        this->man_pos[0]-=1;
        return;
      }
    }
    if(this->plateau[this->man_pos[0]-2][this->man_pos[1]]==TARGET) {
      if(this->plateau[this->man_pos[0]-1][this->man_pos[1]]==CRATE_ON_FREE) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]-1][this->man_pos[1]]=MAN_ON_FREE;
        this->plateau[this->man_pos[0]-2][this->man_pos[1]]=CRATE_ON_TARGET;
        this->man_pos[0]-=1;
        return;
      }
      if(this->plateau[this->man_pos[0]-1][this->man_pos[1]]==CRATE_ON_TARGET) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]-1][this->man_pos[1]]=MAN_ON_TARGET;
        this->plateau[this->man_pos[0]-2][this->man_pos[1]]=CRATE_ON_TARGET;
        this->man_pos[0]-=1;
        return;
      }
    }
  }


  bool can_move_D() {
    if(this->man_pos[0] >= this->plateau.size()-1) return false;
    if(this->plateau[this->man_pos[0]+1][this->man_pos[1]]==FREE || this->plateau[this->man_pos[0]+1][this->man_pos[1]]==TARGET) return true;
    if(this->man_pos[0] <= this->plateau.size()-2) {
      if(this->plateau[this->man_pos[0]+1][this->man_pos[1]]==CRATE_ON_FREE &&
         (this->plateau[this->man_pos[0]+2][this->man_pos[1]]==FREE || this->plateau[this->man_pos[0]+2][this->man_pos[1]]==TARGET)) return true;
      if(this->plateau[this->man_pos[0]+1][this->man_pos[1]]==CRATE_ON_TARGET &&
         (this->plateau[this->man_pos[0]+2][this->man_pos[1]]==FREE || this->plateau[this->man_pos[0]+2][this->man_pos[1]]==TARGET)) return true;
    }
    return false;
  }

  void move_D() {
    if(this->plateau[this->man_pos[0]+1][this->man_pos[1]]==FREE) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
      this->plateau[this->man_pos[0]+1][this->man_pos[1]] = MAN_ON_FREE;
      this->man_pos[0]+=1;
      return;
    }
    if(this->plateau[this->man_pos[0]+1][this->man_pos[1]]==TARGET) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
      this->plateau[this->man_pos[0]+1][this->man_pos[1]] = MAN_ON_TARGET;
      this->man_pos[0]+=1;
      return;
    }
    if(this->plateau[this->man_pos[0]+2][this->man_pos[1]]==FREE) {
      if(this->plateau[this->man_pos[0]+1][this->man_pos[1]]==CRATE_ON_FREE) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]+1][this->man_pos[1]]=MAN_ON_FREE;
        this->plateau[this->man_pos[0]+2][this->man_pos[1]]=CRATE_ON_FREE;
        this->man_pos[0]+=1;
        return;
      }
      if(this->plateau[this->man_pos[0]+1][this->man_pos[1]]==CRATE_ON_TARGET) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]+1][this->man_pos[1]]=MAN_ON_TARGET;
        this->plateau[this->man_pos[0]+2][this->man_pos[1]]=CRATE_ON_FREE;
        this->man_pos[0]+=1;
        return;
      }
    }
    if(this->plateau[this->man_pos[0]+2][this->man_pos[1]]==TARGET) {
      if(this->plateau[this->man_pos[0]+1][this->man_pos[1]]==CRATE_ON_FREE) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]+1][this->man_pos[1]]=MAN_ON_FREE;
        this->plateau[this->man_pos[0]+2][this->man_pos[1]]=CRATE_ON_TARGET;
        this->man_pos[0]+=1;
        return;
      }
      if(this->plateau[this->man_pos[0]+1][this->man_pos[1]]==CRATE_ON_TARGET) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]+1][this->man_pos[1]]=MAN_ON_TARGET;
        this->plateau[this->man_pos[0]+2][this->man_pos[1]]=CRATE_ON_TARGET;
        this->man_pos[0]+=1;
        return;
      }
    }
  }

  bool can_move_L() {
    if(this->man_pos[1] <= 1) return false;
    if(this->plateau[this->man_pos[0]][this->man_pos[1]-1]==FREE || this->plateau[this->man_pos[0]][this->man_pos[1]-1]==TARGET) return true;
    if(this->man_pos[1] >= 1) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]-1]==CRATE_ON_FREE &&
         (this->plateau[this->man_pos[0]][this->man_pos[1]-2]==FREE || this->plateau[this->man_pos[0]][this->man_pos[1]-2]==TARGET)) return true;
      if(this->plateau[this->man_pos[0]][this->man_pos[1]-1]==CRATE_ON_TARGET &&
         (this->plateau[this->man_pos[0]][this->man_pos[1]-2]==FREE || this->plateau[this->man_pos[0]][this->man_pos[1]-2]==TARGET)) return true;
    }
    return false;
  }

  void move_L() {
    if(this->plateau[this->man_pos[0]][this->man_pos[1]-1]==FREE) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
      this->plateau[this->man_pos[0]][this->man_pos[1]-1] = MAN_ON_FREE;
      this->man_pos[1]-=1;
      return;
    }
    if(this->plateau[this->man_pos[0]][this->man_pos[1]-1]==TARGET) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
      this->plateau[this->man_pos[0]][this->man_pos[1]-1] = MAN_ON_TARGET;
      this->man_pos[1]-=1;
      return;
    }
    if(this->plateau[this->man_pos[0]][this->man_pos[1]-2]==FREE) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]-1]==CRATE_ON_FREE) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]-1]=MAN_ON_FREE;
        this->plateau[this->man_pos[0]][this->man_pos[1]-2]=CRATE_ON_FREE;
        this->man_pos[1]-=1;
        return;
      }
      if(this->plateau[this->man_pos[0]][this->man_pos[1]-1]==CRATE_ON_TARGET) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]-1]=MAN_ON_TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]-2]=CRATE_ON_FREE;
        this->man_pos[1]-=1;
        return;
      }
    }
    if(this->plateau[this->man_pos[0]][this->man_pos[1]-2]==TARGET) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]-1]==CRATE_ON_FREE) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]-1]=MAN_ON_FREE;
        this->plateau[this->man_pos[0]][this->man_pos[1]-2]=CRATE_ON_TARGET;
        this->man_pos[1]-=1;
        return;
      }
      if(this->plateau[this->man_pos[0]][this->man_pos[1]-1]==CRATE_ON_TARGET) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]-1]=MAN_ON_TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]-2]=CRATE_ON_TARGET;
        this->man_pos[1]-=1;
        return;
      }
    }
  }

  bool can_move_R() {
    if(this->man_pos[1] >= this->plateau[this->man_pos[0]].size()-1) return false;
    if(this->plateau[this->man_pos[0]][this->man_pos[1]+1]==FREE || this->plateau[this->man_pos[0]][this->man_pos[1]+1]==TARGET) return true;
    if(this->man_pos[1] <= this->plateau[this->man_pos[0]].size()-1) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]+1]==CRATE_ON_FREE &&
         (this->plateau[this->man_pos[0]][this->man_pos[1]+2]==FREE || this->plateau[this->man_pos[0]][this->man_pos[1]+2]==TARGET)) return true;
      if(this->plateau[this->man_pos[0]][this->man_pos[1]+1]==CRATE_ON_TARGET &&
         (this->plateau[this->man_pos[0]][this->man_pos[1]+2]==FREE || this->plateau[this->man_pos[0]][this->man_pos[1]+2]==TARGET)) return true;
    }
    return false;
  }

  void move_R() {
    if(this->plateau[this->man_pos[0]][this->man_pos[1]+1]==FREE) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
      this->plateau[this->man_pos[0]][this->man_pos[1]+1] = MAN_ON_FREE;
      this->man_pos[1]+=1;
      return;
    }
    if(this->plateau[this->man_pos[0]][this->man_pos[1]+1]==TARGET) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
      if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
      this->plateau[this->man_pos[0]][this->man_pos[1]+1] = MAN_ON_TARGET;
      this->man_pos[1]+=1;
      return;
    }
    if(this->plateau[this->man_pos[0]][this->man_pos[1]+2]==FREE) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]+1]==CRATE_ON_FREE) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]+1]=MAN_ON_FREE;
        this->plateau[this->man_pos[0]][this->man_pos[1]+2]=CRATE_ON_FREE;
        this->man_pos[1]+=1;
        return;
      }
      if(this->plateau[this->man_pos[0]][this->man_pos[1]+1]==CRATE_ON_TARGET) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]+1]=MAN_ON_TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]+2]=CRATE_ON_FREE;
        this->man_pos[1]+=1;
        return;
      }
    }
    if(this->plateau[this->man_pos[0]][this->man_pos[1]+2]==TARGET) {
      if(this->plateau[this->man_pos[0]][this->man_pos[1]+1]==CRATE_ON_FREE) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]+1]=MAN_ON_FREE;
        this->plateau[this->man_pos[0]][this->man_pos[1]+2]=CRATE_ON_TARGET;
        this->man_pos[1]+=1;
        return;
      }
      if(this->plateau[this->man_pos[0]][this->man_pos[1]+1]==CRATE_ON_TARGET) {
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_FREE) this->plateau[this->man_pos[0]][this->man_pos[1]]=FREE;
        if(this->plateau[this->man_pos[0]][this->man_pos[1]]==MAN_ON_TARGET) this->plateau[this->man_pos[0]][this->man_pos[1]]=TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]+1]=MAN_ON_TARGET;
        this->plateau[this->man_pos[0]][this->man_pos[1]+2]=CRATE_ON_TARGET;
        this->man_pos[1]+=1;
        return;
      }
    }
  }

  void play(move_t direction){
    if(direction==MOVE_U && can_move_U()){ move_U();}
    if(direction==MOVE_D && can_move_D()){ move_D();}
    if(direction==MOVE_L && can_move_L()){ move_L();}
    if(direction==MOVE_R && can_move_R()){ move_R();}
  }

  void unplay(vector<vector<uint>> &plateau){
    this->plateau.clear();
    this->plateau = plateau;
  }

  list<move_t> nextMoves() {
    move_t moves;
    list <move_t> possibilities;
    if(can_move_U()){ moves = up; possibilities.push_back(moves);}
    if(can_move_L()){ moves = l; possibilities.push_back(moves);}
    if(can_move_R()){ moves = r; possibilities.push_back(moves);}
    if(can_move_D()){ moves = down; possibilities.push_back(moves);}
    return possibilities;
  }

  void DFS(int profondeur){
    if (profondeur==0){
      return;
    }
    list<move_t> moves = nextMoves();
    vector<vector<uint>> copiePlateau = this->plateau;
    for(auto i : moves) {
      printf("Profondeur == %d\n", profondeur);
      cout << i << endl;
      /*play(i);
      affichePlateau();
      DFS(profondeur - 1);
      unplay(copiePlateau);*/
    }
    return;
  }

  void jeu(){
    move_t move;
    while(true){
      static char init;
      struct termios new_kbd_mode;

      if(init)
          return;
      tcgetattr(0, &g_old_kbd_mode);
      memcpy(&new_kbd_mode, &g_old_kbd_mode, sizeof(struct termios));

      new_kbd_mode.c_lflag &= ~(ICANON | ECHO);
      new_kbd_mode.c_cc[VTIME] = 0;
      new_kbd_mode.c_cc[VMIN] = 1;
      tcsetattr(0, TCSANOW, &new_kbd_mode);
      atexit(old_attr);

      char saisie = getchar();

      switch(saisie){
        case 'z': move = up; play(move); break;
        case 's': move = down; play(move); break;
        case 'q': move = l; play(move); break;
        case 'd': move = r; play(move); break;
      }
      system("clear");
      affichePlateau();
    }
  }

  bool blocked (){
    for(uint i = 0; i<this->plateau.size(); i++){
      for(uint j = 0; j<this->plateau[i].size(); j++){
      /*CRATE_ON_FREE dans un coin de mur */
        if(this->plateau[i][j] == CRATE_ON_FREE){
          if(   ( (this->plateau[this->man_pos[0]-1][this->man_pos[1]]==WALL)  && (this->plateau[this->man_pos[0]][this->man_pos[1]+1]==WALL) )  ||

                ( (this->plateau[this->man_pos[0]-1][this->man_pos[1]]==WALL)  && (this->plateau[this->man_pos[0]][this->man_pos[1]-1]==WALL) ) ||

                ( (this->plateau[this->man_pos[0]+1][this->man_pos[1]]==WALL)  && (this->plateau[this->man_pos[0]][this->man_pos[1]-1]==WALL) ) ||

                ( (this->plateau[this->man_pos[0]+1][this->man_pos[1]]==WALL) && (this->plateau[this->man_pos[0]][this->man_pos[1]+1]==WALL)  ) ) {
                return true;
          }
        }
      }
    }
    return false;
  }




};
