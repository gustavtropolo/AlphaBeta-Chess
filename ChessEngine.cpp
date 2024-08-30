  #include <iostream>
  #include <vector>
  #include <string>
  #include <cstdlib>
  #include <chrono>

class Piece;

class ChessBoard{
  public:
    friend class PlayGame;
    std::vector<std::vector<Piece*>> board;
    ChessBoard(){
      //initialize as empty board
      for(int row = 0; row < 8; row++){
        board.push_back({});
        for(int col = 0; col < 8; col++){
          board[row].push_back(nullptr);
        }
      }
    }
  std::vector<Piece*>& operator[](int index) {
    return board[index];
  }
  bool isClear(int fromRow, int fromCol, int toRow, int toCol){
      if(abs(toRow - fromRow) <= 1 && abs(toCol - fromCol) <= 1){
        return true;
      }else if(fromRow == toRow){ //check columns
        int colInc = (fromCol<=toCol ? 1 : -1);
        for(int i = fromCol; i != toCol; i+=colInc){
          if(i == fromCol/* || i == toCol*/){
            continue;
          }
          if(board[fromRow][i] != nullptr){
            return false;
          }
        }
      }else if(fromCol == toCol){ //check rows
        int rowInc = (fromRow<=toRow ? 1 : -1);
        for(int i = fromRow; i != toRow; i+=rowInc){
          if(i == fromRow/* || i == toRow*/){
            continue;
          }
          if(board[i][fromCol] != nullptr){
            return false;
          }
        }
      }else{
        int rowInc = (fromRow<=toRow ? 1 : -1);
        int colInc = (fromCol<=toCol ? 1 : -1);
        int c = fromCol;
        for(int r = fromRow; r != toRow; r+=rowInc){
          if(r == fromRow){
            c += colInc;
            continue;
          }
          if(board[r][c] != nullptr){
            return false;
          }
          c += colInc;
        }
      }
      return true;
    }
};

ChessBoard board; //global chessboard variable

class Piece{
  friend class PlayGame;
  public:
    int row;
    int col;
    bool isWhite, neverMoved = true;
    std::string name;
    Piece(int _row, int _col, bool _isWhite, std::string _name) : row(_row), col(_col), isWhite(_isWhite), name(_name){}
    virtual bool isLegalMoveTo(int _row, int _col) = 0; //pure virtual function
    virtual ~Piece(){}
};

