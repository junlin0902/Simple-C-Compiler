#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <vector>
#include <cctype>
#include <map>
#include <algorithm>
#include <utility>

struct parseTree {
    std::string rule;
    std::vector<std::shared_ptr<parseTree>> children;
};

// reconstruct parse tree
void reconstruct(std::shared_ptr<parseTree> P) {
    std::string line;
    if (std::getline(std::cin, line) && !line.empty()) {
        P->rule = line;
        std::istringstream iss(line);

        std::string cur;
        iss >> cur;
        if (std::isupper(cur[0])) {return;}

        while (iss >> cur) {
            if (cur == ".EMPTY") {return;}
            parseTree push = {};
            std::shared_ptr<parseTree> newAdd = std::make_shared<parseTree>(push);
            P->children.push_back(newAdd);
            reconstruct(P->children.back());
        }
    }
    return;   
}

void getParatype(std::shared_ptr<parseTree> tree, std::vector<std::string>& paraType) {
    std::string type = "";
    for (auto it: tree->children[0]->children[0]->children) {
        std::istringstream iss(it->rule);
        std::string slice;
        iss >> slice;
        iss >> slice;
        type += slice;
    }
    paraType.push_back(type); 
    if (tree->children.size() != 1) { 
        getParatype(tree->children[2], paraType);              
    } 
}


