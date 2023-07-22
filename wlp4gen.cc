#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <memory>
#include <algorithm>

struct parseTree {
    std::string rule;
    std::vector<std::shared_ptr<parseTree>> children;
};

void printParseTree(std::shared_ptr<parseTree>& P) {
    std::cout << P->rule << std::endl;
    for (auto it: P->children) {
        printParseTree(it);
    }
}

void reconstruct(std::shared_ptr<parseTree>& P) {
    std::string line;
    if (std::getline(std::cin, line) && !line.empty()) {
        P->rule = line;
        std::istringstream iss(line);

        std::string cur;
        iss >> cur;
        if (std::isupper(cur[0])) {return;}

        while (iss >> cur) {
            if (cur == ".EMPTY") {return;}
            if (cur == ":") {return;}
            parseTree push = {};
            std::shared_ptr<parseTree> newAdd = std::make_shared<parseTree>(push);
            P->children.push_back(newAdd);
            reconstruct(P->children.back());
        }
    }
    return;   
}

std::string getFirst(std::string& rule) {
    std::istringstream iss(rule);
    std::string out;
    iss >> out;
    iss >> out;
    return out;
}

std::string dataType(std::string& rule) {
    std::istringstream iss(rule);
    std::string type = "";
    while (iss >> type) {
        if (type == ":") {
            iss >> type;
            return type;
        }
    }
    return type;
}

void dcls(std::shared_ptr<parseTree>& tree, std::map<std::string, int>& offsetTable, int& offset, std::map<std::string, int>& valueTable, std::vector<std::string>& usedVar) {
    if (tree->rule != "dcls .EMPTY") {
        dcls(tree->children[0], offsetTable, offset, valueTable, usedVar);
        std::string name = getFirst(tree->children[1]->children[1]->rule);
        if (std::find(usedVar.begin(), usedVar.end(), name) != usedVar.end()) {
            std::istringstream iss(tree->children[3]->rule);
            std::string num;
            iss >> num;
            iss >> num;
            offsetTable[name] = offset;
            offset -= 4;
            std::cout << "lis $3" << std::endl;
    
            if (tree->children[3]->rule.substr(0, 3) == "NUM") {
                std::cout << ".word " << num << std::endl;
                valueTable[name] = std::stoi(num);
            } else { // for NULL
                std::cout << ".word 1" << std::endl;
                valueTable[name] = 1;
            }
            std::cout << "sw $3, -4($30)" << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;
            //if (std::find(usedVarStatement.begin(), usedVarStatement.end(), name) != usedVarStatement.end()) {
            //    valueTable.erase(name);
            //}
        }
    }
}

void store3() {
    std::cout << "sw $3, -4($30)" << std::endl; 
    std::cout << "sub $30, $30, $4" << std::endl;
}

void load5() {
    std::cout << "add $30, $30, $4" << std::endl; 
    std::cout << "lw $5, -4($30)" << std::endl;    
}

std::string getOringinRule(std::string &rule) {
    std::string origin = "";
    std::istringstream iss(rule);
    std::string temp;
    while (iss >> temp) {
        if (temp == ":") {
            break;
        }
        origin += temp + " ";
    }
    origin = origin.substr(0, origin.size() - 1);
    return origin;
}

int binaryOperationcheck(std::shared_ptr<parseTree> tree, std::map<std::string, int>& valueTable) {
    if (getOringinRule(tree->rule) == "factor ID") {
        std::string name = getFirst(tree->children[0]->rule);
        if (valueTable.find(name) != valueTable.end()) {
            return valueTable[name];
        } else {
            std::string err;
            throw err;
        }
    }

    else if (getOringinRule(tree->rule) == "factor NUM") {
        return std::stoi(getFirst(tree->children[0]->rule));
    }

    else if (getOringinRule(tree->rule) == "expr term" || getOringinRule(tree->rule) == "term factor") {
        return binaryOperationcheck(tree->children[0], valueTable);
    }

    else if (getOringinRule(tree->rule) == "expr expr PLUS term") {
        std::shared_ptr<parseTree> expr = tree->children[0];
        std::shared_ptr<parseTree> term = tree->children[2];
        if (dataType(expr->rule) == "int" && dataType(term->rule) == "int") {
            int left = binaryOperationcheck(expr, valueTable);
            int right = binaryOperationcheck(term, valueTable);
            return left + right;
        } 
        
        else if (dataType(expr->rule) == "int*" && dataType(term->rule) == "int") {
            int left = binaryOperationcheck(expr, valueTable);
            int right = binaryOperationcheck(term, valueTable);
            return left + (right * 4);
        }

        else {
            int left = binaryOperationcheck(expr, valueTable);
            int right = binaryOperationcheck(term, valueTable);
            return (left * 4) + right;
        }
    }

    else if (getOringinRule(tree->rule) == "expr expr MINUS term") {
        std::shared_ptr<parseTree> expr = tree->children[0];
        std::shared_ptr<parseTree> term = tree->children[2];
        if (dataType(expr->rule) == "int" && dataType(term->rule) == "int") {
            int left = binaryOperationcheck(expr, valueTable);
            int right = binaryOperationcheck(term, valueTable);
            return left - right;
        }

        else if (dataType(expr->rule) == "int*" && dataType(term->rule) == "int") {
            int left = binaryOperationcheck(expr, valueTable);
            int right = binaryOperationcheck(term, valueTable);
            return left - (right * 4);
        } 

        else {
            int left = binaryOperationcheck(expr, valueTable);
            int right = binaryOperationcheck(term, valueTable);
            return (left - right) / 4;
        }               
    }

    else if (getOringinRule(tree->rule) == "factor LPAREN expr RPAREN") {
        return binaryOperationcheck(tree->children[1], valueTable);
    }

    else if(getOringinRule(tree->rule) == "term term STAR factor") {
        int left = binaryOperationcheck(tree->children[0], valueTable);
        int right = binaryOperationcheck(tree->children[2], valueTable);
        return left * right;
    }

    else if (getOringinRule(tree->rule) == "term term SLASH factor") {
        int left = binaryOperationcheck(tree->children[0], valueTable);
        int right = binaryOperationcheck(tree->children[2], valueTable);
        return left / right;      
    }

    else if (getOringinRule(tree->rule) == "term term PCT factor") {
        int left = binaryOperationcheck(tree->children[0], valueTable);
        int right = binaryOperationcheck(tree->children[2], valueTable);
        return left % right;          
    }

    else {
        std::string err;
        throw err;
    }
}

