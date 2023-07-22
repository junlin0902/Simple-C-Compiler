#include <iostream>
#include <vector>
#include <deque>
#include <sstream>
#include <string>
#include <map>
#include <stack>
#include <algorithm>
#include "wlp4data.h"
#include <memory>


struct Rule {
    std::string left;
    std::vector<std::string> right;
};


struct member {
    std::string lhs = "";
    std::vector<std::string> rhs;
};

struct parseTree {
    member m;
    std::deque<std::shared_ptr<parseTree>> child;
};


void printParsetree(std::shared_ptr<parseTree> tree) {
    // print current member
    if (tree->m.lhs != "") {
        std::cout << tree->m.lhs << " ";
        int num = tree->m.rhs.size();
        for (int i = 0; i < num; i++) {
            if (i == num - 1) {
                std::cout << tree->m.rhs[i];
            } else {
                std::cout << tree->m.rhs[i] << " ";
            }
        }
        std::cout << std::endl;
    }

    for (auto it: tree->child) {
        printParsetree(it);
    } 
}


int main() {
    std::vector<Rule> cfg;
    std::deque<std::pair<std::string, std::string>> input;
    std::deque<std::string> lhs;
    std::map<std::pair<int, std::string>, int> transitions;
    std::map<std::pair<int, std::string>, int> reductions;


    // get input
    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::string temp1, temp2;
            while (iss >> temp1 >> temp2) {
                input.push_back({temp1, temp2});
            }
        }
    }
    input.push_front({"BOF", "BOF"});
    input.push_back({"EOF", "EOF"});



    // CFG
    std::istringstream CFG(WLP4_CFG);
    std::getline(CFG, line);
    while (std::getline(CFG, line) && line != ")END") {
        if (!line.empty()) {
            Rule rule;
            std::istringstream iss(line);
            std::string symbol;
            iss >> rule.left;
            while (iss >> symbol) {
                rule.right.push_back(symbol);
            }
            cfg.push_back(rule);
        }
    }



    // Transitions
    std::istringstream TRANSITIONS(WLP4_TRANSITIONS);
    std::getline(TRANSITIONS, line);
    while (std::getline(TRANSITIONS, line) && line != ")END") {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::string current, next, symbol;
            iss >> current >> symbol >> next;
            int cur = std::stoi(current);
            int n = std::stoi(next);
            transitions[{cur, symbol}] = n;
        }        
    }

    // Reductions
    std::istringstream REDUCTIONS(WLP4_REDUCTIONS);
    std::getline(REDUCTIONS, line);
    while (std::getline(REDUCTIONS, line) && line != ")END") {
        if (!line.empty()) {
            std::istringstream iss(line);
            std::string symbol, state, ruleIndex;
            iss >> state >> ruleIndex >> symbol;
            reductions[{std::stoi(state),symbol}] = std::stoi(ruleIndex);
        }
    }

    int shiftedCount = 0;
    std::stack<int> state;
    state.push(0);
    std::string currentSymbol;
    parseTree parseTtemp = {};
    std::shared_ptr<parseTree> parseT = std::make_shared<parseTree>(parseTtemp);


    while (!input.empty()) {
        currentSymbol = input.front().first;

        //std::cout << currentSymbol << std::endl;
        while (reductions.find({state.top(), currentSymbol}) != reductions.end()) {
            // num is the rule number
            int num = reductions.find({state.top(), currentSymbol})->second;
            parseTree temp = {{cfg[num].left, cfg[num].right}};
            std::shared_ptr<parseTree> upper = std::make_shared<parseTree>(temp);
            //parseTree upper = {{cfg[num].left, cfg[num].right}};
            if (cfg[num].right.size() == 1 && cfg[num].right[0] == ".EMPTY") {

            } 
            else {
                for (unsigned int i = 0; i < cfg[num].right.size(); i++) {
                    lhs.pop_back();
                    state.pop();

                    //parseTree out;
                    // deep copy
                    //out.m.lhs = parseT.child.back().m.lhs;
                    //for (auto& it: parseT.child.back().m.rhs) {
                    //    out.m.rhs.push_back(it);
                    //}
                    //for (auto& it: parseT.child.back().child) {
                    //    out.child.push_back(it);
                    //}
                    std::shared_ptr<parseTree> out = parseT->child.back();
                    //parseTree out = parseT.child.back();
                    parseT->child.pop_back();
                    upper->child.push_front(out);
                }
            }
            parseT->child.push_back(upper);
            lhs.push_back(cfg[num].left);


            if (transitions.find({state.top(), cfg[num].left}) == transitions.end()) {
                std::cerr << "ERROR at " << shiftedCount << std::endl;
                return 1;                
            }
            state.push(transitions.find({state.top(), cfg[num].left})->second);
        }
        parseTree temp2 = {{currentSymbol, {input.front().second}}};
        parseT->child.push_back(std::make_shared<parseTree>(temp2));
        //parseT.child.push_back({{currentSymbol, {input.front().second}}});

        lhs.push_back(currentSymbol);
        input.pop_front();

        if (transitions.find({state.top(), currentSymbol}) == transitions.end()) {
            std::cerr << "ERROR at " << shiftedCount << std::endl;
            return 1;                
        }
        shiftedCount++;
        state.push(transitions.find({state.top(), currentSymbol})->second);
    }

    currentSymbol = ".ACCEPT";
    if (reductions.find({state.top(), currentSymbol}) != reductions.end()) {
        int num = reductions.find({state.top(), currentSymbol})->second;
        //parseTree upper = {{cfg[num].left, cfg[num].right}};
        parseTree temp = {{cfg[num].left, cfg[num].right}};
        std::shared_ptr<parseTree> upper = std::make_shared<parseTree>(temp);
        for (unsigned int i = 0; i < cfg[num].right.size(); i++) {
            lhs.pop_back();
            state.pop();
            std::shared_ptr<parseTree> out = parseT->child.back();
            //parseTree out = parseT.child.back();
            parseT->child.pop_back();
            upper->child.push_front(out);
        }
        lhs.push_back(cfg[num].left);
        parseT->child.push_back(upper);     
    }   
    printParsetree(parseT);
}