std::string typeOf(std::shared_ptr<parseTree> tree, std::map<std::pair<std::string, std::string>, std::string>& symbolTable, std::string& scope, std::map<std::string, std::vector<std::string>>& paraTable) {
    std::string e;
    if (tree->rule == "factor ID" || tree->rule == "lvalue ID") {
        std::istringstream iss(tree->children[0]->rule);
        std::string out;
        iss >> out;
        iss >> out;
        if (symbolTable.find(std::make_pair(scope, out)) != symbolTable.end()) {
            return symbolTable[std::make_pair(scope, out)];
        } else {
            e = "ERROR: Variable use before declaration: " + out; 
            throw e;
        }
    } else if (tree->rule == "factor NUM") {
        return "int";
    } else if (tree->rule == "factor NULL") {
        return "int*";
    } else if (tree->rule == "expr term") {
        return typeOf(tree->children[0], symbolTable, scope, paraTable);
    } else if (tree->rule == "expr expr PLUS term") {
        std::string left = typeOf(tree->children[0], symbolTable, scope, paraTable);
        std::string right = typeOf(tree->children[2], symbolTable, scope, paraTable);
        if (left == "int" && right == "int") {return "int";}
        else if ((left == "int*" && right == "int") || (right == "int*" && left == "int")) {return "int*";}
        else {
            e = "ERROR: " + left + " + " + right;
            throw e;
        }
    } else if (tree->rule == "expr expr MINUS term") {
        std::string left = typeOf(tree->children[0], symbolTable, scope, paraTable);
        std::string right = typeOf(tree->children[2], symbolTable, scope, paraTable);
        if ((left == "int" && right == "int") || (left == "int*" && right == "int*")) {return "int";}
        else if (left == "int*" && right == "int") {return "int*";}
        else {
            e = "ERROR: " + left + " - " + right;
            throw e;
        }        
    } else if (tree->rule == "factor LPAREN expr RPAREN" || tree->rule == "lvalue LPAREN lvalue RPAREN") {
        return typeOf(tree->children[1], symbolTable, scope, paraTable);    
    } else if (tree->rule == "term factor") {
        return typeOf(tree->children[0], symbolTable, scope, paraTable);
    } else if (tree->rule == "term term STAR factor" || tree->rule == "term term SLASH factor" || tree->rule == "term term PCT factor") {
        std::string left = typeOf(tree->children[0], symbolTable, scope, paraTable);
        std::string right = typeOf(tree->children[2], symbolTable, scope, paraTable);

        if(left == "int" && right == "int") {return "int";}
        else {
            e = "ERROR: " + left + "* or / or %" + right;
            throw e;   
        }
    } else if (tree->rule == "factor AMP lvalue") {
        if (typeOf(tree->children[1], symbolTable, scope, paraTable) == "int") {return "int*";}
        else {
            e = "ERROR: D";
            throw e;
        }      
    } else if (tree->rule == "factor STAR factor" || tree->rule == "lvalue STAR factor") {
        if (typeOf(tree->children[1], symbolTable, scope, paraTable) == "int*") {return "int";}
        else {
            e = "ERROR: E";
            throw e;
        }           
    } else if (tree->rule == "factor NEW INT LBRACK expr RBRACK") {
        if (typeOf(tree->children[3], symbolTable, scope, paraTable) == "int") {return "int*";}
        else {
            e = "ERROR: F";
            throw e;
        }    
    } else if (tree->rule == "factor ID LPAREN RPAREN") {
        std::istringstream iss(tree->children[0]->rule);
        std::string out;
        iss >> out;
        iss >> out;
        //std::string scopeName = "global";
        if (paraTable[out].size() != 0) {
            e = "ERROR: invalid number of function parameters for: " + out;
            throw e;
        } 
        if (symbolTable.find(std::make_pair("global", out)) != symbolTable.end()) {
            return symbolTable[std::make_pair("global", out)];
        } else {
            e = "ERROR: function used not defined";
            throw e;
        }        
    } else if (tree->rule == "factor ID LPAREN arglist RPAREN") {
        std::istringstream iss(tree->children[0]->rule);
        std::string out;
        iss >> out;
        iss >> out;

        if (symbolTable.find(std::make_pair("global", out)) == symbolTable.end()) {
            e = "ERROR: undefined function call: " + out;
            throw e;
        } else {
            std::vector<std::string> types;
            std::shared_ptr<parseTree> cur = tree->children[2];
            while (cur->children.size() == 3) {
                types.push_back(typeOf(cur->children[0], symbolTable, scope, paraTable));
                cur = cur->children[2];
            }
            types.push_back(typeOf(cur->children[0], symbolTable, scope, paraTable));

            if (types.size() != paraTable[out].size()) {
                    e = "ERROR: length of parameters doesn't match";
                    throw e;
            }
            std::string type = types[0];
            for (unsigned int i = 0; i < types.size(); i++) {
                if (types[i] != paraTable[out][i]) {
                    e = "ERROR: parameters doesnt match with function calls: " + out;
                    throw e;                    
                }
            }
            return symbolTable[std::make_pair("global", out)];
        }             
    } else if (tree->rule.substr(0, 3) == "NUM") {
        return "int";
    } else if (tree->rule.substr(0, 2) == "ID") {
        std::istringstream iss(tree->rule);
        std::string out;
        iss >> out;
        iss >> out;
        if (symbolTable.find(std::make_pair(scope, out)) != symbolTable.end()) {
            return symbolTable[std::make_pair(scope, out)];
        } else if (symbolTable.find(std::make_pair("global", out)) != symbolTable.end()) {
            return symbolTable[std::make_pair("global", out)];
        } else {
            e = "ERROR: J";
            throw e;
        }             
    } else if (tree->rule.substr(0, 4) == "NULL") {
        return "int*";
    }
    
    return "";
}





void printParsetree(std::shared_ptr<parseTree> tree, std::map<std::pair<std::string, std::string>, std::string>& symbolTable, std::string& scope, std::map<std::string, std::vector<std::string>>& paraTable) {
    std::cout << tree->rule;
    std::string type = typeOf(tree, symbolTable, scope, paraTable);

    if (tree->rule.substr(0, 2) == "ID") {
        std::istringstream iss(tree->rule);
        std::string IDname;
        iss >> IDname;
        iss >> IDname;
        if (scope == "global") {}
        else if (symbolTable.find(std::make_pair(scope, IDname)) == symbolTable.end() &&
            symbolTable.find(std::make_pair("global", IDname)) != symbolTable.end()) {} 
        else {
            if (type != "") {
                std::cout << " : " << type;
            }
        }
    } else {
        if (type != "") {
            std::cout << " : " << type;
        }
    }
    std::cout << std::endl;


    int num = tree->children.size();
    for (int i = 0; i < num; i++) {
        if (tree->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
            scope = "wain";
        }
        printParsetree(tree->children[i], symbolTable, scope, paraTable);
        if (tree->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" && i == 1) {
            std::istringstream iss(tree->children[1]->rule);
            iss >> scope;
            iss >> scope;
        }
        else if (tree->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" && i == num - 1) {
            scope = "global";
        }
    } 
}