bool isSubstring(std::string substring, std::string fullString) {
    return fullString.find(substring) != std::string::npos;
}

std::string stringcheck(std::shared_ptr<parseTree>& tree) {
    std::string get = "";
    if (getOringinRule(tree->rule) == "factor ID") {
        get = getFirst(tree->children[0]->rule);
    }

    else if (getOringinRule(tree->rule) == "factor NUM") {
        get = getFirst(tree->children[0]->rule);
    }

    else if (getOringinRule(tree->rule) == "expr term" || getOringinRule(tree->rule) == "term factor") {
        get = stringcheck(tree->children[0]);
    }

    else if (getOringinRule(tree->rule) == "expr expr PLUS term") {
        std::string left = stringcheck(tree->children[0]);
        std::string right = stringcheck(tree->children[2]);
        get = "(" + left + "+" + right + ")";
    }

    else if (getOringinRule(tree->rule) == "expr expr MINUS term") {
        std::string left = stringcheck(tree->children[0]);
        std::string right = stringcheck(tree->children[2]);
        get = "(" + left + "-" + right + ")";              
    }

    else if (getOringinRule(tree->rule) == "factor LPAREN expr RPAREN") {
        get = stringcheck(tree->children[1]);
    }

    else if(getOringinRule(tree->rule) == "term term STAR factor") {
        std::string left = stringcheck(tree->children[0]);
        std::string right = stringcheck(tree->children[2]);
        get = "(" + left + "*" + right + ")";       
    }

    else if (getOringinRule(tree->rule) == "term term SLASH factor") {
        std::string left = stringcheck(tree->children[0]);
        std::string right = stringcheck(tree->children[2]);
        get = "(" + left + "/" + right + ")";     
    }

    else if (getOringinRule(tree->rule) == "term term PCT factor") {
        std::string left = stringcheck(tree->children[0]);
        std::string right = stringcheck(tree->children[2]);
        get = "(" + left + "%" + right + ")";         
    }

    else if (getOringinRule(tree->rule) == "factor ID LPAREN RPAREN") {
        get =  getFirst(tree->children[0]->rule) + "()";
    }

    else {
        get = "ERROR";
    }

    return get;
}

