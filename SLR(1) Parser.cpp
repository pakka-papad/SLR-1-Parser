#include <bits/stdc++.h>
using namespace std;

int pcnt;
vector<string> prods;
char eps = '#';
char fs = 'S';
char start;
char eof = '$';
char dot = '.';

class item {
    public:
    vector<string> prods;
    map<char,int> next;
};

vector<item> states;
map<char,set<char>> firsts;
map<char,set<char>> follows;

bool isNonTerminal(char c){
    return (c >= 'A' && c <= 'Z');
}

void calcFirst(char c){
    if(firsts[c].size() != 0) return;
    for(auto p: prods){
        if(p[0] != c) continue;
        if(p[3] == p[0]) continue;
        for(int i = 3; i < p.size(); i++){
            bool hasEps = false;
            if(isNonTerminal(p[i]) && p[i] != c){
                calcFirst(p[i]);
                firsts[c].insert(firsts[p[i]].begin(),firsts[p[i]].end());
                if(firsts[p[i]].find(eps) != firsts[p[i]].end()) {
                    firsts[c].erase(eps);
                    hasEps = true;
                }
            } else if(!isNonTerminal(p[i])) {
                firsts[c].insert(p[i]);
                break;
            }
            if(!hasEps) break;
            if(i+1 == p.size()){
                firsts[c].insert(eps);
            }
        }
    }
}

void calcFollow(char c){
    if(follows[c].size() != 0) return;
    if(c == start) follows[c].insert(eof);
    for(auto p: prods){
        for(int i = 3; i < p.size(); i++){
            if(p[i] != c) continue;
            if(i+1 == p.size()){
                if(c != p[0]){
                    calcFollow(p[0]);
                    follows[c].insert(follows[p[0]].begin(),follows[p[0]].end());
                }
            } else if(isNonTerminal(p[i+1])) {
                for(int j = i+1; j < p.size(); j++){
                    bool hasEps = false;
                    if(!isNonTerminal(p[j])){
                        follows[c].insert(p[j]);
                        break;
                    } else {
                        follows[c].insert(firsts[p[j]].begin(),firsts[p[j]].end());
                        if(follows[c].find(eps) != follows[c].end()){
                            follows[c].erase(eps);
                            hasEps = true;
                        }
                    }
                    if(hasEps && j+1 == p.size()){
                        calcFollow(p[0]);
                        follows[c].insert(follows[p[0]].begin(),follows[p[0]].end());
                    }
                }
                
            } else {
                follows[c].insert(p[i+1]);
            }
        }
    }
}

int getDotIndex(string &p){
    int idx = -1;
    for(int i = 0; i < p.size(); i++) if(p[i] == dot){
        idx = i;
        break;
    }
    return idx;
}

vector<string> closure(string s){
    // cout << s << endl;
    vector<string> res;
    res.push_back(s);
    int idx = getDotIndex(s);
    if(idx+1 != s.size() && isNonTerminal(s[idx+1])){
        for(auto p: prods){
            if(s[idx+1] == p[0]){
                string t = p.substr(0,3) + string(1,dot) + (p.size() > 3 ? p.substr(3) : "");
                vector<string> calc;
                if(p[3] != s[idx+1]){
                    calc = closure(t);
                } 
                calc.push_back(t);
                res.insert(res.end(),calc.begin(),calc.end());
            }
        }
    }
    set<string> scl(res.begin(),res.end());
    res = vector<string>(scl.begin(),scl.end());
    return res;
}

void closure(item &other){
    vector<string> newProds;
    for(auto p: other.prods){
        // cout << p << endl;
        auto calc = closure(p);
        newProds.insert(newProds.end(),calc.begin(),calc.end());
    }
    set<string> scl(newProds.begin(),newProds.end());
    other.prods = vector<string>(scl.begin(),scl.end());
}

int findEqualIndex(item &item){
    for(int i = 0; i < states.size(); i++){
        if(states[i].prods == item.prods) return i;
    }
    return -1;
}

void createNext(int itemIdx){
    map<char,vector<string>> pxs;
    for(auto p: states[itemIdx].prods){
        int idx = getDotIndex(p);
        if(idx+1 != p.size()){
            string t = p;
            swap(t[idx],t[idx+1]);
            pxs[p[idx+1]].push_back(t);
        }
    }
    map<char,int> nxt;
    for(auto it = pxs.begin(); it != pxs.end(); it++){
        auto pr = *it;
        item other;
        other.prods = pr.second;
        closure(other);
        int idx = findEqualIndex(other);
        if(idx == -1){
            nxt[pr.first] = (int)states.size();
            states.push_back(other);
        } else {
            nxt[pr.first] = idx;
        }
        
    }
    states[itemIdx].next = nxt;
}

