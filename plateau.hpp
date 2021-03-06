#include <cstdio>
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <list>
#include <termios.h>
#include <set>
#include <cmath>
#include <utility>
#include <functional>

static struct termios g_old_kbd_mode;

static void old_attr(void){
    tcsetattr(0, TCSANOW, &g_old_kbd_mode);
}

using namespace std;

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
enum move_t {up = MOVE_U, down = MOVE_D, l = MOVE_L, r = MOVE_R, none = -1};
int profondeur_max = 200;

class Plateau {

public:
  vector<vector<uint>> plateau;
  uint man_pos[2];
  int crateNumber = 0;
  int count = 0;
  bool found = false;
  set<size_t> parcours;
  set<pair<int, int>> chain;

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
        case '$': vect.push_back(CRATE_ON_FREE); this->crateNumber++; break;
        case '.': vect.push_back(TARGET); break;
        case '*': vect.push_back(CRATE_ON_TARGET); this->crateNumber++; break;
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
    if(direction==MOVE_U && can_move_U()) move_U();
    if(direction==MOVE_D && can_move_D()) move_D();
    if(direction==MOVE_L && can_move_L()) move_L();
    if(direction==MOVE_R && can_move_R()) move_R();
  }

  void unplay(vector<vector<uint>> copiePlateau, move_t direction){
    if(direction==MOVE_U) move_D();
    if(direction==MOVE_D) move_U();
    if(direction==MOVE_L) move_R();
    if(direction==MOVE_R) move_L();
    this->plateau = copiePlateau;
  }

  list<move_t> nextMoves() {
    move_t moves;
    list <move_t> possibilities;
    if(can_move_U()){ moves = up; possibilities.push_back(moves);}
    if(can_move_R()){ moves = r; possibilities.push_back(moves);}
    if(can_move_L()){ moves = l; possibilities.push_back(moves);}
    if(can_move_D()){ moves = down; possibilities.push_back(moves);}
    return possibilities;
  }

  void displayArbre(vector<move_t> graphe){
    for(uint i = 0; i<graphe.size(); i++) cout << "[" << graphe[i] << "]-->";
    cout << "\b\b\b;  \n";
  }

  void displayMoves(list<move_t> moves){
    for(auto i : moves) cout << i << " ";
    cout << endl;
  }

  void displaySet(set<pair<int, int>> moves){
    for(auto i : moves) cout << "[" << i.first << ", " << i.second << "]-->";
    cout << "\b\b\b;  \n";
  }

  bool isIn(size_t vect){
    for(auto i : this->parcours) if(i == vect) return true;
    return false;
  }

  bool isInSet(set<pair<int, int>> &list, pair<int, int> &a){
    for(auto i : list) if(i == a) return true;
    return false;
  }

  size_t vecHash(vector<vector<uint>> & vec) const {
    size_t seed = vec.size();
    for(auto& i : vec)
      for(auto& j : i) seed ^= j + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }

  bool cratesBlock(int i, int j){
    pair<int, int> a;
    a.first = i;
    a.second = j;
    int man_pos[2];
    man_pos[0] = this->man_pos[0];
    man_pos[1] = this->man_pos[1];
    for(int x = -1; x<2; x+=2){
      if((this->plateau[i][j+x] == FREE || (this->plateau[i][j+x] == MAN_ON_FREE || this->plateau[i][j+x] == MAN_ON_TARGET)) && (this->plateau[i][j-x] != MAN_ON_FREE && this->plateau[i][j-x] != MAN_ON_TARGET)){
        this->man_pos[0] = i; this->man_pos[1] = j+x;
        if(x==1 && can_move_L()){
          this->man_pos[0] = man_pos[0];
          this->man_pos[1] = man_pos[1];
          this->chain.erase(a);
          return false;
        }
        else if(x==-1 && can_move_R()){
          this->man_pos[0] = man_pos[0];
          this->man_pos[1] = man_pos[1];
          this->chain.erase(a);
          return false;
        }
        this->man_pos[0] = man_pos[0];
        this->man_pos[1] = man_pos[1];
      }
      if(this->plateau[i][j+x] == CRATE_ON_FREE || this->plateau[i][j+x] == CRATE_ON_TARGET) {
        a.first = i;
        a.second = j+x;
        if(!isInSet(this->chain, a)){
          this->chain.insert(a);
          if(!cratesBlock(i, j+x)) {
            this->chain.erase(a);
            return false;
          }
        }
      }
      if((this->plateau[i+x][j] == FREE || (this->plateau[i+x][j] == MAN_ON_FREE || this->plateau[i+x][j] == MAN_ON_TARGET)) && (this->plateau[i-x][j] != MAN_ON_FREE && this->plateau[i-x][j] != MAN_ON_TARGET)){
        this->man_pos[0] = i+x; this->man_pos[1] = j;

        if(x==1 && can_move_U()){
          this->man_pos[0] = man_pos[0];
          this->man_pos[1] = man_pos[1];
          this->chain.erase(a);
          return false;
        }
        if(x==-1 && can_move_D()){
          this->man_pos[0] = man_pos[0];
          this->man_pos[1] = man_pos[1];
          this->chain.erase(a);
          return false;
        }
        this->man_pos[0] = man_pos[0];
        this->man_pos[1] = man_pos[1];
      }
      if(this->plateau[i+x][j] == CRATE_ON_FREE || this->plateau[i+x][j] == CRATE_ON_TARGET) {
        a.first = i+x;
        a.second = j;
        if(!isInSet(this->chain, a)){
          this->chain.insert(a);
          if(!cratesBlock(i+x, j)) {
            this->chain.erase(a);
            return false;
          }
        }
      }
    }
    return true;
  }

  bool wallCheck(int i, int j){
    int walls1 = 0, forCount = 0, val;
    for(int x = -1; ; x=-x){
      if(forCount == 3) return false;
      forCount++;
      if(this->plateau[i][j+x] == WALL){
        val = x;
        if(walls1 == 1) x=-x;
        for(uint h = i; ; h+=x){
          if(h == this->plateau.size()-1 || h == 0) break;
          if(this->plateau[h][j+val] != WALL) return false;
          if(this->plateau[h+val][j] == TARGET || this->plateau[h+val][j] == MAN_ON_TARGET) return false;
          if(this->plateau[h+x][j] == WALL){
            walls1++;
            break;
          }
        }
        if(walls1 == 1) x=-x;
        if(walls1 == 2) return true;
      }
      if(this->plateau[i+x][j] == WALL){
        val = x;
        if(walls1 == 1) x=-x;
        for(uint h = j; ; h+=x){
          if(h == this->plateau[i].size()-1 || h == 0) break;
          if(this->plateau[i+val][h] != WALL) return false;
          if(this->plateau[i][h+val] == TARGET || this->plateau[i][h+val] == MAN_ON_TARGET) return false;
          if(this->plateau[i][h+x] == WALL){
            walls1++;
            break;
          }
        }
        if(walls1 == 1) x=-x;
        if(walls1 == 2) return true;
      }
    }
    return false;
  }

  bool blocked (){
    for(int i = -1; i<2; i+=2){
      if(this->plateau[this->man_pos[0]+i][this->man_pos[1]] == CRATE_ON_FREE){
        if(cratesBlock(this->man_pos[0]+i,this->man_pos[1])) return true;
        if(wallCheck(this->man_pos[0]+i, this->man_pos[1])) return true;
      }
      if(this->plateau[this->man_pos[0]][this->man_pos[1]+i] == CRATE_ON_FREE){
        if(cratesBlock(this->man_pos[0],this->man_pos[1]+i)) return true;
        if(wallCheck(this->man_pos[0], this->man_pos[1]+i)) return true;
      }
    }
    return false;
  }

  void DFS(int profondeur){

    static vector<move_t> graphe;
    if(profondeur == profondeur_max) return;
    if(finJeu()){
      cout << "Solution trouvée!!\n";
      affichePlateau();
      this->found = true;
      displayArbre(graphe);
      cout << graphe.size() << endl;
      return;
    }

    list<move_t> moves = nextMoves();
    vector<vector<uint>> copiePlateau = this->plateau;
    size_t hash = vecHash(this->plateau);
    this->parcours.insert(hash);

    for(auto i : moves) {
      if(this->found) return;
      play(i);
      hash = vecHash(this->plateau);
      if(isIn(hash)){
        unplay(copiePlateau, i);
        continue;
      }
      if(blocked()){ this->parcours.insert(hash); unplay(copiePlateau, i); continue;}
      else{
        this->count++;
        graphe.push_back(i);
        this->parcours.insert(hash);
        //system("clear");
        //affichePlateau();
        //displayArbre(graphe);
        DFS(profondeur+1);
        unplay(copiePlateau, i);
        graphe.pop_back();
      }
    }
    return;
  }

  void IDS(){
    for(int i = 1; i<260; i++){
      this->parcours.clear();
      profondeur_max = i;
      DFS(0);
      if(this->found) break;
    }
  }

  bool finJeu(){
    int crateOnTarget = 0;
    for(uint i = 0; i<this->plateau.size(); i++)
      for(uint j = 0; j<this->plateau[i].size(); j++)
        if(this->plateau[i][j] == CRATE_ON_TARGET) crateOnTarget++;

    if(crateOnTarget == this->crateNumber) return true;
    return false;
  }

  void jeu(){
    move_t move;
    while(!finJeu()){
      /*for(auto i : nextMoves()){
        cout << i << endl;
      }*/
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
      this->count++;
      system("clear");
      affichePlateau();
      cout << this->count << endl << endl;;
      if(blocked()) cout << "Bloqué!\n";
    }
    exit(1);
  }

};
