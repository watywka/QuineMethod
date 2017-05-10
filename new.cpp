#include <algorithm>
#include <iostream>
#include <string>
#include <cctype>
#include <iterator>
#include <vector>

using namespace std;

class Node
{
    public:
        int type; //1 - const, 2 - var, 3 - operation
        int value; // for const
        char var; // for var
        char op; // for operation
        vector<char>* vars; // all variables
        Node* l_exp;
        Node* r_exp;
        Node(int v);
        Node(char v);
        Node(char v, Node* left, Node* right);
        Node(Node& copy_node);
        string print();
        Node* set_var(char var, int val); // set val for variable var
        ~Node();
};

Node::Node(int v):type(1),value(v)
{
    vars = new vector<char>();
    l_exp = nullptr;
    r_exp = nullptr;
}

Node::Node(char v):type(2),var(v)
{
    vars = new vector<char>();
    vars->push_back(var);
    l_exp = nullptr;
    r_exp = nullptr;
}

Node::Node(char v, Node* left, Node* right):type(3),op(v),l_exp(left),r_exp(right)
{
    vars = new vector<char>();
    vars->insert(vars->begin(), l_exp->vars->begin(),l_exp->vars->end());
    if(r_exp) vars->insert(vars->begin(), r_exp->vars->begin(),r_exp->vars->end());
    sort(vars->begin(),vars->end());
    vars->erase(unique(vars->begin(),vars->end()), vars->end());
}

Node::Node(Node& copy_node)
{
    type = copy_node.type;
    vars = new vector<char>(*(copy_node.vars));
    l_exp = r_exp = nullptr;
    switch(type)
    {
        case 1: value = copy_node.value;
                break;
        case 2: var = copy_node.var;
                break;
        case 3: op = copy_node.op;
                l_exp = new Node(*(copy_node.l_exp));
                if (copy_node.r_exp) r_exp = new Node(*(copy_node.r_exp));
                break;
    }
}



string Node::print()
{
    string tmp = "";
    switch(type)
    {
        case 1: tmp.append(to_string(value));
                break;
        case 2: tmp.append(string(1,var));
                break;
        case 3: tmp.append("(");
                if(r_exp)
                {
                    tmp.append(l_exp->print());
                    tmp.append(string(1,op));
                    tmp.append(r_exp->print());
                }
                else
                {
                    tmp.append(string(1,op));
                    tmp.append(l_exp->print());
                }
                tmp.append(")");
                break;
    }
    return tmp;
}

Node* Node::set_var(char var, int val)
{
    switch(type)
    {
        case 1: return new Node(*this);
                break;
        case 2: if(vars->operator[](0) == var)
                    return new Node(val);
                else
                    return new Node(*this);
        case 3: Node* l_tmp = l_exp->set_var(var,val);
                Node* r_tmp = nullptr;
                if(!r_exp)
                {
                    if(l_tmp->type == 1)
                        return new Node(1-l_tmp->value);
                    else
                    {
                        return new Node('~', l_tmp, nullptr);
                    }
                }
                r_tmp = r_exp->set_var(var, val);
                if(l_tmp->type == 1)
                {
                    if(r_tmp->type == 1)
                    {
                        switch(op)
                        {
                            case '=': return new Node(l_tmp->value == r_tmp->value); break;
                            case '>': return new Node(l_tmp->value <= r_tmp->value); break;
                            case '|': return new Node(l_tmp->value || r_tmp->value); break;
                            case '+': return new Node((l_tmp->value+r_tmp->value)%2); break;
                            case '^': return new Node((l_tmp->value)*(r_tmp->value)); break;
                        }
                    }
                    else
                    {
                        switch(op)
                        {
                            case '=': return new Node('=',l_tmp, r_tmp); break;
                            case '>': if(l_tmp->value == 0) 
                                           return new Node(1);
                                       else
                                           return new Node(*r_tmp);
                                       break;
                            case '|': if(l_tmp->value)
                                           return new Node(1);
                                       else
                                           return new Node(*r_tmp);
                                       break;
                            case '+': if(l_tmp->value == 0)
                                          return new Node(*r_tmp);
                                      else
                                          return new Node('~',r_tmp,nullptr);
                                        break;
                            case '^': if(l_tmp->value)
                                          return new Node(*r_tmp);
                                      else
                                          return new Node(0);
                                      break;
                        }
                    }
                }
                if(r_tmp->type == 1)
                {
                    switch(op)
                    {
                        case '=': return new Node('=',l_tmp,r_tmp); break;
                        case '>': if(r_tmp->value == 0) 
                                      return new Node('~', l_tmp, nullptr);
                                  else
                                      return new Node(1);
                                  break;
                        case '|': if(r_tmp->value)
                                      return new Node(1);
                                  else
                                      return new Node(*l_tmp);
                                  break;
                        case '+': if(r_tmp->value == 0)
                                      return new Node(*l_tmp);
                                  else
                                      return new Node('~',l_tmp,nullptr);
                                  break;
                        case '^': if(r_tmp->value)
                                      return new Node(*l_tmp);
                                  else
                                      return new Node(0);
                                  break;
                    }
                }
                return new Node(op, l_tmp,r_tmp);
                break;
    } 
} 

