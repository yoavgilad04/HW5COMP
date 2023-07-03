#include "source.hpp"
#include <stdlib.h>
#include <ctype.h>
#include "staff/bp.hpp"
#include "singleton.hpp"
#include "staff/bp.hpp"

TableStack table_stack;
bool is_in_loop = false;
int loop_count = 0;
vector<While*> whiles;

bool isInt(const std::string &str) {
    try {
        std::stoi(str);
        return true;
    } catch (const std::invalid_argument &e) {
        return false;
    }
}

bool isString(const std::string &str) {
    if (str.length() < 2)
        return false;
    if (str[0] == '"' && str[str.length() - 1] == '"')
        return true;
    return false;
}

bool isByte(const std::string &str) {
    // Check if the string is empty
    if (str.empty())
        return false;
    for (int i = 0; i < str.length() - 1; i++) {
        if (!isdigit(str[i]))
            return false;
    }
    if (str[str.length() - 1] != 'b')
        return false;
    return true;
}

Exp::Exp(string type, string value) : Node(type)
{
    // value can be NUM (int), false, true or STRING only!!
    Singleton* shaked = Singleton::getInstance();
    string new_var = shaked->getFreshVar();

    if(type == "STRING")
    {
        shaked->addStringStatement(new_var, value);
        this->str_length = value.size();
        this->llvm_var = new_var;
        return;
    }
    string new_value;
    if(type == "INT")
        new_value = value;
    else
    {
        if (value == "true")
            new_value = "1";
        else if (value == "false")
            new_value = "0";
    }
    shaked->addAssignmentCommand(new_var, new_value);
    this->llvm_var = new_var;
    if (value == "true" || value == "false")
    {
        string comp_var = shaked->getFreshVar();
        shaked->code_buffer->emit(shaked->makeCompStatement(comp_var, "==", new_var, "1"));
        int condition_address = shaked->code_buffer->emit(shaked->makeGoToCondStatement(comp_var));
        this->trueList = shaked->code_buffer->makelist(pair<int,BranchLabelIndex>{condition_address, FIRST});
        this->falseList = shaked->code_buffer->makelist(pair<int,BranchLabelIndex>{condition_address, SECOND});
        this->llvm_var = comp_var;
//        shaked->code_buffer->emit("___________________________________________________________________");
    }
}

Exp::Exp(string type) : Node(type), llvm_var(""){}

Exp::Exp(string operation_val, Node& exp_1, Node& exp_2, string op)
{
    Singleton* shaked = Singleton::getInstance();
    Exp* e1 = dynamic_cast<Exp*>(&exp_1);
    Exp* e2 = dynamic_cast<Exp*>(&exp_2);
    string e1_name = e1->getLLVMName();
    string e2_name = e2->getLLVMName();
    if ((exp_1.getType() == "INT" || exp_1.getType() == "BYTE") && (exp_2.getType() == "INT" || exp_2.getType() == "BYTE"))
    {
        if (operation_val == "binop")
        {
            if(exp_1.getType() == "INT" || exp_2.getType() == "INT")
                this->type = "INT";
            else
                this->type = "BYTE";
            if (op == "+")
                op = "ADD";
            if (op == "-")
                op = "SUB";
            if (op == "*")
                op = "MUL";
            if (op == "/")
                op = "DIV";
            if (this->type == "BYTE")
            {
                string new_var = shaked->getFreshVar();
                shaked->code_buffer->emit("%" + new_var + " = trunc i32 %" + e1_name + " to i8");
                string new_var2 = shaked->getFreshVar();
                shaked->code_buffer->emit("%" + new_var2 + " = trunc i32 %" + e2_name + " to i8");
                string result_in_i32 = shaked->getFreshVar();
                shaked->code_buffer->emit(shaked->makeBinaryStatement(result_in_i32, op, new_var, new_var2, true));
                this->llvm_var = shaked->getFreshVar();
                shaked->code_buffer->emit("%" + this->llvm_var + " = zext i8 %" + result_in_i32 + " to i32");
            }
            else
            {
                this->llvm_var = shaked->getFreshVar();
                shaked->code_buffer->emit(shaked->makeBinaryStatement(this->llvm_var, op, e1_name, e2_name));
            }
            return;
        }
        this->llvm_var = shaked->getFreshVar();
        if (operation_val == "relop")    // \<|\>|\<\=|\>\=
        {
            string cmd = shaked->makeCompStatement(this->llvm_var, op, e1_name, e2_name);
            shaked->code_buffer->emit(cmd);
            this->type = "BOOL";
            int condition_address = shaked->code_buffer->emit(shaked->makeGoToCondStatement(this->llvm_var));
            this->trueList = shaked->code_buffer->makelist(pair<int,BranchLabelIndex>{condition_address, FIRST});
            this->falseList = shaked->code_buffer->makelist(pair<int,BranchLabelIndex>{condition_address, SECOND});
            return;
        }
    }
    // todo: reduce freshvar by one beacuse we didn't use the new var
    output::errorMismatch(yylineno);
    exit(0);
}

