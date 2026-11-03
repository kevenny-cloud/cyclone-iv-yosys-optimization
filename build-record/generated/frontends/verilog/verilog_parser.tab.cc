// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.


// Take the name prefix into account.
#define yylex   frontend_verilog_yylex



#include "verilog_parser.tab.hh"


// Unqualified %code blocks.
#line 127 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"

	#include <list>
	#include <stack>
	#include <memory>
	#include <string.h>
	#include "kernel/log.h"
	#include "frontends/verilog/verilog_lexer.h"

	USING_YOSYS_NAMESPACE
	using namespace AST;
	using namespace VERILOG_FRONTEND;

	// Silly little C adapter between C++ bison and C++ flex
	auto frontend_verilog_yylex(YOSYS_NAMESPACE_PREFIX VERILOG_FRONTEND::VerilogLexer* lexer) {
		return lexer->nextToken();
	}

	#define SET_AST_NODE_LOC(WHICH, BEGIN, END) (WHICH)->location = location_range(BEGIN, END)

	#define SET_RULE_LOC(LHS, BEGIN, END) \
		do { (LHS).begin = BEGIN.begin; \
		(LHS).end = (END).end; } while(0)

	YOSYS_NAMESPACE_BEGIN
	namespace VERILOG_FRONTEND {

		static Location location_range(Location begin, Location end) {
			return Location(begin.begin, end.end);
		}

		static void append_attr(AstNode *ast, std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>> al)
		{
			for (auto &it : *al) {
				ast->attributes[it.first] = std::move(it.second);
			}
		}

		static void append_attr_clone(AstNode *ast, std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>> &al)
		{
			for (auto &it : *al) {
				ast->attributes[it.first] = it.second->clone();
			}
		}

		static std::unique_ptr<AstNode> makeRange(parser::location_type loc, int msb = 31, int lsb = 0, bool isSigned = true)
		{
			auto range = std::make_unique<AstNode>(loc, AST_RANGE);
			range->children.push_back(AstNode::mkconst_int(loc, msb, true));
			range->children.push_back(AstNode::mkconst_int(loc, lsb, true));
			range->is_signed = isSigned;
			return range;
		}

		static void addRange(AstNode *parent, int msb = 31, int lsb = 0, bool isSigned = true)
		{
			auto range = makeRange(parent->location, msb, lsb, isSigned);
			parent->children.push_back(std::move(range));
		}

		static std::unique_ptr<AstNode> checkRange(AstNode *type_node, std::unique_ptr<AstNode> range_node)
		{
			if (type_node->range_left >= 0 && type_node->range_right >= 0) {
				// type already restricts the range
				if (range_node) {
					err_at_loc(type_node->location, "integer/genvar types cannot have packed dimensions.");
				}
				else {
					range_node = makeRange(type_node->location, type_node->range_left, type_node->range_right, false);
				}
			}

			if (range_node) {
				bool valid = true;
				if (range_node->type == AST_RANGE) {
					valid = range_node->children.size() == 2;
				} else {  // AST_MULTIRANGE
					for (auto& child : range_node->children) {
						valid = valid && child->children.size() == 2;
					}
				}
				if (!valid)
					err_at_loc(type_node->location, "wire/reg/logic packed dimension must be of the form [<expr>:<expr>]");
			}

			return range_node;
		}

		static void rewriteRange(AstNode *rangeNode)
		{
			if (rangeNode->type == AST_RANGE && rangeNode->children.size() == 1) {
				// SV array size [n], rewrite as [0:n-1]
				rangeNode->children.push_back(std::make_unique<AstNode>(rangeNode->location, AST_SUB, std::move(rangeNode->children[0]), AstNode::mkconst_int(rangeNode->location, 1, true)));
				rangeNode->children[0] = AstNode::mkconst_int(rangeNode->location, 0, false);
			}
		}

		static void rewriteAsMemoryNode(AstNode *node, std::unique_ptr<AstNode> rangeNode)
		{
			node->type = AST_MEMORY;
			if (rangeNode->type == AST_MULTIRANGE) {
				for (auto& child : rangeNode->children)
					rewriteRange(child.get());
			} else
				rewriteRange(rangeNode.get());
			node->children.push_back(std::move(rangeNode));
		}

		static void checkLabelsMatch(const frontend_verilog_yy::parser::location_type& loc, const char *element, const std::string* before, const std::string *after)
		{
			if (!before && after)
				err_at_loc(loc, "%s missing where end label (%s) was given.",
					element, after->c_str() + 1);
			if (before && after && *before != *after)
				err_at_loc(loc, "%s (%s) and end label (%s) don't match.",
					element, before->c_str() + 1, after->c_str() + 1);
		}

		AstNode* ParseState::saveChild(std::unique_ptr<AstNode> child) {
			auto* child_leaky = child.get();
			ast_stack.back()->children.push_back(std::move(child));
			return child_leaky;
		}
		AstNode* ParseState::pushChild(std::unique_ptr<AstNode> child) {
			auto* child_leaky = saveChild(std::move(child));
			ast_stack.push_back(child_leaky);
			return child_leaky;
		}

		void ParseState::addWiretypeNode(std::string *name, AstNode* node)
		{
			log_assert(node);
			node->is_custom_type = true;
			node->children.push_back(std::make_unique<AstNode>(node->location, AST_WIRETYPE));
			node->children.back()->str = *name;
		}

		void ParseState::addTypedefNode(std::string *name, std::unique_ptr<AstNode> node)
		{
			log_assert((bool)node);
			AstNode* tnode = saveChild(std::make_unique<AstNode>(node->location, AST_TYPEDEF, std::move(node)));
			log_assert((bool)name);
			tnode->str = *name;
			auto &user_types = user_type_stack.back();
			user_types[*name] = tnode;
			if (current_ast_mod && current_ast_mod->type == AST_PACKAGE) {
				// typedef inside a package so we need the qualified name
				auto qname = current_ast_mod->str + "::" + (*name).substr(1);
				pkg_user_types[qname] = tnode;
			}
		}

		void ParseState::enterTypeScope()
		{
			user_type_stack.push_back(UserTypeMap());
		}

		void ParseState::exitTypeScope()
		{
			user_type_stack.pop_back();
		}

		bool ParseState::isInLocalScope(const std::string *name)
		{
			// tests if a name was declared in the current block scope
			auto &user_types = user_type_stack.back();
			return (user_types.count(*name) > 0);
		}

		// This transforms a loop like
		//   for (genvar i = 0; i < 10; i++) begin : blk
		// to
		//   genvar _i;
		//   for (_i = 0; _i < 10; _i++) begin : blk
		//     localparam i = _i;
		// where `_i` is actually some auto-generated name.
		void ParseState::rewriteGenForDeclInit(AstNode *loop)
		{
			// check if this generate for loop contains an inline declaration
			log_assert(loop->type == AST_GENFOR);
			auto& decl = loop->children[0];
			if (decl->type == AST_ASSIGN_EQ)
				return;

			log_assert(decl->type == AST_GENVAR);
			log_assert(loop->children.size() == 5);

			// identify each component of the loop
			AstNode *init = loop->children[1].get();
			AstNode *cond = loop->children[2].get();
			AstNode *incr = loop->children[3].get();
			AstNode *body = loop->children[4].get();
			log_assert(init->type == AST_ASSIGN_EQ);
			log_assert(incr->type == AST_ASSIGN_EQ);
			log_assert(body->type == AST_GENBLOCK);

			// create a unique name for the genvar
			std::string old_str = decl->str;
			std::string new_str = stringf("$genfordecl$%d$%s", autoidx++, old_str);

			// rename and move the genvar declaration to the containing description
			decl->str = new_str;
			log_assert(current_ast_mod != nullptr);
			current_ast_mod->children.push_back(std::move(decl));

			// create a new localparam with old name so that the items in the loop
			// can simply use the old name and shadow it as necessary
			auto indirect = std::make_unique<AstNode>(loop->location, AST_LOCALPARAM);
			indirect->str = old_str;
			auto ident = std::make_unique<AstNode>(loop->location, AST_IDENTIFIER);
			ident->str = new_str;
			indirect->children.push_back(std::move(ident));

			body->children.insert(body->children.begin(), std::move(indirect));

			// only perform the renaming for the initialization, guard, and
			// incrementation to enable proper shadowing of the synthetic localparam
			std::function<void(AstNode*)> substitute = [&](AstNode *node) {
				if (node->type == AST_IDENTIFIER && node->str == old_str)
					node->str = new_str;
				for (auto& child : node->children)
					substitute(child.get());
			};
			substitute(init);
			substitute(cond);
			substitute(incr);
			loop->children.erase(loop->children.begin());
		}

		void ParseState::ensureAsgnExprAllowed(const parser::location_type loc, bool sv_mode)
		{
			if (!sv_mode)
				err_at_loc(loc, "Assignments within expressions are only supported in SystemVerilog mode.");
			if (ast_stack.back()->type != AST_BLOCK)
				err_at_loc(loc, "Assignments within expressions are only permitted within procedures.");
		}

		// add a pre/post-increment/decrement statement
		const AstNode *ParseState::addIncOrDecStmt(std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>> stmt_attr,
							std::unique_ptr<AstNode> lhs,
							std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>> op_attr, AST::AstNodeType op,
							Location loc)
		{
			auto one = AstNode::mkconst_int(loc, 1, true);
			auto rhs = std::make_unique<AstNode>(loc, op, lhs->clone(), std::move(one));
			if (op_attr)
				append_attr(rhs.get(), std::move(op_attr));
			auto stmt_owned = std::make_unique<AstNode>(loc, AST_ASSIGN_EQ, std::move(lhs), std::move(rhs));
			auto* stmt = stmt_owned.get();
			ast_stack.back()->children.push_back(std::move(stmt_owned));
			if (stmt_attr)
				append_attr(stmt, std::move(stmt_attr));
			return stmt;
		}

		// create a pre/post-increment/decrement expression, and add the corresponding statement
		std::unique_ptr<AstNode> ParseState::addIncOrDecExpr(std::unique_ptr<AstNode> lhs,
								     std::unique_ptr<dict<IdString,
								     std::unique_ptr<AstNode>>> attr,
								     AST::AstNodeType op, Location loc, bool undo, bool sv_mode)
		{
			ensureAsgnExprAllowed(loc, sv_mode);
			const AstNode *stmt = addIncOrDecStmt(nullptr, std::move(lhs), std::move(attr), op, loc);
			log_assert(stmt->type == AST_ASSIGN_EQ);
			auto expr = stmt->children[0]->clone();
			if (undo) {
				auto one = AstNode::mkconst_int(loc, 1, false, 1);
				auto minus_one = std::make_unique<AstNode>(loc, AST_NEG, std::move(one));
				expr = std::make_unique<AstNode>(loc, op, std::move(expr), std::move(minus_one));
			}
			return expr;
		}

		// add a binary operator assignment statement, e.g., a += b
		std::unique_ptr<AstNode> ParseState::addAsgnBinopStmt(std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>> attr,
								      std::unique_ptr<AstNode> eq_lhs, AST::AstNodeType op, std::unique_ptr<AstNode> rhs)
		{
			Location loc = location_range(eq_lhs->location, rhs->location);
			if (op == AST_SHIFT_LEFT || op == AST_SHIFT_RIGHT ||
				op == AST_SHIFT_SLEFT || op == AST_SHIFT_SRIGHT) {
				rhs = std::make_unique<AstNode>(rhs->location, AST_TO_UNSIGNED, std::move(rhs));
			}
			auto binop_lhs = eq_lhs->clone();
			auto eq_rhs_owned = std::make_unique<AstNode>(loc, op, std::move(binop_lhs), std::move(rhs));
			auto ret_lhs = eq_lhs->clone();
			auto stmt_owned = std::make_unique<AstNode>(loc, AST_ASSIGN_EQ, std::move(eq_lhs), std::move(eq_rhs_owned));
			auto* stmt = stmt_owned.get();
			ast_stack.back()->children.push_back(std::move(stmt_owned));
			if (attr)
				append_attr(stmt, std::move(attr));
			return ret_lhs;
		}
	};
	YOSYS_NAMESPACE_END

	void frontend_verilog_yy::parser::error(const frontend_verilog_yy::parser::location_type& loc, const std::string& msg)
	{
		err_at_loc(loc, "%s", msg);
	}

#line 348 "verilog_parser.tab.cc"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if FRONTEND_VERILOG_YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !FRONTEND_VERILOG_YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !FRONTEND_VERILOG_YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace frontend_verilog_yy {
#line 440 "verilog_parser.tab.cc"

  /// Build a parser object.
  parser::parser (YOSYS_NAMESPACE_PREFIX VERILOG_FRONTEND::VerilogLexer* lexer_yyarg, YOSYS_NAMESPACE_PREFIX VERILOG_FRONTEND::ParseState* extra_yyarg, YOSYS_NAMESPACE_PREFIX VERILOG_FRONTEND::ParseMode* mode_yyarg)
#if FRONTEND_VERILOG_YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      yy_lac_established_ (false),
      lexer (lexer_yyarg),
      extra (extra_yyarg),
      mode (mode_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_al_t: // al_t
      case symbol_kind::S_attr: // attr
      case symbol_kind::S_if_attr: // if_attr
      case symbol_kind::S_case_attr: // case_attr
        value.YY_MOVE_OR_COPY< al_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ast_node_type_t: // ast_node_type_t
      case symbol_kind::S_asgn_binop: // asgn_binop
      case symbol_kind::S_inc_or_dec_op: // inc_or_dec_op
        value.YY_MOVE_OR_COPY< ast_node_type_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ast_t: // ast_t
      case symbol_kind::S_io_wire_type: // io_wire_type
      case symbol_kind::S_non_io_wire_type: // non_io_wire_type
      case symbol_kind::S_wire_type: // wire_type
      case symbol_kind::S_non_opt_range: // non_opt_range
      case symbol_kind::S_non_opt_multirange: // non_opt_multirange
      case symbol_kind::S_range: // range
      case symbol_kind::S_range_or_multirange: // range_or_multirange
      case symbol_kind::S_func_return_type: // func_return_type
      case symbol_kind::S_specify_if: // specify_if
      case symbol_kind::S_specify_condition: // specify_condition
      case symbol_kind::S_enum_type: // enum_type
      case symbol_kind::S_opt_enum_init: // opt_enum_init
      case symbol_kind::S_struct_type: // struct_type
      case symbol_kind::S_struct_union: // struct_union
      case symbol_kind::S_typedef_base_type: // typedef_base_type
      case symbol_kind::S_enum_struct_type: // enum_struct_type
      case symbol_kind::S_rvalue: // rvalue
      case symbol_kind::S_lvalue: // lvalue
      case symbol_kind::S_lvalue_concat_list: // lvalue_concat_list
      case symbol_kind::S_genvar_identifier: // genvar_identifier
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_basic_expr: // basic_expr
      case symbol_kind::S_504_84: // @84
      case symbol_kind::S_concat_list: // concat_list
      case symbol_kind::S_assignment_pattern_list: // assignment_pattern_list
        value.YY_MOVE_OR_COPY< ast_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_boolean_t: // "boolean"
      case symbol_kind::S_opt_signedness_default_signed: // opt_signedness_default_signed
      case symbol_kind::S_opt_signedness_default_unsigned: // opt_signedness_default_unsigned
      case symbol_kind::S_always_comb_or_latch: // always_comb_or_latch
      case symbol_kind::S_always_or_always_ff: // always_or_always_ff
      case symbol_kind::S_opt_property: // opt_property
        value.YY_MOVE_OR_COPY< boolean_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ch_t: // "invalid token"
      case symbol_kind::S_specify_edge: // specify_edge
        value.YY_MOVE_OR_COPY< ch_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_integer_t: // "integer"
      case symbol_kind::S_integer_atom_type: // integer_atom_type
      case symbol_kind::S_integer_vector_type: // integer_vector_type
        value.YY_MOVE_OR_COPY< integer_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_specify_rise_fall_ptr_t: // "specify rise and fall"
      case symbol_kind::S_specify_rise_fall: // specify_rise_fall
        value.YY_MOVE_OR_COPY< specify_rise_fall_ptr_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_specify_target_ptr_t: // "specify target"
      case symbol_kind::S_specify_target: // specify_target
        value.YY_MOVE_OR_COPY< specify_target_ptr_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_specify_triple_ptr_t: // "specify triple"
      case symbol_kind::S_specify_opt_triple: // specify_opt_triple
      case symbol_kind::S_specify_triple: // specify_triple
        value.YY_MOVE_OR_COPY< specify_triple_ptr_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string_t: // "string"
      case symbol_kind::S_TOK_STRING: // TOK_STRING
      case symbol_kind::S_TOK_ID: // TOK_ID
      case symbol_kind::S_TOK_CONSTVAL: // TOK_CONSTVAL
      case symbol_kind::S_TOK_REALVAL: // TOK_REALVAL
      case symbol_kind::S_TOK_PRIMITIVE: // TOK_PRIMITIVE
      case symbol_kind::S_TOK_SVA_LABEL: // TOK_SVA_LABEL
      case symbol_kind::S_TOK_SPECIFY_OPER: // TOK_SPECIFY_OPER
      case symbol_kind::S_TOK_MSG_TASKS: // TOK_MSG_TASKS
      case symbol_kind::S_TOK_BASE: // TOK_BASE
      case symbol_kind::S_TOK_BASED_CONSTVAL: // TOK_BASED_CONSTVAL
      case symbol_kind::S_TOK_UNBASED_UNSIZED_CONSTVAL: // TOK_UNBASED_UNSIZED_CONSTVAL
      case symbol_kind::S_TOK_USER_TYPE: // TOK_USER_TYPE
      case symbol_kind::S_TOK_PKG_USER_TYPE: // TOK_PKG_USER_TYPE
      case symbol_kind::S_hierarchical_id: // hierarchical_id
      case symbol_kind::S_hierarchical_type_id: // hierarchical_type_id
      case symbol_kind::S_type_name: // type_name
      case symbol_kind::S_tok_prim_wrapper: // tok_prim_wrapper
      case symbol_kind::S_opt_label: // opt_label
      case symbol_kind::S_opt_sva_label: // opt_sva_label
      case symbol_kind::S_integral_number: // integral_number
        value.YY_MOVE_OR_COPY< string_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_al_t: // al_t
      case symbol_kind::S_attr: // attr
      case symbol_kind::S_if_attr: // if_attr
      case symbol_kind::S_case_attr: // case_attr
        value.move< al_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ast_node_type_t: // ast_node_type_t
      case symbol_kind::S_asgn_binop: // asgn_binop
      case symbol_kind::S_inc_or_dec_op: // inc_or_dec_op
        value.move< ast_node_type_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ast_t: // ast_t
      case symbol_kind::S_io_wire_type: // io_wire_type
      case symbol_kind::S_non_io_wire_type: // non_io_wire_type
      case symbol_kind::S_wire_type: // wire_type
      case symbol_kind::S_non_opt_range: // non_opt_range
      case symbol_kind::S_non_opt_multirange: // non_opt_multirange
      case symbol_kind::S_range: // range
      case symbol_kind::S_range_or_multirange: // range_or_multirange
      case symbol_kind::S_func_return_type: // func_return_type
      case symbol_kind::S_specify_if: // specify_if
      case symbol_kind::S_specify_condition: // specify_condition
      case symbol_kind::S_enum_type: // enum_type
      case symbol_kind::S_opt_enum_init: // opt_enum_init
      case symbol_kind::S_struct_type: // struct_type
      case symbol_kind::S_struct_union: // struct_union
      case symbol_kind::S_typedef_base_type: // typedef_base_type
      case symbol_kind::S_enum_struct_type: // enum_struct_type
      case symbol_kind::S_rvalue: // rvalue
      case symbol_kind::S_lvalue: // lvalue
      case symbol_kind::S_lvalue_concat_list: // lvalue_concat_list
      case symbol_kind::S_genvar_identifier: // genvar_identifier
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_basic_expr: // basic_expr
      case symbol_kind::S_504_84: // @84
      case symbol_kind::S_concat_list: // concat_list
      case symbol_kind::S_assignment_pattern_list: // assignment_pattern_list
        value.move< ast_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_boolean_t: // "boolean"
      case symbol_kind::S_opt_signedness_default_signed: // opt_signedness_default_signed
      case symbol_kind::S_opt_signedness_default_unsigned: // opt_signedness_default_unsigned
      case symbol_kind::S_always_comb_or_latch: // always_comb_or_latch
      case symbol_kind::S_always_or_always_ff: // always_or_always_ff
      case symbol_kind::S_opt_property: // opt_property
        value.move< boolean_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ch_t: // "invalid token"
      case symbol_kind::S_specify_edge: // specify_edge
        value.move< ch_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_integer_t: // "integer"
      case symbol_kind::S_integer_atom_type: // integer_atom_type
      case symbol_kind::S_integer_vector_type: // integer_vector_type
        value.move< integer_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_specify_rise_fall_ptr_t: // "specify rise and fall"
      case symbol_kind::S_specify_rise_fall: // specify_rise_fall
        value.move< specify_rise_fall_ptr_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_specify_target_ptr_t: // "specify target"
      case symbol_kind::S_specify_target: // specify_target
        value.move< specify_target_ptr_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_specify_triple_ptr_t: // "specify triple"
      case symbol_kind::S_specify_opt_triple: // specify_opt_triple
      case symbol_kind::S_specify_triple: // specify_triple
        value.move< specify_triple_ptr_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_string_t: // "string"
      case symbol_kind::S_TOK_STRING: // TOK_STRING
      case symbol_kind::S_TOK_ID: // TOK_ID
      case symbol_kind::S_TOK_CONSTVAL: // TOK_CONSTVAL
      case symbol_kind::S_TOK_REALVAL: // TOK_REALVAL
      case symbol_kind::S_TOK_PRIMITIVE: // TOK_PRIMITIVE
      case symbol_kind::S_TOK_SVA_LABEL: // TOK_SVA_LABEL
      case symbol_kind::S_TOK_SPECIFY_OPER: // TOK_SPECIFY_OPER
      case symbol_kind::S_TOK_MSG_TASKS: // TOK_MSG_TASKS
      case symbol_kind::S_TOK_BASE: // TOK_BASE
      case symbol_kind::S_TOK_BASED_CONSTVAL: // TOK_BASED_CONSTVAL
      case symbol_kind::S_TOK_UNBASED_UNSIZED_CONSTVAL: // TOK_UNBASED_UNSIZED_CONSTVAL
      case symbol_kind::S_TOK_USER_TYPE: // TOK_USER_TYPE
      case symbol_kind::S_TOK_PKG_USER_TYPE: // TOK_PKG_USER_TYPE
      case symbol_kind::S_hierarchical_id: // hierarchical_id
      case symbol_kind::S_hierarchical_type_id: // hierarchical_type_id
      case symbol_kind::S_type_name: // type_name
      case symbol_kind::S_tok_prim_wrapper: // tok_prim_wrapper
      case symbol_kind::S_opt_label: // opt_label
      case symbol_kind::S_opt_sva_label: // opt_sva_label
      case symbol_kind::S_integral_number: // integral_number
        value.move< string_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_al_t: // al_t
      case symbol_kind::S_attr: // attr
      case symbol_kind::S_if_attr: // if_attr
      case symbol_kind::S_case_attr: // case_attr
        value.copy< al_t > (that.value);
        break;

      case symbol_kind::S_ast_node_type_t: // ast_node_type_t
      case symbol_kind::S_asgn_binop: // asgn_binop
      case symbol_kind::S_inc_or_dec_op: // inc_or_dec_op
        value.copy< ast_node_type_t > (that.value);
        break;

      case symbol_kind::S_ast_t: // ast_t
      case symbol_kind::S_io_wire_type: // io_wire_type
      case symbol_kind::S_non_io_wire_type: // non_io_wire_type
      case symbol_kind::S_wire_type: // wire_type
      case symbol_kind::S_non_opt_range: // non_opt_range
      case symbol_kind::S_non_opt_multirange: // non_opt_multirange
      case symbol_kind::S_range: // range
      case symbol_kind::S_range_or_multirange: // range_or_multirange
      case symbol_kind::S_func_return_type: // func_return_type
      case symbol_kind::S_specify_if: // specify_if
      case symbol_kind::S_specify_condition: // specify_condition
      case symbol_kind::S_enum_type: // enum_type
      case symbol_kind::S_opt_enum_init: // opt_enum_init
      case symbol_kind::S_struct_type: // struct_type
      case symbol_kind::S_struct_union: // struct_union
      case symbol_kind::S_typedef_base_type: // typedef_base_type
      case symbol_kind::S_enum_struct_type: // enum_struct_type
      case symbol_kind::S_rvalue: // rvalue
      case symbol_kind::S_lvalue: // lvalue
      case symbol_kind::S_lvalue_concat_list: // lvalue_concat_list
      case symbol_kind::S_genvar_identifier: // genvar_identifier
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_basic_expr: // basic_expr
      case symbol_kind::S_504_84: // @84
      case symbol_kind::S_concat_list: // concat_list
      case symbol_kind::S_assignment_pattern_list: // assignment_pattern_list
        value.copy< ast_t > (that.value);
        break;

      case symbol_kind::S_boolean_t: // "boolean"
      case symbol_kind::S_opt_signedness_default_signed: // opt_signedness_default_signed
      case symbol_kind::S_opt_signedness_default_unsigned: // opt_signedness_default_unsigned
      case symbol_kind::S_always_comb_or_latch: // always_comb_or_latch
      case symbol_kind::S_always_or_always_ff: // always_or_always_ff
      case symbol_kind::S_opt_property: // opt_property
        value.copy< boolean_t > (that.value);
        break;

      case symbol_kind::S_ch_t: // "invalid token"
      case symbol_kind::S_specify_edge: // specify_edge
        value.copy< ch_t > (that.value);
        break;

      case symbol_kind::S_integer_t: // "integer"
      case symbol_kind::S_integer_atom_type: // integer_atom_type
      case symbol_kind::S_integer_vector_type: // integer_vector_type
        value.copy< integer_t > (that.value);
        break;

      case symbol_kind::S_specify_rise_fall_ptr_t: // "specify rise and fall"
      case symbol_kind::S_specify_rise_fall: // specify_rise_fall
        value.copy< specify_rise_fall_ptr_t > (that.value);
        break;

      case symbol_kind::S_specify_target_ptr_t: // "specify target"
      case symbol_kind::S_specify_target: // specify_target
        value.copy< specify_target_ptr_t > (that.value);
        break;

      case symbol_kind::S_specify_triple_ptr_t: // "specify triple"
      case symbol_kind::S_specify_opt_triple: // specify_opt_triple
      case symbol_kind::S_specify_triple: // specify_triple
        value.copy< specify_triple_ptr_t > (that.value);
        break;

      case symbol_kind::S_string_t: // "string"
      case symbol_kind::S_TOK_STRING: // TOK_STRING
      case symbol_kind::S_TOK_ID: // TOK_ID
      case symbol_kind::S_TOK_CONSTVAL: // TOK_CONSTVAL
      case symbol_kind::S_TOK_REALVAL: // TOK_REALVAL
      case symbol_kind::S_TOK_PRIMITIVE: // TOK_PRIMITIVE
      case symbol_kind::S_TOK_SVA_LABEL: // TOK_SVA_LABEL
      case symbol_kind::S_TOK_SPECIFY_OPER: // TOK_SPECIFY_OPER
      case symbol_kind::S_TOK_MSG_TASKS: // TOK_MSG_TASKS
      case symbol_kind::S_TOK_BASE: // TOK_BASE
      case symbol_kind::S_TOK_BASED_CONSTVAL: // TOK_BASED_CONSTVAL
      case symbol_kind::S_TOK_UNBASED_UNSIZED_CONSTVAL: // TOK_UNBASED_UNSIZED_CONSTVAL
      case symbol_kind::S_TOK_USER_TYPE: // TOK_USER_TYPE
      case symbol_kind::S_TOK_PKG_USER_TYPE: // TOK_PKG_USER_TYPE
      case symbol_kind::S_hierarchical_id: // hierarchical_id
      case symbol_kind::S_hierarchical_type_id: // hierarchical_type_id
      case symbol_kind::S_type_name: // type_name
      case symbol_kind::S_tok_prim_wrapper: // tok_prim_wrapper
      case symbol_kind::S_opt_label: // opt_label
      case symbol_kind::S_opt_sva_label: // opt_sva_label
      case symbol_kind::S_integral_number: // integral_number
        value.copy< string_t > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_al_t: // al_t
      case symbol_kind::S_attr: // attr
      case symbol_kind::S_if_attr: // if_attr
      case symbol_kind::S_case_attr: // case_attr
        value.move< al_t > (that.value);
        break;

      case symbol_kind::S_ast_node_type_t: // ast_node_type_t
      case symbol_kind::S_asgn_binop: // asgn_binop
      case symbol_kind::S_inc_or_dec_op: // inc_or_dec_op
        value.move< ast_node_type_t > (that.value);
        break;

      case symbol_kind::S_ast_t: // ast_t
      case symbol_kind::S_io_wire_type: // io_wire_type
      case symbol_kind::S_non_io_wire_type: // non_io_wire_type
      case symbol_kind::S_wire_type: // wire_type
      case symbol_kind::S_non_opt_range: // non_opt_range
      case symbol_kind::S_non_opt_multirange: // non_opt_multirange
      case symbol_kind::S_range: // range
      case symbol_kind::S_range_or_multirange: // range_or_multirange
      case symbol_kind::S_func_return_type: // func_return_type
      case symbol_kind::S_specify_if: // specify_if
      case symbol_kind::S_specify_condition: // specify_condition
      case symbol_kind::S_enum_type: // enum_type
      case symbol_kind::S_opt_enum_init: // opt_enum_init
      case symbol_kind::S_struct_type: // struct_type
      case symbol_kind::S_struct_union: // struct_union
      case symbol_kind::S_typedef_base_type: // typedef_base_type
      case symbol_kind::S_enum_struct_type: // enum_struct_type
      case symbol_kind::S_rvalue: // rvalue
      case symbol_kind::S_lvalue: // lvalue
      case symbol_kind::S_lvalue_concat_list: // lvalue_concat_list
      case symbol_kind::S_genvar_identifier: // genvar_identifier
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_basic_expr: // basic_expr
      case symbol_kind::S_504_84: // @84
      case symbol_kind::S_concat_list: // concat_list
      case symbol_kind::S_assignment_pattern_list: // assignment_pattern_list
        value.move< ast_t > (that.value);
        break;

      case symbol_kind::S_boolean_t: // "boolean"
      case symbol_kind::S_opt_signedness_default_signed: // opt_signedness_default_signed
      case symbol_kind::S_opt_signedness_default_unsigned: // opt_signedness_default_unsigned
      case symbol_kind::S_always_comb_or_latch: // always_comb_or_latch
      case symbol_kind::S_always_or_always_ff: // always_or_always_ff
      case symbol_kind::S_opt_property: // opt_property
        value.move< boolean_t > (that.value);
        break;

      case symbol_kind::S_ch_t: // "invalid token"
      case symbol_kind::S_specify_edge: // specify_edge
        value.move< ch_t > (that.value);
        break;

      case symbol_kind::S_integer_t: // "integer"
      case symbol_kind::S_integer_atom_type: // integer_atom_type
      case symbol_kind::S_integer_vector_type: // integer_vector_type
        value.move< integer_t > (that.value);
        break;

      case symbol_kind::S_specify_rise_fall_ptr_t: // "specify rise and fall"
      case symbol_kind::S_specify_rise_fall: // specify_rise_fall
        value.move< specify_rise_fall_ptr_t > (that.value);
        break;

      case symbol_kind::S_specify_target_ptr_t: // "specify target"
      case symbol_kind::S_specify_target: // specify_target
        value.move< specify_target_ptr_t > (that.value);
        break;

      case symbol_kind::S_specify_triple_ptr_t: // "specify triple"
      case symbol_kind::S_specify_opt_triple: // specify_opt_triple
      case symbol_kind::S_specify_triple: // specify_triple
        value.move< specify_triple_ptr_t > (that.value);
        break;

      case symbol_kind::S_string_t: // "string"
      case symbol_kind::S_TOK_STRING: // TOK_STRING
      case symbol_kind::S_TOK_ID: // TOK_ID
      case symbol_kind::S_TOK_CONSTVAL: // TOK_CONSTVAL
      case symbol_kind::S_TOK_REALVAL: // TOK_REALVAL
      case symbol_kind::S_TOK_PRIMITIVE: // TOK_PRIMITIVE
      case symbol_kind::S_TOK_SVA_LABEL: // TOK_SVA_LABEL
      case symbol_kind::S_TOK_SPECIFY_OPER: // TOK_SPECIFY_OPER
      case symbol_kind::S_TOK_MSG_TASKS: // TOK_MSG_TASKS
      case symbol_kind::S_TOK_BASE: // TOK_BASE
      case symbol_kind::S_TOK_BASED_CONSTVAL: // TOK_BASED_CONSTVAL
      case symbol_kind::S_TOK_UNBASED_UNSIZED_CONSTVAL: // TOK_UNBASED_UNSIZED_CONSTVAL
      case symbol_kind::S_TOK_USER_TYPE: // TOK_USER_TYPE
      case symbol_kind::S_TOK_PKG_USER_TYPE: // TOK_PKG_USER_TYPE
      case symbol_kind::S_hierarchical_id: // hierarchical_id
      case symbol_kind::S_hierarchical_type_id: // hierarchical_type_id
      case symbol_kind::S_type_name: // type_name
      case symbol_kind::S_tok_prim_wrapper: // tok_prim_wrapper
      case symbol_kind::S_opt_label: // opt_label
      case symbol_kind::S_opt_sva_label: // opt_sva_label
      case symbol_kind::S_integral_number: // integral_number
        value.move< string_t > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if FRONTEND_VERILOG_YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if FRONTEND_VERILOG_YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // FRONTEND_VERILOG_YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // Discard the LAC context in case there still is one left from a
    // previous invocation.
    yy_lac_discard_ ("init");

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (lexer));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;

        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    yy_lac_discard_ ("shift");
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_al_t: // al_t
      case symbol_kind::S_attr: // attr
      case symbol_kind::S_if_attr: // if_attr
      case symbol_kind::S_case_attr: // case_attr
        yylhs.value.emplace< al_t > ();
        break;

      case symbol_kind::S_ast_node_type_t: // ast_node_type_t
      case symbol_kind::S_asgn_binop: // asgn_binop
      case symbol_kind::S_inc_or_dec_op: // inc_or_dec_op
        yylhs.value.emplace< ast_node_type_t > ();
        break;

      case symbol_kind::S_ast_t: // ast_t
      case symbol_kind::S_io_wire_type: // io_wire_type
      case symbol_kind::S_non_io_wire_type: // non_io_wire_type
      case symbol_kind::S_wire_type: // wire_type
      case symbol_kind::S_non_opt_range: // non_opt_range
      case symbol_kind::S_non_opt_multirange: // non_opt_multirange
      case symbol_kind::S_range: // range
      case symbol_kind::S_range_or_multirange: // range_or_multirange
      case symbol_kind::S_func_return_type: // func_return_type
      case symbol_kind::S_specify_if: // specify_if
      case symbol_kind::S_specify_condition: // specify_condition
      case symbol_kind::S_enum_type: // enum_type
      case symbol_kind::S_opt_enum_init: // opt_enum_init
      case symbol_kind::S_struct_type: // struct_type
      case symbol_kind::S_struct_union: // struct_union
      case symbol_kind::S_typedef_base_type: // typedef_base_type
      case symbol_kind::S_enum_struct_type: // enum_struct_type
      case symbol_kind::S_rvalue: // rvalue
      case symbol_kind::S_lvalue: // lvalue
      case symbol_kind::S_lvalue_concat_list: // lvalue_concat_list
      case symbol_kind::S_genvar_identifier: // genvar_identifier
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_basic_expr: // basic_expr
      case symbol_kind::S_504_84: // @84
      case symbol_kind::S_concat_list: // concat_list
      case symbol_kind::S_assignment_pattern_list: // assignment_pattern_list
        yylhs.value.emplace< ast_t > ();
        break;

      case symbol_kind::S_boolean_t: // "boolean"
      case symbol_kind::S_opt_signedness_default_signed: // opt_signedness_default_signed
      case symbol_kind::S_opt_signedness_default_unsigned: // opt_signedness_default_unsigned
      case symbol_kind::S_always_comb_or_latch: // always_comb_or_latch
      case symbol_kind::S_always_or_always_ff: // always_or_always_ff
      case symbol_kind::S_opt_property: // opt_property
        yylhs.value.emplace< boolean_t > ();
        break;

      case symbol_kind::S_ch_t: // "invalid token"
      case symbol_kind::S_specify_edge: // specify_edge
        yylhs.value.emplace< ch_t > ();
        break;

      case symbol_kind::S_integer_t: // "integer"
      case symbol_kind::S_integer_atom_type: // integer_atom_type
      case symbol_kind::S_integer_vector_type: // integer_vector_type
        yylhs.value.emplace< integer_t > ();
        break;

      case symbol_kind::S_specify_rise_fall_ptr_t: // "specify rise and fall"
      case symbol_kind::S_specify_rise_fall: // specify_rise_fall
        yylhs.value.emplace< specify_rise_fall_ptr_t > ();
        break;

      case symbol_kind::S_specify_target_ptr_t: // "specify target"
      case symbol_kind::S_specify_target: // specify_target
        yylhs.value.emplace< specify_target_ptr_t > ();
        break;

      case symbol_kind::S_specify_triple_ptr_t: // "specify triple"
      case symbol_kind::S_specify_opt_triple: // specify_opt_triple
      case symbol_kind::S_specify_triple: // specify_triple
        yylhs.value.emplace< specify_triple_ptr_t > ();
        break;

      case symbol_kind::S_string_t: // "string"
      case symbol_kind::S_TOK_STRING: // TOK_STRING
      case symbol_kind::S_TOK_ID: // TOK_ID
      case symbol_kind::S_TOK_CONSTVAL: // TOK_CONSTVAL
      case symbol_kind::S_TOK_REALVAL: // TOK_REALVAL
      case symbol_kind::S_TOK_PRIMITIVE: // TOK_PRIMITIVE
      case symbol_kind::S_TOK_SVA_LABEL: // TOK_SVA_LABEL
      case symbol_kind::S_TOK_SPECIFY_OPER: // TOK_SPECIFY_OPER
      case symbol_kind::S_TOK_MSG_TASKS: // TOK_MSG_TASKS
      case symbol_kind::S_TOK_BASE: // TOK_BASE
      case symbol_kind::S_TOK_BASED_CONSTVAL: // TOK_BASED_CONSTVAL
      case symbol_kind::S_TOK_UNBASED_UNSIZED_CONSTVAL: // TOK_UNBASED_UNSIZED_CONSTVAL
      case symbol_kind::S_TOK_USER_TYPE: // TOK_USER_TYPE
      case symbol_kind::S_TOK_PKG_USER_TYPE: // TOK_PKG_USER_TYPE
      case symbol_kind::S_hierarchical_id: // hierarchical_id
      case symbol_kind::S_hierarchical_type_id: // hierarchical_type_id
      case symbol_kind::S_type_name: // type_name
      case symbol_kind::S_tok_prim_wrapper: // tok_prim_wrapper
      case symbol_kind::S_opt_label: // opt_label
      case symbol_kind::S_opt_sva_label: // opt_sva_label
      case symbol_kind::S_integral_number: // integral_number
        yylhs.value.emplace< string_t > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // $@1: %empty
#line 593 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
       {
	(void)yynerrs_;
	extra->ast_stack.clear();
	extra->ast_stack.push_back(extra->current_ast);
}
#line 1361 "verilog_parser.tab.cc"
    break;

  case 3: // input: $@1 design
#line 597 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
         {
	extra->ast_stack.pop_back();
	log_assert(GetSize(extra->ast_stack) == 0);
	extra->default_attr_list.clear();
}
#line 1371 "verilog_parser.tab.cc"
    break;

  case 15: // $@2: %empty
#line 617 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
        {
		if (extra->attr_list)
			extra->attr_list_stack.push(std::move(extra->attr_list));
		extra->attr_list = std::make_unique<dict<IdString, std::unique_ptr<AstNode>>>();
		for (auto &it : extra->default_attr_list)
			(*extra->attr_list)[it.first] = it.second->clone();
	}
#line 1383 "verilog_parser.tab.cc"
    break;

  case 16: // attr: $@2 attr_opt
#line 623 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                   {
		yylhs.value.as < al_t > () = std::move(extra->attr_list);
		if (!extra->attr_list_stack.empty()) {
			extra->attr_list = std::move(extra->attr_list_stack.top());
			extra->attr_list_stack.pop();
		} else
			extra->attr_list = nullptr;
	}
#line 1396 "verilog_parser.tab.cc"
    break;

  case 17: // attr_opt: attr_opt ATTR_BEGIN opt_attr_list ATTR_END
#line 633 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                   {
		SET_RULE_LOC(yylhs.location, yystack_[2].location, yylhs.location);
	}
#line 1404 "verilog_parser.tab.cc"
    break;

  case 19: // $@3: %empty
#line 639 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		if (extra->attr_list != nullptr)
			extra->attr_list_stack.push(std::move(extra->attr_list));
		extra->attr_list = std::make_unique<dict<IdString, std::unique_ptr<AstNode>>>();
		extra->default_attr_list.clear();
	}
#line 1415 "verilog_parser.tab.cc"
    break;

  case 20: // $@4: %empty
#line 644 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        {
		extra->attr_list->swap(extra->default_attr_list);
		extra->attr_list.reset();
		if (!extra->attr_list_stack.empty()) {
			extra->attr_list = std::move(extra->attr_list_stack.top());
			extra->attr_list_stack.pop();
		} else
			extra->attr_list = nullptr;
	}
#line 1429 "verilog_parser.tab.cc"
    break;

  case 26: // attr_assign: hierarchical_id
#line 662 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        {
		(*extra->attr_list)[*yystack_[0].value.as < string_t > ()] = AstNode::mkconst_int(yystack_[0].location, 1, false);
	}
#line 1437 "verilog_parser.tab.cc"
    break;

  case 27: // attr_assign: hierarchical_id "'='" expr
#line 665 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                    {
		(*extra->attr_list)[*yystack_[2].value.as < string_t > ()] = std::move(yystack_[0].value.as < ast_t > ());
	}
#line 1445 "verilog_parser.tab.cc"
    break;

  case 28: // hierarchical_id: TOK_ID
#line 670 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < string_t > () = std::move(yystack_[0].value.as < string_t > ());
	}
#line 1453 "verilog_parser.tab.cc"
    break;

  case 29: // hierarchical_id: hierarchical_id TOK_PACKAGESEP TOK_ID
#line 673 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                              {
		if (yystack_[0].value.as < string_t > ()->compare(0, 1, "\\") == 0)
			*yystack_[2].value.as < string_t > () += "::" + yystack_[0].value.as < string_t > ()->substr(1);
		else
			*yystack_[2].value.as < string_t > () += "::" + *yystack_[0].value.as < string_t > ();
		yylhs.value.as < string_t > () = std::move(yystack_[2].value.as < string_t > ());
	}
#line 1465 "verilog_parser.tab.cc"
    break;

  case 30: // hierarchical_id: hierarchical_id "'.'" TOK_ID
#line 680 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                       {
		if (yystack_[0].value.as < string_t > ()->compare(0, 1, "\\") == 0)
			*yystack_[2].value.as < string_t > () += "." + yystack_[0].value.as < string_t > ()->substr(1);
		else
			*yystack_[2].value.as < string_t > () += "." + *yystack_[0].value.as < string_t > ();
		yylhs.value.as < string_t > () = std::move(yystack_[2].value.as < string_t > ());
	}
#line 1477 "verilog_parser.tab.cc"
    break;

  case 31: // hierarchical_type_id: TOK_USER_TYPE
#line 689 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {yylhs.value.as < string_t > () = std::move(yystack_[0].value.as < string_t > ()); }
#line 1483 "verilog_parser.tab.cc"
    break;

  case 32: // hierarchical_type_id: TOK_PKG_USER_TYPE
#line 690 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                            {yylhs.value.as < string_t > () = std::move(yystack_[0].value.as < string_t > ()); }
#line 1489 "verilog_parser.tab.cc"
    break;

  case 33: // hierarchical_type_id: "'('" TOK_USER_TYPE "')'"
#line 691 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                { yylhs.value.as < string_t > () = std::move(yystack_[1].value.as < string_t > ()); }
#line 1495 "verilog_parser.tab.cc"
    break;

  case 34: // $@5: %empty
#line 695 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        {
		extra->enterTypeScope();
	}
#line 1503 "verilog_parser.tab.cc"
    break;

  case 35: // $@6: %empty
#line 697 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {
		extra->do_not_require_port_stubs = false;
		AstNode* mod = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_MODULE));
		extra->current_ast_mod = mod;
		extra->port_stubs.clear();
		extra->port_counter = 0;
		mod->str = *yystack_[0].value.as < string_t > ();
		append_attr(mod, std::move(yystack_[3].value.as < al_t > ()));
	}
#line 1517 "verilog_parser.tab.cc"
    break;

  case 36: // module: attr TOK_MODULE $@5 TOK_ID $@6 module_para_opt module_args_opt "';'" module_body TOK_ENDMODULE opt_label
#line 705 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                          {
		if (extra->port_stubs.size() != 0)
			err_at_loc(yystack_[4].location, "Missing details for module port `%s'.",
					extra->port_stubs.begin()->first.c_str());
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[9].location, yylhs.location);
		extra->ast_stack.pop_back();
		log_assert(extra->ast_stack.size() == 1);
		checkLabelsMatch(yystack_[0].location, "Module name", yystack_[7].value.as < string_t > ().get(), yystack_[0].value.as < string_t > ().get());
		extra->current_ast_mod = nullptr;
		extra->exitTypeScope();
	}
#line 1533 "verilog_parser.tab.cc"
    break;

  case 42: // $@7: %empty
#line 725 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                           {
		extra->astbuf1 = std::make_unique<AstNode>(yylhs.location, AST_PARAMETER);
		extra->astbuf1->children.push_back(AstNode::mkconst_int(yystack_[0].location, 0, true));
		append_attr(extra->astbuf1.get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 1543 "verilog_parser.tab.cc"
    break;

  case 44: // $@8: %empty
#line 730 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                            {
		extra->astbuf1 = std::make_unique<AstNode>(yylhs.location, AST_LOCALPARAM);
		extra->astbuf1->children.push_back(AstNode::mkconst_int(yystack_[0].location, 0, true));
		append_attr(extra->astbuf1.get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 1553 "verilog_parser.tab.cc"
    break;

  case 54: // module_arg_opt_assignment: "'='" expr
#line 747 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    {
		if (extra->ast_stack.back()->children.size() > 0 && extra->ast_stack.back()->children.back()->type == AST_WIRE) {
			if (extra->ast_stack.back()->children.back()->is_input) {
				auto& n = extra->ast_stack.back()->children.back();
				n->attributes[ID::defaultvalue] = std::move(yystack_[0].value.as < ast_t > ());
			} else {
				auto wire = std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER);
				wire->str = extra->ast_stack.back()->children.back()->str;
				if (extra->ast_stack.back()->children.back()->is_reg || extra->ast_stack.back()->children.back()->is_logic)
					extra->ast_stack.back()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_INITIAL, std::make_unique<AstNode>(yylhs.location, AST_BLOCK, std::make_unique<AstNode>(yylhs.location, AST_ASSIGN_LE, std::move(wire), std::move(yystack_[0].value.as < ast_t > ())))));
				else
					extra->ast_stack.back()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_ASSIGN, std::move(wire), std::move(yystack_[0].value.as < ast_t > ())));
			}
		} else
			err_at_loc(yystack_[0].location, "SystemVerilog interface in module port list cannot have a default value.");
	}
#line 1574 "verilog_parser.tab.cc"
    break;

  case 56: // $@9: %empty
#line 766 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		if (extra->ast_stack.back()->children.size() > 0 && extra->ast_stack.back()->children.back()->type == AST_WIRE) {
			auto node = extra->ast_stack.back()->children.back()->clone();
			node->str = *yystack_[0].value.as < string_t > ();
			node->port_id = ++extra->port_counter;
			SET_AST_NODE_LOC(node.get(), yystack_[0].location, yystack_[0].location);
			extra->ast_stack.back()->children.push_back(std::move(node));
		} else {
			if (extra->port_stubs.count(*yystack_[0].value.as < string_t > ()) != 0)
				err_at_loc(yystack_[0].location, "Duplicate module port `%s'.", *yystack_[0].value.as < string_t > ());
			extra->port_stubs[*yystack_[0].value.as < string_t > ()] = ++extra->port_counter;
		}
	}
#line 1592 "verilog_parser.tab.cc"
    break;

  case 58: // $@10: %empty
#line 779 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		extra->astbuf1 = std::make_unique<AstNode>(yylhs.location, AST_INTERFACEPORT);
		extra->astbuf1->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_INTERFACEPORTTYPE));
		extra->astbuf1->children[0]->str = *yystack_[0].value.as < string_t > ();
	}
#line 1602 "verilog_parser.tab.cc"
    break;

  case 59: // $@11: %empty
#line 783 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {  /* SV interfaces */
		if (!mode->sv)
			err_at_loc(yystack_[0].location, "Interface found in port list (%s). This is not supported unless read_verilog is called with -sv!", *yystack_[0].value.as < string_t > ());
		extra->astbuf2 = extra->astbuf1->clone(); // really only needed if multiple instances of same type.
		extra->astbuf2->str = *yystack_[0].value.as < string_t > ();
		extra->astbuf2->port_id = ++extra->port_counter;
		extra->ast_stack.back()->children.push_back(std::move(extra->astbuf2));
	}
#line 1615 "verilog_parser.tab.cc"
    break;

  case 61: // $@12: %empty
#line 791 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                  {
		auto node = std::move(yystack_[2].value.as < ast_t > ());
		node->str = *yystack_[0].value.as < string_t > ();
		SET_AST_NODE_LOC(node.get(), yystack_[0].location, yystack_[0].location);
		node->port_id = ++extra->port_counter;
		auto range = checkRange(node.get(), std::move(yystack_[1].value.as < ast_t > ()));
		if (range != nullptr)
			node->children.push_back(std::move(range));
		if (!node->is_input && !node->is_output)
			err_at_loc(yystack_[0].location, "Module port `%s' is neither input nor output.", *yystack_[0].value.as < string_t > ());
		if (node->is_reg && node->is_input && !node->is_output && !mode->sv)
			err_at_loc(yystack_[0].location, "Input port `%s' is declared as register.", *yystack_[0].value.as < string_t > ());
		append_attr(node.get(), std::move(yystack_[3].value.as < al_t > ()));
		extra->ast_stack.back()->children.push_back(std::move(node));
	}
#line 1635 "verilog_parser.tab.cc"
    break;

  case 63: // module_arg: "'.'" "'.'" "'.'"
#line 806 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                {
		extra->do_not_require_port_stubs = true;
	}
#line 1643 "verilog_parser.tab.cc"
    break;

  case 64: // $@13: %empty
#line 811 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                         {
		extra->enterTypeScope();
	}
#line 1651 "verilog_parser.tab.cc"
    break;

  case 65: // $@14: %empty
#line 813 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {
		AstNode* mod = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_PACKAGE));
		extra->current_ast_mod = mod;
		mod->str = *yystack_[0].value.as < string_t > ();
		append_attr(mod, std::move(yystack_[3].value.as < al_t > ()));
	}
#line 1662 "verilog_parser.tab.cc"
    break;

  case 66: // package: attr TOK_PACKAGE $@13 TOK_ID $@14 "';'" package_body TOK_ENDPACKAGE opt_label
#line 818 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                            {
		extra->ast_stack.pop_back();
		checkLabelsMatch(yystack_[0].location, "Package name", yystack_[5].value.as < string_t > ().get(), yystack_[0].value.as < string_t > ().get());
		extra->current_ast_mod = nullptr;
		extra->exitTypeScope();
	}
#line 1673 "verilog_parser.tab.cc"
    break;

  case 73: // import_stmt: TOK_IMPORT TOK_ID TOK_PACKAGESEP "'*'" "';'"
#line 832 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                               {
		// Create an import node to track wildcard package imports
		auto import_node = std::make_unique<AstNode>(yylhs.location, AST_IMPORT);
		import_node->str = *yystack_[3].value.as < string_t > ();
		extra->ast_stack.back()->children.push_back(std::move(import_node));
	}
#line 1684 "verilog_parser.tab.cc"
    break;

  case 74: // $@15: %empty
#line 838 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                         {
		// Start a specific import: create and push the AST_IMPORT node
		AstNode* import_node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_IMPORT));
		import_node->str = *yystack_[1].value.as < string_t > ();
	}
#line 1694 "verilog_parser.tab.cc"
    break;

  case 75: // import_stmt: TOK_IMPORT TOK_ID TOK_PACKAGESEP $@15 import_item_list "';'"
#line 842 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                       {
		// Done collecting specific items, pop the AST_IMPORT node
		extra->ast_stack.pop_back();
	}
#line 1703 "verilog_parser.tab.cc"
    break;

  case 78: // import_item: TOK_ID
#line 852 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		// Append this specific import name under the current AST_IMPORT
		auto item_node = std::make_unique<AstNode>(yylhs.location, AST_NONE);
		item_node->str = *yystack_[0].value.as < string_t > ();
		extra->ast_stack.back()->children.push_back(std::move(item_node));
	}
#line 1714 "verilog_parser.tab.cc"
    break;

  case 79: // $@16: %empty
#line 860 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		extra->enterTypeScope();
	}
#line 1722 "verilog_parser.tab.cc"
    break;

  case 80: // $@17: %empty
#line 862 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {
		extra->do_not_require_port_stubs = false;
		AstNode* intf = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_INTERFACE));
		extra->current_ast_mod = intf;
		extra->port_stubs.clear();
		extra->port_counter = 0;
		intf->str = *yystack_[0].value.as < string_t > ();
	}
#line 1735 "verilog_parser.tab.cc"
    break;

  case 81: // interface: TOK_INTERFACE $@16 TOK_ID $@17 module_para_opt module_args_opt "';'" interface_body TOK_ENDINTERFACE
#line 869 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                      {
		if (extra->port_stubs.size() != 0)
			err_at_loc(yystack_[3].location, "Missing details for module port `%s'.",
				extra->port_stubs.begin()->first.c_str());
		extra->ast_stack.pop_back();
		log_assert(extra->ast_stack.size() == 1);
		extra->current_ast_mod = nullptr;
		extra->exitTypeScope();
	}
#line 1749 "verilog_parser.tab.cc"
    break;

  case 93: // $@18: %empty
#line 887 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {
		(void)extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_BIND));
	}
#line 1757 "verilog_parser.tab.cc"
    break;

  case 94: // $@19: %empty
#line 890 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    {
		// bind_target should have added at least one child
		log_assert(extra->ast_stack.back()->children.size() >= 1);
	}
#line 1766 "verilog_parser.tab.cc"
    break;

  case 95: // $@20: %empty
#line 894 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		// The single_cell parser in cell_list_no_array uses extra->astbuf1 as
		// a sort of template for constructing cells.
		extra->astbuf1 = std::make_unique<AstNode>(yylhs.location, AST_CELL);
		extra->astbuf1->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_CELLTYPE));
		extra->astbuf1->children[0]->str = *yystack_[0].value.as < string_t > ();
	}
#line 1778 "verilog_parser.tab.cc"
    break;

  case 96: // bind_directive: TOK_BIND $@18 bind_target $@19 TOK_ID $@20 cell_parameter_list_opt cell_list_no_array "';'"
#line 901 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                               {
		// cell_list should have added at least one more child
		log_assert(extra->ast_stack.back()->children.size() >= 2);
		(void)extra->astbuf1.reset();
		extra->ast_stack.pop_back();
	}
#line 1789 "verilog_parser.tab.cc"
    break;

  case 102: // bind_target_instance: hierarchical_id
#line 936 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER);
		node->str = *yystack_[0].value.as < string_t > ();
		extra->ast_stack.back()->children.push_back(std::move(node));
	}
#line 1799 "verilog_parser.tab.cc"
    break;

  case 103: // mintypmax_expr: expr
#line 943 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             { }
#line 1805 "verilog_parser.tab.cc"
    break;

  case 104: // mintypmax_expr: expr "':'" expr "':'" expr
#line 944 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                       { }
#line 1811 "verilog_parser.tab.cc"
    break;

  case 105: // non_opt_delay: "'#'" TOK_ID
#line 947 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { }
#line 1817 "verilog_parser.tab.cc"
    break;

  case 106: // non_opt_delay: "'#'" TOK_CONSTVAL
#line 948 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                              { }
#line 1823 "verilog_parser.tab.cc"
    break;

  case 107: // non_opt_delay: "'#'" TOK_REALVAL
#line 949 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                             { }
#line 1829 "verilog_parser.tab.cc"
    break;

  case 115: // $@21: %empty
#line 961 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
        { extra->astbuf3 = std::make_unique<AstNode>(yylhs.location, AST_WIRE); extra->current_wire_rand = false; extra->current_wire_const = false; extra->current_wire_automatic = false; }
#line 1835 "verilog_parser.tab.cc"
    break;

  case 116: // io_wire_type: $@21 wire_type_token_io wire_type_const_rand opt_wire_type_token wire_type_signedness
#line 963 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
        { yylhs.value.as < ast_t > () = std::move(extra->astbuf3); SET_RULE_LOC(yylhs.location, yystack_[3].location, yylhs.location); }
#line 1841 "verilog_parser.tab.cc"
    break;

  case 117: // $@22: %empty
#line 966 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
        { extra->astbuf3 = std::make_unique<AstNode>(yylhs.location, AST_WIRE); extra->current_wire_rand = false; extra->current_wire_const = false; extra->current_wire_automatic = false; }
#line 1847 "verilog_parser.tab.cc"
    break;

  case 118: // non_io_wire_type: $@22 opt_lifetime wire_type_const_rand wire_type_token wire_type_signedness
#line 968 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
        {
		if (extra->current_wire_automatic)
			extra->astbuf3->set_attribute(ID::nosync, AstNode::mkconst_int(extra->astbuf3->location, 1, false));
		yylhs.value.as < ast_t > () = std::move(extra->astbuf3); SET_RULE_LOC(yylhs.location, yystack_[3].location, yylhs.location);
	}
#line 1857 "verilog_parser.tab.cc"
    break;

  case 119: // wire_type: io_wire_type
#line 975 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                     { yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ()); }
#line 1863 "verilog_parser.tab.cc"
    break;

  case 120: // wire_type: non_io_wire_type
#line 976 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                         { yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ()); }
#line 1869 "verilog_parser.tab.cc"
    break;

  case 121: // wire_type_token_io: TOK_INPUT
#line 979 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  {
		extra->astbuf3->is_input = true;
	}
#line 1877 "verilog_parser.tab.cc"
    break;

  case 122: // wire_type_token_io: TOK_OUTPUT
#line 982 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                   {
		extra->astbuf3->is_output = true;
	}
#line 1885 "verilog_parser.tab.cc"
    break;

  case 123: // wire_type_token_io: TOK_INOUT
#line 985 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  {
		extra->astbuf3->is_input = true;
		extra->astbuf3->is_output = true;
	}
#line 1894 "verilog_parser.tab.cc"
    break;

  case 124: // wire_type_signedness: TOK_SIGNED
#line 991 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                     { extra->astbuf3->is_signed = true;  }
#line 1900 "verilog_parser.tab.cc"
    break;

  case 125: // wire_type_signedness: TOK_UNSIGNED
#line 992 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                     { extra->astbuf3->is_signed = false; }
#line 1906 "verilog_parser.tab.cc"
    break;

  case 127: // wire_type_const_rand: TOK_RAND TOK_CONST
#line 996 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                           {
	    extra->current_wire_rand = true;
	    extra->current_wire_const = true;
	}
#line 1915 "verilog_parser.tab.cc"
    break;

  case 128: // wire_type_const_rand: TOK_CONST
#line 1000 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  {
	    extra->current_wire_const = true;
	}
#line 1923 "verilog_parser.tab.cc"
    break;

  case 129: // wire_type_const_rand: TOK_RAND
#line 1003 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {
	    extra->current_wire_rand = true;
	}
#line 1931 "verilog_parser.tab.cc"
    break;

  case 133: // wire_type_token: net_type
#line 1013 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {
	}
#line 1938 "verilog_parser.tab.cc"
    break;

  case 134: // wire_type_token: net_type logic_type
#line 1015 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                            {
	}
#line 1945 "verilog_parser.tab.cc"
    break;

  case 135: // wire_type_token: TOK_REG
#line 1018 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                {
		extra->astbuf3->is_reg = true;
	}
#line 1953 "verilog_parser.tab.cc"
    break;

  case 136: // wire_type_token: TOK_VAR TOK_REG
#line 1021 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        {
		extra->astbuf3->is_reg = true;
	}
#line 1961 "verilog_parser.tab.cc"
    break;

  case 137: // wire_type_token: TOK_VAR
#line 1025 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                {
		extra->astbuf3->is_logic = true;
	}
#line 1969 "verilog_parser.tab.cc"
    break;

  case 138: // wire_type_token: TOK_VAR logic_type
#line 1028 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                           {
		extra->astbuf3->is_logic = true;
	}
#line 1977 "verilog_parser.tab.cc"
    break;

  case 139: // wire_type_token: logic_type
#line 1031 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                   {
		extra->astbuf3->is_logic = true;
	}
#line 1985 "verilog_parser.tab.cc"
    break;

  case 140: // wire_type_token: TOK_GENVAR
#line 1034 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                   {
		extra->astbuf3->type = AST_GENVAR;
		extra->astbuf3->is_reg = true;
		extra->astbuf3->is_signed = true;
		extra->astbuf3->range_left = 31;
		extra->astbuf3->range_right = 0;
	}
#line 1997 "verilog_parser.tab.cc"
    break;

  case 141: // net_type: TOK_WOR
#line 1043 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                {
		extra->astbuf3->is_wor = true;
	}
#line 2005 "verilog_parser.tab.cc"
    break;

  case 142: // net_type: TOK_WAND
#line 1046 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {
		extra->astbuf3->is_wand = true;
	}
#line 2013 "verilog_parser.tab.cc"
    break;

  case 144: // logic_type: TOK_LOGIC
#line 1052 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  {
	}
#line 2020 "verilog_parser.tab.cc"
    break;

  case 145: // logic_type: integer_atom_type
#line 1054 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		extra->astbuf3->range_left = yystack_[0].value.as < integer_t > () - 1;
		extra->astbuf3->range_right = 0;
		extra->astbuf3->is_signed = true;
	}
#line 2030 "verilog_parser.tab.cc"
    break;

  case 146: // logic_type: hierarchical_type_id
#line 1059 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                             {
		extra->addWiretypeNode(yystack_[0].value.as < string_t > ().get(), extra->astbuf3.get());
	}
#line 2038 "verilog_parser.tab.cc"
    break;

  case 147: // integer_atom_type: TOK_INTEGER
#line 1064 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < integer_t > () = 32; }
#line 2044 "verilog_parser.tab.cc"
    break;

  case 148: // integer_atom_type: TOK_INT
#line 1065 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < integer_t > () = 32; }
#line 2050 "verilog_parser.tab.cc"
    break;

  case 149: // integer_atom_type: TOK_SHORTINT
#line 1066 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < integer_t > () = 16; }
#line 2056 "verilog_parser.tab.cc"
    break;

  case 150: // integer_atom_type: TOK_LONGINT
#line 1067 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < integer_t > () = 64; }
#line 2062 "verilog_parser.tab.cc"
    break;

  case 151: // integer_atom_type: TOK_BYTE
#line 1068 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < integer_t > () =  8; }
#line 2068 "verilog_parser.tab.cc"
    break;

  case 152: // integer_vector_type: TOK_LOGIC
#line 1071 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  { yylhs.value.as < integer_t > () = token::TOK_LOGIC; }
#line 2074 "verilog_parser.tab.cc"
    break;

  case 153: // integer_vector_type: TOK_REG
#line 1072 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  { yylhs.value.as < integer_t > () = token::TOK_REG; }
#line 2080 "verilog_parser.tab.cc"
    break;

  case 154: // non_opt_range: "'['" expr "':'" expr "']'"
#line 1075 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_RANGE);
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[3].value.as < ast_t > ()));
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[1].value.as < ast_t > ()));
	}
#line 2090 "verilog_parser.tab.cc"
    break;

  case 155: // non_opt_range: "'['" expr TOK_POS_INDEXED expr "']'"
#line 1080 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                    {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_RANGE);
		auto expr = std::make_unique<AstNode>(yylhs.location, AST_SELFSZ, std::move(yystack_[3].value.as < ast_t > ()));
		yylhs.value.as < ast_t > ()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_SUB, std::make_unique<AstNode>(yylhs.location, AST_ADD, expr->clone(), std::move(yystack_[1].value.as < ast_t > ())), AstNode::mkconst_int(yylhs.location, 1, true)));
		yylhs.value.as < ast_t > ()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_ADD, std::move(expr), AstNode::mkconst_int(yylhs.location, 0, true)));
	}
#line 2101 "verilog_parser.tab.cc"
    break;

  case 156: // non_opt_range: "'['" expr TOK_NEG_INDEXED expr "']'"
#line 1086 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                    {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_RANGE);
		auto expr = std::make_unique<AstNode>(yylhs.location, AST_SELFSZ, std::move(yystack_[3].value.as < ast_t > ()));
		yylhs.value.as < ast_t > ()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_ADD, expr->clone(), AstNode::mkconst_int(yylhs.location, 0, true)));
		yylhs.value.as < ast_t > ()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_SUB, std::make_unique<AstNode>(yylhs.location, AST_ADD, std::move(expr), AstNode::mkconst_int(yylhs.location, 1, true)), std::move(yystack_[1].value.as < ast_t > ())));
	}
#line 2112 "verilog_parser.tab.cc"
    break;

  case 157: // non_opt_range: "'['" expr "']'"
#line 1092 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                               {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_RANGE);
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[1].value.as < ast_t > ()));
	}
#line 2121 "verilog_parser.tab.cc"
    break;

  case 158: // non_opt_multirange: non_opt_range non_opt_range
#line 1098 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                    {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_MULTIRANGE, std::move(yystack_[1].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 2129 "verilog_parser.tab.cc"
    break;

  case 159: // non_opt_multirange: non_opt_multirange non_opt_range
#line 1101 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                         {
		yylhs.value.as < ast_t > () = std::move(yystack_[1].value.as < ast_t > ());
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 2138 "verilog_parser.tab.cc"
    break;

  case 160: // range: non_opt_range
#line 1107 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ());
	}
#line 2146 "verilog_parser.tab.cc"
    break;

  case 161: // range: %empty
#line 1110 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < ast_t > () = nullptr;
	}
#line 2154 "verilog_parser.tab.cc"
    break;

  case 162: // range_or_multirange: range
#line 1115 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
              { yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ()); }
#line 2160 "verilog_parser.tab.cc"
    break;

  case 163: // range_or_multirange: non_opt_multirange
#line 1116 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                           { yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ()); }
#line 2166 "verilog_parser.tab.cc"
    break;

  case 188: // $@23: %empty
#line 1132 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                       {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_GENBLOCK));
		node->str = *yystack_[1].value.as < string_t > ();
	}
#line 2175 "verilog_parser.tab.cc"
    break;

  case 189: // checker_decl: TOK_CHECKER TOK_ID "';'" $@23 module_body TOK_ENDCHECKER
#line 1135 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                     {
		extra->ast_stack.pop_back();
	}
#line 2183 "verilog_parser.tab.cc"
    break;

  case 190: // $@24: %empty
#line 1140 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		extra->current_function_or_task = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_DPI_FUNCTION, AstNode::mkconst_str(yystack_[1].location, *yystack_[1].value.as < string_t > ()), AstNode::mkconst_str(yystack_[0].location, *yystack_[0].value.as < string_t > ())));
		extra->current_function_or_task->str = *yystack_[0].value.as < string_t > ();
		append_attr(extra->current_function_or_task, std::move(yystack_[3].value.as < al_t > ()));
	}
#line 2193 "verilog_parser.tab.cc"
    break;

  case 191: // task_func_decl: attr TOK_DPI_FUNCTION TOK_ID TOK_ID $@24 opt_dpi_function_args "';'"
#line 1144 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		extra->current_function_or_task = nullptr;
	}
#line 2201 "verilog_parser.tab.cc"
    break;

  case 192: // $@25: %empty
#line 1147 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                          {
		extra->current_function_or_task = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_DPI_FUNCTION, AstNode::mkconst_str(yystack_[1].location, *yystack_[1].value.as < string_t > ()), AstNode::mkconst_str(yystack_[3].location, *yystack_[3].value.as < string_t > ())));
		extra->current_function_or_task->str = *yystack_[0].value.as < string_t > ();
		append_attr(extra->current_function_or_task, std::move(yystack_[5].value.as < al_t > ()));
	}
#line 2211 "verilog_parser.tab.cc"
    break;

  case 193: // task_func_decl: attr TOK_DPI_FUNCTION TOK_ID "'='" TOK_ID TOK_ID $@25 opt_dpi_function_args "';'"
#line 1151 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		extra->current_function_or_task = nullptr;
	}
#line 2219 "verilog_parser.tab.cc"
    break;

  case 194: // $@26: %empty
#line 1154 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                         {
		extra->current_function_or_task = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_DPI_FUNCTION, AstNode::mkconst_str(yystack_[1].location, *yystack_[1].value.as < string_t > ()), AstNode::mkconst_str(location_range(yystack_[5].location, yystack_[3].location), *yystack_[5].value.as < string_t > () + ":" + RTLIL::unescape_id(*yystack_[3].value.as < string_t > ()))));
		extra->current_function_or_task->str = *yystack_[0].value.as < string_t > ();
		append_attr(extra->current_function_or_task, std::move(yystack_[7].value.as < al_t > ()));
	}
#line 2229 "verilog_parser.tab.cc"
    break;

  case 195: // task_func_decl: attr TOK_DPI_FUNCTION TOK_ID "':'" TOK_ID "'='" TOK_ID TOK_ID $@26 opt_dpi_function_args "';'"
#line 1158 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		extra->current_function_or_task = nullptr;
	}
#line 2237 "verilog_parser.tab.cc"
    break;

  case 196: // $@27: %empty
#line 1161 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                           {
		extra->current_function_or_task = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_TASK));
		extra->current_function_or_task->str = *yystack_[0].value.as < string_t > ();
		append_attr(extra->current_function_or_task, std::move(yystack_[3].value.as < al_t > ()));
		extra->current_function_or_task_port_id = 1;
	}
#line 2248 "verilog_parser.tab.cc"
    break;

  case 197: // task_func_decl: attr TOK_TASK opt_automatic TOK_ID $@27 task_func_args_opt "';'" task_func_body TOK_ENDTASK
#line 1166 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                    {
		extra->current_function_or_task = nullptr;
		extra->ast_stack.pop_back();
	}
#line 2257 "verilog_parser.tab.cc"
    break;

  case 198: // $@28: %empty
#line 1170 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                        {
		// The difference between void functions and tasks is that
		// always_comb's implicit sensitivity list behaves as if functions were
		// inlined, but ignores signals read only in tasks. This only matters
		// for event based simulation, and for synthesis we can treat a void
		// function like a task.
		extra->current_function_or_task = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_TASK));
		extra->current_function_or_task->str = *yystack_[0].value.as < string_t > ();
		append_attr(extra->current_function_or_task, std::move(yystack_[4].value.as < al_t > ()));
		extra->current_function_or_task_port_id = 1;
	}
#line 2273 "verilog_parser.tab.cc"
    break;

  case 199: // task_func_decl: attr TOK_FUNCTION opt_automatic TOK_VOID TOK_ID $@28 task_func_args_opt "';'" task_func_body TOK_ENDFUNCTION
#line 1180 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                        {
		extra->current_function_or_task = nullptr;
		extra->ast_stack.pop_back();
	}
#line 2282 "verilog_parser.tab.cc"
    break;

  case 200: // $@29: %empty
#line 1184 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                {
		extra->current_function_or_task = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_FUNCTION));
		extra->current_function_or_task->str = *yystack_[0].value.as < string_t > ();
		append_attr(extra->current_function_or_task, std::move(yystack_[4].value.as < al_t > ()));
		auto outreg = std::make_unique<AstNode>(yylhs.location, AST_WIRE);
		outreg->str = *yystack_[0].value.as < string_t > ();
		outreg->is_signed = false;
		outreg->is_reg = true;
		if (yystack_[1].value.as < ast_t > () != nullptr) {
			outreg->is_signed = yystack_[1].value.as < ast_t > ()->is_signed;
			yystack_[1].value.as < ast_t > ()->is_signed = false;
			outreg->is_custom_type = yystack_[1].value.as < ast_t > ()->type == AST_WIRETYPE;
			outreg->children.push_back(std::move(yystack_[1].value.as < ast_t > ()));
		}
		extra->current_function_or_task->children.push_back(std::move(outreg));
		extra->current_function_or_task_port_id = 1;
	}
#line 2304 "verilog_parser.tab.cc"
    break;

  case 201: // task_func_decl: attr TOK_FUNCTION opt_automatic func_return_type TOK_ID $@29 task_func_args_opt "';'" task_func_body TOK_ENDFUNCTION
#line 1200 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                        {
		extra->current_function_or_task = nullptr;
		extra->ast_stack.pop_back();
	}
#line 2313 "verilog_parser.tab.cc"
    break;

  case 202: // func_return_type: hierarchical_type_id
#line 1206 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                             {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_WIRETYPE);
		yylhs.value.as < ast_t > ()->str = *yystack_[0].value.as < string_t > ();
	}
#line 2322 "verilog_parser.tab.cc"
    break;

  case 203: // func_return_type: opt_type_vec opt_signedness_default_unsigned
#line 1210 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                     {
		yylhs.value.as < ast_t > () = makeRange(yylhs.location, 0, 0, yystack_[0].value.as < boolean_t > ());
	}
#line 2330 "verilog_parser.tab.cc"
    break;

  case 204: // func_return_type: opt_type_vec opt_signedness_default_unsigned non_opt_range
#line 1213 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                   {
		yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ());
		yylhs.value.as < ast_t > ()->is_signed = yystack_[1].value.as < boolean_t > ();
	}
#line 2339 "verilog_parser.tab.cc"
    break;

  case 205: // func_return_type: integer_atom_type opt_signedness_default_signed
#line 1217 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                        {
		yylhs.value.as < ast_t > () = makeRange(yylhs.location, yystack_[1].value.as < integer_t > () - 1, 0, yystack_[0].value.as < boolean_t > ());
	}
#line 2347 "verilog_parser.tab.cc"
    break;

  case 209: // opt_signedness_default_signed: %empty
#line 1228 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < boolean_t > () = true; }
#line 2353 "verilog_parser.tab.cc"
    break;

  case 210: // opt_signedness_default_signed: TOK_SIGNED
#line 1229 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < boolean_t > () = true; }
#line 2359 "verilog_parser.tab.cc"
    break;

  case 211: // opt_signedness_default_signed: TOK_UNSIGNED
#line 1230 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < boolean_t > () = false; }
#line 2365 "verilog_parser.tab.cc"
    break;

  case 212: // opt_signedness_default_unsigned: %empty
#line 1233 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < boolean_t > () = false; }
#line 2371 "verilog_parser.tab.cc"
    break;

  case 213: // opt_signedness_default_unsigned: TOK_SIGNED
#line 1234 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < boolean_t > () = true; }
#line 2377 "verilog_parser.tab.cc"
    break;

  case 214: // opt_signedness_default_unsigned: TOK_UNSIGNED
#line 1235 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < boolean_t > () = false; }
#line 2383 "verilog_parser.tab.cc"
    break;

  case 215: // dpi_function_arg: TOK_ID TOK_ID
#line 1239 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		extra->current_function_or_task->children.push_back(AstNode::mkconst_str(yystack_[1].location, *yystack_[1].value.as < string_t > ()));
	}
#line 2391 "verilog_parser.tab.cc"
    break;

  case 216: // dpi_function_arg: TOK_ID
#line 1242 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		extra->current_function_or_task->children.push_back(AstNode::mkconst_str(yystack_[0].location, *yystack_[0].value.as < string_t > ()));
	}
#line 2399 "verilog_parser.tab.cc"
    break;

  case 225: // opt_lifetime: TOK_AUTOMATIC
#line 1261 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      { extra->current_wire_automatic = true; }
#line 2405 "verilog_parser.tab.cc"
    break;

  case 229: // $@30: %empty
#line 1265 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                    {
		extra->albuf = nullptr;
		extra->astbuf1 = nullptr;
		extra->astbuf2 = nullptr;
	}
#line 2415 "verilog_parser.tab.cc"
    break;

  case 230: // $@31: %empty
#line 1269 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                        {
		(void)extra->astbuf1.reset();
		if (extra->astbuf2 != nullptr)
			(void)extra->astbuf2.reset();
		extra->albuf.reset();
	}
#line 2426 "verilog_parser.tab.cc"
    break;

  case 234: // $@32: %empty
#line 1280 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                           {
		bool prev_was_input = true;
		bool prev_was_output = false;
		if (extra->albuf) {
			prev_was_input = extra->astbuf1->is_input;
			prev_was_output = extra->astbuf1->is_output;
			(void)extra->astbuf1.reset();
			if (extra->astbuf2 != nullptr)
				(void)extra->astbuf2.reset();
			extra->albuf.reset();
		}
		extra->albuf = std::move(yystack_[2].value.as < al_t > ());
		extra->astbuf1 = std::move(yystack_[1].value.as < ast_t > ());
		extra->astbuf2 = checkRange(extra->astbuf1.get(), std::move(yystack_[0].value.as < ast_t > ()));
		if (!extra->astbuf1->is_input && !extra->astbuf1->is_output) {
			if (!mode->sv)
				err_at_loc(yystack_[1].location, "task/function argument direction missing");
			extra->astbuf1->is_input = prev_was_input;
			extra->astbuf1->is_output = prev_was_output;
		}
	}
#line 2452 "verilog_parser.tab.cc"
    break;

  case 236: // $@33: %empty
#line 1301 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
        {
		if (!extra->astbuf1) {
			if (!mode->sv)
				err_at_loc(yylhs.location, "task/function argument direction missing");
			extra->albuf = std::make_unique<dict<IdString, std::unique_ptr<AstNode>>>();
			extra->astbuf1 = std::make_unique<AstNode>(yylhs.location, AST_WIRE);
			extra->current_wire_rand = false;
			extra->current_wire_const = false;
			extra->astbuf1->is_input = true;
			extra->astbuf2 = nullptr;
		}
	}
#line 2469 "verilog_parser.tab.cc"
    break;

  case 243: // specify_item: specify_if "'('" specify_edge expr TOK_SPECIFY_OPER specify_target "')'" "'='" specify_rise_fall "';'"
#line 1328 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                                                {
		auto en_expr = std::move(yystack_[9].value.as < ast_t > ());
		char specify_edge = yystack_[7].value.as < ch_t > ();
		auto src_expr = std::move(yystack_[6].value.as < ast_t > ());
		string *oper = yystack_[5].value.as < string_t > ().get();
		specify_target_ptr_t target = std::move(yystack_[4].value.as < specify_target_ptr_t > ());
		specify_rise_fall_ptr_t timing = std::move(yystack_[1].value.as < specify_rise_fall_ptr_t > ());

		if (specify_edge != 0 && target->dat == nullptr)
			err_at_loc(yystack_[7].location, "Found specify edge but no data spec.");

		auto cell_owned = std::make_unique<AstNode>(yylhs.location, AST_CELL);
		auto cell = cell_owned.get();
		extra->ast_stack.back()->children.push_back(std::move(cell_owned));
		cell->str = stringf("$specify$%d", autoidx++);
		cell->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_CELLTYPE));
		cell->children.back()->str = target->dat ? "$specify3" : "$specify2";
		SET_AST_NODE_LOC(cell, en_expr.get() ? yystack_[9].location : yystack_[8].location, yystack_[0].location);

		char oper_polarity = 0;
		char oper_type = oper->at(0);

		if (oper->size() == 3) {
			oper_polarity = oper->at(0);
			oper_type = oper->at(1);
		}

		cell->children.push_back(std::make_unique<AstNode>(yystack_[5].location, AST_PARASET, AstNode::mkconst_int(yystack_[5].location, oper_type == '*', false, 1)));
		cell->children.back()->str = "\\FULL";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[5].location, AST_PARASET, AstNode::mkconst_int(yystack_[5].location, oper_polarity != 0, false, 1)));
		cell->children.back()->str = "\\SRC_DST_PEN";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[5].location, AST_PARASET, AstNode::mkconst_int(yystack_[5].location, oper_polarity == '+', false, 1)));
		cell->children.back()->str = "\\SRC_DST_POL";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[1].location, AST_PARASET, std::move(timing->rise.t_min)));
		cell->children.back()->str = "\\T_RISE_MIN";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[1].location, AST_PARASET, std::move(timing->rise.t_avg)));
		cell->children.back()->str = "\\T_RISE_TYP";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[1].location, AST_PARASET, std::move(timing->rise.t_max)));
		cell->children.back()->str = "\\T_RISE_MAX";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[1].location, AST_PARASET, std::move(timing->fall.t_min)));
		cell->children.back()->str = "\\T_FALL_MIN";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[1].location, AST_PARASET, std::move(timing->fall.t_avg)));
		cell->children.back()->str = "\\T_FALL_TYP";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[1].location, AST_PARASET, std::move(timing->fall.t_max)));
		cell->children.back()->str = "\\T_FALL_MAX";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[9].location, AST_ARGUMENT, en_expr ? std::move(en_expr) : AstNode::mkconst_int(yystack_[9].location, 1, false, 1)));
		cell->children.back()->str = "\\EN";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[6].location, AST_ARGUMENT, std::move(src_expr)));
		cell->children.back()->str = "\\SRC";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[4].location, AST_ARGUMENT, std::move(target->dst)));
		cell->children.back()->str = "\\DST";

		if (target->dat)
		{
			cell->children.push_back(std::make_unique<AstNode>(yystack_[7].location, AST_PARASET, AstNode::mkconst_int(yystack_[7].location, specify_edge != 0, false, 1)));
			cell->children.back()->str = "\\EDGE_EN";

			cell->children.push_back(std::make_unique<AstNode>(yystack_[7].location, AST_PARASET, AstNode::mkconst_int(yystack_[7].location, specify_edge == 'p', false, 1)));
			cell->children.back()->str = "\\EDGE_POL";

			cell->children.push_back(std::make_unique<AstNode>(yystack_[4].location, AST_PARASET, AstNode::mkconst_int(yystack_[4].location, target->polarity_op != 0, false, 1)));
			cell->children.back()->str = "\\DAT_DST_PEN";

			cell->children.push_back(std::make_unique<AstNode>(yystack_[4].location, AST_PARASET, AstNode::mkconst_int(yystack_[4].location, target->polarity_op == '+', false, 1)));
			cell->children.back()->str = "\\DAT_DST_POL";

			cell->children.push_back(std::make_unique<AstNode>(yystack_[4].location, AST_ARGUMENT, std::move(target->dat)));
			cell->children.back()->str = "\\DAT";
		}
	}
#line 2555 "verilog_parser.tab.cc"
    break;

  case 244: // specify_item: TOK_ID "'('" specify_edge expr specify_condition "','" specify_edge expr specify_condition "','" specify_triple specify_opt_triple "')'" "';'"
#line 1409 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                                                                                               {
		if (*yystack_[13].value.as < string_t > () != "$setup" && *yystack_[13].value.as < string_t > () != "$hold" && *yystack_[13].value.as < string_t > () != "$setuphold" && *yystack_[13].value.as < string_t > () != "$removal" && *yystack_[13].value.as < string_t > () != "$recovery" &&
				*yystack_[13].value.as < string_t > () != "$recrem" && *yystack_[13].value.as < string_t > () != "$skew" && *yystack_[13].value.as < string_t > () != "$timeskew" && *yystack_[13].value.as < string_t > () != "$fullskew" && *yystack_[13].value.as < string_t > () != "$nochange")
			err_at_loc(yystack_[13].location, "Unsupported specify rule type: %s", *yystack_[13].value.as < string_t > ());

		auto src_pen = AstNode::mkconst_int(yystack_[11].location, yystack_[11].value.as < ch_t > () != 0, false, 1);
		auto src_pol = AstNode::mkconst_int(yystack_[11].location, yystack_[11].value.as < ch_t > () == 'p', false, 1);
		auto src_expr = std::move(yystack_[10].value.as < ast_t > ()), src_en = yystack_[9].value.as < ast_t > () ? std::move(yystack_[9].value.as < ast_t > ()) : AstNode::mkconst_int(yystack_[9].location, 1, false, 1);

		auto dst_pen = AstNode::mkconst_int(yystack_[7].location, yystack_[7].value.as < ch_t > () != 0, false, 1);
		auto dst_pol = AstNode::mkconst_int(yystack_[7].location, yystack_[7].value.as < ch_t > () == 'p', false, 1);
		auto dst_expr = std::move(yystack_[6].value.as < ast_t > ()), dst_en = yystack_[5].value.as < ast_t > () ? std::move(yystack_[5].value.as < ast_t > ()) : AstNode::mkconst_int(yystack_[9].location, 1, false, 1);

		specify_triple_ptr_t limit = std::move(yystack_[3].value.as < specify_triple_ptr_t > ());
		specify_triple_ptr_t limit2 = std::move(yystack_[2].value.as < specify_triple_ptr_t > ());

		auto cell_owned = std::make_unique<AstNode>(yylhs.location, AST_CELL);
		auto cell = cell_owned.get();
		extra->ast_stack.back()->children.push_back(std::move(cell_owned));
		cell->str = stringf("$specify$%d", autoidx++);
		cell->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_CELLTYPE));
		cell->children.back()->str = "$specrule";
		SET_AST_NODE_LOC(cell, yystack_[13].location, yystack_[0].location);

		cell->children.push_back(std::make_unique<AstNode>(yystack_[13].location, AST_PARASET, AstNode::mkconst_str(yystack_[13].location, *yystack_[13].value.as < string_t > ())));
		cell->children.back()->str = "\\TYPE";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[3].location, AST_PARASET, std::move(limit->t_min)));
		cell->children.back()->str = "\\T_LIMIT_MIN";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[3].location, AST_PARASET, std::move(limit->t_avg)));
		cell->children.back()->str = "\\T_LIMIT_TYP";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[3].location, AST_PARASET, std::move(limit->t_max)));
		cell->children.back()->str = "\\T_LIMIT_MAX";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[2].location, AST_PARASET, limit2 ? std::move(limit2->t_min) : AstNode::mkconst_int(yystack_[2].location, 0, true)));
		cell->children.back()->str = "\\T_LIMIT2_MIN";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[2].location, AST_PARASET, limit2 ? std::move(limit2->t_avg) : AstNode::mkconst_int(yystack_[2].location, 0, true)));
		cell->children.back()->str = "\\T_LIMIT2_TYP";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[2].location, AST_PARASET, limit2 ? std::move(limit2->t_max) : AstNode::mkconst_int(yystack_[2].location, 0, true)));
		cell->children.back()->str = "\\T_LIMIT2_MAX";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[11].location, AST_PARASET, std::move(src_pen)));
		cell->children.back()->str = "\\SRC_PEN";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[11].location, AST_PARASET, std::move(src_pol)));
		cell->children.back()->str = "\\SRC_POL";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[11].location, AST_PARASET, std::move(dst_pen)));
		cell->children.back()->str = "\\DST_PEN";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[7].location, AST_PARASET, std::move(dst_pol)));
		cell->children.back()->str = "\\DST_POL";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[9].location, AST_ARGUMENT, std::move(src_en)));
		cell->children.back()->str = "\\SRC_EN";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[10].location, AST_ARGUMENT, std::move(src_expr)));
		cell->children.back()->str = "\\SRC";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[5].location, AST_ARGUMENT, std::move(dst_en)));
		cell->children.back()->str = "\\DST_EN";

		cell->children.push_back(std::make_unique<AstNode>(yystack_[6].location, AST_ARGUMENT, std::move(dst_expr)));
		cell->children.back()->str = "\\DST";
	}
#line 2629 "verilog_parser.tab.cc"
    break;

  case 245: // specify_opt_triple: "','" specify_triple
#line 1480 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                 {
		yylhs.value.as < specify_triple_ptr_t > () = std::move(yystack_[0].value.as < specify_triple_ptr_t > ());
	}
#line 2637 "verilog_parser.tab.cc"
    break;

  case 246: // specify_opt_triple: %empty
#line 1483 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < specify_triple_ptr_t > () = nullptr;
	}
#line 2645 "verilog_parser.tab.cc"
    break;

  case 247: // specify_if: TOK_IF "'('" expr "')'"
#line 1488 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		yylhs.value.as < ast_t > () = std::move(yystack_[1].value.as < ast_t > ());
	}
#line 2653 "verilog_parser.tab.cc"
    break;

  case 248: // specify_if: %empty
#line 1491 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < ast_t > () = nullptr;
	}
#line 2661 "verilog_parser.tab.cc"
    break;

  case 249: // specify_condition: TOK_SPECIFY_AND expr
#line 1496 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                             {
		yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ());
	}
#line 2669 "verilog_parser.tab.cc"
    break;

  case 250: // specify_condition: %empty
#line 1499 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < ast_t > () = nullptr;
	}
#line 2677 "verilog_parser.tab.cc"
    break;

  case 251: // specify_target: expr
#line 1504 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		yylhs.value.as < specify_target_ptr_t > () = std::make_unique<struct specify_target>();
		yylhs.value.as < specify_target_ptr_t > ()->polarity_op = 0;
		yylhs.value.as < specify_target_ptr_t > ()->dst = std::move(yystack_[0].value.as < ast_t > ());
		yylhs.value.as < specify_target_ptr_t > ()->dat = nullptr;
	}
#line 2688 "verilog_parser.tab.cc"
    break;

  case 252: // specify_target: "'('" expr "':'" expr "')'"
#line 1510 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                               {
		yylhs.value.as < specify_target_ptr_t > () = std::make_unique<struct specify_target>();
		yylhs.value.as < specify_target_ptr_t > ()->polarity_op = 0;
		yylhs.value.as < specify_target_ptr_t > ()->dst = std::move(yystack_[3].value.as < ast_t > ());
		yylhs.value.as < specify_target_ptr_t > ()->dat = std::move(yystack_[1].value.as < ast_t > ());
	}
#line 2699 "verilog_parser.tab.cc"
    break;

  case 253: // specify_target: "'('" expr TOK_NEG_INDEXED expr "')'"
#line 1516 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                       {
		yylhs.value.as < specify_target_ptr_t > () = std::make_unique<struct specify_target>();
		yylhs.value.as < specify_target_ptr_t > ()->polarity_op = '-';
		yylhs.value.as < specify_target_ptr_t > ()->dst = std::move(yystack_[3].value.as < ast_t > ());
		yylhs.value.as < specify_target_ptr_t > ()->dat = std::move(yystack_[1].value.as < ast_t > ());
	}
#line 2710 "verilog_parser.tab.cc"
    break;

  case 254: // specify_target: "'('" expr TOK_POS_INDEXED expr "')'"
#line 1522 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                       {
		yylhs.value.as < specify_target_ptr_t > () = std::make_unique<struct specify_target>();
		yylhs.value.as < specify_target_ptr_t > ()->polarity_op = '+';
		yylhs.value.as < specify_target_ptr_t > ()->dst = std::move(yystack_[3].value.as < ast_t > ());
		yylhs.value.as < specify_target_ptr_t > ()->dat = std::move(yystack_[1].value.as < ast_t > ());
	}
#line 2721 "verilog_parser.tab.cc"
    break;

  case 255: // specify_edge: TOK_POSEDGE
#line 1530 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    { yylhs.value.as < ch_t > () = 'p'; }
#line 2727 "verilog_parser.tab.cc"
    break;

  case 256: // specify_edge: TOK_NEGEDGE
#line 1531 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    { yylhs.value.as < ch_t > () = 'n'; }
#line 2733 "verilog_parser.tab.cc"
    break;

  case 257: // specify_edge: %empty
#line 1532 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               { yylhs.value.as < ch_t > () = 0; }
#line 2739 "verilog_parser.tab.cc"
    break;

  case 258: // specify_rise_fall: specify_triple
#line 1535 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       {
		yylhs.value.as < specify_rise_fall_ptr_t > () = std::make_unique<struct specify_rise_fall>();
		yylhs.value.as < specify_rise_fall_ptr_t > ()->fall.t_min = yystack_[0].value.as < specify_triple_ptr_t > ()->t_min->clone();
		yylhs.value.as < specify_rise_fall_ptr_t > ()->fall.t_avg = yystack_[0].value.as < specify_triple_ptr_t > ()->t_avg->clone();
		yylhs.value.as < specify_rise_fall_ptr_t > ()->fall.t_max = yystack_[0].value.as < specify_triple_ptr_t > ()->t_max->clone();
		yylhs.value.as < specify_rise_fall_ptr_t > ()->rise = std::move(*yystack_[0].value.as < specify_triple_ptr_t > ());
	}
#line 2751 "verilog_parser.tab.cc"
    break;

  case 259: // specify_rise_fall: "'('" specify_triple "','" specify_triple "')'"
#line 1542 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                      {
		yylhs.value.as < specify_rise_fall_ptr_t > () = std::make_unique<struct specify_rise_fall>();
		yylhs.value.as < specify_rise_fall_ptr_t > ()->rise = std::move(*yystack_[3].value.as < specify_triple_ptr_t > ());
		yylhs.value.as < specify_rise_fall_ptr_t > ()->fall = std::move(*yystack_[1].value.as < specify_triple_ptr_t > ());
	}
#line 2761 "verilog_parser.tab.cc"
    break;

  case 260: // specify_rise_fall: "'('" specify_triple "','" specify_triple "','" specify_triple "')'"
#line 1547 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                               {
		yylhs.value.as < specify_rise_fall_ptr_t > () = std::make_unique<struct specify_rise_fall>();
		yylhs.value.as < specify_rise_fall_ptr_t > ()->rise = std::move(*yystack_[5].value.as < specify_triple_ptr_t > ());
		yylhs.value.as < specify_rise_fall_ptr_t > ()->fall = std::move(*yystack_[3].value.as < specify_triple_ptr_t > ());
		warn_at_loc(yylhs.location, "Path delay expressions beyond rise/fall not currently supported. Ignoring.");
	}
#line 2772 "verilog_parser.tab.cc"
    break;

  case 261: // specify_rise_fall: "'('" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "')'"
#line 1553 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                                                                                          {
		yylhs.value.as < specify_rise_fall_ptr_t > () = std::make_unique<struct specify_rise_fall>();
		yylhs.value.as < specify_rise_fall_ptr_t > ()->rise = std::move(*yystack_[11].value.as < specify_triple_ptr_t > ());
		yylhs.value.as < specify_rise_fall_ptr_t > ()->fall = std::move(*yystack_[9].value.as < specify_triple_ptr_t > ());
		warn_at_loc(yylhs.location, "Path delay expressions beyond rise/fall not currently supported. Ignoring.");
	}
#line 2783 "verilog_parser.tab.cc"
    break;

  case 262: // specify_rise_fall: "'('" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "','" specify_triple "')'"
#line 1559 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                                                                                                                                                                                                                                                {
		yylhs.value.as < specify_rise_fall_ptr_t > () = std::make_unique<struct specify_rise_fall>();
		yylhs.value.as < specify_rise_fall_ptr_t > ()->rise = std::move(*yystack_[23].value.as < specify_triple_ptr_t > ());
		yylhs.value.as < specify_rise_fall_ptr_t > ()->fall = std::move(*yystack_[21].value.as < specify_triple_ptr_t > ());
		warn_at_loc(yylhs.location, "Path delay expressions beyond rise/fall not currently supported. Ignoring.");
	}
#line 2794 "verilog_parser.tab.cc"
    break;

  case 263: // specify_triple: expr
#line 1567 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		yylhs.value.as < specify_triple_ptr_t > () = std::make_unique<struct specify_triple>();
		yylhs.value.as < specify_triple_ptr_t > ()->t_avg = yystack_[0].value.as < ast_t > ()->clone();
		yylhs.value.as < specify_triple_ptr_t > ()->t_max = yystack_[0].value.as < ast_t > ()->clone();
		yylhs.value.as < specify_triple_ptr_t > ()->t_min = std::move(yystack_[0].value.as < ast_t > ());
	}
#line 2805 "verilog_parser.tab.cc"
    break;

  case 264: // specify_triple: expr "':'" expr "':'" expr
#line 1573 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                       {
		yylhs.value.as < specify_triple_ptr_t > () = std::make_unique<struct specify_triple>();
		yylhs.value.as < specify_triple_ptr_t > ()->t_min = std::move(yystack_[4].value.as < ast_t > ());
		yylhs.value.as < specify_triple_ptr_t > ()->t_avg = std::move(yystack_[2].value.as < ast_t > ());
		yylhs.value.as < specify_triple_ptr_t > ()->t_max = std::move(yystack_[0].value.as < ast_t > ());
	}
#line 2816 "verilog_parser.tab.cc"
    break;

  case 337: // ignspec_constant_expression: expr
#line 1736 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {  }
#line 2822 "verilog_parser.tab.cc"
    break;

  case 338: // ignspec_expr: expr
#line 1739 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {  }
#line 2828 "verilog_parser.tab.cc"
    break;

  case 339: // ignspec_expr: expr "':'" expr "':'" expr
#line 1740 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                       {
	}
#line 2835 "verilog_parser.tab.cc"
    break;

  case 340: // $@34: %empty
#line 1744 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {  }
#line 2841 "verilog_parser.tab.cc"
    break;

  case 341: // ignspec_id: TOK_ID $@34 range_or_multirange
#line 1745 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                            {  }
#line 2847 "verilog_parser.tab.cc"
    break;

  case 342: // param_signed: TOK_SIGNED
#line 1750 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                   {
		extra->astbuf1->is_signed = true;
	}
#line 2855 "verilog_parser.tab.cc"
    break;

  case 343: // param_signed: TOK_UNSIGNED
#line 1752 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                         {
		extra->astbuf1->is_signed = false;
	}
#line 2863 "verilog_parser.tab.cc"
    break;

  case 345: // param_integer: type_atom
#line 1757 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  {
		extra->astbuf1->is_reg = false;
	}
#line 2871 "verilog_parser.tab.cc"
    break;

  case 346: // param_real: TOK_REAL
#line 1762 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {
		extra->astbuf1->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_REALVALUE));
	}
#line 2879 "verilog_parser.tab.cc"
    break;

  case 347: // param_range: range
#line 1767 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
              {
		if (yystack_[0].value.as < ast_t > () != nullptr) {
			extra->astbuf1->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
		}
	}
#line 2889 "verilog_parser.tab.cc"
    break;

  case 349: // param_range_type: type_vec param_signed
#line 1775 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                              {
		addRange(extra->astbuf1.get(), 0, 0);
	}
#line 2897 "verilog_parser.tab.cc"
    break;

  case 350: // param_range_type: type_vec param_signed non_opt_range
#line 1778 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		extra->astbuf1->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 2905 "verilog_parser.tab.cc"
    break;

  case 356: // param_type: hierarchical_type_id
#line 1785 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                             {
		extra->addWiretypeNode(yystack_[0].value.as < string_t > ().get(), extra->astbuf1.get());
	}
#line 2913 "verilog_parser.tab.cc"
    break;

  case 357: // $@35: %empty
#line 1790 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                           {
		extra->astbuf1 = std::make_unique<AstNode>(yylhs.location, AST_PARAMETER);
		extra->astbuf1->children.push_back(AstNode::mkconst_int(yylhs.location, 0, true));
		append_attr(extra->astbuf1.get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 2923 "verilog_parser.tab.cc"
    break;

  case 358: // param_decl: attr TOK_PARAMETER $@35 param_type param_decl_list "';'"
#line 1794 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                 {
		(void)extra->astbuf1.reset();
	}
#line 2931 "verilog_parser.tab.cc"
    break;

  case 359: // $@36: %empty
#line 1799 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                            {
		extra->astbuf1 = std::make_unique<AstNode>(yylhs.location, AST_LOCALPARAM);
		extra->astbuf1->children.push_back(AstNode::mkconst_int(yylhs.location, 0, true));
		append_attr(extra->astbuf1.get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 2941 "verilog_parser.tab.cc"
    break;

  case 360: // localparam_decl: attr TOK_LOCALPARAM $@36 param_type param_decl_list "';'"
#line 1803 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                 {
		(void)extra->astbuf1.reset();
	}
#line 2949 "verilog_parser.tab.cc"
    break;

  case 363: // single_param_decl: single_param_decl_ident "'='" expr
#line 1811 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		AstNode *decl = extra->ast_stack.back()->children.back().get();
		log_assert(decl->type == AST_PARAMETER || decl->type == AST_LOCALPARAM);
		decl->children[0] = std::move(yystack_[0].value.as < ast_t > ());
	}
#line 2959 "verilog_parser.tab.cc"
    break;

  case 364: // single_param_decl: single_param_decl_ident
#line 1816 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                {
		AstNode *decl = extra->ast_stack.back()->children.back().get();
		if (decl->type != AST_PARAMETER) {
			log_assert(decl->type == AST_LOCALPARAM);
			err_at_loc(yystack_[0].location, "localparam initialization is missing!");
		}
		if (!mode->sv)
			err_at_loc(yystack_[0].location, "Parameter defaults can only be omitted in SystemVerilog mode!");
		decl->children.erase(decl->children.begin());
	}
#line 2974 "verilog_parser.tab.cc"
    break;

  case 365: // single_param_decl_ident: TOK_ID
#line 1828 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		std::unique_ptr<AstNode> node_owned;
		if (extra->astbuf1 == nullptr) {
			if (!mode->sv)
				err_at_loc(yystack_[0].location, "In pure Verilog (not SystemVerilog), parameter/localparam with an initializer must use the parameter/localparam keyword");
			node_owned = std::make_unique<AstNode>(yylhs.location, AST_PARAMETER);
			node_owned->children.push_back(AstNode::mkconst_int(yylhs.location, 0, true));
		} else {
			node_owned = extra->astbuf1->clone();
		}
		node_owned->str = *yystack_[0].value.as < string_t > ();
		auto node = node_owned.get();
		extra->ast_stack.back()->children.push_back(std::move(node_owned));
		SET_AST_NODE_LOC(node, yystack_[0].location, yystack_[0].location);
	}
#line 2994 "verilog_parser.tab.cc"
    break;

  case 369: // single_defparam_decl: range rvalue "'='" expr
#line 1851 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                 {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_DEFPARAM);
		node->children.push_back(std::move(yystack_[2].value.as < ast_t > ()));
		node->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
		if (yystack_[3].value.as < ast_t > () != nullptr)
			node->children.push_back(std::move(yystack_[3].value.as < ast_t > ()));
		extra->ast_stack.back()->children.push_back(std::move(node));
	}
#line 3007 "verilog_parser.tab.cc"
    break;

  case 370: // $@37: %empty
#line 1864 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    {
		static int enum_count;
		// create parent node for the enum
		extra->astbuf2 = std::make_unique<AstNode>(yylhs.location, AST_ENUM);
		extra->astbuf2->str = std::string("$enum");
		extra->astbuf2->str += std::to_string(enum_count++);
		log_assert(!extra->cell_hack);
		extra->cell_hack = extra->astbuf2.get();
		extra->ast_stack.back()->children.push_back(std::move(extra->astbuf2));
		// create the template for the names
		extra->astbuf1 = std::make_unique<AstNode>(yylhs.location, AST_ENUM_ITEM);
		extra->astbuf1->children.push_back(AstNode::mkconst_int(yylhs.location, 0, true));
	}
#line 3025 "verilog_parser.tab.cc"
    break;

  case 371: // enum_type: TOK_ENUM $@37 enum_base_type "'{'" enum_name_list optional_comma "'}'"
#line 1876 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                           {
		// create template for the enum vars
		log_assert(extra->cell_hack);
		auto tnode_owned = extra->astbuf1->clone();
		auto* tnode = tnode_owned.get();
		extra->astbuf1 = std::move(tnode_owned);
		tnode->type = AST_WIRE;
		tnode->attributes[ID::enum_type] = AstNode::mkconst_str(yylhs.location, extra->cell_hack->str);
		extra->cell_hack = nullptr;
		// drop constant but keep any range
		tnode->children.erase(tnode->children.begin());
		yylhs.value.as < ast_t > () = extra->astbuf1->clone();
	}
#line 3043 "verilog_parser.tab.cc"
    break;

  case 373: // enum_base_type: type_vec type_signing range
#line 1891 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                        { if (yystack_[0].value.as < ast_t > ()) extra->astbuf1->children.push_back(std::move(yystack_[0].value.as < ast_t > ())); }
#line 3049 "verilog_parser.tab.cc"
    break;

  case 374: // enum_base_type: %empty
#line 1892 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                        { extra->astbuf1->is_reg = true; addRange(extra->astbuf1.get()); }
#line 3055 "verilog_parser.tab.cc"
    break;

  case 375: // type_atom: integer_atom_type
#line 1896 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		extra->astbuf1->is_reg = true;
		extra->astbuf1->is_signed = true;
		addRange(extra->astbuf1.get(), yystack_[0].value.as < integer_t > () - 1, 0);
	}
#line 3065 "verilog_parser.tab.cc"
    break;

  case 376: // type_vec: TOK_REG
#line 1902 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                { extra->astbuf1->is_reg   = true; }
#line 3071 "verilog_parser.tab.cc"
    break;

  case 377: // type_vec: TOK_LOGIC
#line 1903 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                { extra->astbuf1->is_logic = true; }
#line 3077 "verilog_parser.tab.cc"
    break;

  case 378: // type_signing: TOK_SIGNED
#line 1907 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                { extra->astbuf1->is_signed = true; }
#line 3083 "verilog_parser.tab.cc"
    break;

  case 379: // type_signing: TOK_UNSIGNED
#line 1908 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                { extra->astbuf1->is_signed = false; }
#line 3089 "verilog_parser.tab.cc"
    break;

  case 383: // enum_name_decl: TOK_ID opt_enum_init
#line 1917 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                             {
		// put in fn
		log_assert((bool)extra->astbuf1);
		log_assert((bool)extra->cell_hack);
		auto node = extra->astbuf1->clone();
		node->str = *yystack_[1].value.as < string_t > ();
		SET_AST_NODE_LOC(node.get(), yystack_[1].location, yystack_[1].location);
		node->children[0] = yystack_[0].value.as < ast_t > () ? std::move(yystack_[0].value.as < ast_t > ()) : std::make_unique<AstNode>(yylhs.location, AST_NONE);
		extra->cell_hack->children.push_back(std::move(node));
	}
#line 3104 "verilog_parser.tab.cc"
    break;

  case 384: // opt_enum_init: "'='" basic_expr
#line 1930 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                        { yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ()); }
#line 3110 "verilog_parser.tab.cc"
    break;

  case 385: // opt_enum_init: %empty
#line 1931 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                { yylhs.value.as < ast_t > () = nullptr; }
#line 3116 "verilog_parser.tab.cc"
    break;

  case 388: // enum_var: TOK_ID
#line 1939 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {
		log_assert((bool)extra->astbuf1);
		auto node = extra->astbuf1->clone();
		node->str = *yystack_[0].value.as < string_t > ();
		SET_AST_NODE_LOC(node.get(), yystack_[0].location, yystack_[0].location);
		node->is_enum = true;
		extra->ast_stack.back()->children.push_back(std::move(node));
	}
#line 3129 "verilog_parser.tab.cc"
    break;

  case 389: // enum_decl: enum_type enum_var_list "';'"
#line 1949 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                        {  }
#line 3135 "verilog_parser.tab.cc"
    break;

  case 390: // $@38: %empty
#line 1957 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                         {
		append_attr(extra->astbuf2.get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 3143 "verilog_parser.tab.cc"
    break;

  case 391: // struct_decl: attr struct_type $@38 struct_var_list "';'"
#line 1959 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                      {
		(void)extra->astbuf2.reset();
	}
#line 3151 "verilog_parser.tab.cc"
    break;

  case 392: // $@39: %empty
#line 1965 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                     {
		extra->astbuf2 = std::move(yystack_[0].value.as < ast_t > ());
		extra->astbuf2->is_custom_type = true;
	}
#line 3160 "verilog_parser.tab.cc"
    break;

  case 393: // struct_type: struct_union $@39 struct_body
#line 1968 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		yylhs.value.as < ast_t > () = extra->astbuf2->clone();
	}
#line 3168 "verilog_parser.tab.cc"
    break;

  case 394: // struct_union: TOK_STRUCT
#line 1974 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                { yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_STRUCT); }
#line 3174 "verilog_parser.tab.cc"
    break;

  case 395: // struct_union: TOK_UNION
#line 1975 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                { yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_UNION); }
#line 3180 "verilog_parser.tab.cc"
    break;

  case 398: // opt_packed: %empty
#line 1983 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               { err_at_loc(yylhs.location, "Only PACKED supported at this time"); }
#line 3186 "verilog_parser.tab.cc"
    break;

  case 399: // opt_signed_struct: TOK_SIGNED
#line 1986 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                { extra->astbuf2->is_signed = true; }
#line 3192 "verilog_parser.tab.cc"
    break;

  case 400: // opt_signed_struct: TOK_UNSIGNED
#line 1987 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                { extra->astbuf2->is_signed = false; }
#line 3198 "verilog_parser.tab.cc"
    break;

  case 404: // struct_member: struct_member_type member_name_list "';'"
#line 1995 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                        { (void)extra->astbuf1.reset(); }
#line 3204 "verilog_parser.tab.cc"
    break;

  case 407: // $@40: %empty
#line 2003 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    {
		extra->astbuf1->str = yystack_[0].value.as < string_t > ()->substr(1);
		extra->astbuf3 = extra->astbuf1->clone();
		log_assert(!extra->member_hack);
		extra->member_hack = extra->astbuf3.get();
		SET_AST_NODE_LOC(extra->member_hack, yystack_[0].location, yystack_[0].location);
		extra->astbuf2->children.push_back(std::move(extra->astbuf3));
	}
#line 3217 "verilog_parser.tab.cc"
    break;

  case 408: // member_name: TOK_ID $@40 range
#line 2010 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                {
		log_assert((bool)extra->member_hack);
		if (yystack_[0].value.as < ast_t > ()) extra->member_hack->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
		extra->member_hack = nullptr;
	}
#line 3227 "verilog_parser.tab.cc"
    break;

  case 409: // $@41: %empty
#line 2017 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    { extra->astbuf1 = std::make_unique<AstNode>(yylhs.location, AST_STRUCT_ITEM); }
#line 3233 "verilog_parser.tab.cc"
    break;

  case 411: // member_type_token: member_type range_or_multirange
#line 2021 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                        {
		auto range = checkRange(extra->astbuf1.get(), std::move(yystack_[0].value.as < ast_t > ()));
		if (range)
			extra->astbuf1->children.push_back(std::move(range));
	}
#line 3243 "verilog_parser.tab.cc"
    break;

  case 412: // $@42: %empty
#line 2026 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
          {
		(void)extra->astbuf1.reset();
	}
#line 3251 "verilog_parser.tab.cc"
    break;

  case 413: // $@43: %empty
#line 2028 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       {
			// stash state on extra->ast_stack
			// sketchy!
			extra->ast_stack.push_back(extra->astbuf2.release());
			extra->astbuf2 = std::move(yystack_[0].value.as < ast_t > ());
		}
#line 3262 "verilog_parser.tab.cc"
    break;

  case 414: // member_type_token: $@42 struct_union $@43 struct_body
#line 2033 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                               {
			extra->astbuf1 = std::move(extra->astbuf2);
			// recover state
			extra->astbuf2.reset(extra->ast_stack.back());
			extra->ast_stack.pop_back();
		}
#line 3273 "verilog_parser.tab.cc"
    break;

  case 417: // member_type: hierarchical_type_id
#line 2043 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                               { extra->addWiretypeNode(yystack_[0].value.as < string_t > ().get(), extra->astbuf1.get()); }
#line 3279 "verilog_parser.tab.cc"
    break;

  case 420: // struct_var: TOK_ID
#line 2051 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                {
		auto var_node = extra->astbuf2->clone();
		var_node->str = *yystack_[0].value.as < string_t > ();
		SET_AST_NODE_LOC(var_node.get(), yystack_[0].location, yystack_[0].location);
		extra->ast_stack.back()->children.push_back(std::move(var_node));
	}
#line 3290 "verilog_parser.tab.cc"
    break;

  case 421: // $@44: %empty
#line 2063 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                           {
		extra->albuf = std::move(yystack_[2].value.as < al_t > ());
		extra->astbuf1 = std::move(yystack_[1].value.as < ast_t > ());
		extra->astbuf2 = checkRange(extra->astbuf1.get(), std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 3300 "verilog_parser.tab.cc"
    break;

  case 422: // $@45: %empty
#line 2067 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                               {
		(void)extra->astbuf1.reset();
		if (extra->astbuf2 != nullptr)
			(void)extra->astbuf2.reset();
		extra->albuf.reset();
	}
#line 3311 "verilog_parser.tab.cc"
    break;

  case 424: // $@46: %empty
#line 2073 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                {
		extra->ast_stack.back()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_WIRE));
		extra->ast_stack.back()->children.back()->str = *yystack_[0].value.as < string_t > ();
		append_attr(extra->ast_stack.back()->children.back().get(), std::move(yystack_[2].value.as < al_t > ()));
		extra->ast_stack.back()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_ASSIGN, std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER), AstNode::mkconst_int(yylhs.location, 0, false, 1)));
		extra->ast_stack.back()->children.back()->children[0]->str = *yystack_[0].value.as < string_t > ();
	}
#line 3323 "verilog_parser.tab.cc"
    break;

  case 426: // $@47: %empty
#line 2080 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                {
		extra->ast_stack.back()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_WIRE));
		extra->ast_stack.back()->children.back()->str = *yystack_[0].value.as < string_t > ();
		append_attr(extra->ast_stack.back()->children.back().get(), std::move(yystack_[2].value.as < al_t > ()));
		extra->ast_stack.back()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_ASSIGN, std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER), AstNode::mkconst_int(yylhs.location, 1, false, 1)));
		extra->ast_stack.back()->children.back()->children[0]->str = *yystack_[0].value.as < string_t > ();
	}
#line 3335 "verilog_parser.tab.cc"
    break;

  case 429: // opt_supply_wires: opt_supply_wires "','" TOK_ID
#line 2090 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		auto wire_node = extra->ast_stack.back()->children.at(GetSize(extra->ast_stack.back()->children)-2)->clone();
		auto assign_node = extra->ast_stack.back()->children.at(GetSize(extra->ast_stack.back()->children)-1)->clone();
		wire_node->str = *yystack_[0].value.as < string_t > ();
		assign_node->children[0]->str = *yystack_[0].value.as < string_t > ();
		extra->ast_stack.back()->children.push_back(std::move(wire_node));
		extra->ast_stack.back()->children.push_back(std::move(assign_node));
	}
#line 3348 "verilog_parser.tab.cc"
    break;

  case 432: // wire_name_and_opt_assign: wire_name
#line 2103 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  {
		bool attr_anyconst = false;
		bool attr_anyseq = false;
		bool attr_allconst = false;
		bool attr_allseq = false;
		if (extra->ast_stack.back()->children.back()->get_bool_attribute(ID::anyconst)) {
			extra->ast_stack.back()->children.back()->attributes.erase(ID::anyconst);
			attr_anyconst = true;
		}
		if (extra->ast_stack.back()->children.back()->get_bool_attribute(ID::anyseq)) {
			extra->ast_stack.back()->children.back()->attributes.erase(ID::anyseq);
			attr_anyseq = true;
		}
		if (extra->ast_stack.back()->children.back()->get_bool_attribute(ID::allconst)) {
			extra->ast_stack.back()->children.back()->attributes.erase(ID::allconst);
			attr_allconst = true;
		}
		if (extra->ast_stack.back()->children.back()->get_bool_attribute(ID::allseq)) {
			extra->ast_stack.back()->children.back()->attributes.erase(ID::allseq);
			attr_allseq = true;
		}
		if (extra->current_wire_rand || attr_anyconst || attr_anyseq || attr_allconst || attr_allseq) {
			auto wire = std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER);
			auto fcall = std::make_unique<AstNode>(yylhs.location, AST_FCALL);
			wire->str = extra->ast_stack.back()->children.back()->str;
			fcall->str = extra->current_wire_const ? "\\$anyconst" : "\\$anyseq";
			if (attr_anyconst)
				fcall->str = "\\$anyconst";
			if (attr_anyseq)
				fcall->str = "\\$anyseq";
			if (attr_allconst)
				fcall->str = "\\$allconst";
			if (attr_allseq)
				fcall->str = "\\$allseq";
			fcall->attributes[ID::reg] = AstNode::mkconst_str(yylhs.location, RTLIL::unescape_id(wire->str));
			extra->ast_stack.back()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_ASSIGN, std::move(wire), std::move(fcall)));
		}
	}
#line 3391 "verilog_parser.tab.cc"
    break;

  case 433: // wire_name_and_opt_assign: wire_name "'='" expr
#line 2141 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                              {
		auto wire = std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER);
		wire->str = extra->ast_stack.back()->children.back()->str;
		if (extra->astbuf1->is_input) {
			extra->astbuf1->attributes[ID::defaultvalue] = std::move(yystack_[0].value.as < ast_t > ());
		}
		else if (extra->astbuf1->is_reg || extra->astbuf1->is_logic){
			auto assign = std::make_unique<AstNode>(yylhs.location, AST_ASSIGN_LE, std::move(wire), std::move(yystack_[0].value.as < ast_t > ()));
			SET_AST_NODE_LOC(assign.get(), yystack_[2].location, yystack_[0].location);
			auto block = std::make_unique<AstNode>(yylhs.location, AST_BLOCK, std::move(assign));
			SET_AST_NODE_LOC(block.get(), yystack_[2].location, yystack_[0].location);
			auto init = std::make_unique<AstNode>(yylhs.location, AST_INITIAL, std::move(block));
			SET_AST_NODE_LOC(init.get(), yystack_[2].location, yystack_[0].location);

			extra->ast_stack.back()->children.push_back(std::move(init));
		}
		else {
			auto assign = std::make_unique<AstNode>(yylhs.location, AST_ASSIGN, std::move(wire), std::move(yystack_[0].value.as < ast_t > ()));
			SET_AST_NODE_LOC(assign.get(), yystack_[2].location, yystack_[0].location);
			extra->ast_stack.back()->children.push_back(std::move(assign));
		}

	}
#line 3419 "verilog_parser.tab.cc"
    break;

  case 434: // wire_name: TOK_ID range_or_multirange
#line 2166 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                   {
		if (extra->astbuf1 == nullptr)
			err_at_loc(yystack_[1].location, "Internal error - should not happen - no AST_WIRE node.");
		auto node = extra->astbuf1->clone();
		node->str = *yystack_[1].value.as < string_t > ();
		append_attr_clone(node.get(), extra->albuf);
		if (extra->astbuf2 != nullptr)
			node->children.push_back(extra->astbuf2->clone());
		if (yystack_[0].value.as < ast_t > () != nullptr) {
			if (node->is_input || node->is_output)
				err_at_loc(yystack_[0].location, "input/output/inout ports cannot have unpacked dimensions.");
			if (!extra->astbuf2 && !node->is_custom_type) {
				addRange(node.get(), 0, 0, false);
			}
			rewriteAsMemoryNode(node.get(), std::move(yystack_[0].value.as < ast_t > ()));
		}
		if (extra->current_function_or_task) {
			if (node->is_input || node->is_output)
				node->port_id = extra->current_function_or_task_port_id++;
		} else if (extra->ast_stack.back()->type == AST_GENBLOCK) {
			if (node->is_input || node->is_output)
				err_at_loc(yystack_[1].location, "Cannot declare module port `%s' within a generate block.", *yystack_[1].value.as < string_t > ());
		} else {
			if (extra->do_not_require_port_stubs && (node->is_input || node->is_output) && extra->port_stubs.count(*yystack_[1].value.as < string_t > ()) == 0) {
				extra->port_stubs[*yystack_[1].value.as < string_t > ()] = ++extra->port_counter;
			}
			if (extra->port_stubs.count(*yystack_[1].value.as < string_t > ()) != 0) {
				if (!node->is_input && !node->is_output)
					err_at_loc(yystack_[1].location, "Module port `%s' is neither input nor output.", *yystack_[1].value.as < string_t > ());
				if (node->is_reg && node->is_input && !node->is_output && !mode->sv)
					err_at_loc(yystack_[1].location, "Input port `%s' is declared as register.", *yystack_[1].value.as < string_t > ());
				node->port_id = extra->port_stubs[*yystack_[1].value.as < string_t > ()];
				extra->port_stubs.erase(*yystack_[1].value.as < string_t > ());
			} else {
				if (node->is_input || node->is_output)
					err_at_loc(yystack_[1].location, "Module port `%s' is not declared in module header.", *yystack_[1].value.as < string_t > ());
			}
		}
		//FIXME: for some reason, TOK_ID has a location which always points to one column *after* the real last column...
		SET_AST_NODE_LOC(node.get(), yystack_[1].location, yystack_[1].location);
		extra->ast_stack.back()->children.push_back(std::move(node));

	}
#line 3467 "verilog_parser.tab.cc"
    break;

  case 438: // assign_expr: lvalue "'='" expr
#line 2217 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                           {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_ASSIGN, std::move(yystack_[2].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ())));
		SET_AST_NODE_LOC(node, yylhs.location, yylhs.location);
	}
#line 3476 "verilog_parser.tab.cc"
    break;

  case 439: // type_name: TOK_ID
#line 2222 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  { yylhs.value.as < string_t > () = std::move(yystack_[0].value.as < string_t > ()); }
#line 3482 "verilog_parser.tab.cc"
    break;

  case 440: // type_name: TOK_USER_TYPE
#line 2223 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                { if (extra->isInLocalScope(yystack_[0].value.as < string_t > ().get())) err_at_loc(yystack_[0].location, "Duplicate declaration of TYPEDEF '%s'", yystack_[0].value.as < string_t > ()->c_str()+1); yylhs.value.as < string_t > () = std::move(yystack_[0].value.as < string_t > ()); }
#line 3488 "verilog_parser.tab.cc"
    break;

  case 441: // typedef_decl: TOK_TYPEDEF typedef_base_type range_or_multirange type_name range_or_multirange "';'"
#line 2227 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                    {
		extra->astbuf1 = std::move(yystack_[4].value.as < ast_t > ());
		extra->astbuf2 = checkRange(extra->astbuf1.get(), std::move(yystack_[3].value.as < ast_t > ()));
		bool has_a_range = (bool)extra->astbuf2;
		if (extra->astbuf2) {
			extra->astbuf1->children.push_back(std::move(extra->astbuf2));
		}

		if (yystack_[1].value.as < ast_t > () != nullptr) {
			if (!has_a_range && !extra->astbuf1->is_custom_type) {
				addRange(extra->astbuf1.get(), 0, 0, false);
			}
			rewriteAsMemoryNode(extra->astbuf1.get(), std::move(yystack_[1].value.as < ast_t > ()));
		}
		extra->addTypedefNode(yystack_[2].value.as < string_t > ().get(), std::move(extra->astbuf1)); }
#line 3508 "verilog_parser.tab.cc"
    break;

  case 442: // typedef_decl: TOK_TYPEDEF enum_struct_type type_name "';'"
#line 2242 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                               { extra->addTypedefNode(yystack_[1].value.as < string_t > ().get(), std::move(yystack_[2].value.as < ast_t > ())); }
#line 3514 "verilog_parser.tab.cc"
    break;

  case 443: // typedef_base_type: hierarchical_type_id
#line 2246 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                             {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_WIRE);
		yylhs.value.as < ast_t > ()->is_logic = true;
		extra->addWiretypeNode(yystack_[0].value.as < string_t > ().get(), yylhs.value.as < ast_t > ().get());
	}
#line 3524 "verilog_parser.tab.cc"
    break;

  case 444: // typedef_base_type: integer_vector_type opt_signedness_default_unsigned
#line 2251 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                            {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_WIRE);
		if (yystack_[1].value.as < integer_t > () == token::TOK_REG) {
			yylhs.value.as < ast_t > ()->is_reg = true;
		} else {
			yylhs.value.as < ast_t > ()->is_logic = true;
		}
		yylhs.value.as < ast_t > ()->is_signed = yystack_[0].value.as < boolean_t > ();
	}
#line 3538 "verilog_parser.tab.cc"
    break;

  case 445: // typedef_base_type: integer_atom_type opt_signedness_default_signed
#line 2260 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                        {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_WIRE);
		yylhs.value.as < ast_t > ()->is_logic = true;
		yylhs.value.as < ast_t > ()->is_signed = yystack_[0].value.as < boolean_t > ();
		yylhs.value.as < ast_t > ()->range_left = yystack_[1].value.as < integer_t > () - 1;
		yylhs.value.as < ast_t > ()->range_right = 0;
	}
#line 3550 "verilog_parser.tab.cc"
    break;

  case 446: // enum_struct_type: enum_type
#line 2269 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    { yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ()); }
#line 3556 "verilog_parser.tab.cc"
    break;

  case 447: // enum_struct_type: struct_type
#line 2270 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      { yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ()); }
#line 3562 "verilog_parser.tab.cc"
    break;

  case 448: // $@48: %empty
#line 2274 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    {
		extra->astbuf1 = std::make_unique<AstNode>(yylhs.location, AST_CELL);
		append_attr(extra->astbuf1.get(), std::move(yystack_[1].value.as < al_t > ()));
		extra->astbuf1->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_CELLTYPE));
		extra->astbuf1->children[0]->str = *yystack_[0].value.as < string_t > ();
	}
#line 3573 "verilog_parser.tab.cc"
    break;

  case 449: // cell_stmt: attr TOK_ID $@48 cell_parameter_list_opt cell_list "';'"
#line 2279 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                        {
		(void)extra->astbuf1.reset();
	}
#line 3581 "verilog_parser.tab.cc"
    break;

  case 450: // $@49: %empty
#line 2282 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                    {
		extra->astbuf1 = std::make_unique<AstNode>(yylhs.location, AST_PRIMITIVE);
		extra->astbuf1->str = *yystack_[1].value.as < string_t > ();
		append_attr(extra->astbuf1.get(), std::move(yystack_[2].value.as < al_t > ()));
	}
#line 3591 "verilog_parser.tab.cc"
    break;

  case 451: // cell_stmt: attr tok_prim_wrapper delay $@49 prim_list "';'"
#line 2286 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                {
		(void)extra->astbuf1.reset();
	}
#line 3599 "verilog_parser.tab.cc"
    break;

  case 452: // tok_prim_wrapper: TOK_PRIMITIVE
#line 2291 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		yylhs.value.as < string_t > () = std::move(yystack_[0].value.as < string_t > ());
	}
#line 3607 "verilog_parser.tab.cc"
    break;

  case 453: // tok_prim_wrapper: TOK_OR
#line 2294 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < string_t > () = std::make_unique<std::string>("or");
	}
#line 3615 "verilog_parser.tab.cc"
    break;

  case 458: // $@50: %empty
#line 2306 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		extra->astbuf2 = extra->astbuf1->clone();
		if (extra->astbuf2->type != AST_PRIMITIVE)
			extra->astbuf2->str = *yystack_[0].value.as < string_t > ();
		// TODO optimize again
		extra->cell_hack = extra->astbuf2.get();
		extra->ast_stack.back()->children.push_back(std::move(extra->astbuf2));
	}
#line 3628 "verilog_parser.tab.cc"
    break;

  case 459: // single_cell_no_array: TOK_ID $@50 "'('" cell_port_list "')'"
#line 2313 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                               {
		log_assert(extra->cell_hack);
		SET_AST_NODE_LOC(extra->cell_hack, yystack_[4].location, yylhs.location);
		extra->cell_hack = nullptr;
	}
#line 3638 "verilog_parser.tab.cc"
    break;

  case 460: // $@51: %empty
#line 2320 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                             {
		extra->astbuf2 = extra->astbuf1->clone();
		if (extra->astbuf2->type != AST_PRIMITIVE)
			extra->astbuf2->str = *yystack_[1].value.as < string_t > ();
		// TODO optimize again
		extra->cell_hack = extra->astbuf2.get();
		extra->ast_stack.back()->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_CELLARRAY, std::move(yystack_[0].value.as < ast_t > ()), std::move(extra->astbuf2)));
	}
#line 3651 "verilog_parser.tab.cc"
    break;

  case 461: // single_cell_arraylist: TOK_ID non_opt_range $@51 "'('" cell_port_list "')'"
#line 2327 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                              {
		log_assert(extra->cell_hack);
		SET_AST_NODE_LOC(extra->cell_hack, yystack_[5].location, yylhs.location);
		extra->cell_hack = nullptr;
	}
#line 3661 "verilog_parser.tab.cc"
    break;

  case 467: // $@52: %empty
#line 2343 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		extra->astbuf2 = extra->astbuf1->clone();
		log_assert(!extra->cell_hack);
		extra->cell_hack = extra->astbuf2.get();
		// TODO optimize again
		extra->ast_stack.back()->children.push_back(std::move(extra->astbuf2));
	}
#line 3673 "verilog_parser.tab.cc"
    break;

  case 468: // single_prim: $@52 "'('" cell_port_list "')'"
#line 2349 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                               {
		log_assert(extra->cell_hack);
		SET_AST_NODE_LOC(extra->cell_hack, yystack_[3].location, yylhs.location);
		extra->cell_hack = nullptr;
	}
#line 3683 "verilog_parser.tab.cc"
    break;

  case 474: // cell_parameter: expr
#line 2363 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_PARASET);
		node->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
		extra->astbuf1->children.push_back(std::move(node));
	}
#line 3693 "verilog_parser.tab.cc"
    break;

  case 475: // cell_parameter: "'.'" TOK_ID "'('" "')'"
#line 2368 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                             {
		// just ignore empty parameters
	}
#line 3701 "verilog_parser.tab.cc"
    break;

  case 476: // cell_parameter: "'.'" TOK_ID "'('" expr "')'"
#line 2371 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                  {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_PARASET);
		node->str = *yystack_[3].value.as < string_t > ();
		node->children.push_back(std::move(yystack_[1].value.as < ast_t > ()));
		extra->astbuf1->children.push_back(std::move(node));
	}
#line 3712 "verilog_parser.tab.cc"
    break;

  case 477: // cell_port_list: cell_port_list_rules
#line 2379 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                             {
		// remove empty args from end of list
		while (!extra->cell_hack->children.empty()) {
			auto& node = extra->cell_hack->children.back();
			if (node->type != AST_ARGUMENT) break;
			if (!node->children.empty()) break;
			if (!node->str.empty()) break;
			extra->cell_hack->children.pop_back();
		}

		// check port types
		bool has_positional_args = false;
		bool has_named_args = false;
		for (auto& node : extra->cell_hack->children) {
			if (node->type != AST_ARGUMENT) continue;
			if (node->str.empty())
				has_positional_args = true;
			else
				has_named_args = true;
		}

		if (has_positional_args && has_named_args)
			err_at_loc(yystack_[0].location, "Mix of positional and named cell ports.");
	}
#line 3741 "verilog_parser.tab.cc"
    break;

  case 480: // cell_port: attr
#line 2408 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_ARGUMENT);
		extra->cell_hack->children.push_back(std::move(node));
	}
#line 3750 "verilog_parser.tab.cc"
    break;

  case 481: // cell_port: attr expr
#line 2412 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_ARGUMENT);
		node->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
		extra->cell_hack->children.push_back(std::move(node));
	}
#line 3760 "verilog_parser.tab.cc"
    break;

  case 482: // cell_port: attr "'.'" TOK_ID "'('" expr "')'"
#line 2417 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                       {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_ARGUMENT);
		node->str = *yystack_[3].value.as < string_t > ();
		node->children.push_back(std::move(yystack_[1].value.as < ast_t > ()));
		extra->cell_hack->children.push_back(std::move(node));
	}
#line 3771 "verilog_parser.tab.cc"
    break;

  case 483: // cell_port: attr "'.'" TOK_ID "'('" "')'"
#line 2423 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                  {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_ARGUMENT);
		node->str = *yystack_[2].value.as < string_t > ();
		extra->cell_hack->children.push_back(std::move(node));
	}
#line 3781 "verilog_parser.tab.cc"
    break;

  case 484: // cell_port: attr "'.'" TOK_ID
#line 2428 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                            {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_ARGUMENT);
		node->str = *yystack_[0].value.as < string_t > ();
		node->children.push_back(std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER));
		node->children.back()->str = *yystack_[0].value.as < string_t > ();
		extra->cell_hack->children.push_back(std::move(node));
	}
#line 3793 "verilog_parser.tab.cc"
    break;

  case 485: // cell_port: attr TOK_WILDCARD_CONNECT
#line 2435 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                  {
		if (!mode->sv)
			err_at_loc(yystack_[0].location, "Wildcard port connections are only supported in SystemVerilog mode.");
		extra->cell_hack->attributes[ID::wildcard_port_conns] = AstNode::mkconst_int(yystack_[0].location, 1, false);
	}
#line 3803 "verilog_parser.tab.cc"
    break;

  case 486: // always_comb_or_latch: TOK_ALWAYS_COMB
#line 2442 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        {
		yylhs.value.as < boolean_t > () = false;
	}
#line 3811 "verilog_parser.tab.cc"
    break;

  case 487: // always_comb_or_latch: TOK_ALWAYS_LATCH
#line 2445 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                         {
		yylhs.value.as < boolean_t > () = true;
	}
#line 3819 "verilog_parser.tab.cc"
    break;

  case 488: // always_or_always_ff: TOK_ALWAYS
#line 2450 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                   {
		yylhs.value.as < boolean_t > () = false;
	}
#line 3827 "verilog_parser.tab.cc"
    break;

  case 489: // always_or_always_ff: TOK_ALWAYS_FF
#line 2453 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		yylhs.value.as < boolean_t > () = true;
	}
#line 3835 "verilog_parser.tab.cc"
    break;

  case 490: // $@53: %empty
#line 2458 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                 {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_ALWAYS));
		append_attr(node, std::move(yystack_[1].value.as < al_t > ()));
		if (yystack_[0].value.as < boolean_t > ())
			node->attributes[ID::always_ff] = AstNode::mkconst_int(yystack_[0].location, 1, false);
	}
#line 3846 "verilog_parser.tab.cc"
    break;

  case 491: // $@54: %empty
#line 2463 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		(void)extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_BLOCK));
	}
#line 3854 "verilog_parser.tab.cc"
    break;

  case 492: // always_stmt: attr always_or_always_ff $@53 always_cond $@54 behavioral_stmt
#line 2465 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[0].location, yystack_[0].location);
		extra->ast_stack.pop_back();

		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[4].location, yylhs.location);
		extra->ast_stack.pop_back();

		SET_RULE_LOC(yylhs.location, yystack_[4].location, yylhs.location);
	}
#line 3868 "verilog_parser.tab.cc"
    break;

  case 493: // $@55: %empty
#line 2474 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                  {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_ALWAYS));
		append_attr(node, std::move(yystack_[1].value.as < al_t > ()));
		if (yystack_[0].value.as < boolean_t > ())
			node->attributes[ID::always_latch] = AstNode::mkconst_int(yystack_[0].location, 1, false);
		else
			node->attributes[ID::always_comb] = AstNode::mkconst_int(yystack_[0].location, 1, false);
		(void)extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_BLOCK));
	}
#line 3882 "verilog_parser.tab.cc"
    break;

  case 494: // always_stmt: attr always_comb_or_latch $@55 behavioral_stmt
#line 2482 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		extra->ast_stack.pop_back();
		extra->ast_stack.pop_back();
	}
#line 3891 "verilog_parser.tab.cc"
    break;

  case 495: // $@56: %empty
#line 2486 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                         {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_INITIAL));
		append_attr(node, std::move(yystack_[1].value.as < al_t > ()));
		(void)extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_BLOCK));
	}
#line 3901 "verilog_parser.tab.cc"
    break;

  case 496: // always_stmt: attr TOK_INITIAL $@56 behavioral_stmt
#line 2490 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		extra->ast_stack.pop_back();
		extra->ast_stack.pop_back();
	}
#line 3910 "verilog_parser.tab.cc"
    break;

  case 506: // always_event: TOK_POSEDGE expr
#line 2509 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                         {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_POSEDGE);
		SET_AST_NODE_LOC(node.get(), yystack_[1].location, yystack_[1].location);
		node->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
		extra->ast_stack.back()->children.push_back(std::move(node));
	}
#line 3921 "verilog_parser.tab.cc"
    break;

  case 507: // always_event: TOK_NEGEDGE expr
#line 2515 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                         {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_NEGEDGE);
		SET_AST_NODE_LOC(node.get(), yystack_[1].location, yystack_[1].location);
		node->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
		extra->ast_stack.back()->children.push_back(std::move(node));
	}
#line 3932 "verilog_parser.tab.cc"
    break;

  case 508: // always_event: expr
#line 2521 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		auto node = std::make_unique<AstNode>(yylhs.location, AST_EDGE);
		node->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
		extra->ast_stack.back()->children.push_back(std::move(node));
	}
#line 3942 "verilog_parser.tab.cc"
    break;

  case 509: // opt_label: "':'" TOK_ID
#line 2528 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       {
		yylhs.value.as < string_t > () = std::move(yystack_[0].value.as < string_t > ());
	}
#line 3950 "verilog_parser.tab.cc"
    break;

  case 510: // opt_label: %empty
#line 2531 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < string_t > () = nullptr;
	}
#line 3958 "verilog_parser.tab.cc"
    break;

  case 511: // opt_sva_label: TOK_SVA_LABEL "':'"
#line 2536 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                              {
		yylhs.value.as < string_t > () = std::move(yystack_[1].value.as < string_t > ());
	}
#line 3966 "verilog_parser.tab.cc"
    break;

  case 512: // opt_sva_label: %empty
#line 2539 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < string_t > () = nullptr;
	}
#line 3974 "verilog_parser.tab.cc"
    break;

  case 513: // opt_property: TOK_PROPERTY
#line 2544 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                     {
		yylhs.value.as < boolean_t > () = true;
	}
#line 3982 "verilog_parser.tab.cc"
    break;

  case 514: // opt_property: TOK_FINAL
#line 2547 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  {
		yylhs.value.as < boolean_t > () = false;
	}
#line 3990 "verilog_parser.tab.cc"
    break;

  case 515: // opt_property: %empty
#line 2550 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < boolean_t > () = false;
	}
#line 3998 "verilog_parser.tab.cc"
    break;

  case 516: // $@57: %empty
#line 2555 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       {
		AstNode* modport = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_MODPORT));
        modport->str = *yystack_[0].value.as < string_t > ();

    }
#line 4008 "verilog_parser.tab.cc"
    break;

  case 517: // $@58: %empty
#line 2559 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        {
        extra->ast_stack.pop_back();
        log_assert(extra->ast_stack.size() == 2);
    }
#line 4017 "verilog_parser.tab.cc"
    break;

  case 525: // modport_member: TOK_ID
#line 2575 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
           {
		AstNode* modport_member = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_MODPORTMEMBER));
        modport_member->str = *yystack_[0].value.as < string_t > ();
        modport_member->is_input = extra->current_modport_input;
        modport_member->is_output = extra->current_modport_output;

    }
#line 4029 "verilog_parser.tab.cc"
    break;

  case 526: // modport_type_token: TOK_INPUT
#line 2584 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
              {extra->current_modport_input = 1; extra->current_modport_output = 0;}
#line 4035 "verilog_parser.tab.cc"
    break;

  case 527: // modport_type_token: TOK_OUTPUT
#line 2584 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                  {extra->current_modport_input = 0; extra->current_modport_output = 1;}
#line 4041 "verilog_parser.tab.cc"
    break;

  case 528: // assert: opt_sva_label TOK_ASSERT opt_property "'('" expr "')'" "';'"
#line 2587 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                     {
		if (mode->noassert) {

		} else {
			AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, mode->assume_asserts ? AST_ASSUME : AST_ASSERT, std::move(yystack_[2].value.as < ast_t > ())));
			SET_AST_NODE_LOC(node, (yystack_[6].value.as < string_t > () != nullptr ? yystack_[6].location : yystack_[5].location), yystack_[1].location);
			if (yystack_[6].value.as < string_t > () != nullptr)
				node->str = *yystack_[6].value.as < string_t > ();
		}
	}
#line 4056 "verilog_parser.tab.cc"
    break;

  case 529: // assert: opt_sva_label TOK_ASSUME opt_property "'('" expr "')'" "';'"
#line 2597 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                     {
		if (mode->noassume) {
		} else {
			AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, mode->assert_assumes ? AST_ASSERT : AST_ASSUME, std::move(yystack_[2].value.as < ast_t > ())));
			SET_AST_NODE_LOC(node, (yystack_[6].value.as < string_t > () != nullptr ? yystack_[6].location : yystack_[5].location), yystack_[1].location);
			if (yystack_[6].value.as < string_t > () != nullptr)
				node->str = *yystack_[6].value.as < string_t > ();
		}
	}
#line 4070 "verilog_parser.tab.cc"
    break;

  case 530: // assert: opt_sva_label TOK_ASSERT opt_property "'('" TOK_EVENTUALLY expr "')'" "';'"
#line 2606 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                    {
		if (mode->noassert) {
		} else {
			AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, mode->assume_asserts ? AST_FAIR : AST_LIVE, std::move(yystack_[2].value.as < ast_t > ())));
			SET_AST_NODE_LOC(node, (yystack_[7].value.as < string_t > () != nullptr ? yystack_[7].location : yystack_[6].location), yystack_[1].location);
			if (yystack_[7].value.as < string_t > () != nullptr)
				node->str = *yystack_[7].value.as < string_t > ();
		}
	}
#line 4084 "verilog_parser.tab.cc"
    break;

  case 531: // assert: opt_sva_label TOK_ASSUME opt_property "'('" TOK_EVENTUALLY expr "')'" "';'"
#line 2615 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                    {
		if (mode->noassume) {
		} else {
			AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, mode->assert_assumes ? AST_LIVE : AST_FAIR, std::move(yystack_[2].value.as < ast_t > ())));
			SET_AST_NODE_LOC(node, (yystack_[7].value.as < string_t > () != nullptr ? yystack_[7].location : yystack_[6].location), yystack_[1].location);
			if (yystack_[7].value.as < string_t > () != nullptr)
				node->str = *yystack_[7].value.as < string_t > ();
		}
	}
#line 4098 "verilog_parser.tab.cc"
    break;

  case 532: // assert: opt_sva_label TOK_COVER opt_property "'('" expr "')'" "';'"
#line 2624 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                    {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_COVER, std::move(yystack_[2].value.as < ast_t > ())));
		SET_AST_NODE_LOC(node, (yystack_[6].value.as < string_t > () != nullptr ? yystack_[6].location : yystack_[5].location), yystack_[1].location);
		if (yystack_[6].value.as < string_t > () != nullptr) {
			node->str = *yystack_[6].value.as < string_t > ();
		}
	}
#line 4110 "verilog_parser.tab.cc"
    break;

  case 533: // assert: opt_sva_label TOK_COVER opt_property "'('" "')'" "';'"
#line 2631 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                               {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_COVER, AstNode::mkconst_int(yylhs.location, 1, false)));
		SET_AST_NODE_LOC(node, (yystack_[5].value.as < string_t > () != nullptr ? yystack_[5].location : yystack_[4].location), yystack_[1].location);
		if (yystack_[5].value.as < string_t > () != nullptr) {
			node->str = *yystack_[5].value.as < string_t > ();
		}
	}
#line 4122 "verilog_parser.tab.cc"
    break;

  case 534: // assert: opt_sva_label TOK_COVER "';'"
#line 2638 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_COVER, AstNode::mkconst_int(yylhs.location, 1, false)));
		SET_AST_NODE_LOC(node, (yystack_[2].value.as < string_t > () != nullptr ? yystack_[2].location : yystack_[1].location), yystack_[1].location);
		if (yystack_[2].value.as < string_t > () != nullptr) {
			node->str = *yystack_[2].value.as < string_t > ();
		}
	}
#line 4134 "verilog_parser.tab.cc"
    break;

  case 535: // assert: opt_sva_label TOK_RESTRICT opt_property "'('" expr "')'" "';'"
#line 2645 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                       {
		if (mode->norestrict) {
		} else {
			AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_ASSUME, std::move(yystack_[2].value.as < ast_t > ())));
			SET_AST_NODE_LOC(node, (yystack_[6].value.as < string_t > () != nullptr ? yystack_[6].location : yystack_[5].location), yystack_[1].location);
			if (yystack_[6].value.as < string_t > () != nullptr)
				node->str = *yystack_[6].value.as < string_t > ();
		}
		if (!yystack_[4].value.as < boolean_t > ())
			warn_at_loc(yystack_[4].location, "SystemVerilog does not allow \"restrict\" without \"property\".");
	}
#line 4150 "verilog_parser.tab.cc"
    break;

  case 536: // assert: opt_sva_label TOK_RESTRICT opt_property "'('" TOK_EVENTUALLY expr "')'" "';'"
#line 2656 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                      {
		if (mode->norestrict) {
		} else {
			AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_FAIR, std::move(yystack_[2].value.as < ast_t > ())));
			SET_AST_NODE_LOC(node, (yystack_[7].value.as < string_t > () != nullptr ? yystack_[7].location : yystack_[6].location), yystack_[1].location);
			if (yystack_[7].value.as < string_t > () != nullptr)
				node->str = *yystack_[7].value.as < string_t > ();
		}
		if (!yystack_[5].value.as < boolean_t > ())
			warn_at_loc(yystack_[5].location, "SystemVerilog does not allow \"restrict\" without \"property\".");
	}
#line 4166 "verilog_parser.tab.cc"
    break;

  case 537: // assert_property: opt_sva_label TOK_ASSERT TOK_PROPERTY "'('" expr "')'" "';'"
#line 2669 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                     {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, mode->assume_asserts ? AST_ASSUME : AST_ASSERT, std::move(yystack_[2].value.as < ast_t > ())));
		SET_AST_NODE_LOC(node, yystack_[6].location, yystack_[1].location);
		if (yystack_[6].value.as < string_t > () != nullptr) {
			extra->ast_stack.back()->children.back()->str = *yystack_[6].value.as < string_t > ();
		}
	}
#line 4178 "verilog_parser.tab.cc"
    break;

  case 538: // assert_property: opt_sva_label TOK_ASSUME TOK_PROPERTY "'('" expr "')'" "';'"
#line 2676 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                     {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_ASSUME, std::move(yystack_[2].value.as < ast_t > ())));
		SET_AST_NODE_LOC(node, yystack_[6].location, yystack_[1].location);
		if (yystack_[6].value.as < string_t > () != nullptr) {
			extra->ast_stack.back()->children.back()->str = *yystack_[6].value.as < string_t > ();
		}
	}
#line 4190 "verilog_parser.tab.cc"
    break;

  case 539: // assert_property: opt_sva_label TOK_ASSERT TOK_PROPERTY "'('" TOK_EVENTUALLY expr "')'" "';'"
#line 2683 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                    {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, mode->assume_asserts ? AST_FAIR : AST_LIVE, std::move(yystack_[2].value.as < ast_t > ())));
		SET_AST_NODE_LOC(node, yystack_[7].location, yystack_[1].location);
		if (yystack_[7].value.as < string_t > () != nullptr) {
			extra->ast_stack.back()->children.back()->str = *yystack_[7].value.as < string_t > ();
		}
	}
#line 4202 "verilog_parser.tab.cc"
    break;

  case 540: // assert_property: opt_sva_label TOK_ASSUME TOK_PROPERTY "'('" TOK_EVENTUALLY expr "')'" "';'"
#line 2690 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                    {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_FAIR, std::move(yystack_[2].value.as < ast_t > ())));
		SET_AST_NODE_LOC(node, yystack_[7].location, yystack_[1].location);
		if (yystack_[7].value.as < string_t > () != nullptr) {
			extra->ast_stack.back()->children.back()->str = *yystack_[7].value.as < string_t > ();
		}
	}
#line 4214 "verilog_parser.tab.cc"
    break;

  case 541: // assert_property: opt_sva_label TOK_COVER TOK_PROPERTY "'('" expr "')'" "';'"
#line 2697 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                    {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_COVER, std::move(yystack_[2].value.as < ast_t > ())));
		SET_AST_NODE_LOC(node, yystack_[6].location, yystack_[1].location);
		if (yystack_[6].value.as < string_t > () != nullptr) {
			extra->ast_stack.back()->children.back()->str = *yystack_[6].value.as < string_t > ();
		}
	}
#line 4226 "verilog_parser.tab.cc"
    break;

  case 542: // assert_property: opt_sva_label TOK_RESTRICT TOK_PROPERTY "'('" expr "')'" "';'"
#line 2704 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                       {
		if (mode->norestrict) {
		} else {
			AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_ASSUME, std::move(yystack_[2].value.as < ast_t > ())));
			SET_AST_NODE_LOC(node, yystack_[6].location, yystack_[1].location);
			if (yystack_[6].value.as < string_t > () != nullptr) {
				extra->ast_stack.back()->children.back()->str = *yystack_[6].value.as < string_t > ();
			}
		}
	}
#line 4241 "verilog_parser.tab.cc"
    break;

  case 543: // assert_property: opt_sva_label TOK_RESTRICT TOK_PROPERTY "'('" TOK_EVENTUALLY expr "')'" "';'"
#line 2714 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                                                      {
		if (mode->norestrict) {
		} else {
			AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_FAIR, std::move(yystack_[2].value.as < ast_t > ())));
			SET_AST_NODE_LOC(node, yystack_[7].location, yystack_[1].location);
			if (yystack_[7].value.as < string_t > () != nullptr) {
				extra->ast_stack.back()->children.back()->str = *yystack_[7].value.as < string_t > ();
			}
		}
	}
#line 4256 "verilog_parser.tab.cc"
    break;

  case 544: // simple_behavioral_stmt: attr lvalue "'='" delay expr
#line 2726 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                      {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_ASSIGN_EQ, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ())));
		SET_AST_NODE_LOC(node, yystack_[3].location, yystack_[0].location);
		append_attr(node, std::move(yystack_[4].value.as < al_t > ()));
	}
#line 4266 "verilog_parser.tab.cc"
    break;

  case 545: // simple_behavioral_stmt: attr lvalue attr inc_or_dec_op
#line 2731 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                       {
		extra->addIncOrDecStmt(std::move(yystack_[3].value.as < al_t > ()), std::move(yystack_[2].value.as < ast_t > ()), std::move(yystack_[1].value.as < al_t > ()), yystack_[0].value.as < ast_node_type_t > (), location_range(yystack_[3].location, yystack_[0].location));
	}
#line 4274 "verilog_parser.tab.cc"
    break;

  case 546: // simple_behavioral_stmt: attr inc_or_dec_op attr lvalue
#line 2734 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                       {
		extra->addIncOrDecStmt(std::move(yystack_[3].value.as < al_t > ()), std::move(yystack_[0].value.as < ast_t > ()), std::move(yystack_[1].value.as < al_t > ()), yystack_[2].value.as < ast_node_type_t > (), location_range(yystack_[3].location, yystack_[0].location));
	}
#line 4282 "verilog_parser.tab.cc"
    break;

  case 547: // simple_behavioral_stmt: attr lvalue OP_LE delay expr
#line 2737 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                     {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_ASSIGN_LE, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ())));
		SET_AST_NODE_LOC(node, yystack_[3].location, yystack_[0].location);
		append_attr(node, std::move(yystack_[4].value.as < al_t > ()));
	}
#line 4292 "verilog_parser.tab.cc"
    break;

  case 548: // simple_behavioral_stmt: attr lvalue asgn_binop delay expr
#line 2742 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		(void)extra->addAsgnBinopStmt(std::move(yystack_[4].value.as < al_t > ()), std::move(yystack_[3].value.as < ast_t > ()), yystack_[2].value.as < ast_node_type_t > (), std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 4300 "verilog_parser.tab.cc"
    break;

  case 549: // asgn_binop: TOK_BIT_OR_ASSIGN
#line 2747 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          { yylhs.value.as < ast_node_type_t > () = AST_BIT_OR; }
#line 4306 "verilog_parser.tab.cc"
    break;

  case 550: // asgn_binop: TOK_BIT_AND_ASSIGN
#line 2748 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                           { yylhs.value.as < ast_node_type_t > () = AST_BIT_AND; }
#line 4312 "verilog_parser.tab.cc"
    break;

  case 551: // asgn_binop: TOK_BIT_XOR_ASSIGN
#line 2749 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                           { yylhs.value.as < ast_node_type_t > () = AST_BIT_XOR; }
#line 4318 "verilog_parser.tab.cc"
    break;

  case 552: // asgn_binop: TOK_ADD_ASSIGN
#line 2750 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       { yylhs.value.as < ast_node_type_t > () = AST_ADD; }
#line 4324 "verilog_parser.tab.cc"
    break;

  case 553: // asgn_binop: TOK_SUB_ASSIGN
#line 2751 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       { yylhs.value.as < ast_node_type_t > () = AST_SUB; }
#line 4330 "verilog_parser.tab.cc"
    break;

  case 554: // asgn_binop: TOK_DIV_ASSIGN
#line 2752 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       { yylhs.value.as < ast_node_type_t > () = AST_DIV; }
#line 4336 "verilog_parser.tab.cc"
    break;

  case 555: // asgn_binop: TOK_MOD_ASSIGN
#line 2753 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       { yylhs.value.as < ast_node_type_t > () = AST_MOD; }
#line 4342 "verilog_parser.tab.cc"
    break;

  case 556: // asgn_binop: TOK_MUL_ASSIGN
#line 2754 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       { yylhs.value.as < ast_node_type_t > () = AST_MUL; }
#line 4348 "verilog_parser.tab.cc"
    break;

  case 557: // asgn_binop: TOK_SHL_ASSIGN
#line 2755 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       { yylhs.value.as < ast_node_type_t > () = AST_SHIFT_LEFT; }
#line 4354 "verilog_parser.tab.cc"
    break;

  case 558: // asgn_binop: TOK_SHR_ASSIGN
#line 2756 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       { yylhs.value.as < ast_node_type_t > () = AST_SHIFT_RIGHT; }
#line 4360 "verilog_parser.tab.cc"
    break;

  case 559: // asgn_binop: TOK_SSHL_ASSIGN
#line 2757 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < ast_node_type_t > () = AST_SHIFT_SLEFT; }
#line 4366 "verilog_parser.tab.cc"
    break;

  case 560: // asgn_binop: TOK_SSHR_ASSIGN
#line 2758 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        { yylhs.value.as < ast_node_type_t > () = AST_SHIFT_SRIGHT; }
#line 4372 "verilog_parser.tab.cc"
    break;

  case 561: // inc_or_dec_op: TOK_INCREMENT
#line 2763 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      { yylhs.value.as < ast_node_type_t > () = AST_ADD; }
#line 4378 "verilog_parser.tab.cc"
    break;

  case 562: // inc_or_dec_op: TOK_DECREMENT
#line 2764 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      { yylhs.value.as < ast_node_type_t > () = AST_SUB; }
#line 4384 "verilog_parser.tab.cc"
    break;

  case 563: // for_initialization: TOK_ID "'='" expr
#line 2767 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                           {
		auto ident = std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER);
		ident->str = *yystack_[2].value.as < string_t > ();
		auto node = std::make_unique<AstNode>(yylhs.location, AST_ASSIGN_EQ, std::move(ident), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(node.get(), yystack_[2].location, yystack_[0].location);
		extra->ast_stack.back()->children.push_back(std::move(node));
	}
#line 4396 "verilog_parser.tab.cc"
    break;

  case 564: // for_initialization: non_io_wire_type range TOK_ID
#line 2774 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                      {
		err_at_loc(yystack_[0].location, "For loop variable declaration is missing initialization!");
	}
#line 4404 "verilog_parser.tab.cc"
    break;

  case 565: // for_initialization: non_io_wire_type range TOK_ID "'='" expr
#line 2777 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                  {
		if (!mode->sv)
			err_at_loc(yystack_[1].location, "For loop inline variable declaration is only supported in SystemVerilog mode!");

		// loop variable declaration
		auto wire = std::move(yystack_[4].value.as < ast_t > ());
		auto range = checkRange(wire.get(), std::move(yystack_[3].value.as < ast_t > ()));
		SET_AST_NODE_LOC(wire.get(), yystack_[4].location, yystack_[2].location);
		SET_AST_NODE_LOC(range.get(), yystack_[3].location, yystack_[3].location);
		if (range != nullptr)
			wire->children.push_back(std::move(range));

		auto ident = std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER);
		ident->str = *yystack_[2].value.as < string_t > ();
		wire->str = *yystack_[2].value.as < string_t > ();

		AstNode *parent = extra->ast_stack.at(extra->ast_stack.size() - 2);
		auto& loop = parent->children.back();
		log_assert(extra->ast_stack.back() == loop.get());

		// loop variable initialization
		SET_AST_NODE_LOC(ident.get(), yystack_[2].location, yystack_[2].location);
		auto asgn = std::make_unique<AstNode>(yylhs.location, AST_ASSIGN_EQ, std::move(ident), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(asgn.get(), yystack_[2].location, yystack_[0].location);
		loop->children.push_back(std::move(asgn));

		// inject a wrapping block to declare the loop variable and
		// contain the current loop
		auto wrapper = std::make_unique<AstNode>(yylhs.location, AST_BLOCK);
		wrapper->str = "$fordecl_block$" + std::to_string(autoidx++);
		wrapper->children.push_back(std::move(wire));
		wrapper->children.push_back(std::move(loop));
		parent->children.back() = std::move(wrapper);
	}
#line 4443 "verilog_parser.tab.cc"
    break;

  case 575: // $@59: %empty
#line 2818 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                             {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_TCALL));
		node->str = *yystack_[0].value.as < string_t > ();
		append_attr(node, std::move(yystack_[1].value.as < al_t > ()));
	}
#line 4453 "verilog_parser.tab.cc"
    break;

  case 576: // behavioral_stmt: attr hierarchical_id $@59 opt_arg_list "';'"
#line 2822 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                  {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[3].location, yystack_[0].location);
		extra->ast_stack.pop_back();
	}
#line 4462 "verilog_parser.tab.cc"
    break;

  case 577: // $@60: %empty
#line 2826 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                           {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_TCALL));
		node->str = *yystack_[0].value.as < string_t > ();
		append_attr(node, std::move(yystack_[1].value.as < al_t > ()));
	}
#line 4472 "verilog_parser.tab.cc"
    break;

  case 578: // behavioral_stmt: attr TOK_MSG_TASKS $@60 opt_arg_list "';'"
#line 2830 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                  {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[3].location, yystack_[0].location);
		extra->ast_stack.pop_back();
	}
#line 4481 "verilog_parser.tab.cc"
    break;

  case 579: // $@61: %empty
#line 2834 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                       {
		extra->enterTypeScope();
	}
#line 4489 "verilog_parser.tab.cc"
    break;

  case 580: // $@62: %empty
#line 2836 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_BLOCK));
		append_attr(node, std::move(yystack_[3].value.as < al_t > ()));
		if (yystack_[0].value.as < string_t > () != nullptr)
			node->str = *yystack_[0].value.as < string_t > ();
	}
#line 4500 "verilog_parser.tab.cc"
    break;

  case 581: // behavioral_stmt: attr TOK_BEGIN $@61 opt_label $@62 behavioral_stmt_list TOK_END opt_label
#line 2841 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                 {
		extra->exitTypeScope();
		checkLabelsMatch(yystack_[0].location, "Begin label", yystack_[4].value.as < string_t > ().get(), yystack_[0].value.as < string_t > ().get());
		AstNode *node = extra->ast_stack.back();
		// In SystemVerilog, unnamed blocks with block item declarations
		// create an implicit hierarchy scope
		if (mode->sv && node->str.empty())
		    for (auto& child : node->children)
			if (child->type == AST_WIRE || child->type == AST_MEMORY || child->type == AST_PARAMETER
				|| child->type == AST_LOCALPARAM || child->type == AST_TYPEDEF) {
			    node->str = "$unnamed_block$" + std::to_string(autoidx++);
			    break;
			}
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[6].location, yystack_[0].location);
		extra->ast_stack.pop_back();
	}
#line 4521 "verilog_parser.tab.cc"
    break;

  case 582: // $@63: %empty
#line 2857 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_FOR));
		append_attr(node, std::move(yystack_[2].value.as < al_t > ()));
	}
#line 4530 "verilog_parser.tab.cc"
    break;

  case 583: // $@64: %empty
#line 2860 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                              {
		extra->ast_stack.back()->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 4538 "verilog_parser.tab.cc"
    break;

  case 584: // $@65: %empty
#line 2862 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                        {
		AstNode* block = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_BLOCK));
		block->str = "$for_loop$" + std::to_string(autoidx++);
	}
#line 4547 "verilog_parser.tab.cc"
    break;

  case 585: // behavioral_stmt: attr TOK_FOR "'('" $@63 for_initialization "';'" expr $@64 "';'" simple_behavioral_stmt "')'" $@65 behavioral_stmt
#line 2865 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[0].location, yystack_[0].location);
		extra->ast_stack.pop_back();
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[11].location, yystack_[0].location);
		extra->ast_stack.pop_back();
	}
#line 4558 "verilog_parser.tab.cc"
    break;

  case 586: // $@66: %empty
#line 2871 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                  {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_WHILE));
		append_attr(node, std::move(yystack_[4].value.as < al_t > ()));
		auto block_owned = std::make_unique<AstNode>(yylhs.location, AST_BLOCK);
		auto* block = block_owned.get();
		extra->ast_stack.back()->children.push_back(std::move(yystack_[1].value.as < ast_t > ()));
		extra->ast_stack.back()->children.push_back(std::move(block_owned));
		extra->ast_stack.push_back(block);
	}
#line 4572 "verilog_parser.tab.cc"
    break;

  case 587: // behavioral_stmt: attr TOK_WHILE "'('" expr "')'" $@66 behavioral_stmt
#line 2879 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[0].location, yystack_[0].location);
		extra->ast_stack.pop_back();
		extra->ast_stack.pop_back();
	}
#line 4582 "verilog_parser.tab.cc"
    break;

  case 588: // $@67: %empty
#line 2884 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                   {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_REPEAT));
		append_attr(node, std::move(yystack_[4].value.as < al_t > ()));
		auto block_owned = std::make_unique<AstNode>(yylhs.location, AST_BLOCK);
		auto* block = block_owned.get();
		extra->ast_stack.back()->children.push_back(std::move(yystack_[1].value.as < ast_t > ()));
		extra->ast_stack.back()->children.push_back(std::move(block_owned));
		extra->ast_stack.push_back(block);
	}
#line 4596 "verilog_parser.tab.cc"
    break;

  case 589: // behavioral_stmt: attr TOK_REPEAT "'('" expr "')'" $@67 behavioral_stmt
#line 2892 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[0].location, yystack_[0].location);
		extra->ast_stack.pop_back();
		extra->ast_stack.pop_back();
	}
#line 4606 "verilog_parser.tab.cc"
    break;

  case 590: // $@68: %empty
#line 2897 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                  {
		std::unique_ptr<AstNode> node_owned;
		AstNode* node = nullptr;
		AstNode *context = extra->ast_stack.back();
		bool patch_block_on_stack = false;
		if (context && context->type == AST_BLOCK && context->get_bool_attribute(ID::promoted_if)) {
			AstNode *outer = extra->ast_stack[extra->ast_stack.size() - 2];
			log_assert (outer && outer->type == AST_CASE);
			if (outer->get_bool_attribute(ID::parallel_case)) {
				// parallel "else if": append condition to outer "if"
				node = outer;
				log_assert (node->children.size());
				node->children.pop_back();
				// `context` has been killed as a grandchild of `outer`
				// we have to undangle it from the stack
				patch_block_on_stack = true;
			} else if (outer->get_bool_attribute(ID::full_case))
				(*yystack_[4].value.as < al_t > ())[ID::full_case] = AstNode::mkconst_int(yylhs.location, 1, false);
		}
		auto expr = std::make_unique<AstNode>(yylhs.location, AST_REDUCE_BOOL, std::move(yystack_[1].value.as < ast_t > ()));
		if (!node) {
			// not parallel "else if": begin new construction
			node_owned = std::make_unique<AstNode>(yylhs.location, AST_CASE);
			node = node_owned.get();
			append_attr(node, std::move(yystack_[4].value.as < al_t > ()));
			node->children.push_back(node->get_bool_attribute(ID::parallel_case) ? AstNode::mkconst_int(yylhs.location, 1, false, 1) : expr->clone());
			extra->ast_stack.back()->children.push_back(std::move(node_owned));
		}
		auto block_owned = std::make_unique<AstNode>(yylhs.location, AST_BLOCK);
		auto* block = block_owned.get();
		auto cond_owned = std::make_unique<AstNode>(yylhs.location, AST_COND, node->get_bool_attribute(ID::parallel_case) ? std::move(expr) : AstNode::mkconst_int(yylhs.location, 1, false, 1), std::move(block_owned));
		SET_AST_NODE_LOC(cond_owned.get(), yystack_[1].location, yystack_[1].location);
		node->children.push_back(std::move(cond_owned));
		// Double it and give it to the next person
		if (patch_block_on_stack)
			extra->ast_stack.back() = block;
		extra->ast_stack.push_back(node);
		extra->ast_stack.push_back(block);
	}
#line 4650 "verilog_parser.tab.cc"
    break;

  case 591: // $@69: %empty
#line 2935 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[0].location, yystack_[0].location);
	}
#line 4658 "verilog_parser.tab.cc"
    break;

  case 592: // behavioral_stmt: if_attr TOK_IF "'('" expr "')'" $@68 behavioral_stmt $@69 optional_else
#line 2937 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        {
		extra->ast_stack.pop_back();
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[7].location, yystack_[0].location);
		extra->ast_stack.pop_back();
	}
#line 4668 "verilog_parser.tab.cc"
    break;

  case 593: // $@70: %empty
#line 2942 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                       {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_CASE, std::move(yystack_[1].value.as < ast_t > ())));
		append_attr(node, std::move(yystack_[4].value.as < al_t > ()));
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[1].location, yystack_[1].location);
	}
#line 4678 "verilog_parser.tab.cc"
    break;

  case 594: // behavioral_stmt: case_attr case_type "'('" expr "')'" $@70 opt_synopsys_attr case_body TOK_ENDCASE
#line 2946 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                  {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[7].location, yystack_[0].location);
		extra->case_type_stack.pop_back();
		extra->ast_stack.pop_back();
	}
#line 4688 "verilog_parser.tab.cc"
    break;

  case 595: // if_attr: attr
#line 2953 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		yylhs.value.as < al_t > () = std::move(yystack_[0].value.as < al_t > ());
	}
#line 4696 "verilog_parser.tab.cc"
    break;

  case 596: // if_attr: attr TOK_UNIQUE0
#line 2956 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                         {
		AstNode *context = extra->ast_stack.back();
		if (context && context->type == AST_BLOCK && context->get_bool_attribute(ID::promoted_if))
			err_at_loc(yystack_[0].location, "unique0 keyword cannot be used for 'else if' branch.");
		(*yystack_[1].value.as < al_t > ())[ID::parallel_case] = AstNode::mkconst_int(yylhs.location, 1, false);
		yylhs.value.as < al_t > () = std::move(yystack_[1].value.as < al_t > ());
	}
#line 4708 "verilog_parser.tab.cc"
    break;

  case 597: // if_attr: attr TOK_PRIORITY
#line 2963 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		AstNode *context = extra->ast_stack.back();
		if (context && context->type == AST_BLOCK && context->get_bool_attribute(ID::promoted_if))
			err_at_loc(yystack_[0].location, "priority keyword cannot be used for 'else if' branch.");
		(*yystack_[1].value.as < al_t > ())[ID::full_case] = AstNode::mkconst_int(yylhs.location, 1, false);
		yylhs.value.as < al_t > () = std::move(yystack_[1].value.as < al_t > ());
	}
#line 4720 "verilog_parser.tab.cc"
    break;

  case 598: // if_attr: attr TOK_UNIQUE
#line 2970 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        {
		AstNode *context = extra->ast_stack.back();
		if (context && context->type == AST_BLOCK && context->get_bool_attribute(ID::promoted_if))
			err_at_loc(yystack_[0].location, "unique keyword cannot be used for 'else if' branch.");
		(*yystack_[1].value.as < al_t > ())[ID::full_case] = AstNode::mkconst_int(yylhs.location, 1, false);
		(*yystack_[1].value.as < al_t > ())[ID::parallel_case] = AstNode::mkconst_int(yylhs.location, 1, false);
		yylhs.value.as < al_t > () = std::move(yystack_[1].value.as < al_t > ());
	}
#line 4733 "verilog_parser.tab.cc"
    break;

  case 599: // case_attr: attr
#line 2980 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		yylhs.value.as < al_t > () = std::move(yystack_[0].value.as < al_t > ());
	}
#line 4741 "verilog_parser.tab.cc"
    break;

  case 600: // case_attr: attr TOK_UNIQUE0
#line 2983 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                         {
		(*yystack_[1].value.as < al_t > ())[ID::parallel_case] = AstNode::mkconst_int(yylhs.location, 1, false);
		yylhs.value.as < al_t > () = std::move(yystack_[1].value.as < al_t > ());
	}
#line 4750 "verilog_parser.tab.cc"
    break;

  case 601: // case_attr: attr TOK_PRIORITY
#line 2987 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		(*yystack_[1].value.as < al_t > ())[ID::full_case] = AstNode::mkconst_int(yylhs.location, 1, false);
		yylhs.value.as < al_t > () = std::move(yystack_[1].value.as < al_t > ());
	}
#line 4759 "verilog_parser.tab.cc"
    break;

  case 602: // case_attr: attr TOK_UNIQUE
#line 2991 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        {
		(*yystack_[1].value.as < al_t > ())[ID::full_case] = AstNode::mkconst_int(yylhs.location, 1, false);
		(*yystack_[1].value.as < al_t > ())[ID::parallel_case] = AstNode::mkconst_int(yylhs.location, 1, false);
		yylhs.value.as < al_t > () = std::move(yystack_[1].value.as < al_t > ());
	}
#line 4769 "verilog_parser.tab.cc"
    break;

  case 603: // case_type: TOK_CASE
#line 2998 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {
		extra->case_type_stack.push_back(0);
	}
#line 4777 "verilog_parser.tab.cc"
    break;

  case 604: // case_type: TOK_CASEX
#line 3001 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  {
		extra->case_type_stack.push_back('x');
	}
#line 4785 "verilog_parser.tab.cc"
    break;

  case 605: // case_type: TOK_CASEZ
#line 3004 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  {
		extra->case_type_stack.push_back('z');
	}
#line 4793 "verilog_parser.tab.cc"
    break;

  case 606: // opt_synopsys_attr: opt_synopsys_attr TOK_SYNOPSYS_FULL_CASE
#line 3009 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                 {
		if (extra->ast_stack.back()->attributes.count(ID::full_case) == 0)
			extra->ast_stack.back()->attributes[ID::full_case] = AstNode::mkconst_int(yylhs.location, 1, false);
	}
#line 4802 "verilog_parser.tab.cc"
    break;

  case 607: // opt_synopsys_attr: opt_synopsys_attr TOK_SYNOPSYS_PARALLEL_CASE
#line 3013 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                     {
		if (extra->ast_stack.back()->attributes.count(ID::parallel_case) == 0)
			extra->ast_stack.back()->attributes[ID::parallel_case] = AstNode::mkconst_int(yylhs.location, 1, false);
	}
#line 4811 "verilog_parser.tab.cc"
    break;

  case 611: // $@71: %empty
#line 3024 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                 {
		extra->ast_stack.pop_back();
		auto block_owned = std::make_unique<AstNode>(yylhs.location, AST_BLOCK);
		auto* block = block_owned.get();
		block->attributes[ID::promoted_if] = AstNode::mkconst_int(yylhs.location, 1, false);
		AstNode* cond = extra->saveChild(
			std::make_unique<AstNode>(yylhs.location, AST_COND,
				std::make_unique<AstNode>(yylhs.location, AST_DEFAULT),
				std::move(block_owned)));
		extra->ast_stack.push_back(block);
		SET_AST_NODE_LOC(cond, yystack_[0].location, yystack_[0].location);
	}
#line 4828 "verilog_parser.tab.cc"
    break;

  case 612: // optional_else: TOK_ELSE $@71 behavioral_stmt
#line 3035 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[0].location, yystack_[0].location);
	}
#line 4836 "verilog_parser.tab.cc"
    break;

  case 616: // $@72: %empty
#line 3045 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
        {
		(void)extra->pushChild(std::make_unique<AstNode>(
				yylhs.location,
				extra->case_type_stack.size() && extra->case_type_stack.back() == 'x' ? AST_CONDX :
				extra->case_type_stack.size() && extra->case_type_stack.back() == 'z' ? AST_CONDZ : AST_COND));
	}
#line 4847 "verilog_parser.tab.cc"
    break;

  case 617: // $@73: %empty
#line 3050 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		(void)extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_BLOCK));
		extra->case_type_stack.push_back(0);
	}
#line 4856 "verilog_parser.tab.cc"
    break;

  case 618: // case_item: $@72 case_select $@73 behavioral_stmt
#line 3053 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                          {
		extra->case_type_stack.pop_back();
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[0].location, yystack_[0].location);
		extra->ast_stack.pop_back();
		extra->ast_stack.pop_back();
	}
#line 4867 "verilog_parser.tab.cc"
    break;

  case 621: // $@74: %empty
#line 3065 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
        {
		(void)extra->pushChild(std::make_unique<AstNode>(
				yylhs.location,
				extra->case_type_stack.size() && extra->case_type_stack.back() == 'x' ? AST_CONDX :
				extra->case_type_stack.size() && extra->case_type_stack.back() == 'z' ? AST_CONDZ : AST_COND));
	}
#line 4878 "verilog_parser.tab.cc"
    break;

  case 622: // $@75: %empty
#line 3070 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		extra->case_type_stack.push_back(0);
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[0].location, yystack_[0].location);
	}
#line 4887 "verilog_parser.tab.cc"
    break;

  case 623: // gen_case_item: $@74 case_select $@75 gen_stmt_block
#line 3073 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                         {
		extra->case_type_stack.pop_back();
		extra->ast_stack.pop_back();
	}
#line 4896 "verilog_parser.tab.cc"
    break;

  case 626: // case_expr_list: TOK_DEFAULT
#line 3083 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_DEFAULT));
		SET_AST_NODE_LOC(node, yystack_[0].location, yystack_[0].location);
	}
#line 4905 "verilog_parser.tab.cc"
    break;

  case 627: // case_expr_list: TOK_SVA_LABEL
#line 3087 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER));
		node->str = *yystack_[0].value.as < string_t > ();
		SET_AST_NODE_LOC(node, yystack_[0].location, yystack_[0].location);
	}
#line 4915 "verilog_parser.tab.cc"
    break;

  case 628: // case_expr_list: expr
#line 3092 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		extra->ast_stack.back()->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 4923 "verilog_parser.tab.cc"
    break;

  case 629: // case_expr_list: case_expr_list "','" expr
#line 3095 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                      {
		extra->ast_stack.back()->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 4931 "verilog_parser.tab.cc"
    break;

  case 630: // rvalue: hierarchical_id "'['" expr "']'" "'.'" rvalue
#line 3100 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                              {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_PREFIX, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		yylhs.value.as < ast_t > ()->str = *yystack_[5].value.as < string_t > ();
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[5].location, yystack_[0].location);
	}
#line 4941 "verilog_parser.tab.cc"
    break;

  case 631: // rvalue: hierarchical_id range
#line 3105 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                              {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER, std::move(yystack_[0].value.as < ast_t > ()));
		yylhs.value.as < ast_t > ()->str = *yystack_[1].value.as < string_t > ();
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[1].location, yystack_[1].location);
		if (yystack_[0].value.as < ast_t > () == nullptr && (yylhs.value.as < ast_t > ()->str == "\\$initstate" ||
				yylhs.value.as < ast_t > ()->str == "\\$anyconst" || yylhs.value.as < ast_t > ()->str == "\\$anyseq" ||
				yylhs.value.as < ast_t > ()->str == "\\$allconst" || yylhs.value.as < ast_t > ()->str == "\\$allseq"))
			yylhs.value.as < ast_t > ()->type = AST_FCALL;
	}
#line 4955 "verilog_parser.tab.cc"
    break;

  case 632: // rvalue: hierarchical_id non_opt_multirange
#line 3114 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                           {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER, std::move(yystack_[0].value.as < ast_t > ()));
		yylhs.value.as < ast_t > ()->str = *yystack_[1].value.as < string_t > ();
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[1].location, yystack_[1].location);
	}
#line 4965 "verilog_parser.tab.cc"
    break;

  case 633: // lvalue: rvalue
#line 3121 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ());
	}
#line 4973 "verilog_parser.tab.cc"
    break;

  case 634: // lvalue: "'{'" lvalue_concat_list "'}'"
#line 3124 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                               {
		yylhs.value.as < ast_t > () = std::move(yystack_[1].value.as < ast_t > ());
	}
#line 4981 "verilog_parser.tab.cc"
    break;

  case 635: // lvalue_concat_list: expr
#line 3129 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_CONCAT);
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 4990 "verilog_parser.tab.cc"
    break;

  case 636: // lvalue_concat_list: expr "','" lvalue_concat_list
#line 3133 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ());
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[2].value.as < ast_t > ()));
	}
#line 4999 "verilog_parser.tab.cc"
    break;

  case 643: // single_arg: expr
#line 3151 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		extra->ast_stack.back()->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 5007 "verilog_parser.tab.cc"
    break;

  case 650: // genvar_identifier: TOK_ID
#line 3165 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER);
		yylhs.value.as < ast_t > ()->str = *yystack_[0].value.as < string_t > ();
	}
#line 5016 "verilog_parser.tab.cc"
    break;

  case 651: // genvar_initialization: TOK_GENVAR genvar_identifier
#line 3171 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                     {
		err_at_loc(yystack_[0].location, "Generate for loop variable declaration is missing initialization!");
	}
#line 5024 "verilog_parser.tab.cc"
    break;

  case 652: // genvar_initialization: TOK_GENVAR genvar_identifier "'='" expr
#line 3174 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                 {
		if (!mode->sv)
			err_at_loc(yystack_[1].location, "Generate for loop inline variable declaration is only supported in SystemVerilog mode!");
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_GENVAR));
		node->is_reg = true;
		node->is_signed = true;
		node->range_left = 31;
		node->range_right = 0;
		node->str = yystack_[2].value.as < ast_t > ()->str;
		node->children.push_back(checkRange(node, nullptr));
		SET_AST_NODE_LOC(node, yystack_[3].location, yystack_[0].location);
		node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_ASSIGN_EQ, std::move(yystack_[2].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ())));
		SET_AST_NODE_LOC(node, yystack_[3].location, yystack_[0].location);
	}
#line 5043 "verilog_parser.tab.cc"
    break;

  case 653: // genvar_initialization: genvar_identifier "'='" expr
#line 3188 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                      {
		AstNode* node = extra->saveChild(std::make_unique<AstNode>(yylhs.location, AST_ASSIGN_EQ, std::move(yystack_[2].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ())));
		SET_AST_NODE_LOC(node, yystack_[2].location, yystack_[0].location);
	}
#line 5052 "verilog_parser.tab.cc"
    break;

  case 654: // $@76: %empty
#line 3195 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                           {
		(void)extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_GENFOR));
	}
#line 5060 "verilog_parser.tab.cc"
    break;

  case 655: // $@77: %empty
#line 3197 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                 {
		extra->ast_stack.back()->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 5068 "verilog_parser.tab.cc"
    break;

  case 656: // gen_stmt: TOK_FOR "'('" $@76 genvar_initialization "';'" expr $@77 "';'" simple_behavioral_stmt "')'" gen_stmt_block
#line 3199 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                       {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[10].location, yystack_[0].location);
		extra->rewriteGenForDeclInit(extra->ast_stack.back());
		extra->ast_stack.pop_back();
	}
#line 5078 "verilog_parser.tab.cc"
    break;

  case 657: // $@78: %empty
#line 3204 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		(void)extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_GENIF));
		extra->ast_stack.back()->children.push_back(std::move(yystack_[1].value.as < ast_t > ()));
	}
#line 5087 "verilog_parser.tab.cc"
    break;

  case 658: // gen_stmt: TOK_IF "'('" expr "')'" $@78 gen_stmt_block opt_gen_else
#line 3207 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                      {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[6].location, yystack_[0].location);
		extra->ast_stack.pop_back();
	}
#line 5096 "verilog_parser.tab.cc"
    break;

  case 659: // $@79: %empty
#line 3211 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                             {
		(void)extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_GENCASE, std::move(yystack_[1].value.as < ast_t > ())));
	}
#line 5104 "verilog_parser.tab.cc"
    break;

  case 660: // gen_stmt: case_type "'('" expr "')'" $@79 gen_case_body TOK_ENDCASE
#line 3213 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                    {
		extra->case_type_stack.pop_back();
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[6].location, yystack_[0].location);
		extra->ast_stack.pop_back();
	}
#line 5114 "verilog_parser.tab.cc"
    break;

  case 661: // $@80: %empty
#line 3218 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                      {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_TECALL));
		node->str = *yystack_[0].value.as < string_t > ();
	}
#line 5123 "verilog_parser.tab.cc"
    break;

  case 662: // gen_stmt: TOK_MSG_TASKS $@80 opt_arg_list "';'"
#line 3221 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                  {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[3].location, yystack_[1].location);
		extra->ast_stack.pop_back();
	}
#line 5132 "verilog_parser.tab.cc"
    break;

  case 663: // $@81: %empty
#line 3227 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                  {
		extra->enterTypeScope();
	}
#line 5140 "verilog_parser.tab.cc"
    break;

  case 664: // $@82: %empty
#line 3229 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    {
		AstNode* node = extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_GENBLOCK));
		node->str = yystack_[0].value.as < string_t > () ? *yystack_[0].value.as < string_t > () : std::string();
	}
#line 5149 "verilog_parser.tab.cc"
    break;

  case 665: // gen_block: TOK_BEGIN $@81 opt_label $@82 module_gen_body TOK_END opt_label
#line 3232 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		extra->exitTypeScope();
		checkLabelsMatch(yystack_[0].location, "Begin label", yystack_[4].value.as < string_t > ().get(), yystack_[0].value.as < string_t > ().get());
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[6].location, yystack_[0].location);
		extra->ast_stack.pop_back();
	}
#line 5160 "verilog_parser.tab.cc"
    break;

  case 666: // $@83: %empty
#line 3241 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
        {
		(void)extra->pushChild(std::make_unique<AstNode>(yylhs.location, AST_GENBLOCK));
	}
#line 5168 "verilog_parser.tab.cc"
    break;

  case 667: // gen_stmt_block: $@83 gen_stmt_or_module_body_stmt
#line 3243 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                       {
		SET_AST_NODE_LOC(extra->ast_stack.back(), yystack_[0].location, yystack_[0].location);
		extra->ast_stack.pop_back();
	}
#line 5177 "verilog_parser.tab.cc"
    break;

  case 671: // expr: basic_expr
#line 3252 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                   {
		yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ());
	}
#line 5185 "verilog_parser.tab.cc"
    break;

  case 672: // expr: basic_expr "'?'" attr expr "':'" expr
#line 3255 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                  {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_TERNARY);
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[5].value.as < ast_t > ()));
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[2].value.as < ast_t > ()));
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[5].location, yylhs.location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[3].value.as < al_t > ()));
	}
#line 5198 "verilog_parser.tab.cc"
    break;

  case 673: // expr: inc_or_dec_op attr rvalue
#line 3263 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                  {
		yylhs.value.as < ast_t > () = extra->addIncOrDecExpr(std::move(yystack_[0].value.as < ast_t > ()), std::move(yystack_[1].value.as < al_t > ()), yystack_[2].value.as < ast_node_type_t > (), location_range(yystack_[2].location, yystack_[0].location), false, mode->sv);
	}
#line 5206 "verilog_parser.tab.cc"
    break;

  case 674: // expr: rvalue inc_or_dec_op
#line 3268 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                             {
		yylhs.value.as < ast_t > () = extra->addIncOrDecExpr(std::move(yystack_[1].value.as < ast_t > ()), nullptr, yystack_[0].value.as < ast_node_type_t > (), location_range(yystack_[1].location, yystack_[0].location), true, mode->sv);
	}
#line 5214 "verilog_parser.tab.cc"
    break;

  case 675: // basic_expr: rvalue
#line 3273 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
               {
		yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ());
	}
#line 5222 "verilog_parser.tab.cc"
    break;

  case 676: // basic_expr: "'('" expr "')'" integral_number
#line 3276 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                   {
		if (yystack_[0].value.as < string_t > ()->compare(0, 1, "'") != 0)
			err_at_loc(yystack_[0].location, "Cast operation must be applied on sized constants e.g. (<expr>)<constval> , while %s is not a sized constant.", *yystack_[0].value.as < string_t > ());
		ConstParser p{yystack_[0].location};
		auto val = p.const2ast(*yystack_[0].value.as < string_t > (), extra->case_type_stack.size() == 0 ? 0 : extra->case_type_stack.back(), !mode->lib);
		if (val == nullptr)
			log_error("Value conversion failed: `%s'\n", *yystack_[0].value.as < string_t > ());
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_TO_BITS, std::move(yystack_[2].value.as < ast_t > ()), std::move(val));
	}
#line 5236 "verilog_parser.tab.cc"
    break;

  case 677: // basic_expr: hierarchical_id integral_number
#line 3285 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                        {
		if (yystack_[0].value.as < string_t > ()->compare(0, 1, "'") != 0)
			err_at_loc(yystack_[0].location, "Cast operation must be applied on sized constants, e.g. <ID>\'d0, while %s is not a sized constant.", *yystack_[0].value.as < string_t > ());
		auto bits = std::make_unique<AstNode>(yylhs.location, AST_IDENTIFIER);
		bits->str = *yystack_[1].value.as < string_t > ();
		SET_AST_NODE_LOC(bits.get(), yystack_[1].location, yystack_[1].location);
		ConstParser p{yystack_[0].location};
		auto val = p.const2ast(*yystack_[0].value.as < string_t > (), extra->case_type_stack.size() == 0 ? 0 : extra->case_type_stack.back(), !mode->lib);
		SET_AST_NODE_LOC(val.get(), yystack_[0].location, yystack_[0].location);
		if (val == nullptr)
			log_error("Value conversion failed: `%s'\n", *yystack_[0].value.as < string_t > ());
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_TO_BITS, std::move(bits), std::move(val));
	}
#line 5254 "verilog_parser.tab.cc"
    break;

  case 678: // basic_expr: integral_number
#line 3298 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                        {
		ConstParser p{yystack_[0].location};
		yylhs.value.as < ast_t > () = p.const2ast(*yystack_[0].value.as < string_t > (), extra->case_type_stack.size() == 0 ? 0 : extra->case_type_stack.back(), !mode->lib);
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[0].location, yystack_[0].location);
		if (yylhs.value.as < ast_t > () == nullptr)
			log_error("Value conversion failed: `%s'\n", *yystack_[0].value.as < string_t > ());
	}
#line 5266 "verilog_parser.tab.cc"
    break;

  case 679: // basic_expr: TOK_REALVAL
#line 3305 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                    {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_REALVALUE);
		char *p = (char*)malloc(GetSize(*yystack_[0].value.as < string_t > ()) + 1), *q;
		for (int i = 0, j = 0; j < GetSize(*yystack_[0].value.as < string_t > ()); j++)
			if ((*yystack_[0].value.as < string_t > ())[j] != '_')
				p[i++] = (*yystack_[0].value.as < string_t > ())[j], p[i] = 0;
		yylhs.value.as < ast_t > ()->realvalue = strtod(p, &q);
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[0].location, yystack_[0].location);
		log_assert(*q == 0);
		free(p);
	}
#line 5282 "verilog_parser.tab.cc"
    break;

  case 680: // basic_expr: TOK_STRING
#line 3316 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                   {
		yylhs.value.as < ast_t > () = AstNode::mkconst_str(yystack_[0].location, *yystack_[0].value.as < string_t > ());
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[0].location, yystack_[0].location);
	}
#line 5291 "verilog_parser.tab.cc"
    break;

  case 681: // @84: %empty
#line 3320 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                    {
		// Here we use "Typed Midrule Actions".
		// https://www.gnu.org/software/bison/manual/html_node/Typed-Midrule-Actions.html
		auto fcall = std::make_unique<AstNode>(yystack_[1].location, AST_FCALL);
		AstNode *fcall_node = fcall.get();
		fcall_node->str = *yystack_[1].value.as < string_t > ();
		extra->ast_stack.push_back(fcall_node);
		SET_AST_NODE_LOC(fcall_node, yystack_[1].location, yystack_[1].location);
		append_attr(fcall_node, std::move(yystack_[0].value.as < al_t > ()));
		yylhs.value.as < ast_t > () = std::move(fcall);
	}
#line 5307 "verilog_parser.tab.cc"
    break;

  case 682: // basic_expr: hierarchical_id attr @84 "'('" arg_list optional_comma "')'"
#line 3330 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                        {
		log_assert(yystack_[4].value.as < ast_t > () != nullptr);
		yylhs.value.as < ast_t > () = std::move(yystack_[4].value.as < ast_t > ());
		extra->ast_stack.pop_back();
	}
#line 5317 "verilog_parser.tab.cc"
    break;

  case 683: // basic_expr: TOK_TO_SIGNED attr "'('" expr "')'"
#line 3335 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                      {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_TO_SIGNED, std::move(yystack_[1].value.as < ast_t > ()));
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[3].value.as < al_t > ()));
	}
#line 5326 "verilog_parser.tab.cc"
    break;

  case 684: // basic_expr: TOK_TO_UNSIGNED attr "'('" expr "')'"
#line 3339 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                        {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_TO_UNSIGNED, std::move(yystack_[1].value.as < ast_t > ()));
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[3].value.as < al_t > ()));
	}
#line 5335 "verilog_parser.tab.cc"
    break;

  case 685: // basic_expr: "'('" expr "')'"
#line 3343 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                   {
		yylhs.value.as < ast_t > () = std::move(yystack_[1].value.as < ast_t > ());
	}
#line 5343 "verilog_parser.tab.cc"
    break;

  case 686: // basic_expr: "'('" expr "':'" expr "':'" expr "')'"
#line 3346 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                             {
		yylhs.value.as < ast_t > () = std::move(yystack_[3].value.as < ast_t > ());
	}
#line 5351 "verilog_parser.tab.cc"
    break;

  case 687: // basic_expr: "'{'" concat_list "'}'"
#line 3349 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                        {
		yylhs.value.as < ast_t > () = std::move(yystack_[1].value.as < ast_t > ());
	}
#line 5359 "verilog_parser.tab.cc"
    break;

  case 688: // basic_expr: OP_CAST "'{'" assignment_pattern_list optional_comma "'}'"
#line 3352 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                           {
		if (!mode->sv)
			err_at_loc(yystack_[4].location, "Assignment patterns are only supported in SystemVerilog mode.");
		yylhs.value.as < ast_t > () = std::move(yystack_[2].value.as < ast_t > ());
	}
#line 5369 "verilog_parser.tab.cc"
    break;

  case 689: // basic_expr: "'{'" expr "'{'" concat_list "'}'" "'}'"
#line 3357 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                                 {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_REPLICATE, std::move(yystack_[4].value.as < ast_t > ()), std::move(yystack_[2].value.as < ast_t > ()));
	}
#line 5377 "verilog_parser.tab.cc"
    break;

  case 690: // basic_expr: "'~'" attr basic_expr
#line 3360 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                  {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_BIT_NOT, std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[2].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5387 "verilog_parser.tab.cc"
    break;

  case 691: // basic_expr: basic_expr "'&'" attr basic_expr
#line 3365 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                           {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_BIT_AND, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5397 "verilog_parser.tab.cc"
    break;

  case 692: // basic_expr: basic_expr OP_NAND attr basic_expr
#line 3370 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                           {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_BIT_NOT, std::make_unique<AstNode>(yylhs.location, AST_BIT_AND, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ())));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5407 "verilog_parser.tab.cc"
    break;

  case 693: // basic_expr: basic_expr "'|'" attr basic_expr
#line 3375 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_BIT_OR, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5417 "verilog_parser.tab.cc"
    break;

  case 694: // basic_expr: basic_expr OP_NOR attr basic_expr
#line 3380 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_BIT_NOT, std::make_unique<AstNode>(yylhs.location, AST_BIT_OR, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ())));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5427 "verilog_parser.tab.cc"
    break;

  case 695: // basic_expr: basic_expr "'^'" attr basic_expr
#line 3385 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                             {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_BIT_XOR, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5437 "verilog_parser.tab.cc"
    break;

  case 696: // basic_expr: basic_expr OP_XNOR attr basic_expr
#line 3390 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                           {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_BIT_XNOR, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5447 "verilog_parser.tab.cc"
    break;

  case 697: // basic_expr: "'&'" attr basic_expr
#line 3395 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_REDUCE_AND, std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[2].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5457 "verilog_parser.tab.cc"
    break;

  case 698: // basic_expr: OP_NAND attr basic_expr
#line 3400 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_REDUCE_AND, std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[2].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_LOGIC_NOT, std::move(yylhs.value.as < ast_t > ()));
	}
#line 5468 "verilog_parser.tab.cc"
    break;

  case 699: // basic_expr: "'|'" attr basic_expr
#line 3406 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                 {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_REDUCE_OR, std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[2].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5478 "verilog_parser.tab.cc"
    break;

  case 700: // basic_expr: OP_NOR attr basic_expr
#line 3411 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                               {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_REDUCE_OR, std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[2].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_LOGIC_NOT, std::move(yylhs.value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[2].location, yystack_[0].location);
	}
#line 5490 "verilog_parser.tab.cc"
    break;

  case 701: // basic_expr: "'^'" attr basic_expr
#line 3418 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                  {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_REDUCE_XOR, std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[2].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5500 "verilog_parser.tab.cc"
    break;

  case 702: // basic_expr: OP_XNOR attr basic_expr
#line 3423 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_REDUCE_XNOR, std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[2].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5510 "verilog_parser.tab.cc"
    break;

  case 703: // basic_expr: basic_expr OP_SHL attr basic_expr
#line 3428 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_SHIFT_LEFT, std::move(yystack_[3].value.as < ast_t > ()), std::make_unique<AstNode>(yylhs.location, AST_TO_UNSIGNED, std::move(yystack_[0].value.as < ast_t > ())));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5520 "verilog_parser.tab.cc"
    break;

  case 704: // basic_expr: basic_expr OP_SHR attr basic_expr
#line 3433 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_SHIFT_RIGHT, std::move(yystack_[3].value.as < ast_t > ()), std::make_unique<AstNode>(yylhs.location, AST_TO_UNSIGNED, std::move(yystack_[0].value.as < ast_t > ())));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5530 "verilog_parser.tab.cc"
    break;

  case 705: // basic_expr: basic_expr OP_SSHL attr basic_expr
#line 3438 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                           {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_SHIFT_SLEFT, std::move(yystack_[3].value.as < ast_t > ()), std::make_unique<AstNode>(yylhs.location, AST_TO_UNSIGNED, std::move(yystack_[0].value.as < ast_t > ())));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5540 "verilog_parser.tab.cc"
    break;

  case 706: // basic_expr: basic_expr OP_SSHR attr basic_expr
#line 3443 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                           {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_SHIFT_SRIGHT, std::move(yystack_[3].value.as < ast_t > ()), std::make_unique<AstNode>(yylhs.location, AST_TO_UNSIGNED, std::move(yystack_[0].value.as < ast_t > ())));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5550 "verilog_parser.tab.cc"
    break;

  case 707: // basic_expr: basic_expr "'<'" attr basic_expr
#line 3448 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_LT, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5560 "verilog_parser.tab.cc"
    break;

  case 708: // basic_expr: basic_expr OP_LE attr basic_expr
#line 3453 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                         {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_LE, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5570 "verilog_parser.tab.cc"
    break;

  case 709: // basic_expr: basic_expr OP_EQ attr basic_expr
#line 3458 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                         {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_EQ, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5580 "verilog_parser.tab.cc"
    break;

  case 710: // basic_expr: basic_expr OP_NE attr basic_expr
#line 3463 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                         {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_NE, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5590 "verilog_parser.tab.cc"
    break;

  case 711: // basic_expr: basic_expr OP_EQX attr basic_expr
#line 3468 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_EQX, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5600 "verilog_parser.tab.cc"
    break;

  case 712: // basic_expr: basic_expr OP_NEX attr basic_expr
#line 3473 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_NEX, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5610 "verilog_parser.tab.cc"
    break;

  case 713: // basic_expr: basic_expr OP_GE attr basic_expr
#line 3478 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                         {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_GE, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5620 "verilog_parser.tab.cc"
    break;

  case 714: // basic_expr: basic_expr "'>'" attr basic_expr
#line 3483 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_GT, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5630 "verilog_parser.tab.cc"
    break;

  case 715: // basic_expr: basic_expr "'+'" attr basic_expr
#line 3488 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_ADD, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5640 "verilog_parser.tab.cc"
    break;

  case 716: // basic_expr: basic_expr "'-'" attr basic_expr
#line 3493 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                             {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_SUB, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5650 "verilog_parser.tab.cc"
    break;

  case 717: // basic_expr: basic_expr "'*'" attr basic_expr
#line 3498 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                             {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_MUL, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5660 "verilog_parser.tab.cc"
    break;

  case 718: // basic_expr: basic_expr "'/'" attr basic_expr
#line 3503 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                             {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_DIV, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5670 "verilog_parser.tab.cc"
    break;

  case 719: // basic_expr: basic_expr "'%'" attr basic_expr
#line 3508 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                            {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_MOD, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5680 "verilog_parser.tab.cc"
    break;

  case 720: // basic_expr: basic_expr OP_POW attr basic_expr
#line 3513 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_POW, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5690 "verilog_parser.tab.cc"
    break;

  case 721: // basic_expr: "'+'" attr basic_expr
#line 3518 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                 {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_POS, std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[2].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5700 "verilog_parser.tab.cc"
    break;

  case 722: // basic_expr: "'-'" attr basic_expr
#line 3523 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                  {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_NEG, std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[2].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5710 "verilog_parser.tab.cc"
    break;

  case 723: // basic_expr: basic_expr OP_LAND attr basic_expr
#line 3528 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                           {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_LOGIC_AND, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5720 "verilog_parser.tab.cc"
    break;

  case 724: // basic_expr: basic_expr OP_LOR attr basic_expr
#line 3533 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                          {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_LOGIC_OR, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[3].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5730 "verilog_parser.tab.cc"
    break;

  case 725: // basic_expr: "'!'" attr basic_expr
#line 3538 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                 {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_LOGIC_NOT, std::move(yystack_[0].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[2].location, yystack_[0].location);
		append_attr(yylhs.value.as < ast_t > ().get(), std::move(yystack_[1].value.as < al_t > ()));
	}
#line 5740 "verilog_parser.tab.cc"
    break;

  case 726: // basic_expr: TOK_SIGNED OP_CAST "'('" expr "')'"
#line 3543 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                      {
		if (!mode->sv)
			err_at_loc(yystack_[3].location, "Static cast is only supported in SystemVerilog mode.");
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_TO_SIGNED, std::move(yystack_[1].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[4].location, yystack_[1].location);
	}
#line 5751 "verilog_parser.tab.cc"
    break;

  case 727: // basic_expr: TOK_UNSIGNED OP_CAST "'('" expr "')'"
#line 3549 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                        {
		if (!mode->sv)
			err_at_loc(yystack_[3].location, "Static cast is only supported in SystemVerilog mode.");
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_TO_UNSIGNED, std::move(yystack_[1].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[4].location, yystack_[1].location);
	}
#line 5762 "verilog_parser.tab.cc"
    break;

  case 728: // basic_expr: basic_expr OP_CAST "'('" expr "')'"
#line 3555 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                      {
		if (!mode->sv)
			err_at_loc(yystack_[3].location, "Static cast is only supported in SystemVerilog mode.");
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_CAST_SIZE, std::move(yystack_[4].value.as < ast_t > ()), std::move(yystack_[1].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[4].location, yystack_[1].location);
	}
#line 5773 "verilog_parser.tab.cc"
    break;

  case 729: // basic_expr: typedef_base_type OP_CAST "'('" expr "')'"
#line 3561 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                             {
		if (!mode->sv)
			err_at_loc(yystack_[3].location, "Static cast is only supported in SystemVerilog mode.");
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_CAST_SIZE, std::move(yystack_[4].value.as < ast_t > ()), std::move(yystack_[1].value.as < ast_t > ()));
		SET_AST_NODE_LOC(yylhs.value.as < ast_t > ().get(), yystack_[4].location, yystack_[1].location);
	}
#line 5784 "verilog_parser.tab.cc"
    break;

  case 730: // basic_expr: "'('" expr "'='" expr "')'"
#line 3567 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                               {
		extra->ensureAsgnExprAllowed(yystack_[2].location, mode->sv);
		yylhs.value.as < ast_t > () = yystack_[3].value.as < ast_t > ()->clone();
		auto node = std::make_unique<AstNode>(yylhs.location, AST_ASSIGN_EQ, std::move(yystack_[3].value.as < ast_t > ()), std::move(yystack_[1].value.as < ast_t > ()));
		SET_AST_NODE_LOC(node.get(), yystack_[3].location, yystack_[1].location);
		extra->ast_stack.back()->children.push_back(std::move(node));
	}
#line 5796 "verilog_parser.tab.cc"
    break;

  case 731: // basic_expr: "'('" expr asgn_binop expr "')'"
#line 3574 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                   {
		extra->ensureAsgnExprAllowed(yystack_[2].location, mode->sv);
		yylhs.value.as < ast_t > () = extra->addAsgnBinopStmt(nullptr, std::move(yystack_[3].value.as < ast_t > ()), yystack_[2].value.as < ast_node_type_t > (), std::move(yystack_[1].value.as < ast_t > ()))-> clone();
	}
#line 5805 "verilog_parser.tab.cc"
    break;

  case 732: // concat_list: expr
#line 3580 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_CONCAT, std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 5813 "verilog_parser.tab.cc"
    break;

  case 733: // concat_list: expr "','" concat_list
#line 3583 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                   {
		yylhs.value.as < ast_t > () = std::move(yystack_[0].value.as < ast_t > ());
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[2].value.as < ast_t > ()));
	}
#line 5822 "verilog_parser.tab.cc"
    break;

  case 734: // assignment_pattern_list: expr
#line 3589 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
             {
		yylhs.value.as < ast_t > () = std::make_unique<AstNode>(yylhs.location, AST_ASSIGN_PATTERN);
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 5831 "verilog_parser.tab.cc"
    break;

  case 735: // assignment_pattern_list: assignment_pattern_list "','" expr
#line 3593 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                               {
		yylhs.value.as < ast_t > () = std::move(yystack_[2].value.as < ast_t > ());
		yylhs.value.as < ast_t > ()->children.push_back(std::move(yystack_[0].value.as < ast_t > ()));
	}
#line 5840 "verilog_parser.tab.cc"
    break;

  case 736: // integral_number: TOK_CONSTVAL
#line 3599 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                     { yylhs.value.as < string_t > () = std::move(yystack_[0].value.as < string_t > ()); }
#line 5846 "verilog_parser.tab.cc"
    break;

  case 737: // integral_number: TOK_UNBASED_UNSIZED_CONSTVAL
#line 3600 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                     { yylhs.value.as < string_t > () = std::move(yystack_[0].value.as < string_t > ()); }
#line 5852 "verilog_parser.tab.cc"
    break;

  case 738: // integral_number: TOK_BASE TOK_BASED_CONSTVAL
#line 3601 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                    {
		yystack_[1].value.as < string_t > ()->append(*yystack_[0].value.as < string_t > ());
		yylhs.value.as < string_t > () = std::move(yystack_[1].value.as < string_t > ());
	}
#line 5861 "verilog_parser.tab.cc"
    break;

  case 739: // integral_number: TOK_CONSTVAL TOK_BASE TOK_BASED_CONSTVAL
#line 3605 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"
                                                 {
		yystack_[2].value.as < string_t > ()->append(*yystack_[1].value.as < string_t > ()).append(*yystack_[0].value.as < string_t > ());
		yylhs.value.as < string_t > () = std::move(yystack_[2].value.as < string_t > ());
	}
#line 5870 "verilog_parser.tab.cc"
    break;


#line 5874 "verilog_parser.tab.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      yy_lac_discard_ ("error recovery");
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  const char *
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    static const char *const yy_sname[] =
    {
    "end of file", "error", "invalid token", "string", "ast_t", "al_t",
  "specify target", "specify triple", "specify rise and fall", "boolean",
  "invalid token", "integer", "ast_node_type_t", "TOK_STRING", "TOK_ID",
  "TOK_CONSTVAL", "TOK_REALVAL", "TOK_PRIMITIVE", "TOK_SVA_LABEL",
  "TOK_SPECIFY_OPER", "TOK_MSG_TASKS", "TOK_BASE", "TOK_BASED_CONSTVAL",
  "TOK_UNBASED_UNSIZED_CONSTVAL", "TOK_USER_TYPE", "TOK_PKG_USER_TYPE",
  "TOK_ASSERT", "TOK_ASSUME", "TOK_RESTRICT", "TOK_COVER", "TOK_FINAL",
  "ATTR_BEGIN", "ATTR_END", "DEFATTR_BEGIN", "DEFATTR_END", "TOK_MODULE",
  "TOK_ENDMODULE", "TOK_PARAMETER", "TOK_LOCALPARAM", "TOK_DEFPARAM",
  "TOK_PACKAGE", "TOK_ENDPACKAGE", "TOK_PACKAGESEP", "TOK_INTERFACE",
  "TOK_ENDINTERFACE", "TOK_MODPORT", "TOK_VAR", "TOK_WILDCARD_CONNECT",
  "TOK_INPUT", "TOK_OUTPUT", "TOK_INOUT", "TOK_WIRE", "TOK_WAND",
  "TOK_WOR", "TOK_REG", "TOK_LOGIC", "TOK_INTEGER", "TOK_SIGNED",
  "TOK_ASSIGN", "TOK_ALWAYS", "TOK_INITIAL", "TOK_ALWAYS_FF",
  "TOK_ALWAYS_COMB", "TOK_ALWAYS_LATCH", "TOK_BEGIN", "TOK_END", "TOK_IF",
  "TOK_ELSE", "TOK_IFNONE", "TOK_FOR", "TOK_WHILE", "TOK_REPEAT",
  "TOK_DPI_FUNCTION", "TOK_POSEDGE", "TOK_NEGEDGE", "TOK_OR",
  "TOK_AUTOMATIC", "TOK_CASE", "TOK_CASEX", "TOK_CASEZ", "TOK_ENDCASE",
  "TOK_DEFAULT", "TOK_FUNCTION", "TOK_ENDFUNCTION", "TOK_TASK",
  "TOK_ENDTASK", "TOK_SPECIFY", "TOK_IGNORED_SPECIFY", "TOK_ENDSPECIFY",
  "TOK_SPECPARAM", "TOK_SPECIFY_AND", "TOK_IGNORED_SPECIFY_AND",
  "TOK_GENERATE", "TOK_ENDGENERATE", "TOK_GENVAR", "TOK_REAL",
  "TOK_SYNOPSYS_FULL_CASE", "TOK_SYNOPSYS_PARALLEL_CASE", "TOK_SUPPLY0",
  "TOK_SUPPLY1", "TOK_TO_SIGNED", "TOK_TO_UNSIGNED", "TOK_POS_INDEXED",
  "TOK_NEG_INDEXED", "TOK_PROPERTY", "TOK_ENUM", "TOK_TYPEDEF", "TOK_RAND",
  "TOK_CONST", "TOK_CHECKER", "TOK_ENDCHECKER", "TOK_EVENTUALLY",
  "TOK_INCREMENT", "TOK_DECREMENT", "TOK_UNIQUE", "TOK_UNIQUE0",
  "TOK_PRIORITY", "TOK_STRUCT", "TOK_PACKED", "TOK_UNSIGNED", "TOK_INT",
  "TOK_BYTE", "TOK_SHORTINT", "TOK_LONGINT", "TOK_VOID", "TOK_UNION",
  "TOK_BIT_OR_ASSIGN", "TOK_BIT_AND_ASSIGN", "TOK_BIT_XOR_ASSIGN",
  "TOK_ADD_ASSIGN", "TOK_SUB_ASSIGN", "TOK_DIV_ASSIGN", "TOK_MOD_ASSIGN",
  "TOK_MUL_ASSIGN", "TOK_SHL_ASSIGN", "TOK_SHR_ASSIGN", "TOK_SSHL_ASSIGN",
  "TOK_SSHR_ASSIGN", "TOK_BIND", "TOK_TIME_SCALE", "TOK_IMPORT", "'!'",
  "'#'", "'%'", "'&'", "'('", "')'", "'*'", "'+'", "','", "'-'", "'.'",
  "'/'", "':'", "';'", "'<'", "'='", "'>'", "'?'", "'@'", "'['", "']'",
  "'^'", "'_'", "'{'", "'|'", "'}'", "'~'", "'n'", "'p'", "'x'", "'z'",
  "OP_LOR", "OP_LAND", "OP_NOR", "OP_XNOR", "OP_NAND", "OP_EQ", "OP_NE",
  "OP_EQX", "OP_NEX", "OP_LE", "OP_GE", "OP_SHL", "OP_SHR", "OP_SSHL",
  "OP_SSHR", "OP_POW", "OP_CAST", "UNARY_OPS", "FAKE_THEN", "$accept",
  "input", "$@1", "design", "attr", "$@2", "attr_opt", "defattr", "$@3",
  "$@4", "opt_attr_list", "attr_list", "attr_assign", "hierarchical_id",
  "hierarchical_type_id", "module", "$@5", "$@6", "module_para_opt",
  "module_para_list", "single_module_para", "$@7", "$@8",
  "module_args_opt", "module_args", "optional_comma",
  "module_arg_opt_assignment", "module_arg", "$@9", "$@10", "$@11", "$@12",
  "package", "$@13", "$@14", "package_body", "package_body_stmt",
  "import_stmt", "$@15", "import_item_list", "import_item", "interface",
  "$@16", "$@17", "interface_body", "interface_body_stmt",
  "bind_directive", "$@18", "$@19", "$@20", "bind_target",
  "opt_bind_target_instance_list", "bind_target_instance_list",
  "bind_target_instance", "mintypmax_expr", "non_opt_delay", "delay",
  "io_wire_type", "$@21", "non_io_wire_type", "$@22", "wire_type",
  "wire_type_token_io", "wire_type_signedness", "wire_type_const_rand",
  "opt_wire_type_token", "wire_type_token", "net_type", "logic_type",
  "integer_atom_type", "integer_vector_type", "non_opt_range",
  "non_opt_multirange", "range", "range_or_multirange", "module_body",
  "module_body_stmt", "checker_decl", "$@23", "task_func_decl", "$@24",
  "$@25", "$@26", "$@27", "$@28", "$@29", "func_return_type",
  "opt_type_vec", "opt_signedness_default_signed",
  "opt_signedness_default_unsigned", "dpi_function_arg",
  "opt_dpi_function_args", "dpi_function_args", "opt_automatic",
  "opt_lifetime", "task_func_args_opt", "$@30", "$@31", "task_func_args",
  "task_func_port", "$@32", "$@33", "task_func_body", "specify_block",
  "specify_item_list", "specify_item", "specify_opt_triple", "specify_if",
  "specify_condition", "specify_target", "specify_edge",
  "specify_rise_fall", "specify_triple", "ignored_specify_block",
  "ignored_specify_item_opt", "ignored_specify_item",
  "specparam_declaration", "specparam_range",
  "list_of_specparam_assignments", "specparam_assignment",
  "path_declaration", "simple_path_declaration",
  "state_dependent_path_declaration", "module_path_expression",
  "module_path_primary", "number", "path_delay_value",
  "list_of_path_delay_extra_expressions", "specify_edge_identifier",
  "parallel_path_description", "full_path_description",
  "list_of_path_inputs", "more_path_inputs", "list_of_path_outputs",
  "opt_polarity_operator", "specify_input_terminal_descriptor",
  "specify_output_terminal_descriptor", "system_timing_declaration",
  "system_timing_arg", "system_timing_args", "ignspec_constant_expression",
  "ignspec_expr", "ignspec_id", "$@34", "param_signed", "param_integer",
  "param_real", "param_range", "param_integer_type", "param_range_type",
  "param_implicit_type", "param_type", "param_decl", "$@35",
  "localparam_decl", "$@36", "param_decl_list", "single_param_decl",
  "single_param_decl_ident", "defparam_decl", "defparam_decl_list",
  "single_defparam_decl", "enum_type", "$@37", "enum_base_type",
  "type_atom", "type_vec", "type_signing", "enum_name_list",
  "enum_name_decl", "opt_enum_init", "enum_var_list", "enum_var",
  "enum_decl", "struct_decl", "$@38", "struct_type", "$@39",
  "struct_union", "struct_body", "opt_packed", "opt_signed_struct",
  "struct_member_list", "struct_member", "member_name_list", "member_name",
  "$@40", "struct_member_type", "$@41", "member_type_token", "$@42",
  "$@43", "member_type", "struct_var_list", "struct_var", "wire_decl",
  "$@44", "$@45", "$@46", "$@47", "opt_supply_wires", "wire_name_list",
  "wire_name_and_opt_assign", "wire_name", "assign_stmt",
  "assign_expr_list", "assign_expr", "type_name", "typedef_decl",
  "typedef_base_type", "enum_struct_type", "cell_stmt", "$@48", "$@49",
  "tok_prim_wrapper", "cell_list", "single_cell", "single_cell_no_array",
  "$@50", "single_cell_arraylist", "$@51", "cell_list_no_array",
  "prim_list", "single_prim", "$@52", "cell_parameter_list_opt",
  "cell_parameter_list", "cell_parameter", "cell_port_list",
  "cell_port_list_rules", "cell_port", "always_comb_or_latch",
  "always_or_always_ff", "always_stmt", "$@53", "$@54", "$@55", "$@56",
  "always_cond", "always_events", "always_event", "opt_label",
  "opt_sva_label", "opt_property", "modport_stmt", "$@57", "$@58",
  "modport_args_opt", "modport_args", "modport_arg", "modport_member",
  "modport_type_token", "assert", "assert_property",
  "simple_behavioral_stmt", "asgn_binop", "inc_or_dec_op",
  "for_initialization", "behavioral_stmt", "$@59", "$@60", "$@61", "$@62",
  "$@63", "$@64", "$@65", "$@66", "$@67", "$@68", "$@69", "$@70",
  "if_attr", "case_attr", "case_type", "opt_synopsys_attr",
  "behavioral_stmt_list", "optional_else", "$@71", "case_body",
  "case_item", "$@72", "$@73", "gen_case_body", "gen_case_item", "$@74",
  "$@75", "case_select", "case_expr_list", "rvalue", "lvalue",
  "lvalue_concat_list", "opt_arg_list", "arg_list", "arg_list2",
  "single_arg", "module_gen_body", "gen_stmt_or_module_body_stmt",
  "genvar_identifier", "genvar_initialization", "gen_stmt", "$@76", "$@77",
  "$@78", "$@79", "$@80", "gen_block", "$@81", "$@82", "gen_stmt_block",
  "$@83", "opt_gen_else", "expr", "basic_expr", "@84", "concat_list",
  "assignment_pattern_list", "integral_number", YY_NULLPTR
    };
    return yy_sname[yysymbol];
  }



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

#if FRONTEND_VERILOG_YYDEBUG
    // Execute LAC once. We don't care if it is successful, we
    // only do it for the sake of debugging output.
    if (!yyparser_.yy_lac_established_)
      yyparser_.yy_lac_check_ (yyla_.kind ());
#endif

    for (int yyx = 0; yyx < YYNTOKENS; ++yyx)
      {
        symbol_kind_type yysym = YY_CAST (symbol_kind_type, yyx);
        if (yysym != symbol_kind::S_YYerror
            && yysym != symbol_kind::S_YYUNDEF
            && yyparser_.yy_lac_check_ (yysym))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = yysym;
          }
      }
    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }




  bool
  parser::yy_lac_check_ (symbol_kind_type yytoken) const
  {
    // Logically, the yylac_stack's lifetime is confined to this function.
    // Clear it, to get rid of potential left-overs from previous call.
    yylac_stack_.clear ();
    // Reduce until we encounter a shift and thereby accept the token.
#if FRONTEND_VERILOG_YYDEBUG
    YYCDEBUG << "LAC: checking lookahead " << symbol_name (yytoken) << ':';
#endif
    std::ptrdiff_t lac_top = 0;
    while (true)
      {
        state_type top_state = (yylac_stack_.empty ()
                                ? yystack_[lac_top].state
                                : yylac_stack_.back ());
        int yyrule = yypact_[+top_state];
        if (yy_pact_value_is_default_ (yyrule)
            || (yyrule += yytoken) < 0 || yylast_ < yyrule
            || yycheck_[yyrule] != yytoken)
          {
            // Use the default action.
            yyrule = yydefact_[+top_state];
            if (yyrule == 0)
              {
                YYCDEBUG << " Err\n";
                return false;
              }
          }
        else
          {
            // Use the action from yytable.
            yyrule = yytable_[yyrule];
            if (yy_table_value_is_error_ (yyrule))
              {
                YYCDEBUG << " Err\n";
                return false;
              }
            if (0 < yyrule)
              {
                YYCDEBUG << " S" << yyrule << '\n';
                return true;
              }
            yyrule = -yyrule;
          }
        // By now we know we have to simulate a reduce.
        YYCDEBUG << " R" << yyrule - 1;
        // Pop the corresponding number of values from the stack.
        {
          std::ptrdiff_t yylen = yyr2_[yyrule];
          // First pop from the LAC stack as many tokens as possible.
          std::ptrdiff_t lac_size = std::ptrdiff_t (yylac_stack_.size ());
          if (yylen < lac_size)
            {
              yylac_stack_.resize (std::size_t (lac_size - yylen));
              yylen = 0;
            }
          else if (lac_size)
            {
              yylac_stack_.clear ();
              yylen -= lac_size;
            }
          // Only afterwards look at the main stack.
          // We simulate popping elements by incrementing lac_top.
          lac_top += yylen;
        }
        // Keep top_state in sync with the updated stack.
        top_state = (yylac_stack_.empty ()
                     ? yystack_[lac_top].state
                     : yylac_stack_.back ());
        // Push the resulting state of the reduction.
        state_type state = yy_lr_goto_state_ (top_state, yyr1_[yyrule]);
        YYCDEBUG << " G" << int (state);
        yylac_stack_.push_back (state);
      }
  }

  // Establish the initial context if no initial context currently exists.
  bool
  parser::yy_lac_establish_ (symbol_kind_type yytoken)
  {
    /* Establish the initial context for the current lookahead if no initial
       context is currently established.

       We define a context as a snapshot of the parser stacks.  We define
       the initial context for a lookahead as the context in which the
       parser initially examines that lookahead in order to select a
       syntactic action.  Thus, if the lookahead eventually proves
       syntactically unacceptable (possibly in a later context reached via a
       series of reductions), the initial context can be used to determine
       the exact set of tokens that would be syntactically acceptable in the
       lookahead's place.  Moreover, it is the context after which any
       further semantic actions would be erroneous because they would be
       determined by a syntactically unacceptable token.

       yy_lac_establish_ should be invoked when a reduction is about to be
       performed in an inconsistent state (which, for the purposes of LAC,
       includes consistent states that don't know they're consistent because
       their default reductions have been disabled).

       For parse.lac=full, the implementation of yy_lac_establish_ is as
       follows.  If no initial context is currently established for the
       current lookahead, then check if that lookahead can eventually be
       shifted if syntactic actions continue from the current context.  */
    if (yy_lac_established_)
      return true;
    else
      {
#if FRONTEND_VERILOG_YYDEBUG
        YYCDEBUG << "LAC: initial context established for "
                 << symbol_name (yytoken) << '\n';
#endif
        yy_lac_established_ = true;
        return yy_lac_check_ (yytoken);
      }
  }

  // Discard any previous initial lookahead context.
  void
  parser::yy_lac_discard_ (const char* event)
  {
   /* Discard any previous initial lookahead context because of Event,
      which may be a lookahead change or an invalidation of the currently
      established initial context for the current lookahead.

      The most common example of a lookahead change is a shift.  An example
      of both cases is syntax error recovery.  That is, a syntax error
      occurs when the lookahead is syntactically erroneous for the
      currently established initial context, so error recovery manipulates
      the parser stacks to try to find a new initial context in which the
      current lookahead is syntactically acceptable.  If it fails to find
      such a context, it discards the lookahead.  */
    if (yy_lac_established_)
      {
        YYCDEBUG << "LAC: initial context discarded due to "
                 << event << '\n';
        yy_lac_established_ = false;
      }
  }


  int
  parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
         In the first two cases, it might appear that the current syntax
         error should have been detected in the previous state when
         yy_lac_check was invoked.  However, at that time, there might
         have been a different syntax error that discarded a different
         initial context during error recovery, leaving behind the
         current lookahead.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const short parser::yypact_ninf_ = -1180;

  const short parser::yytable_ninf_ = -627;

  const short
  parser::yypact_[] =
  {
   -1180,   126,   122, -1180, -1180, -1180,  1000, -1180,   143, -1180,
     909, -1180,   122,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   149,   183, -1180, -1180, -1180, -1180, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180, -1180,   116, -1180,   128,   139,
   -1180, -1180, -1180,   165,   107,   149,   272, -1180, -1180, -1180,
   -1180,   414,   360,   360,   449, -1180, -1180, -1180, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180, -1180, -1180,   339, -1180,   117,
   -1180,   772,   395, -1180, -1180, -1180, -1180, -1180, -1180,   465,
    2824,   165,   165, -1180,   107, -1180, -1180,   413,    42, -1180,
     420,   447,   589,   928,   928,   611,    57, -1180,   795,   635,
     149,   649,   149,   675,   692,  2824,   571, -1180, -1180, -1180,
     556,   189,   189, -1180,   259, -1180,   558, -1180,   713, -1180,
     725, -1180,   573, -1180, -1180, -1180, -1180,   574, -1180, -1180,
    2894, -1180, -1180, -1180,  2824, -1180, -1180, -1180, -1180, -1180,
     580,   112,   576, -1180,   326,   370,  3652, -1180, -1180, -1180,
     165, -1180,   757,   149, -1180,   618,   760, -1180, -1180, -1180,
   -1180, -1180,   165,   297, -1180, -1180, -1180, -1180,   763, -1180,
     297,   763, -1180, -1180,   764,   766, -1180, -1180,   769, -1180,
     128,   770,   139, -1180,   761, -1180, -1180, -1180, -1180, -1180,
     647,   650,   783, -1180, -1180, -1180,   165, -1180, -1180, -1180,
   -1180,   780, -1180,   660,   664,   670,   671,  3262,  3262,   395,
    2275,  3262,  3262,  3262,   132,   655,  3262,  3262,  3262,  3262,
    3262,  2824,  2824, -1180,   165, -1180, -1180,   677,   149, -1180,
    2824,  2824,  2824, -1180, -1180, -1180, -1180, -1180, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180,
   -1180,   685,   678, -1180,   710, -1180, -1180, -1180,   -45, -1180,
     571, -1180, -1180, -1180, -1180, -1180,   216, -1180,   705,   165,
     273,   708,   720,   718,   862, -1180, -1180, -1180,   165,   732,
   -1180,    88,    60,   727,   726,   730, -1180, -1180,   721, -1180,
     870,   945, -1180,  2824,  2824,  2824,  2824, -1180, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180,
   -1180, -1180,   593,  2824,  2824,  2824, -1180, -1180, -1180,  2824,
    2824, -1180, -1180, -1180, -1180, -1180, -1180, -1180,   739,   456,
     745,  2824,   120, -1180,   736,   743,   747,  3262,  3262,  3262,
    3262,  3262,  3262,  3262,  3262,  2824,  3262,  3262,  3262,  3262,
    3262,  3262,  3262,  3262,  3262,  3262,  3262,  3262,  3262,  3262,
    3262,  3262,  3262,  3262,  2824, -1180,   749,   149,   760, -1180,
     650,   763, -1180,  2824, -1180, -1180, -1180,   882,   746,   898,
   -1180,   732,   732, -1180,   767,   771,   422,   166, -1180, -1180,
     900, -1180,   775,   669,   778, -1180, -1180,  3262, -1180,   783,
     762, -1180, -1180, -1180,   303, -1180, -1180,   189,   189, -1180,
     131,   165,   784,   788,   789,   791, -1180,   776,   792,   793,
     782, -1180,   797,  2824,   798,   790,  2824,   802, -1180, -1180,
   -1180,   329,  3898,   329,   527,   527,   329,   651,   651,   805,
    3852,  3836,  3744,  3790,  3836,  3852,  3898,  1236,  1236,  1236,
    1236,   651,   651,   397,   397,   397,   397,   773,   804,   806,
     951, -1180, -1180,   812, -1180, -1180,    47,   953, -1180,   428,
   -1180,   958,   720,   820,   822, -1180,   973, -1180, -1180, -1180,
   -1180,    88,   832,   975,   839, -1180,   701, -1180,   918,   165,
      68,   851,   553,  3698, -1180, -1180,   165,   870, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180, -1180, -1180,  2824, -1180, -1180,
     837, -1180, -1180,   149,   855, -1180, -1180, -1180, -1180,  2824,
   -1180,  1790, -1180, -1180,   312, -1180,   852,   691, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180,   882, -1180,   858, -1180, -1180,
     669,   857, -1180,   999,   436,   928,   928, -1180,  2824, -1180,
   -1180, -1180, -1180, -1180, -1180,   418, -1180,   418,  1003, -1180,
   -1180,   165, -1180,  1004,   872,  1654, -1180, -1180, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180,   465,   874,
   -1180, -1180,  2824,   875, -1180,  1008,   441, -1180, -1180,   881,
     951, -1180,  3413,  1014, -1180, -1180,   720, -1180,   535,   624,
     165,    62, -1180,   165, -1180,   877, -1180,   290,  1065, -1180,
     698, -1180, -1180, -1180, -1180,   785, -1180,   880, -1180,   969,
     662,   763,   763, -1180,   832,   929, -1180,   735,   735, -1180,
     149,   342, -1180, -1180, -1180,    43, -1180, -1180, -1180, -1180,
   -1180,  1022,  1024,   165, -1180, -1180, -1180, -1180, -1180, -1180,
     895, -1180,  1790, -1180, -1180, -1180,   852, -1180,   897,   907,
   -1180, -1180, -1180,    59,   295,    58, -1180,  1029, -1180,  3695,
   -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180,
   -1180,  1031, -1180, -1180, -1180, -1180, -1180, -1180, -1180,   819,
   -1180,   912, -1180, -1180, -1180,   904, -1180, -1180, -1180, -1180,
     913, -1180, -1180, -1180, -1180, -1180, -1180,  1904, -1180, -1180,
     916,   926,   935,   997,  1015,  1016, -1180,  2824,   379, -1180,
   -1180,  1723, -1180,   162,   162,   162,    75, -1180,   938,   941,
   -1180, -1180, -1180, -1180,   955, -1180, -1180, -1180, -1180, -1180,
   -1180, -1180,   307, -1180,   588, -1180, -1180,   307,   832,   931,
     165, -1180,   943,   354, -1180,   933,   698, -1180, -1180, -1180,
     698,   932,  1974, -1180,  1440,   947,   946, -1180,   949, -1180,
     852,  2824, -1180,   952,   954,  1013,    59,   959, -1180,   962,
     963, -1180,    73,   411, -1180, -1180, -1180,   956,   964,   960,
     968, -1180,   967,  2824,  1105,   356, -1180,   970,  3530,   978,
   -1180, -1180, -1180, -1180,   872, -1180,   389, -1180,  1023,  1035,
    1037,  1050,  2824, -1180,   999, -1180,   987,   458,  1002,   949,
     852, -1180,  2824,  2824,   991,  1009,   949,    43,   872,   872,
     326,   872, -1180, -1180,  1020,  1025,  1028, -1180,  1030,  2824,
    2824, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180,  2824,
   -1180,   206, -1180,    43, -1180,  2824, -1180, -1180, -1180,   872,
   -1180,    93, -1180, -1180,  1038, -1180,  1146, -1180, -1180, -1180,
    2824,  1007, -1180,  1040,   207,   587,  2824, -1180, -1180,   587,
     165,   353, -1180, -1180, -1180,  1105,  1021,   693, -1180, -1180,
   -1180, -1180, -1180,  3008,  3008,  1523,  1034, -1180,   406,  1105,
   -1180,  2824, -1180,   861, -1180, -1180, -1180, -1180, -1180,   749,
    1162, -1180,  1031, -1180,  1043,  1044,  1045,  1046,  1055, -1180,
   -1180, -1180,  2824,  2824,  1051, -1180,  1169,  1064,  1071, -1180,
    2824,  1066, -1180,  2824,  2824, -1180,  2824,  2088,  2158,  2272,
    2342,  1072,  1075, -1180, -1180, -1180, -1180, -1180,  1073, -1180,
   -1180,  1209,  1070, -1180, -1180,   432,   451,   999,  1081,  1357,
   -1180,   698, -1180,  1085, -1180,   855, -1180, -1180, -1180, -1180,
    1218,  1077,  1080, -1180, -1180,  2824,  1089,  2824, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180,
     994, -1180, -1180, -1180,  1090,   294,  1079, -1180,  1105, -1180,
     723,  1084,  2894, -1180,  1092,  1091, -1180,  1105,  1105, -1180,
      15, -1180,  2824, -1180, -1180, -1180, -1180, -1180,  1228, -1180,
     474, -1180,  1228, -1180,  2456,  2526,  2640,  2824, -1180,   478,
    1102, -1180, -1180,  1087,   165,  1111, -1180, -1180, -1180, -1180,
   -1180, -1180, -1180,  2824,  1120,  2824,  1121,  2824,  1125,  1119,
    1129, -1180, -1180,   433,  1130, -1180, -1180,  1263, -1180, -1180,
    1131, -1180,  1122, -1180, -1180,  2824,  2824,  1135,   161, -1180,
   -1180, -1180,  2710,  1138,  3585,  1221,  1132,  2824,  2824,  1196,
   -1180,  1268,   722,   722,   722,   722,   722,   722,   722,   722,
   -1180,   963, -1180, -1180, -1180, -1180, -1180, -1180, -1180,  1133,
    1134, -1180,  1105, -1180,  1105, -1180,  1136,  1092,  2459,  2824,
   -1180,  2824, -1180, -1180,  1523,  1141,  1523,  1137,  3437,   165,
     522, -1180, -1180, -1180,  1162, -1180, -1180,   532, -1180,  1147,
    2824,  1151,  2824,  1153,  2824,  1154,  1156, -1180, -1180,  2824,
    2824,   640,  2824,  1296,  2824,   698,   698,  1166,  1159,  1168,
    1163,  1170,  1164, -1180,  1165,   698, -1180, -1180, -1180, -1180,
     999,  1167,  2824, -1180, -1180, -1180,  1606, -1180,  1606, -1180,
    1176, -1180,   852, -1180,  1256,  3660,  2824, -1180, -1180,  2824,
    1175,  3078, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180,
     353, -1180,   353,   353,   353,   353,   353,   353,   353,  1180,
    1182,   490, -1180, -1180, -1180,  1105,  1183,  2824,  1092,  1177,
   -1180, -1180, -1180, -1180, -1180, -1180,  1228, -1180, -1180,  1228,
   -1180, -1180,  1186,  1174,  1187,  1181,  1188,  1197,  1220,  1257,
    1190, -1180,   852, -1180, -1180,  1184, -1180, -1180, -1180,  1222,
   -1180,  1233, -1180,  1248, -1180, -1180, -1180,   455, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180,  1221, -1180, -1180, -1180,  1250,
   -1180,   587,  2894,  1195, -1180,   526,   -41,   357,   854,   457,
     -41, -1180, -1180,  1105,  1105, -1180,  1105,  1229, -1180,  1189,
   -1180,  2824,  1245, -1180, -1180,  1259,  1252, -1180,  1254, -1180,
    1255, -1180, -1180, -1180, -1180,  1720, -1180, -1180,  2824,  1262,
   -1180, -1180, -1180,  1343, -1180, -1180,  1346, -1180, -1180,  2824,
    2091,  1271,   270,   284, -1180, -1180,  2824, -1180, -1180, -1180,
   -1180, -1180, -1180, -1180,   -21, -1180,   321, -1180, -1180, -1180,
   -1180, -1180, -1180, -1180,  1720,    87,  1282,  1196,  2824,  2824,
    2824,  3192,  2824,  1276,  2824,  1280,   874,  1289,  1221,  2824,
   -1180,  1291,   698, -1180,  1221,  1293,  1292,  1294,   229,  2894,
    1290, -1180,  1295,  1300,  2824,  1304,  2824, -1180, -1180, -1180,
   -1180, -1180,   698, -1180,  2824, -1180, -1180, -1180,  1302,  2643,
   -1180,  2824,  1306,  1313,  1314,  1316,   698, -1180,  1317,  2824,
    2824,  1315, -1180,  1321, -1180,  1325, -1180,  2824,  1327,   536,
    1322,  2824, -1180, -1180, -1180,  1320, -1180,  2824,  2824, -1180,
   -1180,   549,   874, -1180,  2824,  1332,  2824,  1334,  2824,   550,
   -1180,  2824,  1335,  2824,  1336,  2824,  1337,  2824,  1339,  2824,
    1340,  2824,  1344, -1180
  };

  const short
  parser::yydefact_[] =
  {
       2,     0,    15,     1,    19,    79,     0,    93,     0,     3,
       0,    18,    15,    15,    15,    15,    15,    15,    15,    15,
      15,    15,    23,     0,    31,    32,   153,   152,   147,   370,
     394,   148,   151,   149,   150,   395,     0,   443,   209,   212,
     446,   447,   392,   161,     0,     0,     0,    34,   357,   359,
      64,     0,   224,   224,    16,     5,     4,    10,    11,    12,
      13,     6,     7,     8,     9,    28,    20,    22,    24,    26,
      80,   374,     0,   210,   211,   445,   213,   214,   444,   398,
       0,   160,   163,   162,     0,   439,   440,     0,   102,    94,
      99,    74,     0,   344,   344,     0,     0,   223,   206,     0,
      23,     0,     0,     0,     0,     0,    38,   376,   377,   375,
       0,   380,   380,    33,   401,   393,     0,   680,   736,   679,
       0,   737,     0,    15,    15,   561,   562,     0,    15,    15,
       0,    15,    15,    15,     0,    15,    15,    15,    15,    15,
       0,   161,     0,    15,   675,     0,   671,   678,   158,   159,
     161,   442,     0,     0,    97,     0,     0,    35,   342,   346,
     343,   356,   161,   344,   353,   352,   354,   355,     0,   345,
     344,     0,    65,   190,     0,     0,   207,   208,     0,   202,
     209,     0,   212,   196,     0,    21,    25,    29,    30,    27,
       0,    48,     0,   378,   379,   372,   161,   399,   400,   397,
     409,     0,   738,     0,     0,     0,     0,     0,     0,    31,
       0,     0,     0,     0,   732,     0,     0,     0,     0,     0,
       0,     0,     0,   681,   632,   631,   677,     0,     0,   674,
       0,     0,     0,   157,    15,    15,    15,    15,    15,    15,
      15,    15,    15,    15,    15,    15,    15,    15,    15,    15,
      15,    15,    15,    15,    15,    15,    15,    15,    15,    15,
      15,     0,     0,    95,    98,   100,    73,    78,     0,    76,
      38,   160,   347,   351,   348,   365,     0,   361,   364,   349,
       0,     0,   218,     0,     0,   198,   205,   200,   203,   228,
      17,    15,    15,     0,   385,    53,   381,   373,   409,   402,
       0,   412,   739,     0,     0,     0,     0,   675,   725,   697,
     549,   550,   551,   552,   553,   554,   555,   556,   557,   558,
     559,   560,   685,     0,     0,     0,   721,   722,   701,     0,
       0,   687,   699,   690,   700,   702,   698,   734,    53,     0,
       0,     0,   161,   673,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   441,   470,     0,     0,    75,
      48,     0,   358,     0,   350,   360,    68,   222,     0,     0,
     192,   228,   228,   204,   229,     0,     0,     0,    39,    46,
      56,    47,     0,   117,    53,    50,    83,     0,   383,    52,
       0,   396,   403,   407,     0,   405,   417,   380,   380,   410,
       0,   161,     0,     0,     0,     0,   676,     0,     0,     0,
     732,   733,     0,    52,     0,   157,   640,     0,   155,   156,
     154,   719,   691,   717,   715,   716,   718,   707,   714,     0,
     695,   693,   724,   723,   694,   696,   692,   709,   710,   711,
     712,   708,   713,   703,   704,   705,   706,   720,     0,     0,
       0,   101,    77,     0,   362,   363,    15,   216,   221,     0,
     191,     0,   218,     0,     0,   227,    15,   239,    42,    44,
      37,    15,    55,     0,     0,   119,     0,   120,   226,   161,
      15,     0,    15,   384,   382,   371,   161,     0,   404,   415,
     416,   413,   411,   726,   683,   684,   727,     0,   730,   731,
       0,   735,   688,     0,    53,   639,   641,   643,   729,     0,
     728,   473,   458,   462,     0,   168,   510,     0,    67,    72,
      71,    70,    69,   215,   217,   220,   194,     0,   239,   239,
     117,    53,   232,     0,    15,   344,   344,    40,     0,    57,
      59,    63,   121,   122,   123,   130,   225,   130,     0,    51,
      49,   161,    81,     0,   114,   117,    82,    92,    84,    85,
      87,    88,    90,    86,    89,    91,   408,   406,   398,     0,
     689,   630,    52,     0,   672,     0,     0,   471,   474,     0,
       0,    96,    15,     0,    66,   219,   218,   193,    15,    15,
     161,    15,   230,   161,   237,     0,   197,     0,   117,   566,
      15,   569,   570,   568,   571,     0,   567,     0,   238,     0,
       0,     0,     0,    54,    55,   129,   128,   132,     0,    61,
       0,     0,   367,   516,   113,     0,   488,   495,   489,   486,
     487,     0,     0,   161,   493,   490,   414,   686,   642,   682,
       0,   469,   473,    15,   463,   661,   510,   663,     0,     0,
     603,   604,   605,   242,     0,     0,   646,     0,   167,   117,
     184,   181,   164,   186,   169,   170,   187,   175,   171,   172,
     174,     0,   179,   180,   176,   177,   173,   178,   182,     0,
     185,     0,   165,   166,   509,     0,   199,   201,   234,   233,
       0,   434,   511,   105,   106,   107,   108,     0,   577,   579,
       0,     0,     0,   602,   600,   601,   574,     0,   161,    15,
     633,    15,   572,   515,   515,   515,   515,   573,     0,     0,
      43,    45,    60,   127,   137,   143,   142,   141,   135,   144,
     140,   146,   126,   131,   133,   139,   145,   126,    55,     0,
     161,   366,     0,     0,   436,     0,    15,   424,   426,   421,
      15,   502,     0,   472,   480,     0,   477,   478,   638,    36,
     510,     0,   654,     0,     0,     0,   242,     0,   340,     0,
       0,   266,     0,     0,   268,   269,   270,     0,     0,     0,
       0,   271,     0,     0,     0,     0,   275,     0,    15,     0,
     448,   452,   453,   390,   114,   388,     0,   386,     0,     0,
       0,     0,     0,   195,     0,   231,     0,     0,   103,   638,
     510,   582,     0,     0,     0,   635,   638,     0,   114,   114,
       0,   114,   514,   513,     0,     0,     0,   534,     0,     0,
       0,   136,   138,   124,   125,   116,   134,   118,    62,     0,
     368,     0,   517,     0,   435,     0,   496,   428,   428,   114,
     494,     0,   491,   475,     0,   485,     0,   481,   459,    15,
     640,     0,   664,     0,     0,   257,     0,   240,   241,   257,
     161,     0,   283,   311,   312,     0,     0,   327,   328,   265,
     267,   278,   279,     0,     0,     0,     0,   337,     0,     0,
     272,     0,   183,   117,   648,   644,   647,   645,   188,   470,
       0,   450,     0,   389,     0,     0,     0,     0,     0,   235,
     109,   110,     0,     0,     0,   580,   117,     0,     0,   634,
       0,     0,   546,     0,     0,   545,     0,     0,     0,     0,
       0,     0,     0,   369,   525,   526,   527,   519,    53,   521,
     524,     0,     0,   437,   438,     0,     0,     0,     0,     0,
     501,    15,   476,   484,   479,    53,   662,   646,   657,   650,
       0,     0,     0,   255,   256,     0,     0,     0,   341,   303,
     302,   305,    15,    15,    15,    15,    15,    15,    15,    15,
       0,   284,   301,   304,     0,   327,     0,   325,     0,   326,
     327,   319,     0,   280,   307,   338,   281,     0,     0,   334,
       0,   333,     0,   273,   276,   277,   649,   168,     0,   420,
       0,   418,   467,   387,     0,     0,     0,     0,   659,     0,
       0,   578,   610,     0,   161,     0,   586,   588,   636,   576,
     544,   547,   548,     0,     0,     0,     0,     0,     0,     0,
       0,   590,   593,    52,     0,   523,   518,     0,   425,   427,
     422,   430,   432,   499,   500,     0,     0,     0,     0,   503,
     508,   492,     0,     0,    15,   666,   651,     0,     0,   250,
     247,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      15,     0,    15,    15,    15,    15,    15,    15,    15,     0,
       0,   319,     0,   321,     0,   320,     0,     0,   338,     0,
     308,     0,   331,   332,     0,     0,     0,     0,    15,   458,
       0,   454,   456,   457,     0,   391,   466,     0,   464,     0,
       0,     0,     0,     0,     0,     0,     0,   620,   111,     0,
       0,    15,     0,     0,     0,    15,    15,     0,     0,     0,
       0,     0,     0,   533,     0,    15,   608,   522,   520,   429,
       0,     0,     0,   506,   507,   498,     0,   497,     0,   483,
       0,   637,   510,   668,   670,    15,     0,   653,   655,     0,
       0,     0,   285,   287,   291,   289,   286,   290,   292,   288,
       0,   282,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   323,   329,   322,     0,     0,     0,   309,     0,
     335,   330,   336,   274,   189,   460,     0,   449,   419,   467,
     451,    15,     0,     0,     0,     0,     0,     0,     0,   621,
       0,   104,   510,   609,   563,   564,   583,   587,   589,     0,
     528,     0,   529,     0,   535,   532,   591,   615,   431,   423,
     433,   504,   505,   482,   665,   666,   658,   667,   652,     0,
     249,   257,     0,     0,   251,   297,   299,   298,   296,   295,
     300,   293,   294,     0,     0,   316,     0,     0,   306,     0,
     310,     0,     0,   455,   465,     0,     0,   537,     0,   538,
       0,   542,   541,   660,   619,     0,   112,   581,     0,     0,
     530,   531,   536,   613,   606,   607,   616,   669,    15,     0,
       0,     0,   327,   327,   324,   313,     0,   339,    15,   468,
     539,   540,   543,   627,   625,   622,     0,   628,   565,    15,
     611,   592,   594,   614,     0,     0,     0,   250,     0,     0,
       0,     0,     0,     0,     0,     0,   339,     0,   666,     0,
     624,     0,    15,   617,   666,     0,     0,     0,     0,     0,
       0,   258,   263,     0,     0,     0,     0,   461,   623,   629,
     584,   612,    15,   656,     0,   254,   253,   252,     0,   263,
     243,     0,     0,     0,     0,     0,    15,   618,   246,     0,
       0,     0,   318,     0,   315,     0,   585,     0,     0,     0,
       0,     0,   317,   314,   245,     0,   259,     0,     0,   264,
     244,     0,   264,   260,     0,     0,     0,     0,     0,     0,
     261,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   262
  };

  const short
  parser::yypgoto_[] =
  {
   -1180, -1180, -1180,  1496,   154, -1180, -1180,    98, -1180, -1180,
    1391, -1180,  1390,   -19,   -66, -1180, -1180, -1180,  1230, -1180,
    1012, -1180, -1180,  1126, -1180,  -329,  -557,  1018, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180,
    1142, -1180, -1180, -1180, -1180, -1180,  -495, -1180, -1180, -1180,
   -1180, -1180, -1180,  -112,  -879,  -383,  -176, -1180, -1180,   563,
   -1180,  -333, -1180,   768,   961, -1180,   885, -1180,  -470,   -42,
   -1180,   -76,  -137,   -73,  -142,   499,  -591, -1180, -1180,  -453,
   -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180,  1347,  1348,
     984,  -404, -1180,  1481, -1180,   363, -1180, -1180, -1180,   924,
   -1180, -1180,   209, -1180,   756, -1180, -1180, -1180,   196, -1180,
    -856, -1180,     8, -1180, -1180,   758,  -566, -1180,   750,   641,
   -1180,  -755, -1180,    45,   302, -1180,   645,  -987, -1180, -1180,
   -1180,   661, -1180,   274,  -963,  -850,  -913, -1180,  -826, -1180,
     533,  -867,  -442, -1180,   254, -1180, -1180, -1180, -1180, -1180,
   -1180,   -72,     0, -1180,    46, -1180,  1387,  -252, -1180,  1062,
   -1180,   807,  1559, -1180, -1180,   -25,    -2,   -69, -1180,  1157,
   -1180, -1180,   646, -1180, -1180, -1180,  1563, -1180,  1150,   983,
   -1180, -1180, -1180,  1274, -1180,  1067, -1180, -1180, -1180, -1180,
   -1180, -1180, -1180, -1180,   439,  -464, -1180, -1180, -1180, -1180,
     707, -1180,   412,  -513,  1074, -1180,   724,  1499,    78,  1580,
   -1180, -1180, -1180, -1180, -1180, -1180,  -973,  -385, -1180, -1180,
   -1180, -1180, -1180,   364, -1180,   673, -1180,   927, -1152, -1180,
     715, -1180, -1180,  1093, -1180, -1180, -1180, -1180, -1180, -1180,
    -831,  -636,  -592,   167, -1180, -1180, -1180, -1180, -1180,   537,
     637, -1180, -1180, -1180, -1179,   868,  -143, -1180,  -586, -1180,
   -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180, -1180,
   -1180, -1180, -1180,   971, -1180, -1180, -1180, -1180, -1180, -1180,
   -1180, -1180, -1180, -1180, -1180, -1180,   262, -1180,   -30,  -596,
     663,  -388,   728, -1180,  1017,   633,   426,   632, -1180,  -571,
   -1180, -1180, -1180, -1180, -1180,  -578, -1180, -1180,  -977, -1180,
   -1180,   -80,   996, -1180,   438, -1180,   101
  };

  const short
  parser::yydefgoto_[] =
  {
       0,     1,     2,     9,   618,    11,    54,   619,    22,   101,
      66,    67,    68,   141,    37,    13,    92,   270,   191,   397,
     398,   555,   556,   293,   404,   410,   559,   405,   492,   493,
     634,   758,    14,    95,   281,   476,   538,    15,   156,   268,
     269,    16,    23,   106,   502,   576,    17,    45,   152,   376,
      89,   154,   264,    90,   827,   620,   645,   495,   496,   497,
     498,   653,   565,   855,   637,   752,   753,   754,   755,    38,
      39,    81,    82,    83,    84,   602,   914,   683,  1027,    18,
     282,   482,   606,   289,   391,   392,   181,   182,    75,    78,
     478,   388,   479,    98,   567,   395,   486,   710,   551,   552,
     824,   553,   554,   685,   785,   786,  1408,   787,  1190,  1273,
     985,  1370,  1371,   686,   793,   794,   687,   804,   805,   806,
     796,   797,   798,  1000,  1001,  1002,  1013,  1120,   895,   799,
     800,   896,  1010,  1211,  1011,   897,  1212,   801,  1019,  1020,
     906,  1021,  1213,   890,   162,   163,   164,   273,   165,   166,
     167,   168,   621,    93,   622,    94,   276,   277,   278,   690,
     641,   642,   691,    71,   110,   169,   170,   195,   295,   296,
     408,   816,   817,   692,   693,   920,   813,    79,    42,   115,
     116,   199,   298,   299,   414,   415,   506,   300,   301,   419,
     420,   588,   421,  1030,  1031,   623,   869,  1171,   867,   868,
     965,  1070,  1071,  1072,   695,   763,   764,    87,   624,   142,
      44,   697,   919,  1032,   814,  1130,  1136,  1132,   599,  1133,
    1292,   534,  1137,  1138,  1139,   470,   596,   597,   775,   776,
     777,   654,   655,   698,   771,   971,   770,   766,   872,  1078,
    1079,   604,   625,   844,   585,   762,   962,   862,   958,   959,
     960,   961,   626,   700,   627,   325,   143,  1045,   628,   836,
     829,   830,  1042,   936,  1309,  1396,  1155,  1156,  1165,  1313,
    1166,   629,   630,   701,  1257,  1151,  1341,  1362,  1316,  1343,
    1344,  1382,  1239,  1304,  1305,  1358,  1335,  1336,   144,   731,
     834,   881,   524,   525,   526,   808,   915,   981,   982,   916,
     884,  1269,  1085,  1147,   778,  1183,   780,   977,  1184,  1185,
    1266,  1372,   146,   340,   215,   338,   147
  };

  const short
  parser::yytable_[] =
  {
     145,   229,    19,    69,   224,   148,   149,   577,   262,   434,
     699,   682,    19,    19,    19,    19,    19,    19,    19,    19,
      19,    19,   171,   539,   703,   189,    88,   161,   161,   109,
     779,   702,   179,   987,   732,   892,  1014,  1014,   581,   399,
     614,   265,  1111,   196,  1025,  1005,   111,  1115,    20,   765,
     210,   109,   109,  1039,   214,  1131,   180,    65,    20,    20,
      20,    20,    20,    20,    20,    20,    20,    20,   225,   112,
     499,   173,   788,   783,   400,   501,  -236,   742,   547,  1295,
      21,    69,   400,    69,   103,   533,   271,   788,   536,   272,
      21,    21,    21,    21,    21,    21,    21,    21,    21,    21,
      12,    65,   275,  1100,   378,   842,  1124,   681,   795,   379,
      12,    12,    12,    12,    12,    12,    12,    12,    12,    12,
     271,    85,   -14,   297,   968,   784,     3,   118,  -626,   474,
    1216,    86,  -626,   120,    88,   121,  1107,  1108,   694,  1346,
      72,   337,   339,   -15,   882,  1117,   893,   894,   149,   684,
     344,   345,   346,     6,   103,     4,    10,    46,  1113,   103,
    1361,  1125,   103,    65,  1126,     5,    10,    10,    10,    10,
      10,    10,    10,    10,    10,    10,  1357,   307,   307,   843,
     866,   307,   307,   307,   870,    73,   307,   307,   307,   307,
     307,   644,   842,   104,   935,   593,    76,    70,   343,   125,
     126,   858,   705,   384,  -248,   224,   401,   727,   -52,   342,
     174,   402,   393,   175,   -52,   664,   699,   610,   803,   402,
     954,   979,   612,   422,   423,   424,   425,   795,     6,   847,
     917,  1290,   802,   807,   -41,   416,  1176,   -41,   969,   399,
     970,   942,   226,   427,   428,   429,   193,    74,    30,   430,
     430,   727,  1218,  1293,   955,   956,    35,   -15,    77,   109,
       7,   437,     8,   104,  1214,   471,   843,   765,   104,   225,
    1240,   104,   222,   105,   852,   449,   417,   204,   205,   512,
     222,   329,   207,   208,   856,   211,   212,   213,  1317,   216,
     217,   218,   219,   220,   468,   223,   330,   228,  1220,   418,
    1222,   980,  1287,   475,   713,   714,   715,  1177,   194,   788,
    1178,   929,   490,   681,    91,   491,   197,   307,   307,   307,
     307,   307,   307,   307,   307,    80,   307,   307,   307,   307,
     307,   307,   307,   307,   307,   307,   307,   307,   307,   307,
     307,   307,   307,   307,   694,  1261,  1201,  1262,   509,   510,
     898,   802,   957,   521,   158,   684,   527,   568,    88,  1353,
    1355,   789,   807,   790,   853,   381,   989,   990,   118,   991,
     382,  1323,  1352,  1324,   120,  1387,   121,   307,   198,   740,
     741,  1378,   517,   791,   675,  1081,  1354,  1383,   347,   348,
     349,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   362,   363,   364,   365,   366,   367,   368,
     369,   370,   371,   372,   373,  1319,   160,   274,  1007,  1286,
    1009,   103,   381,   426,   279,   788,   854,   385,    96,   716,
     271,   644,  1007,   586,  1009,   717,    97,   589,   125,   126,
     792,   934,  1007,  1008,  1009,   396,   403,   954,   941,   594,
    1110,   598,   507,   898,   615,   644,   644,   508,   644,   488,
     489,   600,  -512,  -512,  -512,  -512,   601,   807,   708,     4,
    1359,   711,   230,   231,  1360,   729,   540,   789,   633,   790,
     100,   955,   956,   631,   632,  1373,   644,  1375,   102,   161,
     161,   760,   699,   591,   992,   271,   761,   993,   640,   899,
     675,  1100,   578,   863,   342,   909,   917,  1393,   864,  1395,
     910,   769,   527,   109,   109,   994,   260,   261,   995,  1102,
     996,   616,   541,   232,  -575,   635,   636,   997,   998,   999,
     104,   233,  1106,  -575,  1107,  1108,   699,   682,   922,   222,
     234,   113,     6,   923,   236,   237,  1264,   238,   579,   239,
     703,  1314,  1315,   615,   542,   909,   792,   702,   230,   231,
    1023,  -512,  -512,  -512,  -512,  1243,   898,   151,     4,  1247,
    1248,   751,   751,   153,   544,  1122,  1123,   545,   617,  1256,
     583,  1067,   598,   114,   260,   261,  1068,   661,   730,   681,
     662,   224,   571,   699,   155,   756,   756,   572,   573,   728,
    1067,  1100,   688,   157,   931,  1069,  1307,   932,   118,   232,
     759,   574,    24,    25,   120,   730,   121,   435,   706,  1102,
     694,   342,  1103,  1134,  1148,   172,   342,  1149,  1135,  1064,
     537,   684,  1106,   681,  1107,  1108,  1285,   828,   921,  1286,
     550,     6,   615,   749,    28,   396,  1083,   835,   689,   183,
    -512,  -512,  -512,  -512,   403,   225,   575,     4,   615,     6,
     983,   984,   943,   944,   694,   946,  -512,  -512,  -512,  -512,
     234,  1226,   898,     4,   236,   684,  1227,   617,   751,   239,
     696,  1229,  1416,   185,   271,  1417,  1230,   640,   751,   187,
     681,     7,   874,   967,   877,  1423,  1430,   945,  1424,  1431,
     680,   883,   756,  1107,  1108,  1242,   188,   707,    31,    32,
      33,    34,   756,   190,   260,   261,   615,  -115,  -115,  -115,
     192,   694,   200,   907,  -512,  -512,  -512,  -512,    48,    49,
       6,     4,   684,    36,   201,   989,   990,   118,   991,   670,
     671,   672,   928,   120,   221,   121,     6,   202,   988,   562,
     563,   564,   937,   938,   483,   484,   679,   608,   609,    24,
      25,   203,   206,    51,   227,   550,   617,   431,   432,   951,
     952,   263,   266,    52,   267,    53,  1381,   275,   283,   953,
     284,   744,   617,   285,   287,   964,   745,   746,   747,   748,
     749,    28,   291,   290,   234,   292,  1397,   294,   236,   237,
     527,   238,   302,   239,     6,   303,   986,   730,   688,   304,
    1406,   733,   734,   735,   736,   305,   306,   774,   342,    24,
      25,   331,   341,  1015,  1015,  1015,   107,   108,    28,   750,
     374,  1015,   375,   730,   256,   257,   258,   259,   260,   261,
     617,  1007,  1008,  1009,   342,   818,   819,   820,   821,   176,
     177,    28,   828,  1040,   689,    31,    32,    33,    34,   377,
     835,   383,   386,  1050,  1051,   387,  1052,  1054,  1056,  1058,
    1060,  1007,  1114,  1009,   389,   810,   390,   394,   811,   409,
      36,   406,   407,   837,   413,   840,   696,   411,   433,  1080,
     436,   469,    31,    32,    33,    34,   477,   438,    48,    49,
     480,   845,   846,   848,   439,  1089,   680,  1091,   440,  -115,
    -115,  -115,   481,   485,   -58,    31,    32,    33,    34,   178,
     646,   647,   648,   649,   650,   487,   494,   500,   505,   517,
     513,   329,  1118,    51,   514,   515,   812,   516,   518,   519,
      36,   523,   907,    52,    47,    53,    48,    49,   528,    50,
     530,   531,    24,    25,  1141,  1143,  1145,  1146,   529,   651,
     652,   261,   913,   520,   522,   532,   535,   543,   271,    24,
      25,  1153,   546,  1157,   548,  1159,   549,  1161,    30,    24,
      25,    51,   107,   108,    28,   158,    35,  -236,   558,   560,
     561,    52,  1003,    53,   566,  1173,  1174,   570,  1100,   107,
     108,    28,  1180,   590,   592,   603,   611,  1187,  1188,   851,
     749,    28,   607,   613,   617,  1026,  1102,   639,   643,  1103,
     657,   659,   660,   159,    24,    25,   663,  1105,   704,  1106,
     712,  1107,  1108,   774,   737,   738,   767,   743,   768,  1015,
     772,  1219,   781,   809,  1015,   815,  1015,   160,    31,    32,
      33,    34,   782,  1225,    26,    27,    28,   822,   823,   825,
    1232,   831,  1234,  -598,  1236,    31,    32,    33,    34,   828,
    1241,   832,  1244,    36,  1246,    31,    32,    33,    34,    65,
     833,  -596,  -597,   849,   688,   718,   850,   859,   861,   865,
      36,   871,  1260,   878,   880,   879,  1080,   885,  1080,   886,
      36,   887,    48,    49,   889,    29,  1268,   891,   792,  1270,
     901,  1274,   905,  -115,  -115,  -115,   903,    30,   902,   788,
      31,    32,    33,    34,   904,    35,   911,   924,   688,   719,
     689,  -595,   918,   930,   720,   721,   722,  1289,  1100,   925,
    1101,   926,  -599,  -599,  -599,    36,  1092,  1093,  1094,  1095,
    1096,  1097,  1098,  1099,   927,   933,  1102,   939,   940,  1103,
     973,   976,   696,   651,   652,   947,  1104,  1105,  1006,  1106,
     948,  1107,  1108,   949,   689,   950,  1029,   125,   126,   723,
     724,   725,   680,  1043,   972,   688,   978,  1022,  1034,  1035,
    1036,  1037,  1320,  1003,  1003,  1003,  1003,  1003,  1003,  1003,
    1003,  1038,   729,   308,   309,  1041,   696,   326,   327,   328,
    1046,  1327,   332,   333,   334,   335,   336,  1047,  1061,   726,
    1049,  1062,  1063,   954,  1066,  1337,   680,  1073,  1338,   727,
    1082,   689,   979,  1087,  1088,  1090,  1112,  1109,   913,  1347,
    1116,  1119,  1129,  1152,  1121,  1275,  1356,  1276,  1277,  1278,
    1279,  1280,  1281,  1282,  1200,  1150,  1202,  1203,  1204,  1205,
    1206,  1207,  1208,   696,  1337,  1154,  1158,  1160,  1366,  1367,
    1368,  1162,  1015,  1163,  1015,  1164,  1168,  1169,  1172,  1379,
    1170,  1175,   679,   680,  1181,   667,  1189,  1191,  1186,  1389,
    1209,  1210,  1231,  1215,  1015,  1221,  1015,  1233,  1223,  1235,
    1237,  1003,  1238,  1003,  1003,  1003,  1003,  1003,  1003,  1003,
    1245,  1401,  1249,  1250,  1251,   730,  1253,  1252,  1254,  1255,
    1410,  1259,  1263,  1265,  1271,  1283,   342,  1284,  1297,  1288,
    1291,  1419,  1296,  1298,  1300,  1299,  1306,  1303,  1422,   913,
    1308,  1321,  1326,   441,   442,   443,   444,   445,   446,   447,
     448,  1301,   450,   451,   452,   453,   454,   455,   456,   457,
     458,   459,   460,   461,   462,   463,   464,   465,   466,   467,
     117,    65,   118,   119,  1302,  1325,  1310,  1388,   120,   234,
     121,    24,    25,   236,   237,   774,   238,  1311,   239,  1074,
    1328,   240,  1398,   241,  1192,  1193,  1194,  1195,  1196,  1197,
    1198,  1199,  1312,   503,  1318,  1329,  1330,  1409,  1331,  1332,
    1340,    26,    27,    28,   122,  1414,  1339,   254,   255,   256,
     257,   258,   259,   260,   261,  1421,  1342,  1351,  1364,  1374,
    1075,  1076,  1425,  1376,  1427,  1377,  1429,  1380,  1385,  1432,
    1386,  1434,  1384,  1436,  1390,  1438,  1392,  1440,  1391,  1442,
    1394,  1399,  1402,   117,    65,   118,   119,   123,   124,  1403,
    1404,   120,  1405,   121,    24,    25,  1407,  1412,  1411,   125,
     126,  1413,  1345,  1415,  1420,  1418,   127,    31,    32,    33,
      34,  1426,   774,  1428,  1433,  1435,  1437,   875,  1439,  1441,
    1443,   184,   186,  1345,    26,    27,    28,   122,   128,  1044,
     380,   129,   130,   557,  1077,   131,   473,   132,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,   569,   133,
     472,   134,   135,   757,   136,   857,  1128,   286,   638,   605,
     288,   137,   138,   139,    99,   709,   117,    65,   118,   119,
     123,   124,   888,  1365,   120,   140,   121,    24,    25,  1016,
    1024,   900,   125,   126,   908,  1127,  1004,  1322,   280,   127,
      31,    32,    33,    34,   580,    40,   504,   860,  1033,    41,
     511,   656,   412,  1228,   587,   966,   582,    26,    27,    28,
     122,   128,  1258,   150,   129,   130,    43,   963,   131,   773,
     132,   876,  1028,  1294,   974,   584,  1017,  1018,  1065,   841,
    1167,   739,   133,  1048,   134,   135,  1363,   136,   975,   658,
    1084,  1267,  1086,     0,   137,   138,   139,     0,     0,   117,
      65,   118,   119,   123,   124,     0,     0,   120,   140,   121,
      24,    25,     0,     0,     0,   125,   126,     0,     0,     0,
       0,     0,   127,    31,    32,    33,    34,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      26,    27,    28,   122,   128,     0,     0,   129,   130,     0,
       0,   131,     0,   132,     0,     0,     0,     0,     0,  1075,
    1076,     0,     0,     0,     0,   133,     0,   134,   135,     0,
     136,    48,    49,     0,     0,     0,     0,   137,   138,   139,
       0,     0,  -115,  -115,  -115,     0,   123,   124,     0,     0,
       0,   140,     0,   646,   647,   648,   649,   650,   125,   126,
       0,     0,     0,     0,     0,   127,    31,    32,    33,    34,
       0,     0,     0,   117,    65,   118,   119,     0,  1333,     0,
       0,   120,     0,   121,    24,    25,     0,   128,     0,     0,
     129,   130,   651,   652,   131,     0,   132,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   133,     0,
     134,   135,     0,   136,    26,    27,    28,   122,     0,     0,
     137,   138,   139,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   140,     0,     0,     0,     0,     0,
       0,  1334,     0,   117,    65,   118,   119,     0,     0,     0,
       0,   120,     0,   121,    24,    25,     0,     0,     0,     0,
     123,   124,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   125,   126,     0,     0,     0,     0,     0,   127,
      31,    32,    33,    34,    26,    27,    28,   122,     0,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   128,     0,     0,   129,   130,     0,     0,   131,     0,
     132,     0,     0,     0,     0,     0,     0,     0,     0,   838,
       0,     0,   133,     0,   134,   135,     0,   136,     0,     0,
     123,   124,     0,     0,   137,   138,   139,     0,     0,     0,
       0,     0,   125,   126,   839,     0,     0,     0,   140,   127,
      31,    32,    33,    34,     0,     0,     0,   117,    65,   118,
     119,     0,     0,     0,     0,   120,     0,   121,    24,    25,
       0,   128,     0,     0,   129,   130,     0,     0,   131,     0,
     132,   595,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   133,     0,   134,   135,     0,   136,    26,    27,
      28,   122,     0,     0,   137,   138,   139,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   140,     0,
       0,     0,     0,     0,     0,     0,     0,   117,    65,   118,
     119,     0,     0,     0,     0,   120,     0,   121,    24,    25,
       0,     0,     0,     0,   123,   124,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   125,   126,     0,     0,
       0,     0,     0,   127,    31,    32,    33,    34,    26,    27,
      28,   122,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   826,     0,   128,     0,     0,   129,   130,
       0,     0,   131,     0,   132,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   133,     0,   134,   135,
       0,   136,     0,     0,   123,   124,     0,     0,   137,   138,
     139,     0,     0,     0,     0,     0,   125,   126,     0,     0,
       0,     0,   140,   127,    31,    32,    33,    34,     0,     0,
       0,   117,    65,   118,   119,     0,     0,     0,     0,   120,
       0,   121,    24,    25,     0,   128,     0,     0,   129,   130,
     873,     0,   131,     0,   132,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   133,     0,   134,   135,
       0,   136,    26,    27,    28,   122,     0,     0,   137,   138,
     139,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   140,     0,     0,     0,     0,     0,     0,     0,
       0,   117,    65,   118,   119,     0,     0,     0,     0,   120,
       0,   121,    24,    25,     0,     0,     0,     0,   123,   124,
       0,     0,     0,  1348,  1349,     0,     0,     0,     0,  1053,
     125,   126,     0,     0,     0,     0,     0,   127,    31,    32,
      33,    34,    26,    27,    28,   122,     0,   310,   311,   312,
     313,   314,   315,   316,   317,   318,   319,   320,   321,   128,
       0,     0,   129,   130,     0,     0,   131,   322,   132,     0,
       0,     0,     0,     0,  1350,     0,     0,   324,     0,     0,
     133,     0,   134,   135,     0,   136,     0,     0,   123,   124,
       0,     0,   137,   138,   139,     0,     0,     0,     0,  1055,
     125,   126,     0,     0,     0,     0,   140,   127,    31,    32,
      33,    34,     0,     0,     0,   117,    65,   118,   119,     0,
       0,     0,     0,   120,     0,   121,    24,    25,     0,   128,
       0,     0,   129,   130,     0,     0,   131,     0,   132,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     133,     0,   134,   135,     0,   136,    26,    27,    28,   122,
       0,     0,   137,   138,   139,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   140,     0,     0,     0,
       0,     0,     0,     0,     0,   117,    65,   118,   119,     0,
       0,     0,     0,   120,     0,   121,    24,    25,     0,     0,
       0,     0,   123,   124,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1057,   125,   126,     0,     0,     0,     0,
       0,   127,    31,    32,    33,    34,    26,    27,    28,   122,
       0,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   128,     0,     0,   129,   130,     0,     0,
     131,   322,   132,     0,     0,     0,     0,     0,   323,     0,
       0,   324,     0,     0,   133,     0,   134,   135,     0,   136,
       0,     0,   123,   124,     0,     0,   137,   138,   139,     0,
       0,     0,     0,     0,   125,   126,     0,     0,     0,     0,
     140,   127,    31,    32,    33,    34,     0,     0,     0,   117,
      65,   118,   119,     0,     0,     0,     0,   120,     0,   121,
      24,    25,     0,   128,     0,     0,   129,   130,  1059,     0,
     131,     0,   132,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   133,     0,   134,   135,     0,   136,
      26,    27,    28,   122,     0,     0,   137,   138,   139,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     140,     0,     0,     0,     0,     0,     0,     0,     0,   117,
      65,   118,   119,     0,     0,     0,     0,   120,     0,   121,
      24,    25,     0,     0,     0,     0,   123,   124,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  1140,   125,   126,
       0,     0,     0,     0,     0,   127,    31,    32,    33,    34,
      26,    27,    28,   122,     0,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   128,     0,     0,
     129,   130,     0,     0,   131,   322,   132,     0,     0,     0,
       0,     0,  1217,     0,     0,   324,     0,     0,   133,     0,
     134,   135,     0,   136,     0,     0,   123,   124,     0,     0,
     137,   138,   139,     0,     0,     0,     0,  1142,   125,   126,
       0,     0,     0,     0,   140,   127,    31,    32,    33,    34,
       0,     0,     0,   117,    65,   118,   119,     0,     0,     0,
       0,   120,     0,   121,    24,    25,     0,   128,     0,     0,
     129,   130,     0,     0,   131,     0,   132,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   133,     0,
     134,   135,     0,   136,    26,    27,    28,   122,     0,     0,
     137,   138,   139,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   140,     0,     0,     0,     0,     0,
       0,     0,     0,   117,    65,   118,   119,     0,     0,     0,
       0,   120,     0,   121,    24,    25,     0,     0,     0,     0,
     123,   124,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1144,   125,   126,     0,     0,     0,     0,     0,   127,
      31,    32,    33,    34,    26,    27,    28,   122,     0,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   319,   320,
     321,   128,     0,     0,   129,   130,     0,     0,   131,   322,
     132,     0,     0,     0,     0,     0,  1400,     0,     0,   324,
       0,     0,   133,     0,   134,   135,     0,   136,     0,     0,
     123,   124,     0,     0,   137,   138,   139,     0,     0,     0,
       0,     0,   125,   126,     0,     0,     0,     0,   140,   127,
      31,    32,    33,    34,     0,     0,     0,   117,    65,   118,
     119,     0,     0,     0,     0,   120,     0,   121,    24,    25,
       0,   128,     0,     0,   129,   130,  1179,     0,   131,     0,
     132,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   133,     0,   134,   135,     0,   136,    26,    27,
      28,   122,     0,     0,   137,   138,   139,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   140,     0,
       0,     0,     0,     0,     0,     0,     0,   117,    65,   118,
     119,     0,     0,     0,     0,   120,     0,   121,   209,    25,
       0,     0,     0,     0,   123,   124,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   125,   126,     0,     0,
       0,     0,     0,   127,    31,    32,    33,    34,    26,    27,
      28,   122,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   128,     0,     0,   129,   130,
       0,     0,   131,     0,   132,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   133,     0,   134,   135,
       0,   136,     0,     0,   123,   124,     0,     0,   137,   138,
     139,     0,     0,     0,     0,     0,   125,   126,     0,     0,
       0,     0,   140,   127,    31,    32,    33,    34,     0,     0,
       0,   117,    65,   118,   119,     0,     0,     0,     0,   120,
       0,   121,    24,    25,     0,   128,     0,     0,   129,   130,
       0,     0,   131,     0,   132,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   133,     0,   134,   135,
       0,   136,    26,    27,    28,   122,     0,     0,   137,   138,
     139,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   140,     0,     0,     0,     0,     0,     0,     0,
       0,   117,    65,   118,   119,     0,     0,     0,     0,   120,
       0,   121,    24,    25,     0,     0,     0,     0,   123,   124,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     125,   126,     0,     0,     0,     0,     0,   127,    31,    32,
      33,    34,    26,    27,    28,   122,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   128,
       0,     0,   129,  1012,     0,     0,   131,     0,   132,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     133,     0,   134,   135,     0,   136,     0,     0,   123,   124,
       0,     0,   137,   138,   139,     0,     0,     0,     0,     0,
     125,   126,     0,     0,     0,     0,   140,   127,    31,    32,
      33,    34,     0,     0,     0,   117,    65,   118,   119,     0,
       0,     0,     0,   120,     0,   121,    24,    25,     0,   128,
       0,     0,   129,  1272,     0,     0,   131,     0,   132,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     133,     0,   134,   135,     0,   136,    26,    27,    28,   122,
       0,     0,   137,   138,   139,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   140,     0,     0,     0,
       0,     0,     0,     0,     0,   117,    65,   118,   119,     0,
       0,     0,     0,   120,     0,   121,    24,    25,     0,     0,
       0,     0,   123,   124,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   125,   126,     0,     0,     0,     0,
       0,   127,    31,    32,    33,    34,    26,    27,    28,   122,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   128,     0,     0,   129,  1369,     0,     0,
     131,     0,   132,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   133,     0,   134,   135,     0,   136,
       0,     0,   123,   124,     0,     0,   137,   138,   139,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     140,   127,    31,    32,    33,    34,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   128,     0,     0,   129,   130,     0,     0,
     131,     0,   132,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   133,     0,   134,   135,     0,   136,
       0,   615,     0,   665,     0,     0,   137,   138,   139,  -512,
    -512,  -512,  -512,     0,     0,     0,     4,     0,     0,   666,
     140,     0,   571,     0,     0,   615,     0,   665,     0,     0,
       0,     0,     0,  -512,  -512,  -512,  -512,     0,     0,     0,
       4,   574,     0,     0,     0,     0,   571,   667,     0,   668,
       0,     0,   669,     0,     0,     0,     0,     0,     0,     0,
     670,   671,   672,     0,     0,   574,     0,     0,     0,   673,
     674,   667,   675,   668,     0,   676,   669,     0,     0,     0,
       0,     0,     0,     0,   670,   671,   672,     0,    29,     6,
       0,     0,   677,   673,   674,     0,   675,     0,     0,   676,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     6,     0,     0,   677,  1224,   615,     0,
     665,     7,     0,     0,     0,     0,  -512,  -512,  -512,  -512,
       0,     0,     0,     4,     0,     0,     0,   678,     0,   571,
       0,     0,     0,     0,     0,     7,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   574,     0,
       0,   678,     0,     0,   667,     0,   668,     0,     0,   669,
       0,     0,     0,   615,     0,   665,     0,   670,   671,   672,
       0,  -512,  -512,  -512,  -512,     0,   673,   674,     4,   675,
       0,     0,   676,   912,   571,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     6,     0,     0,   677,
       0,     0,     0,   574,     0,     0,     0,     0,     0,   667,
    1182,   668,     0,     0,   669,     0,     0,     0,     0,     0,
       0,     0,   670,   671,   672,     0,     0,     0,     7,     0,
       0,   673,   674,     0,   675,     0,     0,   676,   615,     0,
     665,     0,     0,     0,     0,     0,  -512,  -512,  -512,  -512,
      29,     6,     0,     4,   677,     0,     0,     0,     0,   571,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   810,
       0,     0,   811,     0,     0,     0,     0,     0,   574,     0,
       0,     0,     0,     7,     0,     0,   668,     0,     0,   669,
       0,     0,    48,    49,     0,     0,     0,   670,   671,   672,
       0,     0,     0,  -115,  -115,  -115,   673,   674,     0,   675,
       0,     0,   676,     0,   646,   647,   648,   649,   650,     0,
       0,     0,     0,     0,     0,    29,     6,    51,     0,   677,
     812,     0,     0,     0,     0,     0,     0,    52,     0,    53,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   651,   652,   234,   235,     0,     7,   236,
     237,     0,   238,     0,   239,     0,     0,   240,     0,   241,
     242,     0,    30,     0,   243,     0,     0,   244,     0,     0,
      35,     0,     0,     0,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   234,   235,     0,     0,   236,   237,     0,   238,     0,
     239,     0,     0,   240,     0,   241,     0,     0,     0,     0,
     243,     0,     0,   244,     0,     0,     0,     0,     0,     0,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   234,   235,     0,
       0,   236,   237,     0,   238,     0,   239,     0,     0,   240,
       0,   241,     0,     0,     0,     0,   243,     0,     0,   244,
       0,     0,     0,     0,     0,     0,     0,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   234,   235,     0,     0,   236,   237,     0,
     238,     0,   239,     0,     0,   240,     0,   241,     0,     0,
       0,     0,   243,     0,     0,   244,     0,     0,     0,     0,
       0,     0,     0,     0,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   257,   258,   259,   260,   261,   234,
     235,     0,     0,   236,   237,     0,   238,     0,   239,     0,
       0,   240,     0,   241,     0,   234,   235,     0,   243,   236,
     237,     0,   238,     0,   239,     0,     0,   240,     0,   241,
       0,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,     0,     0,     0,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   234,     0,     0,     0,   236,   237,     0,   238,     0,
     239,     0,     0,   240,     0,   241,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261
  };

  const short
  parser::yycheck_[] =
  {
      80,   144,     2,    22,   141,    81,    82,   502,   150,   338,
     602,   602,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    94,   476,   602,   105,    45,    93,    94,    71,
     666,   602,    98,   889,   620,   790,   903,   904,   502,   291,
     553,   153,  1005,   112,   911,   895,    71,  1010,     2,   645,
     130,    93,    94,   932,   134,  1028,    98,    14,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,   141,    71,
     403,    14,    14,    14,    14,   404,    14,   634,   482,  1231,
       2,   100,    14,   102,    42,   470,   162,    14,    41,   162,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
       2,    14,    14,   144,   149,    30,    91,   602,   674,   154,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
     196,    14,     0,   196,    31,    66,     0,    15,   149,   381,
    1117,    24,   153,    21,   153,    23,   177,   178,   602,  1318,
      24,   221,   222,    31,   780,  1012,    73,    74,   224,   602,
     230,   231,   232,   106,    42,    33,     2,    14,  1008,    42,
    1339,   146,    42,    14,   149,    43,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,  1328,   207,   208,   104,
     766,   211,   212,   213,   770,    57,   216,   217,   218,   219,
     220,   574,    30,   151,   830,   524,    57,    14,   228,   112,
     113,   758,   606,   279,   145,   342,   146,   164,   146,   228,
     153,   151,   288,   156,   146,   600,   808,   550,   160,   151,
      14,    14,   551,   303,   304,   305,   306,   793,   106,   154,
     808,  1218,   674,   675,   146,   301,    75,   149,   145,   491,
     147,   837,   141,   323,   324,   325,    57,   119,   117,   329,
     330,   164,  1119,  1226,    48,    49,   125,   145,   119,   301,
     138,   341,   140,   151,  1114,   377,   104,   863,   151,   342,
    1149,   151,   160,   156,   744,   355,   301,   123,   124,   421,
     160,   149,   128,   129,   754,   131,   132,   133,  1265,   135,
     136,   137,   138,   139,   374,   141,   164,   143,  1124,   301,
    1126,    94,  1215,   383,    14,    15,    16,   146,   119,    14,
     149,   824,   146,   808,    42,   149,    57,   347,   348,   349,
     350,   351,   352,   353,   354,   160,   356,   357,   358,   359,
     360,   361,   362,   363,   364,   365,   366,   367,   368,   369,
     370,   371,   372,   373,   808,  1176,  1101,  1178,   417,   418,
     792,   793,   146,   433,    57,   808,   436,   499,   377,  1322,
    1323,    66,   804,    68,    57,   149,    13,    14,    15,    16,
     154,  1284,   102,  1286,    21,   146,    23,   407,   119,   631,
     632,  1358,   153,    88,    89,   971,   102,  1364,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,  1271,   119,   163,   148,   149,
     150,    42,   149,   322,   170,    14,   119,   154,    14,   139,
     506,   814,   148,   506,   150,   145,    76,   517,   112,   113,
     145,   829,   148,   149,   150,   291,   292,    14,   836,   529,
     156,   531,   149,   895,    18,   838,   839,   154,   841,    37,
      38,   149,    26,    27,    28,    29,   154,   909,   610,    33,
     149,   613,   102,   103,   153,   618,   476,    66,   558,    68,
      31,    48,    49,   555,   556,  1352,   869,  1354,   149,   555,
     556,   149,  1084,   523,   141,   571,   154,   144,   571,    88,
      89,   144,   502,   149,   523,   149,  1084,  1374,   154,  1376,
     154,   653,   592,   555,   556,   162,   187,   188,   165,   162,
     167,    85,   476,   153,   145,   107,   108,   174,   175,   176,
     151,   161,   175,   154,   177,   178,  1128,  1128,   149,   160,
     143,   146,   106,   154,   147,   148,  1182,   150,   502,   152,
    1128,    96,    97,    18,   476,   149,   145,  1128,   102,   103,
     154,    26,    27,    28,    29,  1151,  1008,   154,    33,  1155,
    1156,   637,   638,   153,   146,  1017,  1018,   149,   142,  1165,
     502,   149,   662,   118,   187,   188,   154,   146,   618,  1084,
     149,   728,    39,  1185,   147,   637,   638,    44,    45,   618,
     149,   144,   602,    14,   146,   154,  1242,   149,    15,   153,
     640,    58,    24,    25,    21,   645,    23,   161,    83,   162,
    1084,   640,   165,   149,   146,    14,   645,   149,   154,   958,
     476,  1084,   175,  1128,   177,   178,   146,   717,   814,   149,
     486,   106,    18,    55,    56,   491,   975,   727,   602,    14,
      26,    27,    28,    29,   500,   728,   502,    33,    18,   106,
      73,    74,   838,   839,  1128,   841,    26,    27,    28,    29,
     143,   149,  1114,    33,   147,  1128,   154,   142,   744,   152,
     602,   149,   146,    34,   760,   149,   154,   760,   754,    14,
    1185,   138,   772,   869,   774,   146,   146,   840,   149,   149,
     602,   781,   744,   177,   178,    65,    14,    83,   120,   121,
     122,   123,   754,   142,   187,   188,    18,    48,    49,    50,
     164,  1185,   164,   803,    26,    27,    28,    29,    37,    38,
     106,    33,  1185,   145,    21,    13,    14,    15,    16,    77,
      78,    79,   822,    21,   164,    23,   106,    22,   890,    48,
      49,    50,   832,   833,   391,   392,   602,   548,   549,    24,
      25,   188,   188,    72,   188,   611,   142,   329,   330,   849,
     850,    14,   154,    82,    14,    84,  1362,    14,    14,   859,
      14,    46,   142,    14,    14,   865,    51,    52,    53,    54,
      55,    56,   145,    32,   143,   145,  1382,    14,   147,   148,
     880,   150,    22,   152,   106,   145,   886,   837,   808,   145,
    1396,    26,    27,    28,    29,   145,   145,   663,   837,    24,
      25,   166,   145,   903,   904,   905,    54,    55,    56,    94,
     145,   911,   154,   863,   183,   184,   185,   186,   187,   188,
     142,   148,   149,   150,   863,    26,    27,    28,    29,    54,
      55,    56,   932,   933,   808,   120,   121,   122,   123,   149,
     940,   156,   154,   943,   944,   145,   946,   947,   948,   949,
     950,   148,   149,   150,   156,    14,    14,   145,    17,   149,
     145,   154,   156,   729,    14,   731,   808,   166,   149,   969,
     145,   142,   120,   121,   122,   123,    14,   161,    37,    38,
     154,   734,   735,   736,   161,   985,   808,   987,   161,    48,
      49,    50,    14,   146,    14,   120,   121,   122,   123,   124,
      59,    60,    61,    62,    63,   154,   151,   149,   166,   153,
     146,   149,  1012,    72,   146,   146,    75,   146,   146,   146,
     145,   151,  1022,    82,    35,    84,    37,    38,   146,    40,
     146,   145,    24,    25,  1034,  1035,  1036,  1037,   153,    98,
      99,   188,   808,   166,   166,    14,   154,    14,  1044,    24,
      25,  1044,    14,  1053,   154,  1055,   154,  1057,   117,    24,
      25,    72,    54,    55,    56,    57,   125,    14,   156,    14,
     151,    82,   891,    84,    76,  1075,  1076,   146,   144,    54,
      55,    56,  1082,   166,   149,   153,   149,  1087,  1088,    54,
      55,    56,   154,    14,   142,   154,   162,    14,    14,   165,
     146,   146,    14,    95,    24,    25,   145,   173,    14,   175,
     153,   177,   178,   879,   154,    66,    14,   108,    14,  1119,
     145,  1121,   145,    14,  1124,    14,  1126,   119,   120,   121,
     122,   123,   145,  1129,    54,    55,    56,   145,   154,   146,
    1140,   145,  1142,    66,  1144,   120,   121,   122,   123,  1149,
    1150,   145,  1152,   145,  1154,   120,   121,   122,   123,    14,
     145,    66,    66,   145,  1084,    20,   145,   156,   145,   156,
     145,   159,  1172,   146,   145,   149,  1176,   145,  1178,   145,
     145,    88,    37,    38,   145,   105,  1186,   145,   145,  1189,
     154,  1191,   145,    48,    49,    50,   156,   117,   154,    14,
     120,   121,   122,   123,   156,   125,   156,   104,  1128,    64,
    1084,    66,   154,   146,    69,    70,    71,  1217,   144,   104,
     146,   104,    77,    78,    79,   145,   992,   993,   994,   995,
     996,   997,   998,   999,   104,   153,   162,   166,   149,   165,
      14,   154,  1084,    98,    99,   145,   172,   173,   147,   175,
     145,   177,   178,   145,  1128,   145,    14,   112,   113,   114,
     115,   116,  1084,    14,   146,  1185,   146,   153,   145,   145,
     145,   145,  1272,  1092,  1093,  1094,  1095,  1096,  1097,  1098,
    1099,   146,  1345,   207,   208,   154,  1128,   211,   212,   213,
     146,  1291,   216,   217,   218,   219,   220,   146,   146,   154,
     154,   146,   149,    14,   154,  1305,  1128,   146,  1308,   164,
     145,  1185,    14,   156,   154,   146,   157,   147,  1084,  1319,
     156,   149,    14,   156,   153,  1200,  1326,  1202,  1203,  1204,
    1205,  1206,  1207,  1208,  1100,   153,  1102,  1103,  1104,  1105,
    1106,  1107,  1108,  1185,  1344,   154,   146,   146,  1348,  1349,
    1350,   146,  1352,   154,  1354,   146,   146,    14,   156,  1359,
     149,   146,  1128,  1185,   146,    64,    90,    19,   156,  1369,
     157,   157,   145,   157,  1374,   154,  1376,   146,   161,   146,
     146,  1200,   146,  1202,  1203,  1204,  1205,  1206,  1207,  1208,
      14,  1391,   146,   154,   146,  1345,   146,   154,   154,   154,
    1400,   154,   146,    67,   149,   145,  1345,   145,   154,   146,
     153,  1411,   146,   146,   146,   154,   146,    80,  1418,  1185,
     156,   146,   153,   347,   348,   349,   350,   351,   352,   353,
     354,   154,   356,   357,   358,   359,   360,   361,   362,   363,
     364,   365,   366,   367,   368,   369,   370,   371,   372,   373,
      13,    14,    15,    16,   154,   146,   154,  1369,    21,   143,
      23,    24,    25,   147,   148,  1231,   150,   154,   152,    32,
     145,   155,  1384,   157,  1092,  1093,  1094,  1095,  1096,  1097,
    1098,  1099,   154,   407,   154,   146,   154,  1399,   154,   154,
      67,    54,    55,    56,    57,  1407,   154,   181,   182,   183,
     184,   185,   186,   187,   188,  1417,    80,   156,   146,   153,
      73,    74,  1424,   153,  1426,   146,  1428,   146,   146,  1431,
     146,  1433,   149,  1435,   154,  1437,   146,  1439,   153,  1441,
     146,   149,   146,    13,    14,    15,    16,   100,   101,   146,
     146,    21,   146,    23,    24,    25,   149,   146,   153,   112,
     113,   146,  1318,   146,   154,   153,   119,   120,   121,   122,
     123,   149,  1328,   149,   149,   149,   149,    47,   149,   149,
     146,   100,   102,  1339,    54,    55,    56,    57,   141,   936,
     270,   144,   145,   491,   147,   148,   380,   150,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,   500,   162,
     378,   164,   165,   638,   167,   757,  1027,   180,   567,   545,
     182,   174,   175,   176,    53,   611,    13,    14,    15,    16,
     100,   101,   786,  1347,    21,   188,    23,    24,    25,   904,
     909,   793,   112,   113,   804,  1022,   895,  1283,   171,   119,
     120,   121,   122,   123,   502,     6,   409,   760,   922,     6,
     420,   588,   298,  1134,   507,   868,   502,    54,    55,    56,
      57,   141,  1170,    84,   144,   145,     6,   863,   148,   662,
     150,   151,   919,  1229,   879,   502,    73,    74,   961,   731,
    1063,   630,   162,   940,   164,   165,  1344,   167,   880,   592,
     977,  1185,   980,    -1,   174,   175,   176,    -1,    -1,    13,
      14,    15,    16,   100,   101,    -1,    -1,    21,   188,    23,
      24,    25,    -1,    -1,    -1,   112,   113,    -1,    -1,    -1,
      -1,    -1,   119,   120,   121,   122,   123,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    55,    56,    57,   141,    -1,    -1,   144,   145,    -1,
      -1,   148,    -1,   150,    -1,    -1,    -1,    -1,    -1,    73,
      74,    -1,    -1,    -1,    -1,   162,    -1,   164,   165,    -1,
     167,    37,    38,    -1,    -1,    -1,    -1,   174,   175,   176,
      -1,    -1,    48,    49,    50,    -1,   100,   101,    -1,    -1,
      -1,   188,    -1,    59,    60,    61,    62,    63,   112,   113,
      -1,    -1,    -1,    -1,    -1,   119,   120,   121,   122,   123,
      -1,    -1,    -1,    13,    14,    15,    16,    -1,    18,    -1,
      -1,    21,    -1,    23,    24,    25,    -1,   141,    -1,    -1,
     144,   145,    98,    99,   148,    -1,   150,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   162,    -1,
     164,   165,    -1,   167,    54,    55,    56,    57,    -1,    -1,
     174,   175,   176,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   188,    -1,    -1,    -1,    -1,    -1,
      -1,    81,    -1,    13,    14,    15,    16,    -1,    -1,    -1,
      -1,    21,    -1,    23,    24,    25,    -1,    -1,    -1,    -1,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   112,   113,    -1,    -1,    -1,    -1,    -1,   119,
     120,   121,   122,   123,    54,    55,    56,    57,    -1,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   141,    -1,    -1,   144,   145,    -1,    -1,   148,    -1,
     150,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   156,
      -1,    -1,   162,    -1,   164,   165,    -1,   167,    -1,    -1,
     100,   101,    -1,    -1,   174,   175,   176,    -1,    -1,    -1,
      -1,    -1,   112,   113,   181,    -1,    -1,    -1,   188,   119,
     120,   121,   122,   123,    -1,    -1,    -1,    13,    14,    15,
      16,    -1,    -1,    -1,    -1,    21,    -1,    23,    24,    25,
      -1,   141,    -1,    -1,   144,   145,    -1,    -1,   148,    -1,
     150,   151,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   162,    -1,   164,   165,    -1,   167,    54,    55,
      56,    57,    -1,    -1,   174,   175,   176,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   188,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,    14,    15,
      16,    -1,    -1,    -1,    -1,    21,    -1,    23,    24,    25,
      -1,    -1,    -1,    -1,   100,   101,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   112,   113,    -1,    -1,
      -1,    -1,    -1,   119,   120,   121,   122,   123,    54,    55,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   139,    -1,   141,    -1,    -1,   144,   145,
      -1,    -1,   148,    -1,   150,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   162,    -1,   164,   165,
      -1,   167,    -1,    -1,   100,   101,    -1,    -1,   174,   175,
     176,    -1,    -1,    -1,    -1,    -1,   112,   113,    -1,    -1,
      -1,    -1,   188,   119,   120,   121,   122,   123,    -1,    -1,
      -1,    13,    14,    15,    16,    -1,    -1,    -1,    -1,    21,
      -1,    23,    24,    25,    -1,   141,    -1,    -1,   144,   145,
     146,    -1,   148,    -1,   150,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   162,    -1,   164,   165,
      -1,   167,    54,    55,    56,    57,    -1,    -1,   174,   175,
     176,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   188,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    13,    14,    15,    16,    -1,    -1,    -1,    -1,    21,
      -1,    23,    24,    25,    -1,    -1,    -1,    -1,   100,   101,
      -1,    -1,    -1,   102,   103,    -1,    -1,    -1,    -1,   111,
     112,   113,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,
     122,   123,    54,    55,    56,    57,    -1,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   141,
      -1,    -1,   144,   145,    -1,    -1,   148,   146,   150,    -1,
      -1,    -1,    -1,    -1,   153,    -1,    -1,   156,    -1,    -1,
     162,    -1,   164,   165,    -1,   167,    -1,    -1,   100,   101,
      -1,    -1,   174,   175,   176,    -1,    -1,    -1,    -1,   111,
     112,   113,    -1,    -1,    -1,    -1,   188,   119,   120,   121,
     122,   123,    -1,    -1,    -1,    13,    14,    15,    16,    -1,
      -1,    -1,    -1,    21,    -1,    23,    24,    25,    -1,   141,
      -1,    -1,   144,   145,    -1,    -1,   148,    -1,   150,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     162,    -1,   164,   165,    -1,   167,    54,    55,    56,    57,
      -1,    -1,   174,   175,   176,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   188,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    13,    14,    15,    16,    -1,
      -1,    -1,    -1,    21,    -1,    23,    24,    25,    -1,    -1,
      -1,    -1,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   111,   112,   113,    -1,    -1,    -1,    -1,
      -1,   119,   120,   121,   122,   123,    54,    55,    56,    57,
      -1,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   141,    -1,    -1,   144,   145,    -1,    -1,
     148,   146,   150,    -1,    -1,    -1,    -1,    -1,   153,    -1,
      -1,   156,    -1,    -1,   162,    -1,   164,   165,    -1,   167,
      -1,    -1,   100,   101,    -1,    -1,   174,   175,   176,    -1,
      -1,    -1,    -1,    -1,   112,   113,    -1,    -1,    -1,    -1,
     188,   119,   120,   121,   122,   123,    -1,    -1,    -1,    13,
      14,    15,    16,    -1,    -1,    -1,    -1,    21,    -1,    23,
      24,    25,    -1,   141,    -1,    -1,   144,   145,   146,    -1,
     148,    -1,   150,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   162,    -1,   164,   165,    -1,   167,
      54,    55,    56,    57,    -1,    -1,   174,   175,   176,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     188,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,
      14,    15,    16,    -1,    -1,    -1,    -1,    21,    -1,    23,
      24,    25,    -1,    -1,    -1,    -1,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,   112,   113,
      -1,    -1,    -1,    -1,    -1,   119,   120,   121,   122,   123,
      54,    55,    56,    57,    -1,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   141,    -1,    -1,
     144,   145,    -1,    -1,   148,   146,   150,    -1,    -1,    -1,
      -1,    -1,   153,    -1,    -1,   156,    -1,    -1,   162,    -1,
     164,   165,    -1,   167,    -1,    -1,   100,   101,    -1,    -1,
     174,   175,   176,    -1,    -1,    -1,    -1,   111,   112,   113,
      -1,    -1,    -1,    -1,   188,   119,   120,   121,   122,   123,
      -1,    -1,    -1,    13,    14,    15,    16,    -1,    -1,    -1,
      -1,    21,    -1,    23,    24,    25,    -1,   141,    -1,    -1,
     144,   145,    -1,    -1,   148,    -1,   150,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   162,    -1,
     164,   165,    -1,   167,    54,    55,    56,    57,    -1,    -1,
     174,   175,   176,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   188,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    13,    14,    15,    16,    -1,    -1,    -1,
      -1,    21,    -1,    23,    24,    25,    -1,    -1,    -1,    -1,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,   112,   113,    -1,    -1,    -1,    -1,    -1,   119,
     120,   121,   122,   123,    54,    55,    56,    57,    -1,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   141,    -1,    -1,   144,   145,    -1,    -1,   148,   146,
     150,    -1,    -1,    -1,    -1,    -1,   153,    -1,    -1,   156,
      -1,    -1,   162,    -1,   164,   165,    -1,   167,    -1,    -1,
     100,   101,    -1,    -1,   174,   175,   176,    -1,    -1,    -1,
      -1,    -1,   112,   113,    -1,    -1,    -1,    -1,   188,   119,
     120,   121,   122,   123,    -1,    -1,    -1,    13,    14,    15,
      16,    -1,    -1,    -1,    -1,    21,    -1,    23,    24,    25,
      -1,   141,    -1,    -1,   144,   145,   146,    -1,   148,    -1,
     150,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   162,    -1,   164,   165,    -1,   167,    54,    55,
      56,    57,    -1,    -1,   174,   175,   176,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   188,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,    14,    15,
      16,    -1,    -1,    -1,    -1,    21,    -1,    23,    24,    25,
      -1,    -1,    -1,    -1,   100,   101,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   112,   113,    -1,    -1,
      -1,    -1,    -1,   119,   120,   121,   122,   123,    54,    55,
      56,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   141,    -1,    -1,   144,   145,
      -1,    -1,   148,    -1,   150,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   162,    -1,   164,   165,
      -1,   167,    -1,    -1,   100,   101,    -1,    -1,   174,   175,
     176,    -1,    -1,    -1,    -1,    -1,   112,   113,    -1,    -1,
      -1,    -1,   188,   119,   120,   121,   122,   123,    -1,    -1,
      -1,    13,    14,    15,    16,    -1,    -1,    -1,    -1,    21,
      -1,    23,    24,    25,    -1,   141,    -1,    -1,   144,   145,
      -1,    -1,   148,    -1,   150,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   162,    -1,   164,   165,
      -1,   167,    54,    55,    56,    57,    -1,    -1,   174,   175,
     176,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   188,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    13,    14,    15,    16,    -1,    -1,    -1,    -1,    21,
      -1,    23,    24,    25,    -1,    -1,    -1,    -1,   100,   101,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     112,   113,    -1,    -1,    -1,    -1,    -1,   119,   120,   121,
     122,   123,    54,    55,    56,    57,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   141,
      -1,    -1,   144,   145,    -1,    -1,   148,    -1,   150,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     162,    -1,   164,   165,    -1,   167,    -1,    -1,   100,   101,
      -1,    -1,   174,   175,   176,    -1,    -1,    -1,    -1,    -1,
     112,   113,    -1,    -1,    -1,    -1,   188,   119,   120,   121,
     122,   123,    -1,    -1,    -1,    13,    14,    15,    16,    -1,
      -1,    -1,    -1,    21,    -1,    23,    24,    25,    -1,   141,
      -1,    -1,   144,   145,    -1,    -1,   148,    -1,   150,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     162,    -1,   164,   165,    -1,   167,    54,    55,    56,    57,
      -1,    -1,   174,   175,   176,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   188,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    13,    14,    15,    16,    -1,
      -1,    -1,    -1,    21,    -1,    23,    24,    25,    -1,    -1,
      -1,    -1,   100,   101,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   112,   113,    -1,    -1,    -1,    -1,
      -1,   119,   120,   121,   122,   123,    54,    55,    56,    57,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   141,    -1,    -1,   144,   145,    -1,    -1,
     148,    -1,   150,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   162,    -1,   164,   165,    -1,   167,
      -1,    -1,   100,   101,    -1,    -1,   174,   175,   176,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     188,   119,   120,   121,   122,   123,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   141,    -1,    -1,   144,   145,    -1,    -1,
     148,    -1,   150,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   162,    -1,   164,   165,    -1,   167,
      -1,    18,    -1,    20,    -1,    -1,   174,   175,   176,    26,
      27,    28,    29,    -1,    -1,    -1,    33,    -1,    -1,    36,
     188,    -1,    39,    -1,    -1,    18,    -1,    20,    -1,    -1,
      -1,    -1,    -1,    26,    27,    28,    29,    -1,    -1,    -1,
      33,    58,    -1,    -1,    -1,    -1,    39,    64,    -1,    66,
      -1,    -1,    69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      77,    78,    79,    -1,    -1,    58,    -1,    -1,    -1,    86,
      87,    64,    89,    66,    -1,    92,    69,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    77,    78,    79,    -1,   105,   106,
      -1,    -1,   109,    86,    87,    -1,    89,    -1,    -1,    92,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   105,   106,    -1,    -1,   109,   110,    18,    -1,
      20,   138,    -1,    -1,    -1,    -1,    26,    27,    28,    29,
      -1,    -1,    -1,    33,    -1,    -1,    -1,   154,    -1,    39,
      -1,    -1,    -1,    -1,    -1,   138,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    58,    -1,
      -1,   154,    -1,    -1,    64,    -1,    66,    -1,    -1,    69,
      -1,    -1,    -1,    18,    -1,    20,    -1,    77,    78,    79,
      -1,    26,    27,    28,    29,    -1,    86,    87,    33,    89,
      -1,    -1,    92,    93,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   105,   106,    -1,    -1,   109,
      -1,    -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    64,
      65,    66,    -1,    -1,    69,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    77,    78,    79,    -1,    -1,    -1,   138,    -1,
      -1,    86,    87,    -1,    89,    -1,    -1,    92,    18,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    26,    27,    28,    29,
     105,   106,    -1,    33,   109,    -1,    -1,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    14,
      -1,    -1,    17,    -1,    -1,    -1,    -1,    -1,    58,    -1,
      -1,    -1,    -1,   138,    -1,    -1,    66,    -1,    -1,    69,
      -1,    -1,    37,    38,    -1,    -1,    -1,    77,    78,    79,
      -1,    -1,    -1,    48,    49,    50,    86,    87,    -1,    89,
      -1,    -1,    92,    -1,    59,    60,    61,    62,    63,    -1,
      -1,    -1,    -1,    -1,    -1,   105,   106,    72,    -1,   109,
      75,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    98,    99,   143,   144,    -1,   138,   147,
     148,    -1,   150,    -1,   152,    -1,    -1,   155,    -1,   157,
     158,    -1,   117,    -1,   162,    -1,    -1,   165,    -1,    -1,
     125,    -1,    -1,    -1,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   143,   144,    -1,    -1,   147,   148,    -1,   150,    -1,
     152,    -1,    -1,   155,    -1,   157,    -1,    -1,    -1,    -1,
     162,    -1,    -1,   165,    -1,    -1,    -1,    -1,    -1,    -1,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   143,   144,    -1,
      -1,   147,   148,    -1,   150,    -1,   152,    -1,    -1,   155,
      -1,   157,    -1,    -1,    -1,    -1,   162,    -1,    -1,   165,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   143,   144,    -1,    -1,   147,   148,    -1,
     150,    -1,   152,    -1,    -1,   155,    -1,   157,    -1,    -1,
      -1,    -1,   162,    -1,    -1,   165,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   143,
     144,    -1,    -1,   147,   148,    -1,   150,    -1,   152,    -1,
      -1,   155,    -1,   157,    -1,   143,   144,    -1,   162,   147,
     148,    -1,   150,    -1,   152,    -1,    -1,   155,    -1,   157,
      -1,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,    -1,    -1,    -1,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   143,    -1,    -1,    -1,   147,   148,    -1,   150,    -1,
     152,    -1,    -1,   155,    -1,   157,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188
  };

  const short
  parser::yystos_[] =
  {
       0,   192,   193,     0,    33,    43,   106,   138,   140,   194,
     195,   196,   198,   206,   223,   228,   232,   237,   270,   343,
     345,   399,   199,   233,    24,    25,    54,    55,    56,   105,
     117,   120,   121,   122,   123,   125,   145,   205,   260,   261,
     353,   367,   369,   400,   401,   238,    14,    35,    37,    38,
      40,    72,    82,    84,   197,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,    14,   201,   202,   203,   204,
      14,   354,    24,    57,   119,   279,    57,   119,   280,   368,
     160,   262,   263,   264,   265,    14,    24,   398,   204,   241,
     244,    42,   207,   344,   346,   224,    14,    76,   284,   284,
      31,   200,   149,    42,   151,   156,   234,    54,    55,   260,
     355,   356,   357,   146,   118,   370,   371,    13,    15,    16,
      21,    23,    57,   100,   101,   112,   113,   119,   141,   144,
     145,   148,   150,   162,   164,   165,   167,   174,   175,   176,
     188,   204,   400,   447,   479,   502,   503,   507,   262,   262,
     398,   154,   239,   153,   242,   147,   229,    14,    57,    95,
     119,   205,   335,   336,   337,   339,   340,   341,   342,   356,
     357,   342,    14,    14,   153,   156,    54,    55,   124,   205,
     260,   277,   278,    14,   201,    34,   203,    14,    14,   502,
     142,   209,   164,    57,   119,   358,   358,    57,   119,   372,
     164,    21,    22,   188,   195,   195,   188,   195,   195,    24,
     502,   195,   195,   195,   502,   505,   195,   195,   195,   195,
     195,   164,   160,   195,   263,   264,   507,   188,   195,   447,
     102,   103,   153,   161,   143,   144,   147,   148,   150,   152,
     155,   157,   158,   162,   165,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   265,    14,   243,   244,   154,    14,   230,   231,
     208,   262,   264,   338,   335,    14,   347,   348,   349,   335,
     347,   225,   271,    14,    14,    14,   279,    14,   280,   274,
      32,   145,   145,   214,    14,   359,   360,   264,   373,   374,
     378,   379,    22,   145,   145,   145,   145,   479,   503,   503,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   146,   153,   156,   446,   503,   503,   503,   149,
     164,   166,   503,   503,   503,   503,   503,   502,   506,   502,
     504,   145,   204,   479,   502,   502,   502,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   145,   154,   240,   149,   149,   154,
     209,   149,   154,   156,   262,   154,   154,   145,   282,   156,
      14,   275,   276,   262,   145,   286,   195,   210,   211,   348,
      14,   146,   151,   195,   215,   218,   154,   156,   361,   149,
     216,   166,   374,    14,   375,   376,   205,   356,   357,   380,
     381,   383,   502,   502,   502,   502,   507,   502,   502,   502,
     502,   505,   505,   149,   216,   161,   145,   502,   161,   161,
     161,   503,   503,   503,   503,   503,   503,   503,   503,   502,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   502,   142,
     416,   244,   231,   214,   348,   502,   226,    14,   281,   283,
     154,    14,   272,   286,   286,   146,   287,   154,    37,    38,
     146,   149,   219,   220,   151,   248,   249,   250,   251,   252,
     149,   216,   235,   503,   360,   166,   377,   149,   154,   358,
     358,   369,   265,   146,   146,   146,   146,   153,   146,   146,
     166,   502,   166,   151,   483,   484,   485,   502,   146,   153,
     146,   145,    14,   408,   412,   154,    41,   195,   227,   270,
     343,   345,   399,    14,   146,   149,    14,   282,   154,   154,
     195,   289,   290,   292,   293,   212,   213,   211,   156,   217,
      14,   151,    48,    49,    50,   253,    76,   285,   265,   218,
     146,    39,    44,    45,    58,   195,   236,   237,   343,   345,
     350,   386,   395,   399,   424,   435,   264,   376,   382,   502,
     166,   479,   149,   216,   502,   151,   417,   418,   502,   409,
     149,   154,   266,   153,   432,   281,   273,   154,   293,   293,
     252,   149,   216,    14,   394,    18,    85,   142,   195,   198,
     246,   343,   345,   386,   399,   433,   443,   445,   449,   462,
     463,   342,   342,   502,   221,   107,   108,   255,   255,    14,
     264,   351,   352,    14,   246,   247,    59,    60,    61,    62,
      63,    98,    99,   252,   422,   423,   370,   146,   485,   146,
      14,   146,   149,   145,   408,    20,    36,    64,    66,    69,
      77,    78,    79,    86,    87,    89,    92,   109,   154,   195,
     198,   237,   267,   268,   270,   294,   304,   307,   343,   345,
     350,   353,   364,   365,   386,   395,   399,   402,   424,   433,
     444,   464,   490,   496,    14,   282,    83,    83,   265,   290,
     288,   265,   153,    14,    15,    16,   139,   145,    20,    64,
      69,    70,    71,   114,   115,   116,   154,   164,   204,   447,
     479,   480,   449,    26,    27,    28,    29,   154,    66,   464,
     348,   348,   217,   108,    46,    51,    52,    53,    54,    55,
      94,   205,   256,   257,   258,   259,   260,   257,   222,   479,
     149,   154,   436,   396,   397,   480,   428,    14,    14,   265,
     427,   425,   145,   418,   195,   419,   420,   421,   495,   432,
     497,   145,   145,    14,    66,   295,   296,   298,    14,    66,
      68,    88,   145,   305,   306,   307,   311,   312,   313,   320,
     321,   328,   333,   160,   308,   309,   310,   333,   486,    14,
      14,    17,    75,   367,   405,    14,   362,   363,    26,    27,
      28,    29,   145,   154,   291,   146,   139,   245,   502,   451,
     452,   145,   145,   145,   481,   502,   450,   195,   156,   181,
     195,   446,    30,   104,   434,   434,   434,   154,   434,   145,
     145,    54,   259,    57,   119,   254,   259,   254,   217,   156,
     352,   145,   438,   149,   154,   156,   449,   389,   390,   387,
     449,   159,   429,   146,   502,    47,   151,   502,   146,   149,
     145,   482,   432,   502,   491,   145,   145,    88,   295,   145,
     334,   145,   312,    73,    74,   319,   322,   326,   333,    88,
     306,   154,   154,   156,   156,   145,   331,   502,   309,   149,
     154,   156,    93,   195,   267,   487,   490,   496,   154,   403,
     366,   247,   149,   154,   104,   104,   104,   104,   502,   394,
     146,   146,   149,   153,   482,   432,   454,   502,   502,   166,
     149,   482,   480,   247,   247,   447,   247,   145,   145,   145,
     145,   502,   502,   502,    14,    48,    49,   146,   439,   440,
     441,   442,   437,   397,   502,   391,   391,   247,    31,   145,
     147,   426,   146,    14,   421,   483,   154,   498,   146,    14,
      94,   488,   489,    73,    74,   301,   502,   301,   265,    13,
      14,    16,   141,   144,   162,   165,   167,   174,   175,   176,
     314,   315,   316,   507,   322,   326,   147,   148,   149,   150,
     323,   325,   145,   317,   332,   502,   317,    73,    74,   329,
     330,   332,   153,   154,   310,   332,   154,   269,   416,    14,
     384,   385,   404,   363,   145,   145,   145,   145,   146,   245,
     502,   154,   453,    14,   250,   448,   146,   146,   481,   154,
     502,   502,   502,   111,   502,   111,   502,   111,   502,   146,
     502,   146,   146,   149,   216,   441,   154,   149,   154,   154,
     392,   393,   394,   146,    32,    73,    74,   147,   430,   431,
     502,   449,   145,   216,   486,   493,   488,   156,   154,   502,
     146,   502,   195,   195,   195,   195,   195,   195,   195,   195,
     144,   146,   162,   165,   172,   173,   175,   177,   178,   147,
     156,   325,   157,   326,   149,   325,   156,   332,   502,   149,
     318,   153,   333,   333,    91,   146,   149,   331,   266,    14,
     406,   407,   408,   410,   149,   154,   407,   413,   414,   415,
     111,   502,   111,   502,   111,   502,   502,   494,   146,   149,
     153,   466,   156,   264,   154,   457,   458,   502,   146,   502,
     146,   502,   146,   154,   146,   459,   461,   440,   146,    14,
     149,   388,   156,   502,   502,   146,    75,   146,   149,   146,
     502,   146,    65,   496,   499,   500,   156,   502,   502,    90,
     299,    19,   315,   315,   315,   315,   315,   315,   315,   315,
     195,   312,   195,   195,   195,   195,   195,   195,   195,   157,
     157,   324,   327,   333,   326,   157,   318,   153,   332,   502,
     329,   154,   329,   161,   110,   262,   149,   154,   385,   149,
     154,   145,   502,   146,   502,   146,   502,   146,   146,   473,
     245,   502,    65,   449,   502,    14,   502,   449,   449,   146,
     154,   146,   154,   146,   154,   154,   449,   465,   393,   154,
     502,   431,   431,   146,   432,    67,   501,   487,   502,   492,
     502,   149,   145,   300,   502,   314,   314,   314,   314,   314,
     314,   314,   314,   145,   145,   146,   149,   327,   146,   502,
     318,   153,   411,   407,   414,   419,   146,   154,   146,   154,
     146,   154,   154,    80,   474,   475,   146,   432,   156,   455,
     154,   154,   154,   460,    96,    97,   469,   499,   154,   301,
     502,   146,   324,   327,   327,   146,   153,   502,   145,   146,
     154,   154,   154,    18,    81,   477,   478,   502,   502,   154,
      67,   467,    80,   470,   471,   195,   445,   502,   102,   103,
     153,   156,   102,   325,   102,   325,   502,   419,   476,   149,
     153,   445,   468,   477,   146,   299,   502,   502,   502,   145,
     302,   303,   502,   332,   153,   332,   153,   146,   499,   502,
     146,   449,   472,   499,   149,   146,   146,   146,   303,   502,
     154,   153,   146,   332,   146,   332,   456,   449,   303,   149,
     153,   502,   146,   146,   146,   146,   449,   149,   297,   303,
     502,   153,   146,   146,   303,   146,   146,   149,   153,   502,
     154,   303,   502,   146,   149,   303,   149,   303,   149,   303,
     146,   149,   303,   149,   303,   149,   303,   149,   303,   149,
     303,   149,   303,   146
  };

  const short
  parser::yyr1_[] =
  {
       0,   191,   193,   192,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   196,   195,   197,   197,   199,
     200,   198,   201,   201,   202,   202,   203,   203,   204,   204,
     204,   205,   205,   205,   207,   208,   206,   209,   209,   210,
     210,   211,   212,   211,   213,   211,   211,   214,   214,   214,
     215,   215,   216,   216,   217,   217,   219,   218,   220,   221,
     218,   222,   218,   218,   224,   225,   223,   226,   226,   227,
     227,   227,   227,   228,   229,   228,   230,   230,   231,   233,
     234,   232,   235,   235,   236,   236,   236,   236,   236,   236,
     236,   236,   236,   238,   239,   240,   237,   241,   242,   242,
     243,   243,   244,   245,   245,   246,   246,   246,   246,   246,
     246,   246,   246,   247,   247,   249,   248,   251,   250,   252,
     252,   253,   253,   253,   254,   254,   254,   255,   255,   255,
     255,   256,   256,   257,   257,   257,   257,   257,   257,   257,
     257,   258,   258,   258,   259,   259,   259,   260,   260,   260,
     260,   260,   261,   261,   262,   262,   262,   262,   263,   263,
     264,   264,   265,   265,   266,   266,   266,   266,   266,   267,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     267,   267,   267,   267,   267,   267,   267,   267,   269,   268,
     271,   270,   272,   270,   273,   270,   274,   270,   275,   270,
     276,   270,   277,   277,   277,   277,   278,   278,   278,   279,
     279,   279,   280,   280,   280,   281,   281,   282,   282,   283,
     283,   283,   283,   284,   284,   285,   285,   286,   286,   287,
     288,   286,   289,   289,   291,   290,   292,   290,   293,   293,
     294,   295,   295,   296,   296,   297,   297,   298,   298,   299,
     299,   300,   300,   300,   300,   301,   301,   301,   302,   302,
     302,   302,   302,   303,   303,   304,   304,   305,   305,   306,
     306,   306,   307,   307,   308,   309,   309,   310,   311,   311,
     312,   312,   313,   313,   314,   314,   314,   314,   314,   314,
     314,   314,   314,   314,   314,   314,   314,   314,   314,   314,
     314,   315,   315,   315,   316,   316,   317,   317,   317,   318,
     318,   319,   319,   320,   320,   320,   321,   321,   321,   322,
     322,   323,   323,   324,   324,   325,   325,   325,   326,   327,
     328,   329,   329,   329,   330,   330,   330,   331,   332,   332,
     334,   333,   335,   335,   335,   336,   337,   338,   339,   340,
     340,   341,   342,   342,   342,   342,   342,   344,   343,   346,
     345,   347,   347,   348,   348,   349,   350,   351,   351,   352,
     354,   353,   355,   355,   355,   356,   357,   357,   358,   358,
     358,   359,   359,   360,   361,   361,   362,   362,   363,   364,
     366,   365,   368,   367,   369,   369,   370,   371,   371,   372,
     372,   372,   373,   373,   374,   375,   375,   377,   376,   379,
     378,   380,   381,   382,   380,   383,   383,   383,   384,   384,
     385,   387,   388,   386,   389,   386,   390,   386,   391,   391,
     392,   392,   393,   393,   394,   395,   396,   396,   397,   398,
     398,   399,   399,   400,   400,   400,   401,   401,   403,   402,
     404,   402,   405,   405,   406,   406,   407,   407,   409,   408,
     411,   410,   412,   412,   413,   413,   414,   415,   414,   416,
     416,   417,   417,   418,   418,   418,   418,   419,   420,   420,
     421,   421,   421,   421,   421,   421,   422,   422,   423,   423,
     425,   426,   424,   427,   424,   428,   424,   429,   429,   429,
     429,   429,   429,   430,   430,   430,   431,   431,   431,   432,
     432,   433,   433,   434,   434,   434,   436,   437,   435,   438,
     438,   439,   439,   440,   440,   441,   442,   442,   443,   443,
     443,   443,   443,   443,   443,   443,   443,   444,   444,   444,
     444,   444,   444,   444,   445,   445,   445,   445,   445,   446,
     446,   446,   446,   446,   446,   446,   446,   446,   446,   446,
     446,   447,   447,   448,   448,   448,   449,   449,   449,   449,
     449,   449,   449,   449,   449,   450,   449,   451,   449,   452,
     453,   449,   454,   455,   456,   449,   457,   449,   458,   449,
     459,   460,   449,   461,   449,   462,   462,   462,   462,   463,
     463,   463,   463,   464,   464,   464,   465,   465,   465,   466,
     466,   468,   467,   467,   469,   469,   471,   472,   470,   473,
     473,   475,   476,   474,   477,   477,   478,   478,   478,   478,
     479,   479,   479,   480,   480,   481,   481,   482,   482,   483,
     483,   484,   484,   485,   486,   486,   486,   487,   487,   487,
     488,   489,   489,   489,   491,   492,   490,   493,   490,   494,
     490,   495,   490,   497,   498,   496,   500,   499,   499,   501,
     501,   502,   502,   502,   502,   503,   503,   503,   503,   503,
     503,   504,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
     503,   503,   505,   505,   506,   506,   507,   507,   507,   507
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     0,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     0,     0,     2,     4,     0,     0,
       0,     5,     1,     0,     1,     3,     1,     3,     1,     3,
       3,     1,     1,     3,     0,     0,    11,     4,     0,     1,
       3,     0,     0,     5,     0,     5,     1,     2,     0,     4,
       1,     3,     1,     0,     2,     0,     0,     3,     0,     0,
       5,     0,     6,     3,     0,     0,     9,     2,     0,     1,
       1,     1,     1,     5,     0,     6,     1,     3,     1,     0,
       0,     9,     2,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     0,     0,     9,     2,     2,     0,
       1,     3,     1,     1,     5,     2,     2,     2,     2,     4,
       4,     6,     8,     1,     0,     0,     5,     0,     5,     1,
       1,     1,     1,     1,     1,     1,     0,     2,     1,     1,
       0,     1,     0,     1,     2,     1,     2,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     5,     5,     5,     3,     2,     2,
       1,     0,     1,     1,     2,     2,     2,     2,     0,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     0,     6,
       0,     7,     0,     9,     0,    11,     0,     9,     0,    10,
       0,    10,     1,     2,     3,     2,     0,     1,     1,     0,
       1,     1,     0,     1,     1,     2,     1,     3,     0,     3,
       2,     1,     0,     1,     0,     1,     0,     2,     0,     0,
       0,     6,     1,     3,     0,     5,     0,     2,     2,     0,
       3,     2,     0,    10,    14,     2,     0,     4,     0,     2,
       0,     1,     5,     5,     5,     1,     1,     0,     1,     5,
       7,    13,    25,     1,     5,     3,     2,     2,     1,     1,
       1,     1,     3,     4,     5,     1,     3,     3,     2,     2,
       3,     3,     5,     2,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     4,     4,     4,     4,     4,     4,     4,
       4,     1,     1,     1,     1,     1,     4,     1,     2,     2,
       3,     1,     1,     7,    12,    11,     6,    12,    11,     2,
       3,     2,     3,     1,     3,     1,     1,     0,     1,     1,
       5,     2,     2,     1,     1,     3,     3,     1,     1,     5,
       0,     3,     1,     1,     0,     1,     1,     1,     2,     2,
       3,     2,     1,     1,     1,     1,     1,     0,     6,     0,
       6,     1,     3,     3,     1,     1,     3,     1,     3,     4,
       0,     7,     2,     3,     0,     1,     1,     1,     1,     1,
       0,     1,     3,     2,     2,     0,     1,     3,     1,     3,
       0,     5,     0,     3,     1,     1,     4,     2,     0,     1,
       1,     0,     1,     2,     3,     1,     3,     0,     3,     0,
       2,     2,     0,     0,     4,     2,     2,     1,     1,     3,
       1,     0,     0,     8,     0,     6,     0,     6,     0,     3,
       1,     3,     1,     3,     2,     4,     1,     3,     3,     1,
       1,     6,     4,     1,     2,     2,     1,     1,     0,     6,
       0,     6,     1,     1,     1,     3,     1,     1,     0,     5,
       0,     6,     1,     3,     1,     3,     1,     0,     4,     4,
       0,     1,     3,     0,     1,     4,     5,     1,     1,     3,
       1,     2,     6,     5,     3,     2,     1,     1,     1,     1,
       0,     0,     6,     0,     4,     0,     4,     4,     4,     3,
       3,     2,     0,     1,     3,     3,     2,     2,     1,     2,
       0,     2,     0,     1,     1,     0,     0,     0,     6,     2,
       4,     1,     3,     2,     1,     1,     1,     1,     7,     7,
       8,     8,     7,     6,     3,     7,     8,     7,     7,     8,
       8,     7,     7,     8,     5,     4,     4,     5,     5,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     5,     1,     1,     1,     1,
       1,     1,     2,     2,     2,     0,     5,     0,     5,     0,
       0,     8,     0,     0,     0,    13,     0,     7,     0,     7,
       0,     0,     9,     0,     9,     1,     2,     2,     2,     1,
       2,     2,     2,     1,     1,     1,     2,     2,     0,     2,
       0,     0,     3,     0,     2,     0,     0,     0,     4,     2,
       0,     0,     0,     4,     2,     1,     1,     1,     1,     3,
       6,     2,     2,     1,     3,     1,     3,     4,     0,     1,
       0,     1,     3,     1,     2,     2,     0,     1,     1,     2,
       1,     2,     4,     3,     0,     0,    11,     0,     7,     0,
       7,     0,     4,     0,     0,     7,     0,     2,     1,     2,
       0,     1,     6,     3,     2,     1,     4,     2,     1,     1,
       1,     0,     7,     5,     5,     3,     7,     3,     5,     6,
       3,     4,     4,     4,     4,     4,     4,     3,     3,     3,
       3,     3,     3,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     3,     3,     4,     4,     3,     5,     5,     5,     5,
       5,     5,     1,     3,     1,     3,     1,     1,     2,     3
  };




#if FRONTEND_VERILOG_YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   593,   593,   593,   604,   605,   606,   607,   608,   609,
     610,   611,   612,   613,   614,   617,   617,   633,   636,   639,
     644,   639,   655,   655,   658,   659,   662,   665,   670,   673,
     680,   689,   690,   691,   695,   697,   695,   718,   718,   721,
     721,   724,   725,   725,   730,   730,   735,   738,   738,   738,
     741,   741,   744,   744,   747,   763,   766,   766,   779,   783,
     779,   791,   791,   806,   811,   813,   811,   826,   826,   829,
     829,   829,   829,   832,   838,   838,   848,   849,   852,   860,
     862,   860,   880,   880,   883,   883,   883,   883,   883,   883,
     883,   884,   884,   887,   890,   894,   887,   921,   926,   927,
     930,   931,   936,   943,   944,   947,   948,   949,   951,   952,
     953,   954,   955,   958,   958,   961,   961,   966,   966,   975,
     976,   979,   982,   985,   991,   992,   993,   996,  1000,  1003,
    1006,  1009,  1009,  1013,  1015,  1018,  1021,  1025,  1028,  1031,
    1034,  1043,  1046,  1049,  1052,  1054,  1059,  1064,  1065,  1066,
    1067,  1068,  1071,  1072,  1075,  1080,  1086,  1092,  1098,  1101,
    1107,  1110,  1115,  1116,  1119,  1121,  1122,  1123,  1124,  1127,
    1127,  1127,  1127,  1127,  1127,  1127,  1127,  1127,  1127,  1128,
    1128,  1128,  1129,  1129,  1129,  1129,  1129,  1129,  1132,  1132,
    1140,  1140,  1147,  1147,  1154,  1154,  1161,  1161,  1170,  1170,
    1184,  1184,  1206,  1210,  1213,  1217,  1222,  1223,  1224,  1228,
    1229,  1230,  1233,  1234,  1235,  1239,  1242,  1247,  1248,  1251,
    1252,  1253,  1254,  1257,  1258,  1261,  1262,  1265,  1265,  1265,
    1269,  1265,  1277,  1277,  1280,  1280,  1301,  1301,  1315,  1316,
    1321,  1324,  1325,  1328,  1409,  1480,  1483,  1488,  1491,  1496,
    1499,  1504,  1510,  1516,  1522,  1530,  1531,  1532,  1535,  1542,
    1547,  1553,  1559,  1567,  1573,  1583,  1584,  1587,  1588,  1591,
    1594,  1595,  1599,  1600,  1606,  1609,  1609,  1612,  1615,  1617,
    1621,  1622,  1626,  1628,  1632,  1634,  1635,  1636,  1637,  1638,
    1639,  1640,  1641,  1643,  1644,  1645,  1646,  1647,  1648,  1649,
    1650,  1655,  1656,  1658,  1666,  1667,  1671,  1672,  1673,  1677,
    1678,  1682,  1682,  1685,  1686,  1687,  1690,  1691,  1692,  1696,
    1697,  1700,  1701,  1704,  1705,  1708,  1708,  1708,  1712,  1716,
    1719,  1722,  1723,  1724,  1727,  1728,  1729,  1736,  1739,  1740,
    1744,  1744,  1750,  1752,  1754,  1757,  1762,  1767,  1773,  1775,
    1778,  1781,  1784,  1784,  1784,  1784,  1785,  1790,  1790,  1799,
    1799,  1808,  1808,  1811,  1816,  1828,  1845,  1848,  1848,  1851,
    1864,  1864,  1890,  1891,  1892,  1896,  1902,  1903,  1907,  1908,
    1909,  1912,  1913,  1917,  1930,  1931,  1935,  1936,  1939,  1949,
    1957,  1957,  1965,  1965,  1974,  1975,  1978,  1982,  1983,  1986,
    1987,  1988,  1991,  1992,  1995,  1999,  2000,  2003,  2003,  2017,
    2017,  2021,  2026,  2028,  2026,  2041,  2042,  2043,  2046,  2047,
    2051,  2063,  2067,  2063,  2073,  2073,  2080,  2080,  2089,  2090,
    2100,  2100,  2103,  2141,  2166,  2211,  2214,  2214,  2217,  2222,
    2223,  2227,  2242,  2246,  2251,  2260,  2269,  2270,  2274,  2274,
    2282,  2282,  2291,  2294,  2299,  2300,  2303,  2303,  2306,  2306,
    2320,  2320,  2334,  2335,  2338,  2339,  2342,  2343,  2343,  2356,
    2356,  2359,  2359,  2362,  2363,  2368,  2371,  2379,  2405,  2405,
    2408,  2412,  2417,  2423,  2428,  2435,  2442,  2445,  2450,  2453,
    2458,  2463,  2458,  2474,  2474,  2486,  2486,  2496,  2497,  2498,
    2499,  2500,  2501,  2504,  2505,  2506,  2509,  2515,  2521,  2528,
    2531,  2536,  2539,  2544,  2547,  2550,  2555,  2559,  2555,  2565,
    2565,  2568,  2568,  2571,  2572,  2575,  2584,  2584,  2587,  2597,
    2606,  2615,  2624,  2631,  2638,  2645,  2656,  2669,  2676,  2683,
    2690,  2697,  2704,  2714,  2726,  2731,  2734,  2737,  2742,  2747,
    2748,  2749,  2750,  2751,  2752,  2753,  2754,  2755,  2756,  2757,
    2758,  2763,  2764,  2767,  2774,  2777,  2814,  2814,  2814,  2814,
    2814,  2814,  2815,  2816,  2817,  2818,  2818,  2826,  2826,  2834,
    2836,  2834,  2857,  2860,  2862,  2857,  2871,  2871,  2884,  2884,
    2897,  2935,  2897,  2942,  2942,  2953,  2956,  2963,  2970,  2980,
    2983,  2987,  2991,  2998,  3001,  3004,  3009,  3013,  3017,  3020,
    3021,  3024,  3024,  3038,  3041,  3042,  3045,  3050,  3045,  3061,
    3062,  3065,  3070,  3065,  3079,  3080,  3083,  3087,  3092,  3095,
    3100,  3105,  3114,  3121,  3124,  3129,  3133,  3139,  3140,  3143,
    3144,  3147,  3148,  3151,  3156,  3157,  3158,  3161,  3161,  3162,
    3165,  3171,  3174,  3188,  3195,  3197,  3195,  3204,  3204,  3211,
    3211,  3218,  3218,  3227,  3229,  3227,  3241,  3241,  3246,  3249,
    3249,  3252,  3255,  3263,  3268,  3273,  3276,  3285,  3298,  3305,
    3316,  3320,  3320,  3335,  3339,  3343,  3346,  3349,  3352,  3357,
    3360,  3365,  3370,  3375,  3380,  3385,  3390,  3395,  3400,  3406,
    3411,  3418,  3423,  3428,  3433,  3438,  3443,  3448,  3453,  3458,
    3463,  3468,  3473,  3478,  3483,  3488,  3493,  3498,  3503,  3508,
    3513,  3518,  3523,  3528,  3533,  3538,  3543,  3549,  3555,  3561,
    3567,  3574,  3580,  3583,  3589,  3593,  3599,  3600,  3601,  3605
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // FRONTEND_VERILOG_YYDEBUG


} // frontend_verilog_yy
#line 8070 "verilog_parser.tab.cc"

