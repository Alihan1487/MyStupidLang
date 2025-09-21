#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <any>
#include <algorithm>
#include "dynamic.cpp"
using namespace std;
class Command{
public:
function<void(string,map<string,Dynamic>)> func;
  Command(function<void(string,map<string,Dynamic>)> func){
    this->func=func;
  }
  void operator()(string cmd,map<string,Dynamic> namespac){
    func(cmd,namespac);
  }
};
vector<string> split(const string& str, char ell){
    vector<string> rar;
    string cur;
    bool inQuotes = false;
    for (size_t i=0;i<str.size();++i){
        char c = str[i];
        if (c == '"'){
            inQuotes = !inQuotes;
            cur += c;
            continue;
        }
        if (!inQuotes && c == ell){
            rar.push_back(cur);
            cur.clear();
        } else {
            cur += c;
        }
    }
    if (!cur.empty()) rar.push_back(cur);
    return rar;
}
template<typename T>
vector<T> slice(const vector<T>& arr, int start, int end) {
    if (start < 0) start = 0;
    if (end > arr.size()) end = arr.size();
    if (start > end) start = end;

    return vector<T>(arr.begin() + start, arr.begin() + end);
}
vector<string> cut(string code,char splitter){
  string tar;
  vector<string> rar;
  for (int i=0;i<code.size();i++){
    if (code[i]==splitter && !tar.empty()){
      rar.push_back(tar);
      tar.clear();
    }
    else
      if (code[i]!=' ' and code[i]!=splitter){
        tar+=code[i];
      }
    }
  if (tar.size()>0){
    rar.push_back(tar);
    tar.clear();
  }
  return rar;
}
bool lookindecs(map<string,function<void(string)>> dict,string name){
  for (auto [key,value]:dict){
    if (key==name){
      return true;
    }
  }
  return false;
}
class Interpreter{
public:
inline static map<string,function<Dynamic(string,map<string,Dynamic>&,Interpreter*)>>__comms;
inline static map<string,function<Dynamic(string,string,map<string,Dynamic>&,Interpreter*)>>__operators;
inline static map<string,Dynamic> vars;
inline static map <string,map<string,Dynamic>>namespaces;
  
  Interpreter(){
    __comms["OUT"]=out;
    __comms["SET"]=set;
    __operators["IF"]=if_;
  }
Dynamic execline(string line, map<string,Dynamic> namespac) {
    // trim (убираем пробелы по краям)
    auto trim = [](string s) {
        size_t start = s.find_first_not_of(" \t");
        size_t end = s.find_last_not_of(" \t");
        if (start == string::npos) return string("");
        return s.substr(start, end - start + 1);
    };
    line = trim(line);

    // === 1. Литералы ===
    if (line == "true") return Dynamic(new Bool(true));
    if (line == "false") return Dynamic(new Bool(false));
    if (line == "null" || line == "NULL") return Dynamic(new Null());

    // число
    bool isnum = !line.empty() && all_of(line.begin(), line.end(), ::isdigit);
    if (isnum) {
        return Dynamic(new Int(stoi(line)));
    }

    // строка в кавычках
    if (line.size() >= 2 && line.front() == '"' && line.back() == '"') {
        return Dynamic(new String(line.substr(1, line.size() - 2)));
    }

    // === 2. Логические выражения ===
    // Проверяем известные операторы
    auto checkOp = [&](string op) -> int {
        size_t pos = line.find(op);
        if (pos != string::npos) return (int)pos;
        return -1;
    };

    vector<string> ops = {"==", "!=", "<=", ">=", "<", ">", "&&", "||"};
    for (auto &op : ops) {
        int pos = checkOp(op);
        if (pos != -1) {
            string leftStr  = trim(line.substr(0, pos));
            string rightStr = trim(line.substr(pos + op.size()));

            Dynamic left  = execline(leftStr, namespac);
            Dynamic right = execline(rightStr, namespac);

            // Int сравнение
            if (dynamic_cast<IntObject*>(left.data) && dynamic_cast<IntObject*>(right.data)) {
                int l = static_cast<IntObject*>(left.data)->get();
                int r = static_cast<IntObject*>(right.data)->get();
                if (op == "==") return Dynamic(new Bool(l == r));
                if (op == "!=") return Dynamic(new Bool(l != r));
                if (op == "<")  return Dynamic(new Bool(l < r));
                if (op == ">")  return Dynamic(new Bool(l > r));
                if (op == "<=") return Dynamic(new Bool(l <= r));
                if (op == ">=") return Dynamic(new Bool(l >= r));
            }

            // Bool сравнение
            if (dynamic_cast<BoolObject*>(left.data) && dynamic_cast<BoolObject*>(right.data)) {
                bool l = static_cast<BoolObject*>(left.data)->get();
                bool r = static_cast<BoolObject*>(right.data)->get();
                if (op == "==") return Dynamic(new Bool(l == r));
                if (op == "!=") return Dynamic(new Bool(l != r));
                if (op == "&&") return Dynamic(new Bool(l && r));
                if (op == "||") return Dynamic(new Bool(l || r));
            }

            // String сравнение
            if (dynamic_cast<StringObject*>(left.data) && dynamic_cast<StringObject*>(right.data)) {
                string l = static_cast<StringObject*>(left.data)->get();
                string r = static_cast<StringObject*>(right.data)->get();
                if (op == "==") return Dynamic(new Bool(l == r));
                if (op == "!=") return Dynamic(new Bool(l != r));
            }

            return Dynamic(new Bool(false)); // если неподдерживаемое сравнение
        }
    }

    // === 3. Переменные в namespace ===
    if (namespac.find(line) != namespac.end()) {
        return namespac[line];
    }

    // === 4. Глобальные переменные (vars) ===
    if (vars.find(line) != vars.end()) {
        return vars[line];
    }

    // === 5. Команды ===
    return callcom(line, namespac);
}