void exprEvaluate(std::shared_ptr<parseTree>& tree, std::map<std::string, int>& offsetTable, std::map<std::string, int>& valueTable) {
    if (getOringinRule(tree->rule) == "factor ID") {
        std::string name = getFirst(tree->children[0]->rule);
        int offset = offsetTable[name];
        std::cout << "lw $3, " << offset << "($29)" << std::endl;
    }

    else if (getOringinRule(tree->rule) == "factor NUM") {
        std::cout << "lis $3" << std::endl;
        std::cout << ".word " << getFirst(tree->children[0]->rule) << std::endl;
    }

    else if (getOringinRule(tree->rule) == "factor NULL") {
        std::cout << "add $3, $0, $11 " << std::endl;
    }

    else if (getOringinRule(tree->rule) == "expr term" || getOringinRule(tree->rule) == "term factor") {
        exprEvaluate(tree->children[0], offsetTable, valueTable);
    }

    else if (getOringinRule(tree->rule) == "expr expr PLUS term") {
        try {
            int value = binaryOperationcheck(tree, valueTable);
            std::cout << "lis $3" << std::endl;
            std::cout << ".word " << std::to_string(value) << std::endl;
        } catch (std::string err) {
            std::shared_ptr<parseTree> expr = tree->children[0];
            std::shared_ptr<parseTree> term = tree->children[2];

            if (dataType(expr->rule) == "int" && dataType(term->rule) == "int") {
                std::string left = stringcheck(expr);
                std::string right = stringcheck(term);
                if (left == right && !isSubstring("ERROR", left) && !isSubstring("ERROR", right)) {
                    exprEvaluate(expr, offsetTable, valueTable);
                    std::cout << "add $3, $3, $3" << std::endl;                                            
                } else {
                    try {
                        int val = binaryOperationcheck(expr, valueTable);
                        exprEvaluate(term, offsetTable, valueTable);
                        std::cout << "lis $5" << std::endl;    
                        std::cout << ".word " << val << std::endl;    
                        std::cout << "add $3, $3, $5" << std::endl;    
                    } catch (std::string err) {
                        try {
                            int val = binaryOperationcheck(term, valueTable);
                            exprEvaluate(expr, offsetTable, valueTable);
                            std::cout << "lis $5" << std::endl;    
                            std::cout << ".word " << val << std::endl;    
                            std::cout << "add $3, $3, $5" << std::endl; 
                        } catch (std::string err) {
                            exprEvaluate(expr, offsetTable, valueTable);
                            store3();
                            exprEvaluate(term, offsetTable, valueTable);
                            load5();
                            std::cout << "add $3, $3, $5" << std::endl; 
                        }                    
                    }                      
                }       
            } 
            
            else if (dataType(expr->rule) == "int*" && dataType(term->rule) == "int") {
                try {
                    int val = binaryOperationcheck(expr, valueTable);
                    exprEvaluate(term, offsetTable, valueTable);
                    std::cout << "mult $3, $4" << std::endl;
                    std::cout << "mflo $3" << std::endl;                                        
                    std::cout << "lis $5" << std::endl;    
                    std::cout << ".word " << val << std::endl;    
                    std::cout << "add $3, $3, $5" << std::endl;    
                } catch (std::string err) {
                    try {
                        int val = binaryOperationcheck(term, valueTable) * 4;
                        exprEvaluate(expr, offsetTable, valueTable);
                        std::cout << "lis $5" << std::endl;    
                        std::cout << ".word " << val << std::endl;    
                        std::cout << "add $3, $3, $5" << std::endl; 
                    } catch (std::string err) {
                        exprEvaluate(expr, offsetTable, valueTable);
                        store3();
                        exprEvaluate(term, offsetTable, valueTable);
                        std::cout << "mult $3, $4" << std::endl;
                        std::cout << "mflo $3" << std::endl;
                        load5();
                        std::cout << "add $3, $3, $5" << std::endl;
                    }                    
                } 
            }
    
            else if (dataType(expr->rule) == "int" && dataType(term->rule) == "int*") {
                try {
                    int val = binaryOperationcheck(expr, valueTable) * 4;
                    exprEvaluate(term, offsetTable, valueTable);                                       
                    std::cout << "lis $5" << std::endl;    
                    std::cout << ".word " << val << std::endl;    
                    std::cout << "add $3, $3, $5" << std::endl;    
                } catch (std::string err) {
                    try {
                        int val = binaryOperationcheck(term, valueTable);
                        exprEvaluate(expr, offsetTable, valueTable);
                        std::cout << "mult $3, $4" << std::endl;
                        std::cout << "mflo $3" << std::endl;                        
                        std::cout << "lis $5" << std::endl;    
                        std::cout << ".word " << val << std::endl;    
                        std::cout << "add $3, $3, $5" << std::endl; 
                    } catch (std::string err) {
                        exprEvaluate(expr, offsetTable, valueTable);
                        std::cout << "mult $3, $4" << std::endl;
                        std::cout << "mflo $3" << std::endl;
                        store3();
                        exprEvaluate(term, offsetTable, valueTable);
                        load5();
                        std::cout << "add $3, $3, $5" << std::endl;
                    }                    
                }
            }
        }
    }

    else if (getOringinRule(tree->rule) == "expr expr MINUS term") {
        try {
            int value = binaryOperationcheck(tree, valueTable);
            std::cout << "lis $3" << std::endl;
            std::cout << ".word " << std::to_string(value) << std::endl;            
        } catch (std::string err) {
            std::shared_ptr<parseTree> expr = tree->children[0];
            std::shared_ptr<parseTree> term = tree->children[2];
            if (dataType(expr->rule) == "int" && dataType(term->rule) == "int") {
                std::string left = stringcheck(expr);
                std::string right = stringcheck(term);
                if (left != right || isSubstring("ERROR", left) || isSubstring("ERROR", right)) {
                    exprEvaluate(expr, offsetTable, valueTable);
                    store3();
                    exprEvaluate(term, offsetTable, valueTable);
                    load5();
                    std::cout << "sub $3, $5, $3" << std::endl;
                } else {
                    std::cout << "add $3, $0, $0" << std::endl;
                }
            }
    
            else if (dataType(expr->rule) == "int*" && dataType(term->rule) == "int") {
                exprEvaluate(expr, offsetTable, valueTable);
                store3();
                exprEvaluate(term, offsetTable, valueTable);
                std::cout << "mult $3, $4" << std::endl;
                std::cout << "mflo $3" << std::endl;
                load5();
                std::cout << "sub $3, $5, $3" << std::endl;
            } 
    
            else if (dataType(expr->rule) == "int*" && dataType(term->rule) == "int*") {
                exprEvaluate(expr, offsetTable, valueTable);
                store3();
                exprEvaluate(term, offsetTable, valueTable);
                load5();
                std::cout << "sub $3, $5, $3" << std::endl;
                std::cout << "div $3, $4" << std::endl;
                std::cout << "mflo $3" << std::endl;
            }   
        }            
    }

    else if (getOringinRule(tree->rule) == "factor LPAREN expr RPAREN" ||
             getOringinRule(tree->rule) == "lvalue LPAREN lvalue RPAREN") {
        exprEvaluate(tree->children[1], offsetTable, valueTable);
    }

    else if (getOringinRule(tree->rule) == "term term STAR factor") {
        try {
            int value = binaryOperationcheck(tree, valueTable);
            std::cout << "lis $3" << std::endl;
            std::cout << ".word " << std::to_string(value) << std::endl;             
        } catch (std::string err) {
            //std::string left = stringcheck(tree->children[0]);
            //std::string right = stringcheck(tree->children[2]);
            //if (left != right || isSubstring("ERROR", left) || isSubstring("ERROR", right)) {
                exprEvaluate(tree->children[0], offsetTable, valueTable);
                store3();
                exprEvaluate(tree->children[2], offsetTable, valueTable);
                load5();
                std::cout << "mult $3, $5" << std::endl;
                std::cout << "mflo $3" << std::endl;
            //} else {
            //    exprEvaluate(tree->children[0], offsetTable, valueTable);
            //    std::cout << "mult $3, $3" << std::endl;
            //    std::cout << "mflo $3" << std::endl;
            //}
        }
    }

    else if (getOringinRule(tree->rule) == "term term SLASH factor") {
        try {
            int value = binaryOperationcheck(tree, valueTable);
            std::cout << "lis $3" << std::endl;
            std::cout << ".word " << std::to_string(value) << std::endl;             
        } catch (std::string err) {
            std::string left = stringcheck(tree->children[0]);
            std::string right = stringcheck(tree->children[2]);
            if (left != right || isSubstring("ERROR", left) || isSubstring("ERROR", right)) {
                exprEvaluate(tree->children[0], offsetTable, valueTable);
                store3();
                exprEvaluate(tree->children[2], offsetTable, valueTable);
                load5();
                std::cout << "div $5, $3" << std::endl;
                std::cout << "mflo $3" << std::endl; 
            } else {
                std::cout << "lis $3" << std::endl;
                std::cout << ".word 1" << std::endl;
            }            
        }  
    }

    else if (getOringinRule(tree->rule) == "term term PCT factor") {
        try {
            int value = binaryOperationcheck(tree, valueTable);
            std::cout << "lis $3" << std::endl;
            std::cout << ".word " << std::to_string(value) << std::endl;             
        } catch (std::string err) {
            std::string left = stringcheck(tree->children[0]);
            std::string right = stringcheck(tree->children[2]);
            if (left != right || isSubstring("ERROR", left) || isSubstring("ERROR", right)) {
                exprEvaluate(tree->children[0], offsetTable, valueTable);
                store3();
                exprEvaluate(tree->children[2], offsetTable, valueTable);
                load5();
                std::cout << "div $5, $3" << std::endl;
                std::cout << "mfhi $3" << std::endl; 
            } else {
                std::cout << "add $3, $0, $0" << std::endl;
            }
        }                 
    }

    else if (getOringinRule(tree->rule) == "factor AMP lvalue") {
        std::shared_ptr<parseTree> cur = tree->children[1];
        while (true) {
            if (getOringinRule(cur->rule) == "lvalue LPAREN lvalue RPAREN") {
                cur = cur->children[1];
            }
            if (getOringinRule(cur->rule) == "lvalue ID") {
                std::string name = getFirst(cur->children[0]->rule);
                int offset = offsetTable[name];
                std::cout << "lis $3" << std::endl;
                std::cout << ".word " << offset << std::endl;
                std::cout << "add $3, $3, $29" << std::endl;
                break;
            } else if (getOringinRule(cur->rule) == "lvalue STAR factor") {
                exprEvaluate(cur->children[1], offsetTable, valueTable);
                break;
            }
        }
    }

    else if (getOringinRule(tree->rule) == "factor STAR factor") {
        exprEvaluate(tree->children[1], offsetTable, valueTable);
        std::cout << "lw $3, 0($3)" << std::endl; 
    }

    else if (getOringinRule(tree->rule) == "factor NEW INT LBRACK expr RBRACK") {
        exprEvaluate(tree->children[3], offsetTable, valueTable);
        std::cout << "add $1, $3, $0" << std::endl;
        std::cout << "sw $31, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "lis $5" << std::endl;
        std::cout << ".word new" << std::endl;
        std::cout << "jalr $5" << std::endl;
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $31, -4($30)" << std::endl;
        std::cout << "bne $3, $0, 1" << std::endl;
        std::cout << "add $3, $11, $0" << std::endl;
    }

    else if (getOringinRule(tree->rule) == "factor ID LPAREN RPAREN") {
        std::string name = "F" + getFirst(tree->children[0]->rule);
        std::cout << "sw $29, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "sw $31, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;

        std::cout << "lis $5" << std::endl;
        std::cout << ".word " << name << std::endl;
        std::cout << "jalr $5 " << std::endl;

        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $31, -4($30)" << std::endl;
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $29, -4($30)" << std::endl;
    }

    else if (getOringinRule(tree->rule) == "factor ID LPAREN arglist RPAREN") {
        std::string name = "F" + getFirst(tree->children[0]->rule);
        std::cout << "sw $29, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "sw $31, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;

        std::shared_ptr<parseTree> cur = tree->children[2];
        while (true) {
            exprEvaluate(cur->children[0], offsetTable, valueTable);
            std::cout << "sw $3, -4($30)" << std::endl;
            std::cout << "sub $30, $30, $4" << std::endl;

            if (getOringinRule(cur->rule) == "arglist expr COMMA arglist") {
                cur = cur->children[2];
            } else {
                break;
            }
        }

        std::cout << "lis $5" << std::endl;
        std::cout << ".word " << name << std::endl;
        std::cout << "jalr $5" << std::endl;

        cur = tree->children[2];
        while (true) {
            std::cout << "add $30, $30, $4" << std::endl;
            if (getOringinRule(cur->rule) == "arglist expr COMMA arglist") {
                cur = cur->children[2];
            } else {
                break;
            }
        }

        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $31, -4($30)" << std::endl;
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $29, -4($30)" << std::endl;       
    }
}