void check_dclRule(std::shared_ptr<parseTree> tree, std::map<std::pair<std::string, std::string>, std::string>& symbolTable, std::string& scope, std::map<std::string, std::vector<std::string>>& paraTable) {
    if (tree->children.size() != 0 && tree->rule != "dcls .EMPTY") {
        std::string out, name;
        std::string type = "";

        for (auto it: tree->children[1]->children[0]->children) {
            std::istringstream iss(it->rule);
            std::string slice;
            iss >> slice;
            iss >> slice;
            type += slice;
        }

        std::istringstream iss2(tree->children[1]->children[1]->rule);
        iss2 >> name;
        iss2 >> name;

        if (symbolTable.find(std::make_pair(scope, name)) != symbolTable.end()) {
            std::string errmsg = "ERROR: double declaration with variabel name: " + name;
            throw errmsg;
        } else {
            std::string varType = typeOf(tree->children[3], symbolTable, scope, paraTable);
            //std::cout << name << " " << type << "=" << varType << std::endl;  
            if (varType != type) {
                std::string errmsg = "ERROR: variable value doesn't match type";
                throw errmsg;
            }
            symbolTable[std::make_pair(scope, name)] = type;   
        }
        check_dclRule(tree->children[0], symbolTable, scope, paraTable);              
    }
}




void checkStatement(std::shared_ptr<parseTree> tree, std::map<std::pair<std::string, std::string>, std::string>& symbolTable, std::string& scope, std::map<std::string, std::vector<std::string>>& paraTable) {
    //std::cout << tree->rule << std::endl;
    if (tree->rule == "statements statements statement") {
        for (auto it: tree->children) {
            checkStatement(it, symbolTable, scope, paraTable);
        }
    } else if (tree->rule == "statement lvalue BECOMES expr SEMI") {
        std::string left = typeOf(tree->children[0], symbolTable, scope, paraTable);
        std::string right = typeOf(tree->children[2], symbolTable, scope, paraTable);
        if (left != right) {
            std::string errmsg = "ERROR: left type doesn't match right type";
            throw errmsg;
        }
    } else if (tree->rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
        checkStatement(tree->children[2], symbolTable, scope, paraTable);
        checkStatement(tree->children[5], symbolTable, scope, paraTable);
        checkStatement(tree->children[9], symbolTable, scope, paraTable);
    } else if (tree->rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
        checkStatement(tree->children[2], symbolTable, scope, paraTable);
        checkStatement(tree->children[5], symbolTable, scope, paraTable);
    } else if (tree->rule == "statement PRINTLN LPAREN expr RPAREN SEMI") {
        if (typeOf(tree->children[2], symbolTable, scope, paraTable) != "int") {
            std::string errmsg = "ERROR: type of print is not int";
            throw errmsg;
        }
    } else if (tree->rule == "statement DELETE LBRACK RBRACK expr SEMI") {
        if (typeOf(tree->children[3], symbolTable, scope, paraTable) != "int*") {
            std::string errmsg = "ERROR: type of delete is not int*";
            throw errmsg;
        }        
    } else if (tree->rule.substr(0, 4) == "test") {
        std::string left = typeOf(tree->children[0], symbolTable, scope, paraTable);
        std::string right = typeOf(tree->children[2], symbolTable, scope, paraTable);
        if (left != right) {
            std::string errmsg = "ERROR: unary operation type error";
            throw errmsg;
        }
    }
}