class Bishop : virtual public Piece{
  public:
    Bishop(int _row, int _col, int _isWhite, std::string _name) : Piece(_row, _col, _isWhite, _name){}
    virtual bool isLegalMoveTo(int _row, int _col){
      if(abs(row-_row) != abs(col-_col)){
        return false;
      }else if(!board.isClear(row, col, _row, _col)){
        return false;
      }
      if(board[_row][_col] != nullptr){
        if(board[_row][_col]->isWhite == isWhite){//can't capture same color
          return false;
        }
      }
      return true;
    }
    virtual ~Bishop(){}
};
class Rook : virtual public Piece{
  public:
    Rook(int _row, int _col, int _isWhite, std::string _name) : Piece(_row, _col, _isWhite, _name){}
    virtual bool isLegalMoveTo(int _row, int _col){
      if(_row != row && _col != col){
        return false;
      }else if(!board.isClear(row, col, _row, _col)){
        return false;
      }
      if(board[_row][_col] != nullptr){
        if(board[_row][_col]->isWhite == isWhite){//can't capture same color
          return false;
        }
      }
      return true;
    }
    virtual ~Rook(){}
};
class Knight : public Piece{
  public:
    Knight(int _row, int _col, int _isWhite, std::string _name) : Piece(_row, _col, _isWhite, _name){}
    bool isLegalMoveTo(int _row, int _col){
      if( (abs(row-_row)!=2 || abs(col-_col)!=1) && (abs(row-_row)!=1 || abs(col-_col)!=2) ){
        return false;
      }
      if(board[_row][_col] != nullptr){
        if(board[_row][_col]->isWhite == isWhite){//can't capture same color
          return false;
        }
      }
      return true;
    }
    ~Knight(){}
};
class King : public Piece{
  public:
    King(int _row, int _col, int _isWhite, std::string _name) : Piece(_row, _col, _isWhite, _name){}
    bool isLegalMoveTo(int _row, int _col){
      if(abs(row-_row) > 1 || abs(col-_col) > 1){
        return false;
      }
      if(board[_row][_col] != nullptr){
        if(board[_row][_col]->isWhite == isWhite){//can't capture same color
          return false;
        }
      }
      return true;
    }
    ~King(){}
};
class Queen : public Bishop, public Rook{
  public:
    Queen(int _row, int _col, int _isWhite, std::string _name) : Piece(_row, _col, _isWhite, _name), Bishop(_row, _col, _isWhite, _name), Rook(_row, _col, _isWhite, _name){}
    bool isLegalMoveTo(int _row, int _col){
      if(Bishop::isLegalMoveTo(_row, _col) || Rook::isLegalMoveTo(_row, _col)){
        return true;
      }
      return false;
    }
   virtual ~Queen(){}
};
class Pawn : public Queen{
  public:
    bool isQueen = false;
    Pawn(int _row, int _col, int _isWhite, std::string _name) : Piece(_row, _col, _isWhite, _name), Queen(_row, _col, _isWhite, _name){}
    bool isLegalMoveTo(int _row, int _col){
      if (isQueen) {
        if(Queen::isLegalMoveTo(_row, _col)){
          return true;
        }
        return false;
      }
      if(isWhite && _row-row >= 0 || !isWhite && _row-row <= 0){//white must move up and black must move down
        return false;
      }
      if(board[_row][_col] != nullptr){
        if(board[_row][_col]->isWhite == isWhite){//can't capture same color
          return false;
        }else if(abs(col-_col) != 1){
          return false;
        }
      }else{
        if(col != _col){//can't change columns if not capturing
          return false;
        }
      }
      if(abs(row-_row) > 2 || abs(col-_col) > 1){//can't move more than this
        return false;
      }else if(abs(row-_row) == 2 && (!neverMoved || !board.isClear(row, col, _row, _col))){ //can't move a pawn two squares if it has moved before
        return false;
      }else if(abs(col-_col) == 1 && abs(row-_row) == 2){//can't capture and move two squares
        return false;
      }
      return true;
    }
    ~Pawn(){}
};


void printBoard(int fromR, int fromC, int toR, int toC){
std::cout << "   0  1  2  3  4  5  6  7 " << std::endl;
for(int r = 0; r < 8; r++){
  std::cout << r << " ";
  for(int c = 0; c < 8; c++){
    if(board[r][c] != nullptr){
      bool isWhite = board[r][c]->isWhite;
      if(!isWhite){
        std::cout << "\033[0;35m";
      }
      if(toR == r && toC == c){
        std::cout << "\033[1;31m" << board[r][c]->name << " ";
        std::cout << "\033[0m";
      }else{
        std::cout << board[r][c]->name << " ";
        std::cout << "\033[0m";
      }
    }else{
      if(fromR == r && fromC == c){
        std::cout << "\033[1;31m" << "-- ";
        std::cout << "\033[0m";
      }else{
        std::cout << "-- ";
        std::cout << "\033[0m";
      }
    }
  }
  std::cout << '\n' << std::endl;
}
std::cout << std::endl;
}

class PlayGame{
public:
  bool playerPlaysWhite;
  int nodeCount = 0;
  int row1=-1, col1=-1, row2=-1, col2=-1;
  std::vector<int> gameLog;

  void printGameLog() {
    int counter = 0;
    for(int i = 0; i < gameLog.size(); i++) {
      std::cout << gameLog[i] << " ";
      counter++;
      if(counter % 4 == 0) {
        std::cout << "  " << std::endl;
      }
    }
  }
  void makeMove(int fromRow, int fromCol, int toRow, int toCol){
    board[fromRow][fromCol]->neverMoved = false; //definitely moved
    board[fromRow][fromCol]->row = toRow;
    board[fromRow][fromCol]->col = toCol;
    board[toRow][toCol] = board[fromRow][fromCol];
    board[fromRow][fromCol] = nullptr;
  }
  void undoMakeMove(int fromRow, int fromCol, int toRow, int toCol, bool neverMovedBefore, Piece* capturedPiece){
      if (neverMovedBefore)  board[toRow][toCol]->neverMoved = true; //restore neverMoved
      board[toRow][toCol]->row = fromRow;
      board[toRow][toCol]->col = fromCol;
      board[fromRow][fromCol] = board[toRow][toCol];
      if(capturedPiece != nullptr){
        board[toRow][toCol] = capturedPiece;
      }else{
        board[toRow][toCol] = nullptr;
      }
  }