std::string createLabel(int& labelCount) {
    std::string get = "label" + std::to_string(labelCount);
    labelCount++;                                                                                                                                                               
    return get;
}

std::string getTestInfo(std::shared_ptr<parseTree>& tree) {
    std::string left = stringcheck(tree->children[0]);
    std::string right = stringcheck(tree->children[2]);
    std::istringstream iss(tree->rule);
    std::string op;
    iss >> op;
    iss >> op;
    iss >> op;
    return left + op + right;
}

void checkDeadcode(std::shared_ptr<parseTree>& tree) {
    if (tree->rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
        std::string left = stringcheck(tree->children[2]->children[0]);
        std::string right = stringcheck(tree->children[2]->children[2]);
        std::istringstream iss(tree->children[2]->rule);
        std::string op;
        iss >> op;
        iss >> op;
        iss >> op;

        std::shared_ptr<parseTree> cur = tree->children[5];
        while (true) {
            if (cur->rule == "statements .EMPTY") {break;}
            else if (cur->rule == "statements statements statement") {
                if (cur->children[1]->rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE" ||
                    cur->children[1]->rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
                    std::string doing = getTestInfo(cur->children[1]->children[2]);

                    bool deleteIF = false;
                    bool deleteELSE = false;
    
                    if (op == "EQ") {
                        if (doing == left + "NE" + right || doing == right + "NE" + left ||
                            doing == left + "LT" + right || doing == right + "LT" + left ||
                            doing == left + "GT" + right || doing == right + "GT" + left) {
                            deleteIF = true;
                        }
    
                        else if (doing == left + "EQ" + right || doing == right + "EQ" + left ||
                                 doing == left + "LE" + right || doing == right + "LE" + left ||
                                 doing == left + "GE" + right || doing == right + "GE" + left) {
                            deleteELSE = true;
                        }
                    }
    
                    else if (op == "NE") {
                        if (doing == left + "EQ" + right || doing == right + "EQ" + left) {
                            deleteIF = true;
                        }
    
                        else if (doing == left + "NE" + right || doing == right + "NE" + left) {
                            deleteELSE = true;
                        }
                    }
    
                    else if (op == "LT") {
                        if (doing == left + "EQ" + right || doing == right + "EQ" + left ||
                            doing == left + "GT" + right || doing == right + "LT" + left ||
                            doing == left + "GE" + right || doing == right + "LE" + left) {
                            deleteIF = true;
                        }
    
                        else if (doing == left + "NE" + right || doing == right + "NE" + left ||
                                 doing == left + "LT" + right || doing == right + "GT" + left ||
                                 doing == left + "LE" + right || doing == right + "GE" + left) {
                            deleteELSE = true;
                        }
                    }
    
                    else if (op == "GT") {
                        if (doing == right + "EQ" + left || doing == left + "EQ" + right ||
                            doing == right + "GT" + left || doing == left + "LT" + right ||
                            doing == right + "GE" + left || doing == left + "LE" + right) {
                            deleteIF = true;
                        }
    
                        else if (doing == right + "NE" + left || doing == left + "NE" + right ||
                                 doing == right + "LT" + left || doing == left + "GT" + right ||
                                 doing == right + "LE" + left || doing == left + "GE" + right) {
                            deleteELSE = true;
                        }
                    }
    
                    else if (op == "LE") {
                        if (doing == left + "GT" + right || doing == right + "LT" + left) {
                            deleteIF = true;
                        }
    
                        else if (doing == left + "LE" + right || doing == right + "GE" + left) {
                            deleteELSE = true;
                        }
                    } 
    
                    else if (op == "GE") {
                        if (doing == left + "LT" + right || doing == right + "GT" + left) {
                            deleteIF = true;
                        }
    
                        else if (doing == right + "LE" + left || doing == left + "GE" + right) {
                            deleteELSE = true;
                        }
                    }
    
                    // if
                    if (cur->children[1]->rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
                        if (deleteIF) {
                            cur->children[1]->children[5]->rule = "statements .EMPTY";
                            cur->children[1]->children[5]->children.clear();
                        } else if (deleteELSE) {
                            cur->children[1]->children[9]->rule = "statements .EMPTY";
                            cur->children[1]->children[9]->children.clear();
                        }                        
                    } 
                    // while
                    else {
                        if (deleteIF) {
                            cur->children[1]->children[5]->rule = "statements .EMPTY";
                            cur->children[1]->children[5]->children.clear();
                        }
                    }                                                       
                }
                cur = cur->children[0];                    
            } 
        }
        
        cur = tree->children[9];
        if (op == "EQ") {op = "NE";}
        else if (op == "NE") {op = "EQ";}
        else if (op == "LT") {op = "GE";}
        else if (op == "LE") {op = "GT";}
        else if (op == "GE") {op = "LT";}
        else if (op == "GT") {op = "LE";}

        while (true) {
            if (cur->rule == "statements .EMPTY") {break;}
            else if (cur->rule == "statements statements statement") {
                if (cur->children[1]->rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE" ||
                    cur->children[1]->rule == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
                    std::string doing = getTestInfo(cur->children[1]->children[2]);

                    bool deleteIF = false;
                    bool deleteELSE = false;
    
                    if (op == "EQ") {
                        if (doing == left + "NE" + right || doing == right + "NE" + left ||
                            doing == left + "LT" + right || doing == right + "LT" + left ||
                            doing == left + "GT" + right || doing == right + "GT" + left) {
                            deleteIF = true;
                        }
    
                        else if (doing == left + "EQ" + right || doing == right + "EQ" + left ||
                                 doing == left + "LE" + right || doing == right + "LE" + left ||
                                 doing == left + "GE" + right || doing == right + "GE" + left) {
                            deleteELSE = true;
                        }
                    }
    
                    else if (op == "NE") {
                        if (doing == left + "EQ" + right || doing == right + "EQ" + left) {
                            deleteIF = true;
                        }
    
                        else if (doing == left + "NE" + right || doing == right + "NE" + left) {
                            deleteELSE = true;
                        }
                    }
    
                    else if (op == "LT") {
                        if (doing == left + "EQ" + right || doing == right + "EQ" + left ||
                            doing == left + "GT" + right || doing == right + "LT" + left ||
                            doing == left + "GE" + right || doing == right + "LE" + left) {
                            deleteIF = true;
                        }
    
                        else if (doing == left + "NE" + right || doing == right + "NE" + left ||
                                 doing == left + "LT" + right || doing == right + "GT" + left ||
                                 doing == left + "LE" + right || doing == right + "GE" + left) {
                            deleteELSE = true;
                        }
                    }
    
                    else if (op == "GT") {
                        if (doing == right + "EQ" + left || doing == left + "EQ" + right ||
                            doing == right + "GT" + left || doing == left + "LT" + right ||
                            doing == right + "GE" + left || doing == left + "LE" + right) {
                            deleteIF = true;
                        }
    
                        else if (doing == right + "NE" + left || doing == left + "NE" + right ||
                                 doing == right + "LT" + left || doing == left + "GT" + right ||
                                 doing == right + "LE" + left || doing == left + "GE" + right) {
                            deleteELSE = true;
                        }
                    }
    
                    else if (op == "LE") {
                        if (doing == left + "GT" + right || doing == right + "LT" + left) {
                            deleteIF = true;
                        }
    
                        else if (doing == left + "LE" + right || doing == right + "GE" + left) {
                            deleteELSE = true;
                        }
                    } 
    
                    else if (op == "GE") {
                        if (doing == left + "LT" + right || doing == right + "GT" + left) {
                            deleteIF = true;
                        }
    
                        else if (doing == right + "LE" + left || doing == left + "GE" + right) {
                            deleteELSE = true;
                        }
                    }
    
                    // if
                    if (cur->children[1]->rule == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
                        if (deleteIF) {
                            cur->children[1]->children[5]->rule = "statements .EMPTY";
                            cur->children[1]->children[5]->children.clear();
                        } else if (deleteELSE) {
                            cur->children[1]->children[9]->rule = "statements .EMPTY";
                            cur->children[1]->children[9]->children.clear();
                        }                        
                    } 
                    // while
                    else {
                        if (deleteIF) {
                            cur->children[1]->children[5]->rule = "statements .EMPTY";
                            cur->children[1]->children[5]->children.clear();
                        }
                    }                                                       
                }
                cur = cur->children[0];                    
            } 
        }
    } 
}

