#include "gtest/gtest.h"

#include "Parser.h"
#include "AstVisitor.h"

using namespace ink;

class BinVisitor: public VisitorBase
{
    public:
        typedef void (*VisitProc)(AstBinaryExp*);

        virtual void Visit(AstBinaryExp* t)
        {
            proc_(t);
        }

        void SetProc(VisitProc proc) { proc_ = proc; }

    private:
        VisitProc proc_;
};

TEST(ink_test_suit, test_var_definition)
{
    const char* txt = "a = 23 b = a + 2.2";
    Parser p(txt, "dummy.cc");

    p.StartParsing();
    std::vector<AstBasePtr>& res = p.GetResult();

    ASSERT_EQ(2, res.size());

    AstBasePtr pt = res[0];
    ASSERT_EQ(AST_OP_BINARY, pt->GetType());

    AstBinaryExpPtr bpt = boost::dynamic_pointer_cast<AstBinaryExp>(pt);

    ASSERT_EQ(TOK_AS, bpt->GetOpType());

    AstBasePtr lhs = bpt->GetLeftOperand();
    ASSERT_EQ(AST_VAR, lhs->GetType());
    AstVarExpPtr spl = boost::dynamic_pointer_cast<AstVarExp>(lhs);
    ASSERT_STREQ("a", spl->GetName().c_str());

    AstBasePtr rhs = bpt->GetRightOperand();
    ASSERT_EQ(AST_INT, rhs->GetType());
    AstIntExpPtr spr = boost::dynamic_pointer_cast<AstIntExp>(rhs);
    ASSERT_EQ(23, spr->GetValue());

    // 2th expression
    AstBasePtr pt2 = res[1];
    ASSERT_EQ(AST_OP_BINARY, pt2->GetType());

    AstBinaryExpPtr bpt2 = boost::dynamic_pointer_cast<AstBinaryExp>(pt2);

    ASSERT_EQ(TOK_AS, bpt2->GetOpType());

    AstBasePtr lhs2 = bpt2->GetLeftOperand();
    ASSERT_EQ(AST_VAR, lhs2->GetType());
    AstVarExpPtr spl2 = boost::dynamic_pointer_cast<AstVarExp>(lhs2);
    ASSERT_STREQ("b", spl2->GetName().c_str());

    AstBasePtr rhs2 = bpt2->GetRightOperand();
    ASSERT_EQ(AST_OP_BINARY, rhs2->GetType());
    AstBinaryExpPtr spr2 = boost::dynamic_pointer_cast<AstBinaryExp>(rhs2);

    ASSERT_EQ(AST_OP_BINARY, spr2->GetType());
    ASSERT_EQ(TOK_ADD, spr2->GetOpType());

    AstBasePtr op1 = spr2->GetLeftOperand();
    AstBasePtr op2 = spr2->GetRightOperand();

    ASSERT_EQ(AST_VAR, op1->GetType());
    AstVarExpPtr var_op1 = boost::dynamic_pointer_cast<AstVarExp>(op1);
    ASSERT_STREQ("a", var_op1->GetName().c_str());

    ASSERT_EQ(AST_FLOAT, op2->GetType());
    AstFloatExpPtr float_op2 = boost::dynamic_pointer_cast<AstFloatExp>(op2);
    ASSERT_DOUBLE_EQ(2.2, float_op2->GetValue());
}