void traverse(std::shared_ptr<parseTree> tree, std::map<std::pair<std::string, std::string>, std::string>& symbolTable, std::string& scope, std::map<std::string, std::vector<std::string>>& paraTable) {
    if (tree->rule == "dcl type ID") {
        //std::istringstream iss(tree->children[0]->rule);
        std::string out, name;
        std::string type = "";

        for (auto it: tree->children[0]->children) {
            std::istringstream iss(it->rule);
            std::string slice;
            iss >> slice;
            iss >> slice;
            type += slice;
        }

        std::istringstream iss2(tree->children[1]->rule);
        iss2 >> name;
        iss2 >> name;
         
        // double declaraion error check
        if (symbolTable.find(std::make_pair(scope, name)) != symbolTable.end()) {
            std::string errmsg = "ERROR: double declaration with variabel name: " + name;
            throw errmsg;
        }
        symbolTable[std::make_pair(scope, name)] = type;
    } 

    else if (tree->rule.substr(0, 4) == "dcls") {
        check_dclRule(tree, symbolTable, scope, paraTable);
    }

    else {
        int num = tree->children.size();
        for (int i = 0; i < num; i++) {
            if (tree->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" && i == 7) {
                checkStatement(tree->children[i], symbolTable, scope, paraTable); 
            }

            if (tree->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" && i == 9) {
                checkStatement(tree->children[i], symbolTable, scope, paraTable);
            }


            // check if main have correct return type
            if (tree->children[i]->rule.substr(0, 4) == "expr" && tree->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
                if (typeOf(tree->children[i], symbolTable, scope, paraTable) != "int") {
                    std::string e = "ERROR: function should have int return type";
                    throw e;
                }
            }

            if (tree->children[i]->rule.substr(0, 4) == "expr" && tree->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE") {
                // check if variable use before declaration
                std::istringstream iss(tree->children[1]->rule);
                std::string funcName;
                iss >> funcName;
                iss >> funcName;
                if (typeOf(tree->children[i], symbolTable, scope, paraTable) != symbolTable[std::make_pair("global", funcName)]) {
                    std::string e = "ERROR: return type doesn't match for function: " + funcName;
                    throw e;
                }
            }


            if (tree->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" && i == 1) {
                std::istringstream iss(tree->children[1]->rule);
                std::string funcName;
                iss >> funcName;
                iss >> funcName;
                if (symbolTable.find(std::make_pair(scope, funcName)) != symbolTable.end()) {
                    std::string errmsg = "ERROR: double declaration with function name: " + funcName;
                    throw errmsg;
                }
                symbolTable[std::make_pair(scope, funcName)] = "int";
                scope = funcName;
                std::vector<std::string> paraType;
                if (tree->children[3]->rule != "params .EMPTY") {
                    getParatype(tree->children[3]->children[0], paraType);
                    paraTable[funcName] = paraType;
                } else {
                    paraTable[funcName] = {};
                }

            
            } else if (tree->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" && i == num - 1) {
                scope = "global";
            } else if (tree->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" && i == 0) {
                scope = "wain";
            }

            traverse(tree->children[i], symbolTable, scope, paraTable);

            // check if main 2nd parameter is int
            if (tree->rule == "main INT WAIN LPAREN dcl COMMA dcl RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" && i == 5) {
                if (typeOf(tree->children[i]->children[1], symbolTable, scope, paraTable) != "int") {
                    std::string e = "ERROR: Wain should has INT type for second parameter";
                    throw e; 
                }
            }             
        }
    }
}





int main() {
    // reconstruct parse tree
    parseTree parset = {};
    std::shared_ptr<parseTree> parseT = std::make_shared<parseTree>(parset);
    reconstruct(parseT);


    std::map<std::pair<std::string, std::string>, std::string> symbolTable;
    std::map<std::string, std::vector<std::string>> paraTable;
    std::string scope = "global";
    try {
        traverse(parseT, symbolTable, scope, paraTable);
    } catch (std::string& errmsg) {
        std::cerr << errmsg << std::endl;
        return 1;
    }
    scope = "global";
    printParsetree(parseT, symbolTable, scope, paraTable);
}