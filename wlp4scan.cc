#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdint>
#include <cctype>
#include <algorithm>
#include <utility>
#include <set>
#include <ostream>
#include <array>

class Token;
std::vector<Token> scan(const std::string &input);
class Token {
  public:
    enum Kind {
      ID = 0,
      NUM,
      LPAREN,
      RPAREN,
      LBRACE,
      RBRACE,
      RETURN,
      IF,
      ELSE,
      WHILE,
      PRINTLN,
      WAIN,
      BECOMES,
      INT,
      EQ,
      NE,
      LT,
      GT,
      LE,
      GE,
      PLUS,
      MINUS,
      STAR,
      SLASH,
      PCT,
      COMMA,
      SEMI,
      NEW,
      DELETE,
      LBRACK,
      RBRACK,
      AMP,
      NONE,
      SPACE,
      COMMENT
    };

  private:
    Kind kind;
    std::string lexeme;

  public:
    Token(Kind kind, std::string lexeme):
    kind(kind), lexeme(std::move(lexeme)) {}
    Kind getKind() const { return kind; }
    const std::string &getLexeme() const { return lexeme; }
    int64_t toNumber() const {
      std::istringstream iss;
      int64_t result;
      if (kind == NUM) {
        iss.str(lexeme);
      } else {
        return 0;
      }
      iss >> result;
      return result;
    }
};

class ScanningFailure {
    std::string message;

  public:
    ScanningFailure(std::string message);

    // Returns the message associated with the exception.
    const std::string &what() const;
};


std::ostream &operator<<(std::ostream &out, const Token &tok) {
  switch (tok.getKind()) {
    case Token::ID:         out << "ID";          break;       
    case Token::NUM:        out << "NUM";         break;         
    case Token::LPAREN:     out << "LPAREN";      break;               
    case Token::RPAREN:     out << "RPAREN";      break;               
    case Token::LBRACE:     out << "LBRACE";      break;               
    case Token::RBRACE:     out << "RBRACE";      break;               
    case Token::RETURN:     out << "RETURN";      break;               
    case Token::IF:         out << "IF";          break;       
    case Token::ELSE:       out << "ELSE";        break;           
    case Token::WHILE:      out << "WHILE";       break;             
    case Token::PRINTLN:    out << "PRINTLN";     break;                 
    case Token::WAIN:       out << "WAIN";        break;           
    case Token::BECOMES:    out << "BECOMES";     break;                 
    case Token::INT:        out << "INT";         break;         
    case Token::EQ:         out << "EQ";          break;       
    case Token::NE:         out << "NE";          break;       
    case Token::LT:         out << "LT";          break;       
    case Token::GT:         out << "GT";          break;       
    case Token::LE:         out << "LE";          break;       
    case Token::GE:         out << "GE";          break;       
    case Token::PLUS:       out << "PLUS";        break;           
    case Token::MINUS:      out << "MINUS";       break;             
    case Token::STAR:       out << "STAR";        break;           
    case Token::SLASH:      out << "SLASH";       break;             
    case Token::PCT:        out << "PCT";         break;         
    case Token::COMMA:      out << "COMMA";       break;             
    case Token::SEMI:       out << "SEMI";        break;           
    case Token::NEW:        out << "NEW";         break;         
    case Token::DELETE:     out << "DELETE";      break;               
    case Token::LBRACK:     out << "LBRACK";      break;               
    case Token::RBRACK:     out << "RBRACK";      break;               
    case Token::AMP:        out << "AMP";         break;         
    case Token::NONE:       out << "NULL";        break;
    case Token::SPACE:      out << "SPACE";       break;
    case Token::COMMENT:    out << "COMMENT";     break;       
  }
  out << " " << tok.getLexeme() << std::endl;
  return out;
}

ScanningFailure::ScanningFailure(std::string message):
  message(std::move(message)) {}

const std::string &ScanningFailure::what() const { return message; }

class Wlp4DFA {
  public:
    enum State {
      ID = 0,
      NUM,
      LPAREN,
      RPAREN,
      LBRACE,
      RBRACE,
      RETURN,
      IF,
      ELSE,
      WHILE,
      PRINTLN,
      WAIN,
      BECOMES,
      INT,
      EQ,
      NE,
      LT,
      GT,
      LE,
      GE,
      PLUS,
      MINUS,
      STAR,
      SLASH,
      PCT,
      COMMA,
      SEMI,
      NEW,
      DELETE,
      LBRACK,
      RBRACK,
      AMP,
      NONE,
      SPACE,
      COMMENT,
      EXCLAME,
      FAIL,
      ZERO,
      START,
      LARGEST_STATE = START
    };

