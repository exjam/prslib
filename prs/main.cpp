#include "parser.h"

#include <string>
#include <iostream>

auto whitespace = atomic(*(char_(' ') | char_('\t') | char_('\n') | char_('\r')));

struct Node
{
   enum Type
   {
      Constant,
      BinaryOp
   };

   Node(Type type) : type(type)
   {
   }

   virtual void print() = 0;
   int type;
};

struct Constant : public Node
{
   Constant(std::string value) : Node(Node::Constant), value(value)
   {
   }
   virtual void print()
   {
      std::cout << value;
   }

   std::string value;
};

struct BinaryOp : public Node
{
   BinaryOp(char op, Node *left, Node *right) : Node(Node::BinaryOp), op(op), left(left), right(right)
   {
   }
   virtual void print()
   {
      std::cout << '(';
      left->print();
      std::cout << op;
      right->print();
      std::cout << ')';
   }

   char op;
   Node *left;
   Node *right;
};

struct ast_term
{
   Node *node;

   template<typename Type>
   void construct(Type &in)
   {
      node = std::get<0>(in).node;

      for (auto &right : std::get<1>(in)) {
         node = new BinaryOp(std::get<0>(right), node, std::get<1>(right).node);
      }
   }
};

struct ast_factor
{
   Node *node;

   template<typename Type>
   void construct(Type &in)
   {
      if (in.which == 0) {
         node = new Constant(ast_to_string<std::string>(*std::get<0>(in)));
      } else {
         node = std::get<1>(*std::get<1>(in)).node;
      }
   }
};

struct ast_expression
{
   Node *node;

   template<typename Type>
   void construct(Type &in)
   {
      node = std::get<0>(in).node;

      for (auto &right : std::get<1>(in)) {
         node = new BinaryOp(std::get<0>(right), node, std::get<1>(right).node);
      }
   }
};

struct ParseContext
{
   decltype(whitespace) whitespace_parser;
};

template<typename AstType> using rule_ = rule<AstType, std::string::iterator, ParseContext>;

rule_<ast_expression> expression;
rule_<ast_factor> factor;
rule_<ast_term> term;

auto constant       = atomic(+char_range('0', '9'));
auto factor_def     = constant | (char_('(') >> expression >> char_(')'));
auto term_def       = factor >> *((char_('*') >> factor) | (char_('/') >> factor));
auto expression_def = term >> *((char_('+') >> term) | (char_('-') >> term));

/*
constant    = +[0-9];
factor      = constant | '(' >> expression >> ')';
term        = factor >> *(('*' >> factor) | ('/' >> factor));
expression  = term >> *(('+' >> term) | ('-' >> term));
*/
int main()
{
   expression = expression_def;
   factor = factor_def;
   term = term_def;

   ParseContext ctx = { whitespace };

   std::string input = "1 + 2*4 - 5 / 3";
   auto pos = input.begin();

   ast_expression ast;
   expression.parse(pos, input.end(), ctx, ast);

   Node *root = ast.node;
   root->print();
   return 0;
}