  void printLegalMoves(bool isWhiteMove) {
    std::vector<int> moves = getLegalMoves(isWhiteMove);
    for(int i = 0; i < moves.size(); i+=4) {
      std::cout << moves[i] << " " << moves[i+1] << " " << moves[i+2] << " " << moves[i+3] << std::endl;
    }
  }

  std::vector<int> getLegalMoves(bool whiteTurn){
    std::vector<int> moves; //cur row, cur col, to row, to col
    for (int r = 0; r < 8; r++) {
      for (int c = 0; c < 8; c++) {
        Piece* piece = board[r][c];
        if(piece == nullptr){
          continue;
        }else if(!whiteTurn && piece->isWhite){//skip white pieces on black's turn
          continue;
        }else if(whiteTurn && !piece->isWhite){//skip black pieces on white's turn
          continue;
        }
        // Using dynamic_cast to determine the type of the piece
        if(dynamic_cast<Queen*>(piece)){
          //check all the columns and files the rook might want to go
          int row = r, col = c;
          while(row < 7){
            row += 1;
          }
          while(row>=0){
            if(piece->isLegalMoveTo(row, col) && row!=r) {
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
            row -= 1;
          }
          row = r;
          col = c;
          while(col>0){
            col -= 1;
          }
          while(col<=7){
            if(piece->isLegalMoveTo(row, col) && c!=col){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
            col += 1;
          }
          //check all the diagonal squares the bishop might want to go
          row = r;
          col = c;
          while(row<7 && col<7){
            row += 1;
            col += 1;
          }
          while(row>=0 && col>=0){
            if(piece->isLegalMoveTo(row, col) && r!=row){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
            row -= 1;
            col -= 1;
          }
          row = r;
          col = c;
          while(row<7 && col>0){
            row += 1;
            col -= 1;
          }
          while(row>=0 && col<8){
            if(piece->isLegalMoveTo(row, col) && r!=row){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
            row -= 1;
            col += 1;
          }
        }else if (dynamic_cast<Bishop*>(piece)) {
          //check all the diagonal squares the bishop might want to go
          int row = r, col = c;
          while(row<7 && col<7){
            row += 1;
            col += 1;
          }
          while(row>=0 && col>=0){
            if(piece->isLegalMoveTo(row, col) && r!=row){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
            row -= 1;
            col -= 1;
          }
          row = r;
          col = c;
          while(row<7 && col>0){
            row += 1;
            col -= 1;
          }
          while(row>=0 && col<8){
            if(piece->isLegalMoveTo(row, col) && r!=row){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
            row -= 1;
            col += 1;
          }
        } else if (dynamic_cast<Knight*>(piece)) {
          int row=r, col=c;
          row = r+2;
          col = c+1;
          if(row<=7 && col <=7){
            if(piece->isLegalMoveTo(row, col)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
          }
          row = r+1;
          col = c+2;
          if(row<=7 && col <=7){
            if(piece->isLegalMoveTo(row, col)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
          }
          row = r-2;
          col = c+1;
          if(row>=0 && col<=7){
            if(piece->isLegalMoveTo(row, col)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
          }
          row = r+1;
          col = c-2;
          if(row<=7 && col>=0){
            if(piece->isLegalMoveTo(row, col)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
          }
          row = r-1;
          col = c+2;
          if(row>=0 && col<=7){
            if(piece->isLegalMoveTo(row, col)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
          }
          row = r+2;
          col = c-1;
          if(row<=7 && col>=0){
            if(piece->isLegalMoveTo(row, col)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
          }
          row = r-2;
          col = c-1;
          if(row>=0 && col>=0){
            if(piece->isLegalMoveTo(row, col)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
          }
          row = r-1;
          col = c-2;
          if(row>=0 && col>=0){
            if(piece->isLegalMoveTo(row, col)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
          }
        } else if (dynamic_cast<Rook*>(piece)) {
          //check all the columns and files the rook might want to go
          int row = r, col = c;
          while(row < 7){
            row += 1;
          }
          while(row>=0){
            if(piece->isLegalMoveTo(row, col) && r!=row){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
            row -= 1;
          }
          row = r;
          col = c;
          while(col>0){
            col -= 1;
          }
          while(col<=7){
            if(piece->isLegalMoveTo(row, col) && c!=col){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(row);
              moves.push_back(col);
            }
            col += 1;
          }
        }else if (dynamic_cast<King*>(piece)) {
          if(r+1<=7 && c+1<=7){
            if(piece->isLegalMoveTo(r+1, c+1)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(r+1);
              moves.push_back(c+1);
            }
          }
          if(r+1<=7 && c-1>=0){
            if(piece->isLegalMoveTo(r+1, c-1)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(r+1);
              moves.push_back(c-1);
            }
          }
          if(r-1>=0 && c+1<=7){
            if(piece->isLegalMoveTo(r-1, c+1)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(r-1);
              moves.push_back(c+1);
            }
          }
          if(r-1>=0 && c-1>=0){
            if(piece->isLegalMoveTo(r-1, c-1)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(r-1);
              moves.push_back(c-1);
            }
          }
          if(r+1<=7){
            if(piece->isLegalMoveTo(r+1, c)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(r+1);
              moves.push_back(c);
            }
          }
          if(r-1>=0){
            if(piece->isLegalMoveTo(r-1, c)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(r-1);
              moves.push_back(c);
            }
          }
          if(c+1<=7){
            if(piece->isLegalMoveTo(r, c+1)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(r);
              moves.push_back(c+1);
            }
          }
          if(c-1>=0){
            if(piece->isLegalMoveTo(r, c-1)){
              moves.push_back(r);
              moves.push_back(c);
              moves.push_back(r);
              moves.push_back(c-1);
            }
          }
        } else if (dynamic_cast<Pawn*>(piece)) {
          if(piece->isWhite){
            if(piece->neverMoved){
              if(piece->isLegalMoveTo(r-2, c)){
                moves.push_back(r);
                moves.push_back(c);
                moves.push_back(r-2);
                moves.push_back(c);
              }
            }
            if(r-1>=0){
              if(piece->isLegalMoveTo(r-1, c)){
                moves.push_back(r);
                moves.push_back(c);
                moves.push_back(r-1);
                moves.push_back(c);
              }
            }
            if(r-1>=0 && c-1>=0){
              if(piece->isLegalMoveTo(r-1, c-1)){
                moves.push_back(r);
                moves.push_back(c);
                moves.push_back(r-1);
                moves.push_back(c-1);
              }
            }
            if(r-1>=0 && c+1<=7){
              if(piece->isLegalMoveTo(r-1, c+1)){
                moves.push_back(r);
                moves.push_back(c);
                moves.push_back(r-1);
                moves.push_back(c+1);
              }
            }
          }else{//pawn is black
            if(piece->neverMoved){
              if(piece->isLegalMoveTo(r+2, c)){
                moves.push_back(r);
                moves.push_back(c);
                moves.push_back(r+2);
                moves.push_back(c);
              }
            }
            if(r+1>=0){
              if(piece->isLegalMoveTo(r+1, c)){
                moves.push_back(r);
                moves.push_back(c);
                moves.push_back(r+1);
                moves.push_back(c);
              }
            }
            if(r+1>=0 && c-1>=0){
              if(piece->isLegalMoveTo(r+1, c-1)){
                moves.push_back(r);
                moves.push_back(c);
                moves.push_back(r+1);
                moves.push_back(c-1);
              }
            }
            if(r+1>=0 && c+1<=7){
              if(piece->isLegalMoveTo(r+1, c+1)){
                moves.push_back(r);
                moves.push_back(c);
                moves.push_back(r+1);
                moves.push_back(c+1);
              }
            }
          }
        }
      }
    }
    return moves;
  }

  int heuristicEvaluation(){ //value of the black pieces minus the value of the white pieces
    int eval = 0;
    for(int r = 0; r < 8; r++){
      for(int c = 0; c < 8; c++){
        Piece* piece = board[r][c];
        if(piece == nullptr){
          continue;
        }
        if(dynamic_cast<Pawn*>(piece)) {
          Pawn* pawn = dynamic_cast<Pawn*>(piece);
          if (pawn->isQueen) {
            if(piece->isWhite){
              eval -= 9;
            }else{
              eval += 9;
            }
          } else {
            if(piece->isWhite){
              eval -= 1;
            }else{
              eval += 1;
            }
          }
        }else if(dynamic_cast<Queen*>(piece)){
          if(piece->isWhite){
            eval -= 9;
          }else{
            eval += 9;
          }
        }else if(dynamic_cast<Rook*>(piece)){
          if(piece->isWhite){
            eval -= 5;
          }else{
            eval += 5;
          }
        }else if(dynamic_cast<Bishop*>(piece)){
          if(piece->isWhite){
            eval -= 3;
          }else{
            eval += 3;
          }
        }else if(dynamic_cast<Knight*>(piece)) {
          if(piece->isWhite){
            eval -= 3;
          }else{
            eval += 3;
          }
        }
      }
    }
    return eval;
  }

  bool isCheck(bool whiteInCheck){
    int kingLocation[2] = {-1, -1};
    for(int r = 0; r < 8; r++){//find kings
      for(int c = 0; c < 8; c++){
        Piece* piece = board[r][c];
        if(piece == nullptr){
          continue;
        }
        if(whiteInCheck == piece->isWhite && dynamic_cast<King*>(piece)) {
          kingLocation[0] = r;
          kingLocation[1] = c;
        }
      }
    }
    for(int r = 0; r < 8; r++){//find checks
      for(int c = 0; c < 8; c++){
        Piece* piece = board[r][c];
        if(piece == nullptr){
          continue;
        }
        if(piece->isWhite != whiteInCheck){
          if(dynamic_cast<Pawn*>(piece)){
            if(piece->isLegalMoveTo(kingLocation[0], kingLocation[1])){ //should be fine whether or not its a queen
              return true;
            }
          } else if(dynamic_cast<Queen*>(piece)){
            if(piece->isLegalMoveTo(kingLocation[0], kingLocation[1])){
              return true;
            }
          }else if(dynamic_cast<Bishop*>(piece)){
            if(piece->isLegalMoveTo(kingLocation[0], kingLocation[1])){
              return true;
            }
          }else if(dynamic_cast<Knight*>(piece)){
            if(piece->isLegalMoveTo(kingLocation[0], kingLocation[1])){
              return true;
            }
          }else if(dynamic_cast<Rook*>(piece)){
            if(piece->isLegalMoveTo(kingLocation[0], kingLocation[1])){
              return true;
            }
          }else if(dynamic_cast<King*>(piece)){
            if(piece->isLegalMoveTo(kingLocation[0], kingLocation[1])){
              return true;
            }
          }
        }
      }
    }
    return false;
  }
  bool isCheckmate(){
    std::vector<int> whiteMoves = getLegalMoves(true);
    for(int i = 0; i < whiteMoves.size(); i+=4){
      nodeCount++;
      Piece* capturedPiece = board[whiteMoves[i+2]][whiteMoves[i+3]];
      bool neverMovedBefore = board[whiteMoves[i]][whiteMoves[i+1]]->neverMoved;
      makeMove(whiteMoves[i], whiteMoves[i+1], whiteMoves[i+2], whiteMoves[i+3]);
      if(isCheck(true)){ //if white is in check
        undoMakeMove(whiteMoves[i], whiteMoves[i+1], whiteMoves[i+2], whiteMoves[i+3], neverMovedBefore, capturedPiece);
        whiteMoves.erase(whiteMoves.begin() + i, whiteMoves.begin() + i + 4); //erase the 4 bad values
        i-=4;
        continue;
      }
      undoMakeMove(whiteMoves[i], whiteMoves[i+1], whiteMoves[i+2], whiteMoves[i+3], neverMovedBefore, capturedPiece);

    }
    if(whiteMoves.empty()){
      return true;
    }
    return false;
  }

  void promotePawn(Piece* piece) {
    Pawn* pawn = dynamic_cast<Pawn*>(piece);
    pawn->isQueen = true;
    pawn->name = "Q" + std::to_string(pawn->col);
  }

  int mobilityBonus(){
    return getLegalMoves(false).size()/4;
  }

  void takeTurnAI(){
    int bestMove=-1, depth=4;
    int bestScore = -9999;
    std::vector<int> legalMoves = getLegalMoves(false);

    for(int i = 0; i < legalMoves.size(); i+=4){
      int score;
      nodeCount++;
      Piece* capturedPiece = board[legalMoves[i+2]][legalMoves[i+3]];
      bool neverMovedBefore = board[legalMoves[i]][legalMoves[i+1]]->neverMoved;
      makeMove(legalMoves[i], legalMoves[i+1], legalMoves[i+2], legalMoves[i+3]);
      if(isCheck(false)){ //if black is in check
        undoMakeMove(legalMoves[i], legalMoves[i+1], legalMoves[i+2], legalMoves[i+3], neverMovedBefore, capturedPiece);
        legalMoves.erase(legalMoves.begin() + i, legalMoves.begin() + i + 4); //erase the 4 bad values
        i-=4; //fix the index
        continue;
      }
      bool promoted = false;
      Piece* movedPiece = board[legalMoves[i+2]][legalMoves[i+3]];
      if (dynamic_cast<Pawn*>(movedPiece)) {
        Pawn* pawn = dynamic_cast<Pawn*>(movedPiece);
        if (!movedPiece->isWhite && legalMoves[i+2] == 7 && !pawn->isQueen) {
          promotePawn(movedPiece);
          promoted = true;
        }
      }
      score = miniMax(depth-1, false, -9999, 9999);
      if(promoted) {
        Pawn* pawn = dynamic_cast<Pawn*>(movedPiece);
        pawn->isQueen = false;
      }
      std::cout << legalMoves[i] << " " << legalMoves[i+1] << " " << legalMoves[i+2] << " " << legalMoves[i+3] << " score: " << score << std::endl;

      score *= 10;
      int bonus = mobilityBonus();
      score += bonus;
      if(score > bestScore){
        bestScore = score;
        bestMove = i;
      }

      undoMakeMove(legalMoves[i], legalMoves[i+1], legalMoves[i+2], legalMoves[i+3], neverMovedBefore, capturedPiece);
    }
    if(legalMoves.empty()){
      std::cout << "\nYou win by checkmate" << std::endl;
      std::exit(0);
    }
    std::cout << "Computer Moves: " << legalMoves[bestMove] << " " << legalMoves[bestMove+1] << " to " << legalMoves[bestMove+2] << " " << legalMoves[bestMove+3] << std::endl;
    std::cout << "best score: " << bestScore << std::endl;
    std::cout << "Node Count: " << nodeCount << std::endl;
    nodeCount = 0;
    makeMove(legalMoves[bestMove], legalMoves[bestMove+1], legalMoves[bestMove+2], legalMoves[bestMove+3]);
    if(board[legalMoves[bestMove+2]][legalMoves[bestMove+3]]->neverMoved){
      board[legalMoves[bestMove+2]][legalMoves[bestMove+3]]->neverMoved = false;
    }
    Piece* movedPiece = board[legalMoves[bestMove+2]][legalMoves[bestMove+3]];
    if (dynamic_cast<Pawn*>(movedPiece)) {
      Pawn* pawn = dynamic_cast<Pawn*>(movedPiece);
      if (!movedPiece->isWhite && legalMoves[bestMove+2] == 7 && !pawn->isQueen) {
        promotePawn(movedPiece);
      }
    }
    row1 = legalMoves[bestMove];
    col1 = legalMoves[bestMove+1];
    row2 = legalMoves[bestMove+2];
    col2 = legalMoves[bestMove+3];
  }


  int miniMax(int depth, bool isMaximising, int alpha, int beta){
    nodeCount++;
    int score, bestScore;
    std::vector<int> legalMoves;
    if(isMaximising){
      legalMoves = getLegalMoves(false);
    }else{
      legalMoves = getLegalMoves(true);
    }
    if(depth == 0){
      return heuristicEvaluation();
    }
    if(isMaximising){
      bestScore = -999;
      for(int i = 0; i < legalMoves.size(); i+=4){
        Piece* capturedPiece = board[legalMoves[i+2]][legalMoves[i+3]];
        bool neverMovedBefore = board[legalMoves[i]][legalMoves[i+1]]->neverMoved;
        makeMove(legalMoves[i], legalMoves[i+1], legalMoves[i+2], legalMoves[i+3]);
        if(isCheck(false)){ //if black is in check
          undoMakeMove(legalMoves[i], legalMoves[i+1], legalMoves[i+2], legalMoves[i+3], neverMovedBefore, capturedPiece);
          legalMoves.erase(legalMoves.begin() + i, legalMoves.begin() + i + 4); //erase the 4 bad values
          i-=4; //fix the index
          continue;
        }
        bool promoted = false;
        Piece* movedPiece = board[legalMoves[i+2]][legalMoves[i+3]];
        //std::cout << "Type of movedPiece: " << typeid(*movedPiece).name() << std::endl;
        if (dynamic_cast<Pawn*>(movedPiece)) {
          Pawn* pawn = dynamic_cast<Pawn*>(movedPiece);
          if (legalMoves[i+2] == 0 && pawn->isQueen == false) {
            promotePawn(movedPiece);
            promoted = true;
            //printBoard(-1, -1, -1, -1);
          }
        }
        score = miniMax(depth-1, false, -9999, 9999);
        if(promoted) {
          Pawn* pawn = dynamic_cast<Pawn*>(movedPiece);
          pawn->isQueen = false;
          pawn->name = std::to_string('Q' + pawn->col);
          printBoard(-1, -1, -1, -1);
        }
        undoMakeMove(legalMoves[i], legalMoves[i+1], legalMoves[i+2], legalMoves[i+3], neverMovedBefore, capturedPiece);
        if(bestScore < score){
          bestScore = score;
          alpha = std::max(alpha, bestScore);
          if(alpha >= beta){
            break;
          }
        }
      }
      if(legalMoves.empty()){ //checkmate or stalemate
        if(!isCheck(false)){
          printBoard(-1, -1, -1, -1);
        }
        return isCheck(false) ? -1000+(30-depth*10) : 0;
      }
      return bestScore;
    }else{
      bestScore = 999;
      for(int i = 0; i < legalMoves.size(); i+=4){
        Piece* capturedPiece = board[legalMoves[i+2]][legalMoves[i+3]];
        bool neverMovedBefore = board[legalMoves[i]][legalMoves[i+1]]->neverMoved;
        makeMove(legalMoves[i], legalMoves[i+1], legalMoves[i+2], legalMoves[i+3]);
        if(isCheck(true)){ //if white is in check
          undoMakeMove(legalMoves[i], legalMoves[i+1], legalMoves[i+2], legalMoves[i+3], neverMovedBefore, capturedPiece);
          legalMoves.erase(legalMoves.begin() + i, legalMoves.begin() + i + 4); //erase the 4 bad values
          i-=4; //fix the index
          continue;
        }
        bool promoted = false;
        Piece* movedPiece = board[legalMoves[i+2]][legalMoves[i+3]];
        //std::cout << "Type of movedPiece: " << typeid(*movedPiece).name() << std::endl;
        if (dynamic_cast<Pawn*>(movedPiece)) {
          Pawn* pawn = dynamic_cast<Pawn*>(movedPiece);
          if (legalMoves[i+2] == 0 && pawn->isQueen == false) {
            promotePawn(movedPiece);
            promoted = true;
            //printBoard(-1, -1, -1, -1);
          }
        }
        score = miniMax(depth-1, true, -9999, 9999);
        if(promoted) {
          Pawn* pawn = dynamic_cast<Pawn*>(movedPiece);
          pawn->isQueen = false;
          pawn->name = std::to_string(pawn->col + 10);
          printBoard(-1, -1, -1, -1);
        }
        undoMakeMove(legalMoves[i], legalMoves[i+1], legalMoves[i+2], legalMoves[i+3], neverMovedBefore, capturedPiece);
        if (promoted) {
          printBoard(-1, -1, -1, -1);
        }
        if(bestScore > score){
          bestScore = score;
          beta = std::min(beta, bestScore);
          if(alpha >= beta){
            break;
          }
        }
      }
      if(legalMoves.empty()){ //checkmate or stalemate
        return isCheck(true) ? 1000-(30-depth*10) : 0;
      }
      return bestScore;
    }
  }

};

void setupBoard() {
  //first row black pieces
  board[0][0] = new Rook(0, 0, false, "r1");
  board[0][1] = new Knight(0, 1, false, "n1");
  board[0][2] = new Bishop(0, 2, false, "b1");
  board[0][3] = new Queen(0, 3, false, "q1");
  board[0][4] = new King(0, 4, false, "k1");
  board[0][5] = new Bishop(0, 5, false, "b2");
  board[0][6] = new Knight(0, 6, false, "n2");
  board[0][7] = new Rook(0, 7, false, "r2");
  //black pawns
  for (int i = 0; i < 8; i++) {
    board[1][i] = new Pawn(1, i, false, std::to_string(i+20));
  }

  //white pieces
  board[7][0] = new Rook(7, 0, true, "R1");
  board[7][1] = new Knight(7, 1, true, "N1");
  board[7][2] = new Bishop(7, 2, true, "B1");
  board[7][3] = new Queen(7, 3, true, "Q1");
  board[7][4] = new King(7, 4, true, "K1");
  board[7][5] = new Bishop(7, 5, true, "B2");
  board[7][6] = new Knight(7, 6, true, "N2");
  board[7][7] = new Rook(7, 7, true, "R2");
  //white pawns
  for (int i = 0; i < 8; i++) {
    board[6][i] = new Pawn(6, i, true, std::to_string(i+10));
  }
}

int main() {
  setupBoard();

  int row=-1, col=-1, toRow=-1, toCol=-1;
  std::string goingFirst = "null";
  bool isPlayerMove = true;
  PlayGame play;
  printBoard(-1, -1, -1, -1);
  do{
  std::cout << "Would you like to play first? (y/n) ";
  std::cin >> goingFirst;
  if(goingFirst == "y"){
    play.playerPlaysWhite = true;
  }else if(goingFirst == "n"){
    play.playerPlaysWhite = false;
    isPlayerMove = false;
  }else{
    std::cout << "invalid entry" << std::endl;
  }
  }while(goingFirst == "null");
  while(true){
  printBoard(play.row1, play.col1, play.row2, play.col2);
  while(isPlayerMove){
    while (true) {
      std::cout << "What move would you like to make?\nEnter the row followed by the column: \n";
      if (!(std::cin >> row >> col)) {
        std::cin.clear(); // Clear error flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore bad input
        std::cout << "Invalid input. Please enter numbers only." << std::endl;
        continue;
      }
      std::cout << "What is the target row and target column:\n";
      if (!(std::cin >> toRow >> toCol)) {
        std::cin.clear(); //clear the error flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //ignore bad input
        std::cout << "Invalid input. Please enter numbers only." << std::endl;
        continue;
      }
      if(row == 0 && col == 0 && toRow == 0 && toCol == 0) {
        play.printGameLog();
      }
      if (row < 0 || col < 0 || toRow < 0 || toCol < 0 || row > 7 || col > 7 || toRow > 7 || toCol > 7) {
        std::cout << "Invalid input. Coordinates must be between 0 and 7." << std::endl;
      }else{
        break; //valid input
      }
    }
    if(board[row][col] == nullptr){
      std::cout << "There's no piece on that square, try again." << std::endl;
    }else if(play.playerPlaysWhite && !board[row][col]->isWhite){
      std::cout << "You cannot move a piece of the opposite color, try again." << std::endl;
    }else if(board[row][col]->isLegalMoveTo(toRow, toCol)){
      Piece* capturedPiece = board[toRow][toCol];
      bool neverMovedBefore = board[row][col]->neverMoved;
      play.makeMove(row, col, toRow, toCol);
      if(play.isCheck(true)){
        play.undoMakeMove(row, col, toRow, toCol, neverMovedBefore, capturedPiece);
        //play.makeMove(toRow, toCol, row, col); //undo move
        std::cout << "\nThat would leave your king in check, try again.\n" << std::endl;
        continue;
      }
      Piece* movedPiece = board[toRow][toCol];
      if (dynamic_cast<Pawn*>(movedPiece)) {
        Pawn* pawn = dynamic_cast<Pawn*>(movedPiece);
        if (pawn->row == 0 && pawn->isQueen == false) {
          play.promotePawn(pawn);
        }
      }
      play.gameLog.push_back(row);
      play.gameLog.push_back(col);
      play.gameLog.push_back(toRow);
      play.gameLog.push_back(toCol);
      //test
      isPlayerMove = false;
    }else{
      std::cout << "Illegal move, try again." << std::endl;
    }
  }

  printBoard(row, col, toRow, toCol);
  std::chrono::steady_clock::time_point t1;
  t1 = std::chrono::steady_clock::now();
  play.takeTurnAI();
  std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
  std::chrono::duration<double> d = t2-t1;
  std::cout << "The computer's move took " << d.count() << " seconds." << std::endl;
  isPlayerMove = true;

    if(play.isCheckmate()){
      printBoard(play.row1, play.col1, play.row2, play.col2);
      char wantGameLog;
      std::cout << "\nThat's checkmate, you lose." << std::endl;
      std::cin >> wantGameLog;
      if(wantGameLog == 'y'){
        play.printGameLog();
      }
      std::exit(0);
    }
  }
}