  private:
    std::set<State> acceptingStates;
    std::array<std::array<State, 128>, LARGEST_STATE + 1> transitionFunction;
    Token::Kind stateToKind(State s) const {
      switch(s) {                  
        case ZERO:       return Token::NUM;
        case ID:         return Token::ID;                      
        case NUM:        return Token::NUM;                
        case LPAREN:     return Token::LPAREN;                   
        case RPAREN:     return Token::RPAREN;                   
        case LBRACE:     return Token::LBRACE;                   
        case RBRACE:     return Token::RBRACE;                   
        case RETURN:     return Token::RETURN;                   
        case IF:         return Token::IF;               
        case ELSE:       return Token::ELSE;                 
        case WHILE:      return Token::WHILE;                  
        case PRINTLN:    return Token::PRINTLN;                      
        case WAIN:       return Token::WAIN;                 
        case BECOMES:    return Token::BECOMES;                      
        case INT:        return Token::INT;                
        case EQ:         return Token::EQ;               
        case NE:         return Token::NE;               
        case LT:         return Token::LT;               
        case GT:         return Token::GT;               
        case LE:         return Token::LE;               
        case GE:         return Token::GE;               
        case PLUS:       return Token::PLUS;                 
        case MINUS:      return Token::MINUS;                  
        case STAR:       return Token::STAR;                 
        case SLASH:      return Token::SLASH;                  
        case PCT:        return Token::PCT;                
        case COMMA:      return Token::COMMA;                  
        case SEMI:       return Token::SEMI;                 
        case NEW:        return Token::NEW;                
        case DELETE:     return Token::DELETE;                   
        case LBRACK:     return Token::LBRACK;                   
        case RBRACK:     return Token::RBRACK;                   
        case AMP:        return Token::AMP;                
        case NONE:       return Token::NONE;                 
        case SPACE:      return Token::SPACE;                                    
        case COMMENT:    return Token::COMMENT;
                       
                   
        default: throw ScanningFailure("ERROR: Cannot convert state to kind.");
      }
    }


  public:
    std::vector<Token> simplifiedMaximalMunch(const std::string &input) const {
      std::vector<Token> result;
      State state = start();
      std::string munchedInput;
      for (std::string::const_iterator inputPosn = input.begin();
           inputPosn != input.end();) {
        State oldState = state;
        state = transition(state, *inputPosn);
        if (!failed(state)) {
          munchedInput += *inputPosn;
          oldState = state;
          ++inputPosn;
        }
        if (inputPosn == input.end() || failed(state)) {
          if (accept(oldState)) {
            result.push_back(Token(stateToKind(oldState), munchedInput));
            munchedInput = "";
            state = start();
          } else {
            if (failed(state)) {
              munchedInput += *inputPosn;
            }
            throw ScanningFailure("ERROR: Simplified maximal munch failed on input: "
                                 + munchedInput);
          }
        }
      }
      return result;
    }



    // complete this
    Wlp4DFA() {
      acceptingStates = {ID, NUM, LPAREN, RPAREN, LBRACE, RBRACE, BECOMES, EQ,
                         NE, LT, GT, LE, GE, PLUS, MINUS, STAR, SLASH, PCT,
                         COMMA, SEMI, AMP, SPACE, COMMENT, LBRACK, RBRACK, ZERO};

      // Initialize transitions for the DFA
      for (size_t i = 0; i < transitionFunction.size(); ++i) {
        for (size_t j = 0; j < transitionFunction[0].size(); ++j) {
          transitionFunction[i][j] = FAIL;
        }
      }
      registerTransition(START, isalpha, ID);
      registerTransition(ID, isalnum, ID);

      registerTransition(START, "123456789", NUM);
      registerTransition(NUM, "0123456789", NUM);

      registerTransition(START, "0", ZERO);

      registerTransition(START, "(", LPAREN);
      registerTransition(START, ")", RPAREN);
      registerTransition(START, "{", LBRACE);
      registerTransition(START, "}", RBRACE);

      registerTransition(START, "=", BECOMES);
      registerTransition(BECOMES, "=", EQ);
      
      registerTransition(START, "!", EXCLAME);
      registerTransition(EXCLAME, "=", NE);

      registerTransition(START, "<", LT);
      registerTransition(START, ">", GT);
      registerTransition(LT, "=", LE);
      registerTransition(GT, "=", GE);

      registerTransition(START, "+", PLUS);
      registerTransition(START, "-", MINUS);
      registerTransition(START, "*", STAR);
      registerTransition(START, "/", SLASH);
      registerTransition(START, "%", PCT);
      registerTransition(START, ",", COMMA);
      registerTransition(START, ";", SEMI);
      registerTransition(START, "[", LBRACK);
      registerTransition(START, "]", RBRACK);
      registerTransition(START, "&", AMP);   
      registerTransition(START, "/", SLASH);
      registerTransition(SLASH, "//", COMMENT);
      // for white space
      registerTransition(START, isspace, SPACE);
      registerTransition(SPACE, isspace, SPACE);
      // for comment
      registerTransition(COMMENT, [](int c) -> int { return c != '\n'; }, COMMENT);
    }