void statementEvaluate(std::shared_ptr<parseTree>& tree, std::map<std::string, int>& offsetTable, int& labelCount, std::map<std::string, int>& valueTable) {
    if (getOringinRule(tree->rule) == "statements statements statement") {
        statementEvaluate(tree->children[0], offsetTable, labelCount, valueTable);
        statementEvaluate(tree->children[1], offsetTable, labelCount, valueTable);
    }

    else if (getOringinRule(tree->rule) == "statement lvalue BECOMES expr SEMI") {
        exprEvaluate(tree->children[2], offsetTable, valueTable);
        std::shared_ptr<parseTree> cur = tree->children[0];
        while (true) {
            if (getOringinRule(cur->rule) == "lvalue LPAREN lvalue RPAREN") {
                cur = cur->children[1];
            } else if (getOringinRule(cur->rule) == "lvalue STAR factor") {
                store3();
                exprEvaluate(cur->children[1], offsetTable, valueTable);
                load5();
                std::cout << "sw $5, 0($3)" << std::endl;
                break;                
            } else if (getOringinRule(cur->rule) == "lvalue ID") {
                std::string name = getFirst(cur->children[0]->rule);
                int offset = offsetTable[name];
                std::cout << "sw $3, " << offset << "($29)" << std::endl;
                break;                
            }
        }
    }

    else if (getOringinRule(tree->rule) == "statement IF LPAREN test RPAREN LBRACE statements RBRACE ELSE LBRACE statements RBRACE") {
        checkDeadcode(tree);
        int jump = 0;
        try {
            int left = binaryOperationcheck(tree->children[2]->children[0], valueTable);
            int right = binaryOperationcheck(tree->children[2]->children[2], valueTable);
            bool result;
            std::istringstream iss(tree->children[2]->rule);
            std::string op;
            iss >> op;
            iss >> op;
            iss >> op;
            if (op == "EQ") {result = (left == right);}
            else if (op == "NE") {result = (left != right);}
            else if (op == "LT") {result = (left < right);}
            else if (op == "LE") {result = (left <= right);}
            else if (op == "GE") {result = (left >= right);}
            else {result = (left > right);}

            if (!result) {
                tree->children[5]->rule = "statements .EMPTY";
                tree->children[5]->children.clear();
                jump = 2;
            } else {
                tree->children[9]->rule = "statements .EMPTY";
                tree->children[9]->children.clear();
                jump = 1;
            }
        } catch (std::string err) {}

        std::string label1 = createLabel(labelCount);
        std::string label2 = createLabel(labelCount);
        
        // test
        if (jump == 0) {
            statementEvaluate(tree->children[2], offsetTable, labelCount, valueTable);
            std::cout << "beq $3, $0, " << label1 << std::endl;
        } else if (jump == 1) {

        } else if (jump == 2) {
            std::cout << "beq $3, $0, " << label1 << std::endl;
        }

        // do if
        statementEvaluate(tree->children[5], offsetTable, labelCount, valueTable);
        std::cout << "beq $0, $0, " << label2 << std::endl;

        std::cout << label1 << ":" << std::endl;
        // do else
        statementEvaluate(tree->children[9], offsetTable, labelCount, valueTable);
        std::cout << label2 << ":" << std::endl;
    }

    else if (getOringinRule(tree->rule) == "statement WHILE LPAREN test RPAREN LBRACE statements RBRACE") {
        std::string label1 = createLabel(labelCount);
        std::string label2 = createLabel(labelCount);

        // begin of loop
        std::cout << label1 << ":" << std::endl;
        statementEvaluate(tree->children[2], offsetTable, labelCount, valueTable);
        std::cout << "beq $3, $0, " << label2 << std::endl;
        statementEvaluate(tree->children[5], offsetTable, labelCount, valueTable);
        std::cout << "beq $0, $0, " << label1 << std::endl;
        std::cout << label2 << ":" << std::endl;
    }

    // ==
    else if (getOringinRule(tree->rule) == "test expr EQ expr") {
        exprEvaluate(tree->children[0], offsetTable, valueTable);
        store3();
        exprEvaluate(tree->children[2], offsetTable, valueTable);
        load5();
        std::cout << "slt $6, $5, $3" << std::endl;
        std::cout << "slt $7, $3, $5" << std::endl;
        // if left == right, $3 = 0, else $3 = 1
        std::cout << "add $3, $6, $7" << std::endl;
        // convert 0 to 1 and 1 to 0
        std::cout << "slt $3, $3, $11" << std::endl;
    }

    // !=
    else if (getOringinRule(tree->rule) == "test expr NE expr") {
        exprEvaluate(tree->children[0], offsetTable, valueTable);
        store3();
        exprEvaluate(tree->children[2], offsetTable, valueTable);
        load5();
        std::cout << "slt $6, $5, $3" << std::endl;
        std::cout << "slt $7, $3, $5" << std::endl;
        std::cout << "add $3, $6, $7" << std::endl;
    }

    // <
    else if (getOringinRule(tree->rule) == "test expr LT expr") {
        exprEvaluate(tree->children[0], offsetTable, valueTable);
        store3();
        exprEvaluate(tree->children[2], offsetTable, valueTable);
        load5();
        if (dataType(tree->children[0]->rule) == "int*") {
            std::cout << "sltu $3, $5, $3" << std::endl;
        } else {
            std::cout << "slt $3, $5, $3" << std::endl;
        }
    }

    // <=
    else if (getOringinRule(tree->rule) == "test expr LE expr") {
        exprEvaluate(tree->children[0], offsetTable, valueTable);
        store3();
        exprEvaluate(tree->children[2], offsetTable, valueTable);
        load5();
        if (dataType(tree->children[0]->rule) == "int*") {
            std::cout << "sltu $3, $3, $5" << std::endl;
        } else {
            std::cout << "slt $3, $3, $5" << std::endl;    
        }
        std::cout << "sub $3, $11, $3" << std::endl;        
    }

    // >=
    else if (getOringinRule(tree->rule) == "test expr GE expr") {
        exprEvaluate(tree->children[0], offsetTable, valueTable);
        store3();
        exprEvaluate(tree->children[2], offsetTable, valueTable);
        load5();
        if (dataType(tree->children[0]->rule) == "int*") {
            std::cout << "sltu $3, $5, $3" << std::endl;
        } else {
            std::cout << "slt $3, $5, $3" << std::endl;    
        }
        std::cout << "sub $3, $11, $3" << std::endl;
    }

    // >
    else if (getOringinRule(tree->rule) == "test expr GT expr") {
        exprEvaluate(tree->children[0], offsetTable, valueTable);
        store3();
        exprEvaluate(tree->children[2], offsetTable, valueTable);
        load5();
        if (dataType(tree->children[0]->rule) == "int*") {
            std::cout << "sltu $3, $3, $5" << std::endl;
        } else {
            std::cout << "slt $3, $3, $5" << std::endl;    
        }
    }

    // print
    else if (getOringinRule(tree->rule) == "statement PRINTLN LPAREN expr RPAREN SEMI") {
        std::cout << "sw $1, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;

        exprEvaluate(tree->children[2], offsetTable, valueTable);

        std::cout << "add $1, $3, $0" << std::endl;

        std::cout << "sw $31, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;

        std::cout << "lis $5" << std::endl;
        std::cout << ".word print" << std::endl;
        std::cout << "jalr $5" << std::endl;

        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $31, -4($30)" << std::endl;

        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $1, -4($30)" << std::endl;
    }

    else if (getOringinRule(tree->rule) == "statement DELETE LBRACK RBRACK expr SEMI") {
        exprEvaluate(tree->children[3], offsetTable, valueTable);
        std::string label = createLabel(labelCount);
        std::cout << "beq $3, $11, " << label << std::endl;
        std::cout << "add $1, $3, $0" << std::endl;
        std::cout << "sw $31, -4($30)" << std::endl;
        std::cout << "sub $30, $30, $4" << std::endl;
        std::cout << "lis $5" << std::endl;
        std::cout << ".word delete" << std::endl;
        std::cout << "jalr $5" << std::endl;
        std::cout << "add $30, $30, $4" << std::endl;
        std::cout << "lw $31, -4($30)" << std::endl;
        std::cout << label << ":" << std::endl;
    }
}