Exp::Exp(Node& exp_1, string operation_val, Node& exp_2, string op, string exp_2_label)
{
    if (exp_1.getType() == "BOOL" && exp_2.getType() == "BOOL"){
        if (operation_val == "bool_op"){
            this->type = "BOOL";
            Exp* e1 = dynamic_cast<Exp*>(&exp_1);
            Exp* e2 = dynamic_cast<Exp*>(&exp_2);
            Singleton* yoav = Singleton::getInstance();
            if (op == "or")
            {
                this->trueList = e1->trueList;
                yoav->code_buffer->bpatch(e1->falseList,exp_2_label);
                this->trueList = yoav->code_buffer->merge(this->trueList,e2->trueList);
                this->falseList = e2->falseList;
            }
            if (op == "and")
            {
                this->falseList = e1->falseList;
                yoav->code_buffer->bpatch(e1->trueList,exp_2_label);
                this->falseList = yoav->code_buffer->merge(this->falseList,e2->falseList);
                this->trueList = e2->trueList;
            }
            return;
        }
    }
    output::errorMismatch(yylineno);
    exit(0);
}

Exp::Exp(Node &exp, const string &conversion_type)
{
    if (conversion_type == "not")
    {
        if (exp.getType() == "BOOL")
        {
            this->type = "BOOL";
            Exp* e = dynamic_cast<Exp*>(&exp);
            this->trueList = e->falseList;
            this->falseList = e->trueList;
            return;
        }
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (conversion_type == "INT" || conversion_type == "BYTE")
    {
        if (exp.getType() == "INT" || exp.getType() == "BYTE")
        {
            Exp* e = dynamic_cast<Exp*>(&exp);
            if (conversion_type == "INT")
            {
                this->type = "INT";
                this->llvm_var = e->getLLVMName();
            }
            else // conversion type == "BYTE"
            {
                Singleton* shaked = Singleton::getInstance();
                string new_var = shaked->getFreshVar();
                shaked->code_buffer->emit("%" + new_var + " = trunc i32 %" + e->getLLVMName() + " to i8");
                string new_var2 = shaked->getFreshVar();
                shaked->code_buffer->emit("%" + new_var2 + " = zext i8 %" + new_var + " to i32");
                this->type = "BYTE";
                this->llvm_var = new_var2;
            }
            return;
        }
        output::errorMismatch(yylineno);
        exit(0);
    }
//    assert(conversion_type == "BOOL");
    if(conversion_type == "BOOL")
    {
        if (exp.getType() == "BOOL")
        {
            this->type = "BOOL";
            return;
        }
        output::errorMismatch(yylineno);
        exit(0);
    }
    if(conversion_type == "id")
    {
        Symbol* t = table_stack.searchForSymbol(exp.getType()); // in this case type will be the name of the i
        if (t == nullptr || t->isFunc())
        {
            output::errorUndef(yylineno, exp.getType());
        }
        Singleton* shaked = Singleton::getInstance();
        CodeBuffer* code_buffer = shaked->code_buffer;
        string new_var;
        int offset = t->getOffset();
        if (offset >= 0)
        {
            string ptr_name = shaked->getFreshVar();
            new_var = shaked->getFreshVar();
            shaked->makeLoadCommand(new_var, ptr_name, t->getType(), t->getOffset());
        }
        else
        {
            new_var = shaked->getFreshVar();
            shaked->code_buffer->emit(shaked->makeBinaryStatement(new_var, "ADD", "%" + to_string(((-1 * offset) -1)) , "0"));
        }
        if (t->getType() == "BOOL")
        {
            string cmp_var = shaked->getFreshVar();
            code_buffer->emit(shaked->makeCompStatement(cmp_var, "==", new_var, "1"));
            int goto_line = code_buffer->emit(shaked->makeGoToCondStatement(cmp_var));
            this->trueList = code_buffer->makelist(pair<int,BranchLabelIndex>{goto_line, FIRST});
            this->falseList = code_buffer->makelist(pair<int,BranchLabelIndex>{goto_line, SECOND});
            this->type = "BOOL";
        }
        this->type = t->getType();
        this->llvm_var = new_var;
        return;
    }
    assert(conversion_type == "call");
    this->type = exp.getType();

    Call* call = dynamic_cast<Call*>(&exp);
    this->llvm_var = call->getLLVMName();
    if(this->type == "BOOL")
    {
        Singleton* yoav = Singleton::getInstance();
        string comp_var = yoav->getFreshVar();
        yoav->code_buffer->emit(yoav->makeCompStatement(comp_var, "==", this->llvm_var, "1"));
        int condition_address = yoav->code_buffer->emit(yoav->makeGoToCondStatement(comp_var));
        this->trueList = yoav->code_buffer->makelist(pair<int,BranchLabelIndex>{condition_address, FIRST});
        this->falseList = yoav->code_buffer->makelist(pair<int,BranchLabelIndex>{condition_address, SECOND});
        this->llvm_var = comp_var;
    }
}

Exp::Exp(Node &n): Node(n.getType())
{
    if (isInt(n.getType()))
    {
        int real_val = stoi(n.getType());
        if (real_val > 255 || real_val < 0)
        {
            output::errorByteTooLarge(yylineno, n.getType());
            exit(0);
        }
        Singleton* shaked = Singleton::getInstance();
        this->llvm_var = shaked->getFreshVar();
        shaked->addAssignmentCommand(this->llvm_var, n.getType());
        this->type = "BYTE";
        return;
    }
    assert(true==false); //must not get here
}

ExpList::ExpList(Node &exp)
{
    Exp* e = dynamic_cast<Exp*>(&exp);

    Singleton* yoav = Singleton::getInstance();
    if(e->getType() == "BOOL")
    {
        string new_var = yoav->getFreshVar();
        yoav->addPhiStatements(*e,new_var);
        e->setLLVMName(new_var);
    }

    this->insert(*e);
}


ExpList::ExpList(Node &exp, Node *exp_list)
{
    ExpList* old_exp_list = dynamic_cast<ExpList*>(exp_list);
    this->exp_list = *(old_exp_list->getExpList());
    delete old_exp_list;
    Exp* e = dynamic_cast<Exp*>(&exp);

    Singleton* yoav = Singleton::getInstance();
    this->insert(*e);
}


Call::Call(Node &function_name)
{
    string name = function_name.getType();
    vector<FuncSymbol*> funcs = table_stack.getAllFunctionsWithName(name);

    if (funcs.empty())
    {
        output::errorUndefFunc(yylineno,name);
    }
    bool foundEligibleFunc = false;
    string func_type ="";
    string func_var_name = "func_var_name";
    for (int i=0; i<funcs.size(); i++)
    {
        vector<string> func_args = funcs[i]->getArgs();
        if(func_args.size() == 0)
        {
            foundEligibleFunc = true;
            func_type = funcs[i]->getType();
            func_var_name = funcs[i]->getLLVMName();
            break; // as no two functions whereas their only different is their return type is allowed
        }
    }
    if(!foundEligibleFunc)
        output::errorPrototypeMismatch(yylineno,name);
    this->type = func_type;
    Singleton* yoav = Singleton::getInstance();
    this->llvm_var = yoav->getFreshVar();
    vector<Exp> empty_list = {};
    yoav->addFunctionCall(this->llvm_var,func_var_name,empty_list, func_type);
}

Call::Call(Node &function_name, Node* exp_list)
{
    ExpList* expList = dynamic_cast<ExpList*>(exp_list);
    vector<Exp>* true_exp_list = expList->getExpList();
    int candidate_funcs = 0;
    string func_type ="";
    string name = function_name.getType();
    string func_var_name = "func_var_name";
    vector<FuncSymbol*> funcs = table_stack.getAllFunctionsWithName(name);
    if (funcs.empty())
    {
        output::errorUndefFunc(yylineno,name);
    }
    for (int i=0; i<funcs.size(); i++)
    {
        vector<string> func_args = funcs[i]->getArgs();
        if(TableStack::isVectorEqualWithConvert(func_args, *true_exp_list))
        {
            func_type = funcs[i]->getType();
            func_var_name = funcs[i]->getLLVMName();
            candidate_funcs++;

        }
        if(candidate_funcs > 1)
        {
            output::errorAmbiguousCall(yylineno, name);
        }
    }
    if(candidate_funcs == 0)
    {
        output::errorPrototypeMismatch(yylineno,name);
    }
    this->type = func_type;
    Singleton* yoav = Singleton::getInstance();

//    for(int i =0; i<true_exp_list->size(); i++)
//    {
//        Exp& exp = true_exp_list->operator[](i);
//        if (exp.getType() == "BOOL")
//        {
//            string new_var = yoav->getFreshVar();
//            yoav->addPhiStatements(exp,new_var)
//            exp.setLLVMName(new_var);
//        }
//    }
    if (name == "print")
    {
        Exp exp = true_exp_list->front();
        int str_len = true_exp_list->front().getLength();
        string dimension = "[" + std::to_string(str_len + 1) + " x i8]";
        string string_var = yoav->getFreshVar();
        string get_str = "%" + string_var + " = getelementptr " + dimension + " , "+dimension + "* @." + true_exp_list->front().getLLVMName() + ", i32 0, i32 0";
        yoav->code_buffer->emit(get_str);
        string output_string = "call void (i8*) @print(i8* %" + string_var + ")";
        yoav->code_buffer->emit(output_string);
        return;
    }
    if (name == "printi")
    {
        string output_string = "call void @printi(i32 %" + true_exp_list->front().getLLVMName() + ")";
        yoav->code_buffer->emit(output_string);
        return;
    }
    this->llvm_var = yoav->getFreshVar();
    yoav->addFunctionCall(this->llvm_var,func_var_name,*true_exp_list, func_type);
}