    // Register a transition on all chars in chars
    void registerTransition(State oldState, const std::string &chars,
        State newState) {
      for (char c : chars) {
        transitionFunction[oldState][c] = newState;
      }
    }

    // Register a transition on all chars matching test
    // For some reason the cctype functions all use ints, hence the function
    // argument type.
    void registerTransition(State oldState, int (*test)(int), State newState) {
      for (int c = 0; c < 128; ++c) {
        if (test(c)) {
          transitionFunction[oldState][c] = newState;
        }
      }
    }


    /* Returns the state corresponding to following a transition
     * from the given starting state on the given character,
     * or a special fail state if the transition does not exist.
     */
    State transition(State state, char nextChar) const {
      return transitionFunction[state][nextChar];
    }


    /* Checks whether the state returned by transition
     * corresponds to failure to transition.
     */
    bool failed(State state) const { return state == FAIL; }

    /* Checks whether the state returned by transition
     * is an accepting state.
     */
    bool accept(State state) const {
      return acceptingStates.count(state) > 0;
    }

    /* Returns the starting state of the DFA
     */
    State start() const { return START; }
};


// need to change
std::vector<Token> scan(const std::string &input) {
  static Wlp4DFA theDFA;

  std::vector<Token> tokens = theDFA.simplifiedMaximalMunch(input);
  std::vector<Token> newTokens;

  for (auto &token : tokens) {
    if (token.getKind() == Token::ID) {
      if (token.getLexeme() == "return") {
        newTokens.push_back(Token(Token::RETURN, "return"));
      }
      else if (token.getLexeme() == "if") {
        newTokens.push_back(Token(Token::IF, "if"));
      }
      else if (token.getLexeme() == "else") {
        newTokens.push_back(Token(Token::ELSE, "else"));
      }
      else if (token.getLexeme() == "while") {
        newTokens.push_back(Token(Token::WHILE, "while"));
      }
      else if (token.getLexeme() == "println") {
        newTokens.push_back(Token(Token::PRINTLN, "println"));
      }
      else if (token.getLexeme() == "wain") {
        newTokens.push_back(Token(Token::WAIN, "wain"));
      }
      else if (token.getLexeme() == "int") {
        newTokens.push_back(Token(Token::INT, "int"));
      }
      else if (token.getLexeme() == "new") {
        newTokens.push_back(Token(Token::NEW, "new"));
      }
      else if (token.getLexeme() == "delete") {
        newTokens.push_back(Token(Token::DELETE, "delete"));
      }
      else if (token.getLexeme() == "NULL") {
        newTokens.push_back(Token(Token::NONE, "NULL"));
      }
      // var
      else {
        newTokens.push_back(Token(Token::ID, token.getLexeme()));
      }
    } else if (token.getKind() == Token::NUM) {
      int64_t value = token.toNumber();
      if (value > 2147483647) {
        throw ScanningFailure("ERROR: integer overflow"); 
      }
      newTokens.push_back(token);
    } else if (token.getKind() != Token::SPACE
        && token.getKind() != Token::Kind::COMMENT) {
      newTokens.push_back(token);
    }
  }
  return newTokens;
}

int main() {
  std::string line;
  try {
    std::istream& in = std::cin;
    while (getline(in, line)) {
      std::vector<Token> tokenLine = scan(line);
      for (auto it: tokenLine) {
        std::cout << it;
      }
    }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  } 
}