void getUsedvar(std::shared_ptr<parseTree>& tree, std::vector<std::string>& valueTable, std::vector<std::string>& change) {
    if (getOringinRule(tree->rule) == "statement lvalue BECOMES expr SEMI") {
        std::shared_ptr<parseTree> cur = tree->children[0];
        while (true) {
            if (getOringinRule(cur->rule) == "lvalue LPAREN lvalue RPAREN") {
                cur = cur->children[1];
            } else if (getOringinRule(cur->rule) == "lvalue STAR factor") {
                break;                
            } else if (getOringinRule(cur->rule) == "lvalue ID") {
                std::string name = getFirst(cur->children[0]->rule);
                if (std::find(valueTable.begin(), valueTable.end(), name) == valueTable.end()) {
                    valueTable.push_back(name);
                }
                if (std::find(change.begin(), change.end(), name) == change.end()) {
                    change.push_back(name);
                }                
                break;                
            }
        }
    }

    else if (getOringinRule(tree->rule) == "factor ID" || getOringinRule(tree->rule) == "lvalue ID") {
        std::string name = getFirst(tree->children[0]->rule);
        if (std::find(valueTable.begin(), valueTable.end(), name) == valueTable.end()) {
            valueTable.push_back(name);
        }      
    }
    for (auto it: tree->children) {
        getUsedvar(it, valueTable, change);
    }  
}