  static Dynamic out(string args,map<string,Dynamic>& namespac,Interpreter* self){
    auto blos=split(args,' ');
    for (int i=0;i<blos.size();i++){
      cout<<self->execline(blos[i],namespac);
    }
    return Dynamic(nullptr);
  }
  static Dynamic set(string args,map<string,Dynamic>& namespac,Interpreter* self){
    auto blos=split(args,' ');
    namespac[blos[0]]=self->execline(blos[1],namespac);
    return Dynamic(nullptr);
  }
static Dynamic if_(string args,string block,map<string,Dynamic>& namespac,Interpreter* self){
    Dynamic cond = self->execline(args,namespac);

    // Проверяем, что это BoolObject
    if (auto b = dynamic_cast<BoolObject*>(cond.data)) {
        if (b->get()) {
            for(auto l:self->splittoblocks(block)){
                self->callcom(l,namespac);
            }
        }
    }
    else if (auto i = dynamic_cast<IntObject*>(cond.data)) {
        if (i->get() != 0) { // в стиле C: любое ненулевое число — true
            for(auto l:self->splittoblocks(block)){
                self->callcom(l,namespac);
            }
        }
    }
    else {
        cout << "[IF error] условие не bool и не int!" << endl;
    }

    return Dynamic(nullptr);
}
  static Dynamic function_definition(string args,string block,map<string,Dynamic>& namespac,Interpreter* self){
    auto ars=split(args,' ');
    auto argsnames=split(args,' ');
    namespac[ars[0]]=Dynamic(new Function(function<Dynamic(string,map<string,Dynamic>&)>([=](string args,map<string,Dynamic>& namespac){
      static auto argsnamesinner=argsnames;
      auto argvals=split(args,' ');
      map<string,Dynamic> locals;
      for (auto i:argsnamesinner){
        for (auto j:split(args,',')){
          locals[i]=self->execline(j,namespac);
        }
      }
      return self->execline(block,locals);
    })));
    return Dynamic(nullptr);
  }
  Dynamic callcom(string cmd,map<string,Dynamic>& namespac){
    vector<string> cmm=split(cmd,' ');
    for (int i=0;i<cmm.size();i++){
      if (__comms.find(cmm[i])!=__comms.end()){
        bool argpart=false;
        string args;
        for (auto m:slice(cmm,i,cmm.size())){
          if (argpart){
            if (!args.empty()){
              args+=" "+m;
            }
            else{
              args+=m;
            }
          }
          if (m==")" || m=="("){
            argpart=!argpart;
            if (!argpart){
              args.pop_back();
            }
          }
        }
        return __comms[cmm[i]](args,namespac,this);
      }
    if (__operators.find(cmm[i])!=__operators.end()){
      bool operpart=false;
      int dep=0;
      string args;
      string block;
      vector<string> rar;
      string tar;
      for (auto k:slice(cmm,i+1,cmm.size())){
        if (k=="{"){
          if (dep==0){
            operpart=true;
            dep+=1;
            continue;
          }
          dep+=1;
        }
        else if(k=="}"){
          dep-=1;
          if (dep==0){
            operpart=false;
            break;
          }
        }
        if (!operpart){
          if (!args.empty()){
            args=args+" "+k;
          }
          else{
            args+=k;
          }
        }
        else{
          if(!block.empty()){
            block=block+" "+k;
          }
          else{
            block+=k;
          }
        }
      }
      return __operators[cmm[i]](args,block,namespac,this);
    }
    }
    return Dynamic(nullptr);
  }
  vector<string> splittoblocks(string cod){
    vector<string> code=cut(cod,' ');
    string tar;
    vector<string> rar;
    int dep=0;
    bool qark=false;
    for (int i=0;i<code.size();i++){
      string a=code[i];
      if (!tar.empty()){
        tar+=" "+a;
      }
      else{
        tar+=a;
      }
      if (a=="}"){
        dep-=1;
        if (dep==0){
          rar.push_back(tar);
          tar="";
          }
        }
      else if (a=="{"){
        dep+=1;
      }
      else if (a==")" || a=="("){
        qark=!qark;
        if (!qark){
          rar.push_back(tar);
          tar="";
        }
      }
      }
    if (tar!=""){
      rar.push_back(tar);
      tar="";
    }
    return rar;
    }
};
Interpreter g;
int main() { 
  cout<<"WHY\n";
  g.callcom("OUT ( \"kokos\" )",g.vars);
  g.callcom("SET ( a 90 )",g.vars);
  cout<<"new";
  g.callcom("OUT ( a )",g.vars);
  cout<<"\n";
  g.callcom("IF 9==9 { OUT ( \"lol\" ) }",g.vars);
}