void prettyPrint(vector<vector<string>> &table){
    int r = table.size();
    int c = table[0].size();
    int w = 0;
    for(int i = 0; i < r; i++) for(int j = 0; j < c; j++) w = max(w,(int)table[i][j].size());
    string sml(w+1,'-');
    sml[0] = '+';
    string hr = "";
    for(int j = 0; j < c; j++) hr += sml;
    hr.push_back('+');
    cout << hr << "\n";
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            cout << '|';
            cout << table[i][j];
            int k = table[i][j].size();
            while(k < w){
                k++;
                cout << ' ';
            }
        }
        cout << "|\n";
        cout << hr << "\n";
    }
}

int getProdIdx(string p){
    p.pop_back();
    for(int i = 0; i < prods.size(); i++){
        if(p == prods[i]) return i+1;
    }
    return -1;
}

int main(){
    set<char> sx;
    cin >> pcnt;
    string s;
    for(int i = 0; i < pcnt; i++){
        cin >> s;
        prods.push_back(s);
        sx.insert(s[0]);
        for(int j = 3; j < s.size(); j++){
            if(s[j] != eps) sx.insert(s[j]);
        }
    }
    start = prods[0][0];
    for(auto p: prods){
        calcFirst(p[0]);
    }
    for(auto p: prods){
        calcFollow(p[0]);
    }
    for(int i = 0; i < pcnt; i++){
        if(prods[i].back() == eps) prods[i].pop_back();
    }
    if(sx.find('X') == sx.end()){
        fs = 'X';
    } else if(sx.find('S') == sx.end()){
        fs = 'S';
    } else {
        fs = 'Z';
    }
    item i0;
    i0.prods.push_back(string(1,fs) + "->." + string(1,prods[0][0]));
    states.push_back(i0);
    closure(states[0]);
    int t = 0;
    while(t < states.size()){
        createNext(t);
        t++;
    }
    int rows = 1 + states.size();
    int cols = 1 + sx.size();
    vector<char> colchar(sx.begin(),sx.end());
    t = -1;
    for(int i = 0; i < colchar.size(); i++){
        if(isNonTerminal(colchar[i])){
            t++;
            swap(colchar[t],colchar[i]);
        }
    }
    colchar.push_back(eof);
    cols++;
    reverse(colchar.begin(),colchar.end());
    vector<vector<string>> table(rows,vector<string>(cols,""));
    for(int i = 1; i < rows; i++){
        table[i][0] = "I" + to_string(i-1);
    }
    for(int i = 1; i < cols; i++){
        table[0][i] = string(1,colchar[i-1]);
    }
    auto getColIdx = [&colchar](char c){
        for(int i = 0; i < colchar.size(); i++){
            if(colchar[i] == c) return i+1;
        }
        return -1;
    };
    for(int i = 1; i < rows; i++){
        for(auto p: states[i-1].next){
            int idx = getColIdx(p.first);
            if(idx != -1){
                string re = (isNonTerminal(p.first) ? "" : "S" )+ to_string(p.second);
                if(table[i][idx] != "") table[i][idx] += "/" + re;
                else table[i][idx] = re;
            } 
        }
        for(auto p: states[i-1].prods){
            if(p.back() == dot){
                auto folset = follows[p[0]];
                for(auto cx: folset){
                    string re = "R" + to_string(getProdIdx(p));
                    if(table[i][getColIdx(cx)] != "") table[i][getColIdx(cx)] += "/" + re;
                    else table[i][getColIdx(cx)] = re;
                }
                if(p == string(1,fs) + "->" + string(1,start) + string(1,dot)){
                    table[i][getColIdx(eof)] = "ACC";
                }
            }
        }
    }

    for(int i = 0; i < states.size(); i++){
        cout << "I" + to_string(i) << '\n';
        cout << "productions: {\n";
        for(int j = 0; j < states[i].prods.size(); j++){
            cout << "\t";
            cout << states[i].prods[j];
            if(j+1 != states[i].prods.size()) cout << "\n";
        }
        cout << "\n}\n";
        cout << "next: {\n";
        for(auto p: states[i].next){
            cout << "\t";
            cout << "On symbol " << p.first << " goto state I" << to_string(p.second) << "\n";
        }
        cout << "}\n\n";       
    }
    prettyPrint(table);
    return 0;
}
/**
 * 
 * # is used to denote epsilon
 * 
 * sample input
 * 
 * 6
 * E->E+T
 * E->T
 * T->T*F
 * T->F
 * F->(E)
 * F->@
 * 
*/