void printWain(std::shared_ptr<parseTree>& tree, std::map<std::string, int>& offsetTable, int& offset, int& labelCount) {
    std::map<std::string, int> valueTable;
    std::cout << "wain:" << std::endl;
    // initialize constants
    std::cout << "lis $4" << std::endl;
    std::cout << ".word 4" << std::endl;
    std::cout << "sub $29, $30, $4" << std::endl;
    std::cout << "lis $11" << std::endl;
    std::cout << ".word 1" << std::endl;

    std::string a = getFirst(tree->children[3]->children[1]->rule);
    std::string b = getFirst(tree->children[5]->children[1]->rule);

    offsetTable[a] = offset;
    offset -= 4;
    offsetTable[b] = offset;
    offset -= 4;

    std::cout << "sw $1, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    std::cout << "sw $2, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;

    // initialize heap
    std::cout << "sw $2, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    std::cout << "sw $31, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;

    // if the first parameter of wain is int, set $2 = 0 here
    std::string firstPara = dataType(tree->children[3]->children[1]->rule);
    if (firstPara == "int") {std::cout << "add $2, $0, $0" << std::endl;}

    std::cout << "lis $5" << std::endl;
    std::cout << ".word init" << std::endl;
    std::cout << "jalr $5" << std::endl;
    std::cout << "add $30, $30, $4" << std::endl;
    std::cout << "lw $31, -4($30)" << std::endl;
    std::cout << "add $30, $30, $4" << std::endl;
    std::cout << "lw $2, -4($30)" << std::endl;

    std::vector<std::string> usedVar;
    std::vector<std::string> usedVarStatement;
    getUsedvar(tree->children[9], usedVar, usedVarStatement);
    getUsedvar(tree->children[11], usedVar, usedVarStatement);

    dcls(tree->children[8], offsetTable, offset, valueTable, usedVar);

    for (auto it: usedVarStatement) {
        if (valueTable.find(it) != valueTable.end()) {
            valueTable.erase(it);
        }
    }

    //changeValueUsed(tree->children[9], valueTable);
    statementEvaluate(tree->children[9], offsetTable, labelCount, valueTable);
    exprEvaluate(tree->children[11], offsetTable, valueTable);
    std::cout << "add $30, $29, $4" << std::endl;
    std::cout << "jr $31" << std::endl;   
}

