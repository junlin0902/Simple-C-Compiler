#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include "scanner.h"
#include <map>

void error_print(std::vector<Token> tokenLine) {
  int length = tokenLine.size();
  std::cerr << "ERROR: ";
  for (int i = 0; i < length; i++) {
    if (i < length - 1) {
      if (tokenLine[i + 1].getLexeme() == ",") {
        std::cerr << tokenLine[i].getLexeme();
      } else {
        std::cerr << tokenLine[i].getLexeme() << " ";
      }
    } else {
      std::cerr << tokenLine[i].getLexeme() << " ";
    }
  }
  std::cerr << std::endl;
}

int main() {
  std::string line;
  std::map<std::string, int> table;
  std::vector<std::vector<Token>> tokenSet;
  int line_counter = 0;
  try {
    // first pass
    while (getline(std::cin, line)) {
      std::vector<Token> tokenLine = scan(line);
      int length = tokenLine.size();
      for (int i = 0; i < length; i++) {
        if (tokenLine[i].getKind() == Token::LABEL) {
          std::string label = tokenLine[i].getLexeme();
          label = label.substr(0, label.length() - 1);
          if (table.count(label) == 0) {
            table.insert(std::pair<std::string, int>(label, line_counter));
          } else {
            error_print(tokenLine);
            ScanningFailure errmsg = ScanningFailure("Duplicate label define");
            throw errmsg;
          }
        } else if (tokenLine[i].getKind() == Token::WORD || tokenLine[i].getKind() == Token::ID) {
          line_counter += 4;
          break;
        }
      }
      tokenSet.push_back(tokenLine);
    }


    int max_linecounter = line_counter - 4;
    line_counter = 0;
    // second pass
    int loop_time = tokenSet.size();
    for (int j = 0; j < loop_time; j++) {
      std::vector<Token> tokenLine = tokenSet[j];
      int length = tokenLine.size();
      int encoding = 0;
      bool if_putchar = false;
      if (length == 0) {continue;}
      
      for (int i = 0; i < length; i++) {
        // label
        if (tokenLine[0].getKind() == Token::LABEL) {
          tokenLine.erase(tokenLine.begin());
          i -= 1;
          length -= 1;
        }

        // .word
        else if (tokenLine[0].getKind() == Token::WORD) {
          if (length != 2) {
            error_print(tokenLine);
            ScanningFailure errmsg = ScanningFailure("invalid command");
            throw errmsg;
          }
          // .word int or .word hex
          if (tokenLine[1].getKind() == Token::INT || tokenLine[1].getKind() == Token::HEXINT) {
            int64_t number = tokenLine[1].toNumber();
            if (number > 4294967295 || number < -2147483648) {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("int overflow or undeflow");
              throw errmsg;
            }
            line_counter += 4;
            encoding = (number << 0);
            if_putchar = true;
            break;
          } else if (tokenLine[1].getKind() == Token::ID) {
            std::string label = tokenLine[1].getLexeme();
            // if we find such label in symbol table
            if (table.count(label) > 0) {
              line_counter += 4;
              int number = table.find(label)->second;
              encoding = (number << 0);
              if_putchar = true;
              break;
            } else {
              // if we dont find such label in symbol table
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("label missing");
              throw errmsg;
            }
          } else {
            error_print(tokenLine);
            ScanningFailure errmsg = ScanningFailure("invalid command");
            throw errmsg;
          }
        }
  
  
        else if (tokenLine[0].getKind() == Token::ID) {
          if (tokenLine[0].getLexeme() == "add") {
            // add $d, $s, $t
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                tokenLine[3].getKind() == Token::REG && tokenLine[4].getKind() == Token::COMMA &&
                tokenLine[5].getKind() == Token::REG && length == 6) {
              int64_t s = tokenLine[3].toNumber();
              int64_t t = tokenLine[5].toNumber();
              int64_t d = tokenLine[1].toNumber();
              if (s > 31 || t > 31 || d > 31 || s < 0 || t < 0 || d < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (s << 21) | (t << 16) | (d << 11) | 32;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }


          else if (tokenLine[0].getLexeme() == "sub") {
            // sub $d, $s, $t
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                tokenLine[3].getKind() == Token::REG && tokenLine[4].getKind() == Token::COMMA &&
                tokenLine[5].getKind() == Token::REG && length == 6) {
              int64_t s = tokenLine[3].toNumber();
              int64_t t = tokenLine[5].toNumber();
              int64_t d = tokenLine[1].toNumber();
              if (s > 31 || t > 31 || d > 31 || s < 0 || t < 0 || d < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (s << 21) | (t << 16) | (d << 11) | 34;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "mult") {
            // mult $s, $t
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                tokenLine[3].getKind() == Token::REG && length == 4) {
              int64_t s = tokenLine[1].toNumber();
              int64_t t = tokenLine[3].toNumber();
              if (s > 31 || t > 31 || s < 0 || t < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (s << 21) | (t << 16) | 24;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "multu") {
            // multu $s, $t
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                tokenLine[3].getKind() == Token::REG && length == 4) {
              int64_t s = tokenLine[1].toNumber();
              int64_t t = tokenLine[3].toNumber();
              if (s > 31 || t > 31 || s < 0 || t < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (s << 21) | (t << 16) | 25;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "div") {
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                tokenLine[3].getKind() == Token::REG && length == 4) {
              int64_t s = tokenLine[1].toNumber();
              int64_t t = tokenLine[3].toNumber();
              if (s > 31 || t > 31 || s < 0 || t < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (s << 21) | (t << 16) | 26;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "divu") {
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                tokenLine[3].getKind() == Token::REG && length == 4) {
              int64_t s = tokenLine[1].toNumber();
              int64_t t = tokenLine[3].toNumber();
              if (s > 31 || t > 31 || s < 0 || t < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (s << 21) | (t << 16) | 27;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "mfhi") {
            if (tokenLine[1].getKind() == Token::REG && length == 2) {
              int64_t d = tokenLine[1].toNumber();
              if (d > 31 || d < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (d << 11) | 16;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "mflo") {
            if (tokenLine[1].getKind() == Token::REG && length == 2) {
              int64_t d = tokenLine[1].toNumber();
              if (d > 31 || d < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (d << 11) | 18;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "lis") {
            if (tokenLine[1].getKind() == Token::REG && length == 2) {
              int64_t d = tokenLine[1].toNumber();
              if (d > 31 || d < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (d << 11) | 20;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          if (tokenLine[0].getLexeme() == "lw") {
            // lw $t, i($s)
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
               (tokenLine[3].getKind() == Token::INT || tokenLine[3].getKind() == Token::HEXINT) && 
                tokenLine[4].getKind() == Token::LPAREN && tokenLine[5].getKind() == Token::REG && 
                tokenLine[6].getKind() == Token::RPAREN && length == 7) {
              int64_t t = tokenLine[1].toNumber();
              int64_t i = tokenLine[3].toNumber();
              int64_t s = tokenLine[5].toNumber();
              if (s > 31 || t > 31 || s < 0 || t < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              if (i > 4294967295 || i < -2147483648) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("int overflow or undeflow");
                throw errmsg;
              }
              line_counter += 4;
              if (i < 0) {
                encoding = (i & 65535) | (35 << 26) | (s << 21) | (t << 16);
              } else {
                encoding = (35 << 26) | (s << 21) | (t << 16) | i;
              }
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          if (tokenLine[0].getLexeme() == "sw") {
            // lw $t, i($s)
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
               (tokenLine[3].getKind() == Token::INT || tokenLine[3].getKind() == Token::HEXINT) && 
                tokenLine[4].getKind() == Token::LPAREN && tokenLine[5].getKind() == Token::REG && 
                tokenLine[6].getKind() == Token::RPAREN && length == 7) {
              int64_t t = tokenLine[1].toNumber();
              int64_t i = tokenLine[3].toNumber();
              int64_t s = tokenLine[5].toNumber();
              if (s > 31 || t > 31 || s < 0 || t < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              if (i > 4294967295 || i < -2147483648) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("int overflow or undeflow");
                throw errmsg;
              }
              line_counter += 4;
              if (i < 0) {
                encoding = (i & 65535) | (43 << 26) | (s << 21) | (t << 16);
              } else {
                encoding = (43 << 26) | (s << 21) | (t << 16) | i;
              }
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "slt") {
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                tokenLine[3].getKind() == Token::REG && tokenLine[4].getKind() == Token::COMMA &&
                tokenLine[5].getKind() == Token::REG && length == 6) {
              int64_t d = tokenLine[1].toNumber();
              int64_t s = tokenLine[3].toNumber();
              int64_t t = tokenLine[5].toNumber();
              if (s > 31 || t > 31 || d > 31 || s < 0 || t < 0 || d < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (s << 21) | (t << 16) | (d << 11) | 42;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "sltu") {
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                tokenLine[3].getKind() == Token::REG && tokenLine[4].getKind() == Token::COMMA &&
                tokenLine[5].getKind() == Token::REG && length == 6) {
              int64_t d = tokenLine[1].toNumber();
              int64_t s = tokenLine[3].toNumber();
              int64_t t = tokenLine[5].toNumber();
              if (s > 31 || t > 31 || d > 31 || s < 0 || t < 0 || d < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (s << 21) | (t << 16) | (d << 11) | 43;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "beq") {
            // beq $s, $t, i
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                tokenLine[3].getKind() == Token::REG && tokenLine[4].getKind() == Token::COMMA &&
                (tokenLine[5].getKind() == Token::INT || tokenLine[5].getKind() == Token::HEXINT) && length == 6) {
                int64_t s = tokenLine[1].toNumber();
                int64_t t = tokenLine[3].toNumber();
                int64_t i = tokenLine[5].toNumber();
                if (s > 31 || t > 31 || s < 0 || t < 0) {
                  error_print(tokenLine);
                  ScanningFailure errmsg = ScanningFailure("register out of range");
                  throw errmsg;
                }
                if (i > 4294967295 || i < -2147483648) {
                  error_print(tokenLine);
                  ScanningFailure errmsg = ScanningFailure("int overflow or undeflow");
                  throw errmsg;
                }
                if (line_counter + (i * 4) + 4 < 0 || line_counter + (i * 4) + 4 > max_linecounter) {
                  error_print(tokenLine);
                  ScanningFailure errmsg = ScanningFailure("i out of range");
                  throw errmsg;
                }
                if (i < 0) {
                  // if negtive, 65535 equals to "1111 1111 1111 1111"
                  encoding = (i & 65535) | (4 << 26) | (s << 21) | (t << 16);
                } else {
                  encoding = (4 << 26) | (s << 21) | (t << 16) | i;
                }
                line_counter += 4;
                if_putchar = true;
                break;
            // beq $s, $t, label
            } else if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                       tokenLine[3].getKind() == Token::REG && tokenLine[4].getKind() == Token::COMMA &&
                       tokenLine[5].getKind() == Token::ID && length == 6) {
              std::string label = tokenLine[5].getLexeme();
              if (table.count(label) > 0) {
                int64_t s = tokenLine[1].toNumber();
                int64_t t = tokenLine[3].toNumber();
                int i = (table.find(label)->second - (line_counter + 4)) / 4;
                if (s > 31 || t > 31 || s < 0 || t < 0) {
                  error_print(tokenLine);
                  ScanningFailure errmsg = ScanningFailure("register out of range");
                  throw errmsg;
                }
                if (i < 0) {
                  encoding = (i & 65535) | (4 << 26) | (s << 21) | (t << 16);
                } else {
                  encoding = (4 << 26) | (s << 21) | (t << 16) | i;
                }              
                line_counter += 4;
                if_putchar = true;
                break;
              } else {
                // if we dont find such label in symbol table
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("label missing");
                throw errmsg;
              }          
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command: beq");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "bne") {
            // bne $s, $t, i
            if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                tokenLine[3].getKind() == Token::REG && tokenLine[4].getKind() == Token::COMMA &&
                (tokenLine[5].getKind() == Token::INT || tokenLine[5].getKind() == Token::HEXINT) && length == 6) {
                int64_t s = tokenLine[1].toNumber();
                int64_t t = tokenLine[3].toNumber();
                int64_t i = tokenLine[5].toNumber();
                if (s > 31 || t > 31 || s < 0 || t < 0) {
                  error_print(tokenLine);
                  ScanningFailure errmsg = ScanningFailure("register out of range");
                  throw errmsg;
                }
                if (i > 4294967295 || i < -2147483648) {
                  error_print(tokenLine);
                  ScanningFailure errmsg = ScanningFailure("int overflow or undeflow");
                  throw errmsg;
                }
                if (line_counter + (i * 4) + 4 < 0 || line_counter + (i * 4) + 4 > max_linecounter) {
                  error_print(tokenLine);
                  ScanningFailure errmsg = ScanningFailure("i out of range");
                  throw errmsg;
                }
                if (i < 0) {
                  encoding = (i & 65535) | (5 << 26) | (s << 21) | (t << 16);
                } else {
                  encoding = (5 << 26) | (s << 21) | (t << 16) | i;
                }
                line_counter += 4;
                if_putchar = true;
                break;
            // bne $s, $t, label
            } else if (tokenLine[1].getKind() == Token::REG && tokenLine[2].getKind() == Token::COMMA &&
                       tokenLine[3].getKind() == Token::REG && tokenLine[4].getKind() == Token::COMMA &&
                       tokenLine[5].getKind() == Token::ID && length == 6) {
              std::string label = tokenLine[5].getLexeme();
              if (table.count(label) > 0) {
                int64_t s = tokenLine[1].toNumber();
                int64_t t = tokenLine[3].toNumber();
                int i = (table.find(label)->second - (line_counter + 4)) / 4;
                if (s > 31 || t > 31 || s < 0 || t < 0) {
                  error_print(tokenLine);
                  ScanningFailure errmsg = ScanningFailure("register out of range");
                  throw errmsg;
                }
                if (i < 0) {
                  encoding = (i & 65535) | (5 << 26) | (s << 21) | (t << 16);
                } else {
                  encoding = (5 << 26) | (s << 21) | (t << 16) | i;
                }              
                line_counter += 4;
                if_putchar = true;
                break;
              } else {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("label missing");
                throw errmsg;
              }          
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "jr") {
            if (tokenLine[1].getKind() == Token::REG && length == 2) {
              int64_t s = tokenLine[1].toNumber();
              if (s > 31 || s < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (s << 21) | 8;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }

          else if (tokenLine[0].getLexeme() == "jalr") {
            if (tokenLine[1].getKind() == Token::REG && length == 2) {
              int64_t s = tokenLine[1].toNumber();
              if (s > 31 || s < 0) {
                error_print(tokenLine);
                ScanningFailure errmsg = ScanningFailure("register out of range");
                throw errmsg;
              }
              line_counter += 4;
              encoding = (s << 21) | 9;
              if_putchar = true;
              break; 
            } else {
              error_print(tokenLine);
              ScanningFailure errmsg = ScanningFailure("Invalid for command");
              throw errmsg;
            }
          }
        }
      }

      if (if_putchar) {
        std::putchar(encoding >> 24);
        std::putchar(encoding >> 16);
        std::putchar(encoding >> 8);
        std::putchar(encoding);
      }
    }
  } catch (ScanningFailure &f) {
    std::cerr << f.what() << std::endl;
    return 1;
  }  
  return 0;
}