Node::~Node()
{
    delete vars;
    if(l_exp) delete l_exp;
    if(r_exp) delete r_exp;
}

Node* strToExp(string &str){
    int level = 0;//inside parentheses check
    //=
    for(int i=str.size()-1;i>=0;--i){
        char c = str[i];
        if(c == ')'){
            ++level;
            continue;
        }
        if(c == '('){
            --level;
            continue;
        }
        if(level>0) continue;
        if(c == '='){
            string left(str.substr(0,i));
            string right(str.substr(i+1));
            return new Node(c, strToExp(left), strToExp(right));
        }
    }
    // ->
    for(int i=str.size()-1;i>=0;--i){
        char c = str[i];
        if(c == ')'){
            ++level;
            continue;
        }
        if(c == '('){
            --level;
            continue;
        }
        if(level>0) continue;
        if(c == '>'){
            string left(str.substr(0,i));
            string right(str.substr(i+1));
            return new Node(c, strToExp(left), strToExp(right));
        }
    }
    // \/ or +
    for(int i=str.size()-1;i>=0;--i){
        char c = str[i];
        if(c == ')'){
            ++level;
            continue;
        }
        if(c == '('){
            --level;
            continue;
        }
        if(level>0) continue;
        if((c == '+') || (c=='|')){
            string left(str.substr(0,i));
            string right(str.substr(i+1));
            return new Node(c, strToExp(left), strToExp(right));
        }
    }
    //^
    for(int i=str.size()-1;i>=0;--i){
        char c = str[i];
        if(c == ')'){
            ++level;
            continue;
        }
        if(c == '('){
            --level;
            continue;
        }
        if(level>0) continue;
        if(c == '^'){
            string left(str.substr(0,i));
            string right(str.substr(i+1));
            return new Node(c, strToExp(left), strToExp(right));
        }
    }
    if(str[0]=='~')
    {
        char c = str[0];
        string left(str.substr(1));
        return new Node(c, strToExp(left), NULL);
    }
    if(str[0]=='('){
        //case ()
        //pull out inside and to strToExp
        for(unsigned int i=0;i<str.size();++i){
            if(str[i]=='('){
                ++level;
                continue;
            }
            if(str[i]==')'){
                --level;
                if(level==0){
                    string exp(str.substr(1, i-1));
                    return strToExp(exp);
                }
                continue;
            }
        }
    } else
        //case value
        return new Node(str[0]);
    cerr << "Error:never execute point" << endl;
    return NULL;//never
}

void pr_razb(Node* tree, int level, vector<char>& v)
{
    Node* tmp = tree->set_var(v[level],0);
    for(int i=0;i<=level;i++) cout << '-';
    cout<< tmp->print()<<endl;
    if(tmp->type!=1) pr_razb(tmp,level+1,v);
    delete tmp;
    tmp = tree->set_var(v[level],1);
    for(int i=0;i<=level;i++) cout << '-';
    cout<< tmp->print()<<endl;
    if(tmp->type!=1) pr_razb(tmp,level+1,v);
    delete tmp;
    
}

int main(){
    string exp;
    cin >> exp;
    exp.erase(remove_if(exp.begin(), exp.end(), ::isspace), exp.end());
    Node *ex = strToExp(exp);
    vector<char> variables = *(ex->vars);
    pr_razb(ex,0,variables);
    delete ex;
}