void printProcedures(std::shared_ptr<parseTree>& tree, int& labelCount) {
    std::map<std::string, int> valueTable;
    std::map<std::string, int> offsetTable;
    std::string funcName = "F" + getFirst(tree->children[1]->rule);
    std::cout << funcName << ":" << std::endl;

    std::cout << "sub $29, $30, $4" << std::endl;

    std::vector<std::string> paraInfo;
    std::shared_ptr<parseTree> cur = tree->children[3];
    if (getOringinRule(cur->rule) != "params .EMPTY") {
        cur = cur->children[0];
        while (true) {
            paraInfo.push_back(getFirst(cur->children[0]->children[1]->rule));
            if (getOringinRule(cur->rule) == "paramlist dcl COMMA paramlist") {
                cur = cur->children[2];
            } else {
                break;
            }
        }
    }

    // update offset for offsettable
    int tempContain = 4;
    while (paraInfo.size() != 0) {
       offsetTable[paraInfo.back()] = tempContain; 
       tempContain += 4;
       paraInfo.pop_back();
    }

    tempContain = 0;
    std::vector<std::string> usedVar;
    std::vector<std::string> usedVarStatement;
    getUsedvar(tree->children[7], usedVar, usedVarStatement);
    getUsedvar(tree->children[9], usedVar, usedVarStatement);

    dcls(tree->children[6], offsetTable, tempContain, valueTable, usedVar);
    for (auto it: usedVarStatement) {
        if (valueTable.find(it) != valueTable.end()) {
            valueTable.erase(it);
        }
    }

    std::cout << "sw $5, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    std::cout << "sw $6, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;
    std::cout << "sw $7, -4($30)" << std::endl;
    std::cout << "sub $30, $30, $4" << std::endl;

    //changeValueUsed(tree->children[7], valueTable);
    statementEvaluate(tree->children[7], offsetTable, labelCount, valueTable);
    exprEvaluate(tree->children[9], offsetTable, valueTable);

    std::cout << "add $30, $30, $4" << std::endl;
    std::cout << "lw $7, -4($30)" << std::endl;
    std::cout << "add $30, $30, $4" << std::endl;
    std::cout << "lw $6, -4($30)" << std::endl;
    std::cout << "add $30, $30, $4" << std::endl;
    std::cout << "lw $5, -4($30)" << std::endl;
    std::cout << "add $30, $29, $4" << std::endl;
    std::cout << "jr $31" << std::endl;
}

void generator(std::shared_ptr<parseTree>& p, int& labelCount) {
    for (unsigned int i = 0; i < p->children.size(); i++) {
        if (p->rule.substr(0, 4) == "main" && i == 0) {
            int offset = 0;
            std::map<std::string, int> offsetTable;
            printWain(p, offsetTable, offset, labelCount);
        } 
        
        else if (p->rule == "procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE" && i == 0) {
            printProcedures(p, labelCount);
        }
        
        else {
            generator(p->children[i], labelCount);
        }
    }
}

int main() {
    // reconstruct parseTree
    parseTree parset = {};
    std::shared_ptr<parseTree> parseT = std::make_shared<parseTree>(parset);
    reconstruct(parseT);
    
    std::cout << ".import init" << std::endl;
    std::cout << ".import new" << std::endl;
    std::cout << ".import delete" << std::endl;
    std::cout << ".import print" << std::endl;
    std::cout << "beq $0, $0, wain" << std::endl;

    int labelCount = 0;

    generator(parseT, labelCount);
}