TEST(ink_test_suit, test_var_calc)
{
    // binary & unary operator

    const char* txt = "a = 23 - a * (2 + c) + 2.2\n a + (c) + ~2 * 3";
    /*
                |+|                   |  +  |
                /  \                 /      \
               /    \               /        \
             |-|   |2.2|          |+|        |*|
             / \                  / \        /  \
            /   \                /   \      /    \
          |23|  |*|            |a|  |c|    |~|   |3|
                / \                        /
               /   \                      /
             |a|   |+|                   |2|
                   / \
                  /   \
                |2|   |c|
   */


    Parser p(txt, "dummy.cc");

    p.StartParsing();
    std::vector<AstBasePtr>& res = p.GetResult();

    ASSERT_EQ(2, res.size());

    AstBasePtr ep1 = res[0];
    AstBasePtr ep2 = res[1];

    ASSERT_EQ(AST_OP_BINARY, ep1->GetType());
    ASSERT_EQ(AST_OP_BINARY, ep2->GetType());

    AstBinaryExpPtr bsp1 = boost::dynamic_pointer_cast<AstBinaryExp>(ep1);
    AstBinaryExpPtr bsp2 = boost::dynamic_pointer_cast<AstBinaryExp>(ep2);

    ASSERT_EQ(TOK_AS, bsp1->GetOpType());
    ASSERT_EQ(TOK_ADD, bsp2->GetOpType());

    AstBasePtr sp1 = bsp1->GetLeftOperand();
    AstBasePtr sp2 = bsp1->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, sp2->GetType());

    AstBinaryExpPtr bin_sp = boost::dynamic_pointer_cast<AstBinaryExp>(sp2);
    ASSERT_EQ(TOK_ADD, bin_sp->GetOpType());

    sp1 = bin_sp->GetLeftOperand();
    sp2 = bin_sp->GetRightOperand();

    ASSERT_EQ(AST_OP_BINARY, sp1->GetType());
    ASSERT_EQ(AST_FLOAT, sp2->GetType());

    AstFloatExpPtr flt_sp = boost::dynamic_pointer_cast<AstFloatExp>(sp2);
    ASSERT_DOUBLE_EQ(2.2, flt_sp->GetValue());

    bin_sp = boost::dynamic_pointer_cast<AstBinaryExp>(sp1);
    ASSERT_EQ(TOK_SUB, bin_sp->GetOpType());

    sp1 = bin_sp->GetLeftOperand();
    sp2 = bin_sp->GetRightOperand();

    ASSERT_EQ(AST_INT, sp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, sp2->GetType());

    AstIntExpPtr int_sp = boost::dynamic_pointer_cast<AstIntExp>(sp1);
    ASSERT_EQ(23, int_sp->GetValue());

    bin_sp = boost::dynamic_pointer_cast<AstBinaryExp>(sp2);
    ASSERT_EQ(TOK_MUL, bin_sp->GetOpType());

    sp1 = bin_sp->GetLeftOperand();
    sp2 = bin_sp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, sp2->GetType());

    bin_sp = boost::dynamic_pointer_cast<AstBinaryExp>(sp2);
    ASSERT_EQ(TOK_ADD, bin_sp->GetOpType());

    sp1 = bin_sp->GetLeftOperand();
    sp2 = bin_sp->GetRightOperand();

    ASSERT_EQ(AST_INT, sp1->GetType());
    ASSERT_EQ(AST_VAR, sp2->GetType());

    // 2th expression
    bin_sp = boost::dynamic_pointer_cast<AstBinaryExp>(bsp2);
    ASSERT_EQ(TOK_ADD, bin_sp->GetOpType());

    sp1 = bin_sp->GetLeftOperand();
    sp2 = bin_sp->GetRightOperand();

    ASSERT_EQ(AST_OP_BINARY, sp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, sp2->GetType());

    bin_sp = boost::dynamic_pointer_cast<AstBinaryExp>(sp1);
    ASSERT_EQ(TOK_ADD, bin_sp->GetOpType());

    AstBasePtr sp = bin_sp->GetLeftOperand();
    ASSERT_EQ(AST_VAR, sp->GetType());

    sp = bin_sp->GetRightOperand();
    ASSERT_EQ(AST_VAR, sp->GetType());

    bin_sp = boost::dynamic_pointer_cast<AstBinaryExp>(sp2);
    ASSERT_EQ(TOK_MUL, bin_sp->GetOpType());

    sp1 = bin_sp->GetLeftOperand();
    sp2 = bin_sp->GetRightOperand();

    ASSERT_EQ(AST_OP_UNARY, sp1->GetType());
    ASSERT_EQ(AST_INT, sp2->GetType());

    int_sp = boost::dynamic_pointer_cast<AstIntExp>(sp2);
    ASSERT_EQ(3, int_sp->GetValue());

    AstUnaryExpPtr usp = boost::dynamic_pointer_cast<AstUnaryExp>(sp1);
    ASSERT_EQ(TOK_INV, usp->GetOpType());

    sp = usp->GetOperand();
    ASSERT_EQ(AST_INT, sp->GetType());

    int_sp = boost::dynamic_pointer_cast<AstIntExp>(sp);
    ASSERT_EQ(2, int_sp->GetValue());

    // 35 -> 36 -> 37 -> 38 -> 39 -> 40 -> 41 41 -> 42 42 42 42
    // =  -> || -> && -> |  -> ^  -> &  -> == != -> < <= > >=
    // 43 43 -> 44 44 -> 45 45 -> 46 -> 47 -> 48 48
    // >> << -> + - -> * / -> % -> power -> ~ !
    txt = "a = (b || c && !d | e) + (f | g ^ h & i + k == j)";
    p.SetBuffer(txt);
    p.StartParsing();

    std::vector<AstBasePtr>& ret = p.GetResult();
    ASSERT_EQ(1, ret.size());

    sp = ret[0];
    ASSERT_EQ(AST_OP_BINARY, sp->GetType());

    AstBinaryExpPtr bsp = boost::dynamic_pointer_cast<AstBinaryExp>(sp);
    ASSERT_EQ(TOK_AS, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, sp2->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(sp2);
    ASSERT_EQ(TOK_ADD, bsp->GetOpType());

    AstBasePtr fsp1 = bsp->GetLeftOperand();
    AstBasePtr fsp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_OP_BINARY, fsp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, fsp2->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(fsp1);
    ASSERT_EQ(TOK_LOR, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, sp2->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(sp2);
    ASSERT_EQ(TOK_LAND, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, sp2->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(sp2);
    ASSERT_EQ(TOK_OR, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp2->GetType());
    ASSERT_EQ(AST_OP_UNARY, sp1->GetType());

    usp = boost::dynamic_pointer_cast<AstUnaryExp>(sp1);
    ASSERT_EQ(TOK_NEG, usp->GetOpType());

    /// (f | g ^ h & i + k == j)
    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(fsp2);
    ASSERT_EQ(TOK_OR, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, sp2->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(sp2);
    ASSERT_EQ(TOK_XOR, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, sp2->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(sp2);
    ASSERT_EQ(TOK_AND, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, sp2->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(sp2);
    ASSERT_EQ(TOK_EQ, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_OP_BINARY, sp1->GetType());
    ASSERT_EQ(AST_VAR, sp2->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(sp1);
    ASSERT_EQ(TOK_ADD, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_VAR, sp2->GetType());
}

TEST(ink_test_suit, test_array_definition)
{
    const char* txt = "a = [2, 3, \"abc\"] b = [] c = a[0] + b[1]";
    Parser p(txt, "dummy.cc");

    p.StartParsing();
    std::vector<AstBasePtr>& res = p.GetResult();

    ASSERT_EQ(3, res.size());

    ASSERT_EQ(AST_OP_BINARY, res[0]->GetType());
    ASSERT_EQ(AST_OP_BINARY, res[1]->GetType());
    ASSERT_EQ(AST_OP_BINARY, res[2]->GetType());

    AstBinaryExpPtr bp1 = boost::dynamic_pointer_cast<AstBinaryExp>(res[0]);
    AstBinaryExpPtr bp2 = boost::dynamic_pointer_cast<AstBinaryExp>(res[1]);
    AstBinaryExpPtr bp3 = boost::dynamic_pointer_cast<AstBinaryExp>(res[2]);

    ASSERT_EQ(TOK_AS, bp1->GetOpType());
    ASSERT_EQ(TOK_AS, bp2->GetOpType());
    ASSERT_EQ(TOK_AS, bp3->GetOpType());

    AstBasePtr vpl = bp1->GetLeftOperand();
    AstBasePtr vpr = bp1->GetRightOperand();

    ASSERT_EQ(AST_VAR, vpl->GetType());
    ASSERT_EQ(AST_ARR, vpr->GetType());

    AstVarExpPtr var_sp = boost::dynamic_pointer_cast<AstVarExp>(vpl);
    ASSERT_STREQ("a", var_sp->GetName().c_str());

    AstArrayExpPtr arr_sp = boost::dynamic_pointer_cast<AstArrayExp>(vpr);
    std::vector<AstBasePtr> arr = arr_sp->GetArray();

    ASSERT_EQ(3, arr.size());
    ASSERT_EQ(AST_INT, arr[0]->GetType());
    ASSERT_EQ(AST_INT, arr[1]->GetType());
    ASSERT_EQ(AST_STRING, arr[2]->GetType());

    AstIntExpPtr int_sp = boost::dynamic_pointer_cast<AstIntExp>(arr[0]);
    ASSERT_EQ(2, int_sp->GetValue());

    int_sp = boost::dynamic_pointer_cast<AstIntExp>(arr[1]);
    ASSERT_EQ(3, int_sp->GetValue());

    AstStringExpPtr str_sp = boost::dynamic_pointer_cast<AstStringExp>(arr[2]);
    ASSERT_STREQ("abc", str_sp->GetValue().c_str());

    // 2th arr definition
    vpl = bp2->GetLeftOperand();
    vpr = bp2->GetRightOperand();

    ASSERT_EQ(AST_VAR, vpl->GetType());
    ASSERT_EQ(AST_ARR, vpr->GetType());

    var_sp = boost::dynamic_pointer_cast<AstVarExp>(vpl);
    ASSERT_STREQ("b", var_sp->GetName().c_str());

    arr_sp = boost::dynamic_pointer_cast<AstArrayExp>(vpr);
    arr = arr_sp->GetArray();

    ASSERT_EQ(0, arr.size());

    // 3th arr definition
    vpl = bp3->GetLeftOperand();
    vpr = bp3->GetRightOperand();

    ASSERT_EQ(AST_VAR, vpl->GetType());
    ASSERT_EQ(AST_OP_BINARY, vpr->GetType());

    var_sp = boost::dynamic_pointer_cast<AstVarExp>(vpl);
    ASSERT_STREQ("c", var_sp->GetName().c_str());

    AstBinaryExpPtr bin_sp = boost::dynamic_pointer_cast<AstBinaryExp>(vpr);
    ASSERT_EQ(TOK_ADD, bin_sp->GetOpType());

    AstBasePtr spl = bin_sp->GetLeftOperand();
    AstBasePtr spr = bin_sp->GetRightOperand();

    ASSERT_EQ(AST_ARR_INDEX, spl->GetType());
    ASSERT_EQ(AST_ARR_INDEX, spr->GetType());

    AstArrayIndexExpPtr ispl = boost::dynamic_pointer_cast<AstArrayIndexExp>(spl);
    AstArrayIndexExpPtr ispr = boost::dynamic_pointer_cast<AstArrayIndexExp>(spr);

    ASSERT_STREQ("a", ispl->GetArrayName().c_str());
    ASSERT_STREQ("b", ispr->GetArrayName().c_str());

    AstBasePtr inp1 = ispl->GetIndexAst();
    AstBasePtr inp2 = ispr->GetIndexAst();
    ASSERT_EQ(AST_INT, inp1->GetType());
    ASSERT_EQ(AST_INT, inp2->GetType());

    AstIntExpPtr int_sp1 = boost::dynamic_pointer_cast<AstIntExp>(inp1);
    AstIntExpPtr int_sp2 = boost::dynamic_pointer_cast<AstIntExp>(inp2);

    ASSERT_EQ(0, int_sp1->GetValue());
    ASSERT_EQ(1, int_sp2->GetValue());
}

TEST(ink_test_suit, test_array_indexing)
{
    const char* txt = "a = b[2] \n c = d[ a * 2 + 1]";
    Parser p(txt, "dummy.cc");

    p.StartParsing();
    std::vector<AstBasePtr> res = p.GetResult();

    ASSERT_EQ(2, res.size());
    AstBasePtr asp1 = res[0];

    ASSERT_EQ(AST_OP_BINARY, asp1->GetType());

    AstBinaryExpPtr bsp = boost::dynamic_pointer_cast<AstBinaryExp>(asp1);
    ASSERT_EQ(TOK_AS, bsp->GetOpType());

    AstBasePtr sp1 = bsp->GetLeftOperand();
    AstBasePtr sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_ARR_INDEX, sp2->GetType());

    AstArrayIndexExpPtr aip =
        boost::dynamic_pointer_cast<AstArrayIndexExp>(sp2);

    ASSERT_STREQ("b", aip->GetArrayName().c_str());
    AstBasePtr bp = aip->GetIndexAst();
    ASSERT_EQ(AST_INT, bp->GetType());

    AstIntExpPtr isp = boost::dynamic_pointer_cast<AstIntExp>(bp);
    ASSERT_EQ(2, isp->GetValue());

    // 2th expression

    AstBasePtr asp2 = res[1];

    ASSERT_EQ(AST_OP_BINARY, asp2->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(asp2);
    ASSERT_EQ(TOK_AS, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_ARR_INDEX, sp2->GetType());

    aip = boost::dynamic_pointer_cast<AstArrayIndexExp>(sp2);

    ASSERT_STREQ("d", aip->GetArrayName().c_str());

    bp = aip->GetIndexAst();
    ASSERT_EQ(AST_OP_BINARY, bp->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(bp);
    ASSERT_EQ(TOK_ADD, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_OP_BINARY, sp1->GetType());
    ASSERT_EQ(AST_INT, sp2->GetType());

    isp = boost::dynamic_pointer_cast<AstIntExp>(sp2);
    ASSERT_EQ(1, isp->GetValue());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(sp1);
    ASSERT_EQ(TOK_MUL, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_INT, sp2->GetType());

    isp = boost::dynamic_pointer_cast<AstIntExp>(sp2);
    ASSERT_EQ(2, isp->GetValue());

    AstVarExpPtr vsp = boost::dynamic_pointer_cast<AstVarExp>(sp1);
    ASSERT_STREQ("a", vsp->GetName().c_str());
}

TEST(ink_test_suit, test_if_statement)
{
    const char* txt = "if (23) { a = 2 } elif (a) { a = a + 1 }\n"
        "elif (!s) { if (a && b) {} } else {}";

    Parser p(txt, "dummy.cc");

    p.StartParsing();
    std::vector<AstBasePtr> res = p.GetResult();

    ASSERT_EQ(1, res.size());
    AstBasePtr asp = res[0];

    ASSERT_EQ(AST_IF, asp->GetType());

    AstIfExpPtr ifp = boost::dynamic_pointer_cast<AstIfExp>(asp);

    std::vector<AstIfExp::IfEntity> exe = ifp->GetBody();
    ASSERT_EQ(4, exe.size());

    AstBasePtr cp = exe[0].cond;
    AstScopeStatementExpPtr scp = exe[0].exp;

    ASSERT_EQ(AST_INT, cp->GetType());
    AstIntExpPtr isp = boost::dynamic_pointer_cast<AstIntExp>(cp);
    ASSERT_EQ(23, isp->GetValue());

    std::vector<AstBasePtr> exp = scp->GetBody();
    ASSERT_EQ(1, exp.size());

    asp = exp[0];
    ASSERT_EQ(AST_OP_BINARY, asp->GetType());

    AstBinaryExpPtr bsp = boost::dynamic_pointer_cast<AstBinaryExp>(asp);

    AstBasePtr sp1 = bsp->GetLeftOperand();
    AstBasePtr sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_INT, sp2->GetType());

    // elif
    cp = exe[1].cond;
    scp = exe[1].exp;

    ASSERT_EQ(AST_VAR, cp->GetType());
    AstVarExpPtr vsp = boost::dynamic_pointer_cast<AstVarExp>(cp);
    ASSERT_STREQ("a", vsp->GetName().c_str());

    exp = scp->GetBody();
    ASSERT_EQ(1, exp.size());

    asp = exp[0];
    ASSERT_EQ(AST_OP_BINARY, asp->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(asp);

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_OP_BINARY, sp2->GetType());

    // elif (!s) { if (a && b) {} }
    cp = exe[2].cond;
    scp = exe[2].exp;

    ASSERT_EQ(AST_OP_UNARY, cp->GetType());
    AstUnaryExpPtr usp = boost::dynamic_pointer_cast<AstUnaryExp>(cp);
    ASSERT_EQ(TOK_NEG, usp->GetOpType());

    exp = scp->GetBody();
    ASSERT_EQ(1, exp.size());

    asp = exp[0];
    ASSERT_EQ(AST_IF, asp->GetType());
    AstIfExpPtr ifsp = boost::dynamic_pointer_cast<AstIfExp>(asp);

    std::vector<AstIfExp::IfEntity> vi2 = ifsp->GetBody();
    ASSERT_EQ(1, vi2.size());

    asp = vi2[0].cond;
    ASSERT_EQ(0, vi2[0].exp->GetBody().size());

    ASSERT_EQ(AST_OP_BINARY, asp->GetType());
    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(asp);
    ASSERT_EQ(TOK_LAND, bsp->GetOpType());

    sp1 = bsp->GetLeftOperand();
    sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_VAR, sp2->GetType());

    cp = exe[3].cond;
    scp = exe[3].exp;

    ASSERT_TRUE(!cp);

    exp = scp->GetBody();
    ASSERT_EQ(0, exp.size());
}

TEST(ink_test_suit, test_while_statement)
{
    const char* txt = "while (a || c) { a + 2; if (1) { a = 2} }";
    Parser p(txt, "dummy.cc");

    p.StartParsing();
    std::vector<AstBasePtr>& res = p.GetResult();

    ASSERT_EQ(1, res.size());

    AstBasePtr asp = res[0];
    ASSERT_EQ(AST_WHILE, asp->GetType());

    AstWhileExpPtr wsp = boost::dynamic_pointer_cast<AstWhileExp>(asp);

    asp = wsp->GetCondition();
    AstScopeStatementExpPtr body = wsp->GetBody();

    ASSERT_EQ(AST_OP_BINARY, asp->GetType());

    AstBinaryExpPtr bsp = boost::dynamic_pointer_cast<AstBinaryExp>(asp);
    ASSERT_EQ(TOK_LOR, bsp->GetOpType());
    AstBasePtr sp1 = bsp->GetLeftOperand();
    AstBasePtr sp2 = bsp->GetRightOperand();

    ASSERT_EQ(AST_VAR, sp1->GetType());
    ASSERT_EQ(AST_VAR, sp2->GetType());

    std::vector<AstBasePtr> exp = body->GetBody();
    ASSERT_EQ(2, exp.size());

    asp = exp[0];
    ASSERT_EQ(AST_OP_BINARY, asp->GetType());
    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(asp);
    ASSERT_EQ(TOK_ADD, bsp->GetOpType());

    asp = exp[1];
    ASSERT_EQ(AST_IF, asp->GetType());

    AstIfExpPtr ifsp = boost::dynamic_pointer_cast<AstIfExp>(asp);
    std::vector<AstIfExp::IfEntity> en = ifsp->GetBody();

    ASSERT_EQ(1, en.size());
    asp = en[0].cond;
    ASSERT_EQ(AST_INT, asp->GetType());

    AstScopeStatementExpPtr scp = en[0].exp;
    exp = scp->GetBody();
    ASSERT_EQ(1, exp.size());

    asp = exp[0];
    ASSERT_EQ(AST_OP_BINARY, asp->GetType());

    bsp = boost::dynamic_pointer_cast<AstBinaryExp>(asp);
    ASSERT_EQ(TOK_AS, bsp->GetOpType());
}

TEST(ink_test_suit, test_func_declaration)
{
}

TEST(ink_test_suit, test_func_definition)
{
}

TEST(ink_test_suit, test_func_call)
{
}

TEST(ink_test_suit, test_class_def)
{
}
