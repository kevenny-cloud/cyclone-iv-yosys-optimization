// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file verilog_parser.tab.hh
 ** Define the frontend_verilog_yy::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_FRONTEND_VERILOG_YY_VERILOG_PARSER_TAB_HH_INCLUDED
# define YY_FRONTEND_VERILOG_YY_VERILOG_PARSER_TAB_HH_INCLUDED
// "%code requires" blocks.
#line 48 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"

	#include "kernel/yosys_common.h"
	#include "frontends/verilog/verilog_error.h"
	#include "frontends/verilog/verilog_location.h"
	YOSYS_NAMESPACE_BEGIN
	namespace VERILOG_FRONTEND {
		struct ParseState;
		struct ParseMode;
		class VerilogLexer;
	};
	YOSYS_NAMESPACE_END
#line 427 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"

	#include <map>
	#include <string>
	#include <memory>
	#include "frontends/verilog/verilog_frontend.h"

	struct specify_target {
		char polarity_op;
		std::unique_ptr<YOSYS_NAMESPACE_PREFIX AST::AstNode> dst, dat;
		specify_target& operator=(specify_target&& other) noexcept {
			if (this != &other) {
				dst = std::move(other.dst);
				dat = std::move(other.dat);
				polarity_op = other.polarity_op;
			}
			return *this;
		}
	};

	struct specify_triple {
		std::unique_ptr<YOSYS_NAMESPACE_PREFIX AST::AstNode> t_min, t_avg, t_max;
		specify_triple& operator=(specify_triple&& other) noexcept {
			if (this != &other) {
				t_min = std::move(other.t_min);
				t_avg = std::move(other.t_avg);
				t_max = std::move(other.t_max);
			}
			return *this;
		}
	};

	struct specify_rise_fall {
		specify_triple rise;
		specify_triple fall;
	};

	using string_t = std::unique_ptr<std::string>;
	using ast_t = std::unique_ptr<YOSYS_NAMESPACE_PREFIX AST::AstNode>;
	using al_t = std::unique_ptr<YOSYS_NAMESPACE_PREFIX dict<YOSYS_NAMESPACE_PREFIX RTLIL::IdString, std::unique_ptr<YOSYS_NAMESPACE_PREFIX AST::AstNode>>>;
	using specify_target_ptr_t = std::unique_ptr<struct specify_target>;
	using specify_triple_ptr_t = std::unique_ptr<struct specify_triple>;
	using specify_rise_fall_ptr_t = std::unique_ptr<struct specify_rise_fall>;
	using boolean_t = bool;
	using ch_t = char;
	using integer_t = int;
	using ast_node_type_t = YOSYS_NAMESPACE_PREFIX AST::AstNodeType;

#line 109 "verilog_parser.tab.hh"


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif



#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef FRONTEND_VERILOG_YYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define FRONTEND_VERILOG_YYDEBUG 1
#  else
#   define FRONTEND_VERILOG_YYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define FRONTEND_VERILOG_YYDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined FRONTEND_VERILOG_YYDEBUG */

namespace frontend_verilog_yy {
#line 252 "verilog_parser.tab.hh"




  /// A Bison parser.
  class parser
  {
  public:
#ifdef FRONTEND_VERILOG_YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define FRONTEND_VERILOG_YYSTYPE in C++, use %define api.value.type"
# endif
    typedef FRONTEND_VERILOG_YYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
    {
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {}

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // al_t
      // attr
      // if_attr
      // case_attr
      char dummy1[sizeof (al_t)];

      // ast_node_type_t
      // asgn_binop
      // inc_or_dec_op
      char dummy2[sizeof (ast_node_type_t)];

      // ast_t
      // io_wire_type
      // non_io_wire_type
      // wire_type
      // non_opt_range
      // non_opt_multirange
      // range
      // range_or_multirange
      // func_return_type
      // specify_if
      // specify_condition
      // enum_type
      // opt_enum_init
      // struct_type
      // struct_union
      // typedef_base_type
      // enum_struct_type
      // rvalue
      // lvalue
      // lvalue_concat_list
      // genvar_identifier
      // expr
      // basic_expr
      // @84
      // concat_list
      // assignment_pattern_list
      char dummy3[sizeof (ast_t)];

      // "boolean"
      // opt_signedness_default_signed
      // opt_signedness_default_unsigned
      // always_comb_or_latch
      // always_or_always_ff
      // opt_property
      char dummy4[sizeof (boolean_t)];

      // "invalid token"
      // specify_edge
      char dummy5[sizeof (ch_t)];

      // "integer"
      // integer_atom_type
      // integer_vector_type
      char dummy6[sizeof (integer_t)];

      // "specify rise and fall"
      // specify_rise_fall
      char dummy7[sizeof (specify_rise_fall_ptr_t)];

      // "specify target"
      // specify_target
      char dummy8[sizeof (specify_target_ptr_t)];

      // "specify triple"
      // specify_opt_triple
      // specify_triple
      char dummy9[sizeof (specify_triple_ptr_t)];

      // "string"
      // TOK_STRING
      // TOK_ID
      // TOK_CONSTVAL
      // TOK_REALVAL
      // TOK_PRIMITIVE
      // TOK_SVA_LABEL
      // TOK_SPECIFY_OPER
      // TOK_MSG_TASKS
      // TOK_BASE
      // TOK_BASED_CONSTVAL
      // TOK_UNBASED_UNSIZED_CONSTVAL
      // TOK_USER_TYPE
      // TOK_PKG_USER_TYPE
      // hierarchical_id
      // hierarchical_type_id
      // type_name
      // tok_prim_wrapper
      // opt_label
      // opt_sva_label
      // integral_number
      char dummy10[sizeof (string_t)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef Location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        FRONTEND_VERILOG_YYEMPTY = -2,
    FRONTEND_VERILOG_YYEOF = 0,    // "end of file"
    FRONTEND_VERILOG_YYerror = 256, // error
    FRONTEND_VERILOG_YYUNDEF = 257, // "invalid token"
    string_t = 258,                // "string"
    ast_t = 259,                   // ast_t
    al_t = 260,                    // al_t
    specify_target_ptr_t = 261,    // "specify target"
    specify_triple_ptr_t = 262,    // "specify triple"
    specify_rise_fall_ptr_t = 263, // "specify rise and fall"
    boolean_t = 264,               // "boolean"
    ch_t = 265,                    // "invalid token"
    integer_t = 266,               // "integer"
    ast_node_type_t = 267,         // ast_node_type_t
    TOK_STRING = 268,              // TOK_STRING
    TOK_ID = 269,                  // TOK_ID
    TOK_CONSTVAL = 270,            // TOK_CONSTVAL
    TOK_REALVAL = 271,             // TOK_REALVAL
    TOK_PRIMITIVE = 272,           // TOK_PRIMITIVE
    TOK_SVA_LABEL = 273,           // TOK_SVA_LABEL
    TOK_SPECIFY_OPER = 274,        // TOK_SPECIFY_OPER
    TOK_MSG_TASKS = 275,           // TOK_MSG_TASKS
    TOK_BASE = 276,                // TOK_BASE
    TOK_BASED_CONSTVAL = 277,      // TOK_BASED_CONSTVAL
    TOK_UNBASED_UNSIZED_CONSTVAL = 278, // TOK_UNBASED_UNSIZED_CONSTVAL
    TOK_USER_TYPE = 279,           // TOK_USER_TYPE
    TOK_PKG_USER_TYPE = 280,       // TOK_PKG_USER_TYPE
    TOK_ASSERT = 281,              // TOK_ASSERT
    TOK_ASSUME = 282,              // TOK_ASSUME
    TOK_RESTRICT = 283,            // TOK_RESTRICT
    TOK_COVER = 284,               // TOK_COVER
    TOK_FINAL = 285,               // TOK_FINAL
    ATTR_BEGIN = 286,              // ATTR_BEGIN
    ATTR_END = 287,                // ATTR_END
    DEFATTR_BEGIN = 288,           // DEFATTR_BEGIN
    DEFATTR_END = 289,             // DEFATTR_END
    TOK_MODULE = 290,              // TOK_MODULE
    TOK_ENDMODULE = 291,           // TOK_ENDMODULE
    TOK_PARAMETER = 292,           // TOK_PARAMETER
    TOK_LOCALPARAM = 293,          // TOK_LOCALPARAM
    TOK_DEFPARAM = 294,            // TOK_DEFPARAM
    TOK_PACKAGE = 295,             // TOK_PACKAGE
    TOK_ENDPACKAGE = 296,          // TOK_ENDPACKAGE
    TOK_PACKAGESEP = 297,          // TOK_PACKAGESEP
    TOK_INTERFACE = 298,           // TOK_INTERFACE
    TOK_ENDINTERFACE = 299,        // TOK_ENDINTERFACE
    TOK_MODPORT = 300,             // TOK_MODPORT
    TOK_VAR = 301,                 // TOK_VAR
    TOK_WILDCARD_CONNECT = 302,    // TOK_WILDCARD_CONNECT
    TOK_INPUT = 303,               // TOK_INPUT
    TOK_OUTPUT = 304,              // TOK_OUTPUT
    TOK_INOUT = 305,               // TOK_INOUT
    TOK_WIRE = 306,                // TOK_WIRE
    TOK_WAND = 307,                // TOK_WAND
    TOK_WOR = 308,                 // TOK_WOR
    TOK_REG = 309,                 // TOK_REG
    TOK_LOGIC = 310,               // TOK_LOGIC
    TOK_INTEGER = 311,             // TOK_INTEGER
    TOK_SIGNED = 312,              // TOK_SIGNED
    TOK_ASSIGN = 313,              // TOK_ASSIGN
    TOK_ALWAYS = 314,              // TOK_ALWAYS
    TOK_INITIAL = 315,             // TOK_INITIAL
    TOK_ALWAYS_FF = 316,           // TOK_ALWAYS_FF
    TOK_ALWAYS_COMB = 317,         // TOK_ALWAYS_COMB
    TOK_ALWAYS_LATCH = 318,        // TOK_ALWAYS_LATCH
    TOK_BEGIN = 319,               // TOK_BEGIN
    TOK_END = 320,                 // TOK_END
    TOK_IF = 321,                  // TOK_IF
    TOK_ELSE = 322,                // TOK_ELSE
    TOK_IFNONE = 323,              // TOK_IFNONE
    TOK_FOR = 324,                 // TOK_FOR
    TOK_WHILE = 325,               // TOK_WHILE
    TOK_REPEAT = 326,              // TOK_REPEAT
    TOK_DPI_FUNCTION = 327,        // TOK_DPI_FUNCTION
    TOK_POSEDGE = 328,             // TOK_POSEDGE
    TOK_NEGEDGE = 329,             // TOK_NEGEDGE
    TOK_OR = 330,                  // TOK_OR
    TOK_AUTOMATIC = 331,           // TOK_AUTOMATIC
    TOK_CASE = 332,                // TOK_CASE
    TOK_CASEX = 333,               // TOK_CASEX
    TOK_CASEZ = 334,               // TOK_CASEZ
    TOK_ENDCASE = 335,             // TOK_ENDCASE
    TOK_DEFAULT = 336,             // TOK_DEFAULT
    TOK_FUNCTION = 337,            // TOK_FUNCTION
    TOK_ENDFUNCTION = 338,         // TOK_ENDFUNCTION
    TOK_TASK = 339,                // TOK_TASK
    TOK_ENDTASK = 340,             // TOK_ENDTASK
    TOK_SPECIFY = 341,             // TOK_SPECIFY
    TOK_IGNORED_SPECIFY = 342,     // TOK_IGNORED_SPECIFY
    TOK_ENDSPECIFY = 343,          // TOK_ENDSPECIFY
    TOK_SPECPARAM = 344,           // TOK_SPECPARAM
    TOK_SPECIFY_AND = 345,         // TOK_SPECIFY_AND
    TOK_IGNORED_SPECIFY_AND = 346, // TOK_IGNORED_SPECIFY_AND
    TOK_GENERATE = 347,            // TOK_GENERATE
    TOK_ENDGENERATE = 348,         // TOK_ENDGENERATE
    TOK_GENVAR = 349,              // TOK_GENVAR
    TOK_REAL = 350,                // TOK_REAL
    TOK_SYNOPSYS_FULL_CASE = 351,  // TOK_SYNOPSYS_FULL_CASE
    TOK_SYNOPSYS_PARALLEL_CASE = 352, // TOK_SYNOPSYS_PARALLEL_CASE
    TOK_SUPPLY0 = 353,             // TOK_SUPPLY0
    TOK_SUPPLY1 = 354,             // TOK_SUPPLY1
    TOK_TO_SIGNED = 355,           // TOK_TO_SIGNED
    TOK_TO_UNSIGNED = 356,         // TOK_TO_UNSIGNED
    TOK_POS_INDEXED = 357,         // TOK_POS_INDEXED
    TOK_NEG_INDEXED = 358,         // TOK_NEG_INDEXED
    TOK_PROPERTY = 359,            // TOK_PROPERTY
    TOK_ENUM = 360,                // TOK_ENUM
    TOK_TYPEDEF = 361,             // TOK_TYPEDEF
    TOK_RAND = 362,                // TOK_RAND
    TOK_CONST = 363,               // TOK_CONST
    TOK_CHECKER = 364,             // TOK_CHECKER
    TOK_ENDCHECKER = 365,          // TOK_ENDCHECKER
    TOK_EVENTUALLY = 366,          // TOK_EVENTUALLY
    TOK_INCREMENT = 367,           // TOK_INCREMENT
    TOK_DECREMENT = 368,           // TOK_DECREMENT
    TOK_UNIQUE = 369,              // TOK_UNIQUE
    TOK_UNIQUE0 = 370,             // TOK_UNIQUE0
    TOK_PRIORITY = 371,            // TOK_PRIORITY
    TOK_STRUCT = 372,              // TOK_STRUCT
    TOK_PACKED = 373,              // TOK_PACKED
    TOK_UNSIGNED = 374,            // TOK_UNSIGNED
    TOK_INT = 375,                 // TOK_INT
    TOK_BYTE = 376,                // TOK_BYTE
    TOK_SHORTINT = 377,            // TOK_SHORTINT
    TOK_LONGINT = 378,             // TOK_LONGINT
    TOK_VOID = 379,                // TOK_VOID
    TOK_UNION = 380,               // TOK_UNION
    TOK_BIT_OR_ASSIGN = 381,       // TOK_BIT_OR_ASSIGN
    TOK_BIT_AND_ASSIGN = 382,      // TOK_BIT_AND_ASSIGN
    TOK_BIT_XOR_ASSIGN = 383,      // TOK_BIT_XOR_ASSIGN
    TOK_ADD_ASSIGN = 384,          // TOK_ADD_ASSIGN
    TOK_SUB_ASSIGN = 385,          // TOK_SUB_ASSIGN
    TOK_DIV_ASSIGN = 386,          // TOK_DIV_ASSIGN
    TOK_MOD_ASSIGN = 387,          // TOK_MOD_ASSIGN
    TOK_MUL_ASSIGN = 388,          // TOK_MUL_ASSIGN
    TOK_SHL_ASSIGN = 389,          // TOK_SHL_ASSIGN
    TOK_SHR_ASSIGN = 390,          // TOK_SHR_ASSIGN
    TOK_SSHL_ASSIGN = 391,         // TOK_SSHL_ASSIGN
    TOK_SSHR_ASSIGN = 392,         // TOK_SSHR_ASSIGN
    TOK_BIND = 393,                // TOK_BIND
    TOK_TIME_SCALE = 394,          // TOK_TIME_SCALE
    TOK_IMPORT = 395,              // TOK_IMPORT
    TOK_EXCL = 396,                // "'!'"
    TOK_HASH = 397,                // "'#'"
    TOK_PERC = 398,                // "'%'"
    TOK_AMP = 399,                 // "'&'"
    TOK_LPAREN = 400,              // "'('"
    TOK_RPAREN = 401,              // "')'"
    TOK_ASTER = 402,               // "'*'"
    TOK_PLUS = 403,                // "'+'"
    TOK_COMMA = 404,               // "','"
    TOK_MINUS = 405,               // "'-'"
    TOK_DOT = 406,                 // "'.'"
    TOK_SLASH = 407,               // "'/'"
    TOK_COL = 408,                 // "':'"
    TOK_SEMICOL = 409,             // "';'"
    TOK_LT = 410,                  // "'<'"
    TOK_EQ = 411,                  // "'='"
    TOK_GT = 412,                  // "'>'"
    TOK_QUE = 413,                 // "'?'"
    TOK_AT = 414,                  // "'@'"
    TOK_LBRA = 415,                // "'['"
    TOK_RBRA = 416,                // "']'"
    TOK_CARET = 417,               // "'^'"
    TOK_UNDER = 418,               // "'_'"
    TOK_LCURL = 419,               // "'{'"
    TOK_PIPE = 420,                // "'|'"
    TOK_RCURL = 421,               // "'}'"
    TOK_TILDE = 422,               // "'~'"
    TOK_n = 423,                   // "'n'"
    TOK_p = 424,                   // "'p'"
    TOK_x = 425,                   // "'x'"
    TOK_z = 426,                   // "'z'"
    OP_LOR = 427,                  // OP_LOR
    OP_LAND = 428,                 // OP_LAND
    OP_NOR = 429,                  // OP_NOR
    OP_XNOR = 430,                 // OP_XNOR
    OP_NAND = 431,                 // OP_NAND
    OP_EQ = 432,                   // OP_EQ
    OP_NE = 433,                   // OP_NE
    OP_EQX = 434,                  // OP_EQX
    OP_NEX = 435,                  // OP_NEX
    OP_LE = 436,                   // OP_LE
    OP_GE = 437,                   // OP_GE
    OP_SHL = 438,                  // OP_SHL
    OP_SHR = 439,                  // OP_SHR
    OP_SSHL = 440,                 // OP_SSHL
    OP_SSHR = 441,                 // OP_SSHR
    OP_POW = 442,                  // OP_POW
    OP_CAST = 443,                 // OP_CAST
    UNARY_OPS = 444,               // UNARY_OPS
    FAKE_THEN = 445                // FAKE_THEN
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 191, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_string_t = 3,                          // "string"
        S_ast_t = 4,                             // ast_t
        S_al_t = 5,                              // al_t
        S_specify_target_ptr_t = 6,              // "specify target"
        S_specify_triple_ptr_t = 7,              // "specify triple"
        S_specify_rise_fall_ptr_t = 8,           // "specify rise and fall"
        S_boolean_t = 9,                         // "boolean"
        S_ch_t = 10,                             // "invalid token"
        S_integer_t = 11,                        // "integer"
        S_ast_node_type_t = 12,                  // ast_node_type_t
        S_TOK_STRING = 13,                       // TOK_STRING
        S_TOK_ID = 14,                           // TOK_ID
        S_TOK_CONSTVAL = 15,                     // TOK_CONSTVAL
        S_TOK_REALVAL = 16,                      // TOK_REALVAL
        S_TOK_PRIMITIVE = 17,                    // TOK_PRIMITIVE
        S_TOK_SVA_LABEL = 18,                    // TOK_SVA_LABEL
        S_TOK_SPECIFY_OPER = 19,                 // TOK_SPECIFY_OPER
        S_TOK_MSG_TASKS = 20,                    // TOK_MSG_TASKS
        S_TOK_BASE = 21,                         // TOK_BASE
        S_TOK_BASED_CONSTVAL = 22,               // TOK_BASED_CONSTVAL
        S_TOK_UNBASED_UNSIZED_CONSTVAL = 23,     // TOK_UNBASED_UNSIZED_CONSTVAL
        S_TOK_USER_TYPE = 24,                    // TOK_USER_TYPE
        S_TOK_PKG_USER_TYPE = 25,                // TOK_PKG_USER_TYPE
        S_TOK_ASSERT = 26,                       // TOK_ASSERT
        S_TOK_ASSUME = 27,                       // TOK_ASSUME
        S_TOK_RESTRICT = 28,                     // TOK_RESTRICT
        S_TOK_COVER = 29,                        // TOK_COVER
        S_TOK_FINAL = 30,                        // TOK_FINAL
        S_ATTR_BEGIN = 31,                       // ATTR_BEGIN
        S_ATTR_END = 32,                         // ATTR_END
        S_DEFATTR_BEGIN = 33,                    // DEFATTR_BEGIN
        S_DEFATTR_END = 34,                      // DEFATTR_END
        S_TOK_MODULE = 35,                       // TOK_MODULE
        S_TOK_ENDMODULE = 36,                    // TOK_ENDMODULE
        S_TOK_PARAMETER = 37,                    // TOK_PARAMETER
        S_TOK_LOCALPARAM = 38,                   // TOK_LOCALPARAM
        S_TOK_DEFPARAM = 39,                     // TOK_DEFPARAM
        S_TOK_PACKAGE = 40,                      // TOK_PACKAGE
        S_TOK_ENDPACKAGE = 41,                   // TOK_ENDPACKAGE
        S_TOK_PACKAGESEP = 42,                   // TOK_PACKAGESEP
        S_TOK_INTERFACE = 43,                    // TOK_INTERFACE
        S_TOK_ENDINTERFACE = 44,                 // TOK_ENDINTERFACE
        S_TOK_MODPORT = 45,                      // TOK_MODPORT
        S_TOK_VAR = 46,                          // TOK_VAR
        S_TOK_WILDCARD_CONNECT = 47,             // TOK_WILDCARD_CONNECT
        S_TOK_INPUT = 48,                        // TOK_INPUT
        S_TOK_OUTPUT = 49,                       // TOK_OUTPUT
        S_TOK_INOUT = 50,                        // TOK_INOUT
        S_TOK_WIRE = 51,                         // TOK_WIRE
        S_TOK_WAND = 52,                         // TOK_WAND
        S_TOK_WOR = 53,                          // TOK_WOR
        S_TOK_REG = 54,                          // TOK_REG
        S_TOK_LOGIC = 55,                        // TOK_LOGIC
        S_TOK_INTEGER = 56,                      // TOK_INTEGER
        S_TOK_SIGNED = 57,                       // TOK_SIGNED
        S_TOK_ASSIGN = 58,                       // TOK_ASSIGN
        S_TOK_ALWAYS = 59,                       // TOK_ALWAYS
        S_TOK_INITIAL = 60,                      // TOK_INITIAL
        S_TOK_ALWAYS_FF = 61,                    // TOK_ALWAYS_FF
        S_TOK_ALWAYS_COMB = 62,                  // TOK_ALWAYS_COMB
        S_TOK_ALWAYS_LATCH = 63,                 // TOK_ALWAYS_LATCH
        S_TOK_BEGIN = 64,                        // TOK_BEGIN
        S_TOK_END = 65,                          // TOK_END
        S_TOK_IF = 66,                           // TOK_IF
        S_TOK_ELSE = 67,                         // TOK_ELSE
        S_TOK_IFNONE = 68,                       // TOK_IFNONE
        S_TOK_FOR = 69,                          // TOK_FOR
        S_TOK_WHILE = 70,                        // TOK_WHILE
        S_TOK_REPEAT = 71,                       // TOK_REPEAT
        S_TOK_DPI_FUNCTION = 72,                 // TOK_DPI_FUNCTION
        S_TOK_POSEDGE = 73,                      // TOK_POSEDGE
        S_TOK_NEGEDGE = 74,                      // TOK_NEGEDGE
        S_TOK_OR = 75,                           // TOK_OR
        S_TOK_AUTOMATIC = 76,                    // TOK_AUTOMATIC
        S_TOK_CASE = 77,                         // TOK_CASE
        S_TOK_CASEX = 78,                        // TOK_CASEX
        S_TOK_CASEZ = 79,                        // TOK_CASEZ
        S_TOK_ENDCASE = 80,                      // TOK_ENDCASE
        S_TOK_DEFAULT = 81,                      // TOK_DEFAULT
        S_TOK_FUNCTION = 82,                     // TOK_FUNCTION
        S_TOK_ENDFUNCTION = 83,                  // TOK_ENDFUNCTION
        S_TOK_TASK = 84,                         // TOK_TASK
        S_TOK_ENDTASK = 85,                      // TOK_ENDTASK
        S_TOK_SPECIFY = 86,                      // TOK_SPECIFY
        S_TOK_IGNORED_SPECIFY = 87,              // TOK_IGNORED_SPECIFY
        S_TOK_ENDSPECIFY = 88,                   // TOK_ENDSPECIFY
        S_TOK_SPECPARAM = 89,                    // TOK_SPECPARAM
        S_TOK_SPECIFY_AND = 90,                  // TOK_SPECIFY_AND
        S_TOK_IGNORED_SPECIFY_AND = 91,          // TOK_IGNORED_SPECIFY_AND
        S_TOK_GENERATE = 92,                     // TOK_GENERATE
        S_TOK_ENDGENERATE = 93,                  // TOK_ENDGENERATE
        S_TOK_GENVAR = 94,                       // TOK_GENVAR
        S_TOK_REAL = 95,                         // TOK_REAL
        S_TOK_SYNOPSYS_FULL_CASE = 96,           // TOK_SYNOPSYS_FULL_CASE
        S_TOK_SYNOPSYS_PARALLEL_CASE = 97,       // TOK_SYNOPSYS_PARALLEL_CASE
        S_TOK_SUPPLY0 = 98,                      // TOK_SUPPLY0
        S_TOK_SUPPLY1 = 99,                      // TOK_SUPPLY1
        S_TOK_TO_SIGNED = 100,                   // TOK_TO_SIGNED
        S_TOK_TO_UNSIGNED = 101,                 // TOK_TO_UNSIGNED
        S_TOK_POS_INDEXED = 102,                 // TOK_POS_INDEXED
        S_TOK_NEG_INDEXED = 103,                 // TOK_NEG_INDEXED
        S_TOK_PROPERTY = 104,                    // TOK_PROPERTY
        S_TOK_ENUM = 105,                        // TOK_ENUM
        S_TOK_TYPEDEF = 106,                     // TOK_TYPEDEF
        S_TOK_RAND = 107,                        // TOK_RAND
        S_TOK_CONST = 108,                       // TOK_CONST
        S_TOK_CHECKER = 109,                     // TOK_CHECKER
        S_TOK_ENDCHECKER = 110,                  // TOK_ENDCHECKER
        S_TOK_EVENTUALLY = 111,                  // TOK_EVENTUALLY
        S_TOK_INCREMENT = 112,                   // TOK_INCREMENT
        S_TOK_DECREMENT = 113,                   // TOK_DECREMENT
        S_TOK_UNIQUE = 114,                      // TOK_UNIQUE
        S_TOK_UNIQUE0 = 115,                     // TOK_UNIQUE0
        S_TOK_PRIORITY = 116,                    // TOK_PRIORITY
        S_TOK_STRUCT = 117,                      // TOK_STRUCT
        S_TOK_PACKED = 118,                      // TOK_PACKED
        S_TOK_UNSIGNED = 119,                    // TOK_UNSIGNED
        S_TOK_INT = 120,                         // TOK_INT
        S_TOK_BYTE = 121,                        // TOK_BYTE
        S_TOK_SHORTINT = 122,                    // TOK_SHORTINT
        S_TOK_LONGINT = 123,                     // TOK_LONGINT
        S_TOK_VOID = 124,                        // TOK_VOID
        S_TOK_UNION = 125,                       // TOK_UNION
        S_TOK_BIT_OR_ASSIGN = 126,               // TOK_BIT_OR_ASSIGN
        S_TOK_BIT_AND_ASSIGN = 127,              // TOK_BIT_AND_ASSIGN
        S_TOK_BIT_XOR_ASSIGN = 128,              // TOK_BIT_XOR_ASSIGN
        S_TOK_ADD_ASSIGN = 129,                  // TOK_ADD_ASSIGN
        S_TOK_SUB_ASSIGN = 130,                  // TOK_SUB_ASSIGN
        S_TOK_DIV_ASSIGN = 131,                  // TOK_DIV_ASSIGN
        S_TOK_MOD_ASSIGN = 132,                  // TOK_MOD_ASSIGN
        S_TOK_MUL_ASSIGN = 133,                  // TOK_MUL_ASSIGN
        S_TOK_SHL_ASSIGN = 134,                  // TOK_SHL_ASSIGN
        S_TOK_SHR_ASSIGN = 135,                  // TOK_SHR_ASSIGN
        S_TOK_SSHL_ASSIGN = 136,                 // TOK_SSHL_ASSIGN
        S_TOK_SSHR_ASSIGN = 137,                 // TOK_SSHR_ASSIGN
        S_TOK_BIND = 138,                        // TOK_BIND
        S_TOK_TIME_SCALE = 139,                  // TOK_TIME_SCALE
        S_TOK_IMPORT = 140,                      // TOK_IMPORT
        S_TOK_EXCL = 141,                        // "'!'"
        S_TOK_HASH = 142,                        // "'#'"
        S_TOK_PERC = 143,                        // "'%'"
        S_TOK_AMP = 144,                         // "'&'"
        S_TOK_LPAREN = 145,                      // "'('"
        S_TOK_RPAREN = 146,                      // "')'"
        S_TOK_ASTER = 147,                       // "'*'"
        S_TOK_PLUS = 148,                        // "'+'"
        S_TOK_COMMA = 149,                       // "','"
        S_TOK_MINUS = 150,                       // "'-'"
        S_TOK_DOT = 151,                         // "'.'"
        S_TOK_SLASH = 152,                       // "'/'"
        S_TOK_COL = 153,                         // "':'"
        S_TOK_SEMICOL = 154,                     // "';'"
        S_TOK_LT = 155,                          // "'<'"
        S_TOK_EQ = 156,                          // "'='"
        S_TOK_GT = 157,                          // "'>'"
        S_TOK_QUE = 158,                         // "'?'"
        S_TOK_AT = 159,                          // "'@'"
        S_TOK_LBRA = 160,                        // "'['"
        S_TOK_RBRA = 161,                        // "']'"
        S_TOK_CARET = 162,                       // "'^'"
        S_TOK_UNDER = 163,                       // "'_'"
        S_TOK_LCURL = 164,                       // "'{'"
        S_TOK_PIPE = 165,                        // "'|'"
        S_TOK_RCURL = 166,                       // "'}'"
        S_TOK_TILDE = 167,                       // "'~'"
        S_TOK_n = 168,                           // "'n'"
        S_TOK_p = 169,                           // "'p'"
        S_TOK_x = 170,                           // "'x'"
        S_TOK_z = 171,                           // "'z'"
        S_OP_LOR = 172,                          // OP_LOR
        S_OP_LAND = 173,                         // OP_LAND
        S_OP_NOR = 174,                          // OP_NOR
        S_OP_XNOR = 175,                         // OP_XNOR
        S_OP_NAND = 176,                         // OP_NAND
        S_OP_EQ = 177,                           // OP_EQ
        S_OP_NE = 178,                           // OP_NE
        S_OP_EQX = 179,                          // OP_EQX
        S_OP_NEX = 180,                          // OP_NEX
        S_OP_LE = 181,                           // OP_LE
        S_OP_GE = 182,                           // OP_GE
        S_OP_SHL = 183,                          // OP_SHL
        S_OP_SHR = 184,                          // OP_SHR
        S_OP_SSHL = 185,                         // OP_SSHL
        S_OP_SSHR = 186,                         // OP_SSHR
        S_OP_POW = 187,                          // OP_POW
        S_OP_CAST = 188,                         // OP_CAST
        S_UNARY_OPS = 189,                       // UNARY_OPS
        S_FAKE_THEN = 190,                       // FAKE_THEN
        S_YYACCEPT = 191,                        // $accept
        S_input = 192,                           // input
        S_193_1 = 193,                           // $@1
        S_design = 194,                          // design
        S_attr = 195,                            // attr
        S_196_2 = 196,                           // $@2
        S_attr_opt = 197,                        // attr_opt
        S_defattr = 198,                         // defattr
        S_199_3 = 199,                           // $@3
        S_200_4 = 200,                           // $@4
        S_opt_attr_list = 201,                   // opt_attr_list
        S_attr_list = 202,                       // attr_list
        S_attr_assign = 203,                     // attr_assign
        S_hierarchical_id = 204,                 // hierarchical_id
        S_hierarchical_type_id = 205,            // hierarchical_type_id
        S_module = 206,                          // module
        S_207_5 = 207,                           // $@5
        S_208_6 = 208,                           // $@6
        S_module_para_opt = 209,                 // module_para_opt
        S_module_para_list = 210,                // module_para_list
        S_single_module_para = 211,              // single_module_para
        S_212_7 = 212,                           // $@7
        S_213_8 = 213,                           // $@8
        S_module_args_opt = 214,                 // module_args_opt
        S_module_args = 215,                     // module_args
        S_optional_comma = 216,                  // optional_comma
        S_module_arg_opt_assignment = 217,       // module_arg_opt_assignment
        S_module_arg = 218,                      // module_arg
        S_219_9 = 219,                           // $@9
        S_220_10 = 220,                          // $@10
        S_221_11 = 221,                          // $@11
        S_222_12 = 222,                          // $@12
        S_package = 223,                         // package
        S_224_13 = 224,                          // $@13
        S_225_14 = 225,                          // $@14
        S_package_body = 226,                    // package_body
        S_package_body_stmt = 227,               // package_body_stmt
        S_import_stmt = 228,                     // import_stmt
        S_229_15 = 229,                          // $@15
        S_import_item_list = 230,                // import_item_list
        S_import_item = 231,                     // import_item
        S_interface = 232,                       // interface
        S_233_16 = 233,                          // $@16
        S_234_17 = 234,                          // $@17
        S_interface_body = 235,                  // interface_body
        S_interface_body_stmt = 236,             // interface_body_stmt
        S_bind_directive = 237,                  // bind_directive
        S_238_18 = 238,                          // $@18
        S_239_19 = 239,                          // $@19
        S_240_20 = 240,                          // $@20
        S_bind_target = 241,                     // bind_target
        S_opt_bind_target_instance_list = 242,   // opt_bind_target_instance_list
        S_bind_target_instance_list = 243,       // bind_target_instance_list
        S_bind_target_instance = 244,            // bind_target_instance
        S_mintypmax_expr = 245,                  // mintypmax_expr
        S_non_opt_delay = 246,                   // non_opt_delay
        S_delay = 247,                           // delay
        S_io_wire_type = 248,                    // io_wire_type
        S_249_21 = 249,                          // $@21
        S_non_io_wire_type = 250,                // non_io_wire_type
        S_251_22 = 251,                          // $@22
        S_wire_type = 252,                       // wire_type
        S_wire_type_token_io = 253,              // wire_type_token_io
        S_wire_type_signedness = 254,            // wire_type_signedness
        S_wire_type_const_rand = 255,            // wire_type_const_rand
        S_opt_wire_type_token = 256,             // opt_wire_type_token
        S_wire_type_token = 257,                 // wire_type_token
        S_net_type = 258,                        // net_type
        S_logic_type = 259,                      // logic_type
        S_integer_atom_type = 260,               // integer_atom_type
        S_integer_vector_type = 261,             // integer_vector_type
        S_non_opt_range = 262,                   // non_opt_range
        S_non_opt_multirange = 263,              // non_opt_multirange
        S_range = 264,                           // range
        S_range_or_multirange = 265,             // range_or_multirange
        S_module_body = 266,                     // module_body
        S_module_body_stmt = 267,                // module_body_stmt
        S_checker_decl = 268,                    // checker_decl
        S_269_23 = 269,                          // $@23
        S_task_func_decl = 270,                  // task_func_decl
        S_271_24 = 271,                          // $@24
        S_272_25 = 272,                          // $@25
        S_273_26 = 273,                          // $@26
        S_274_27 = 274,                          // $@27
        S_275_28 = 275,                          // $@28
        S_276_29 = 276,                          // $@29
        S_func_return_type = 277,                // func_return_type
        S_opt_type_vec = 278,                    // opt_type_vec
        S_opt_signedness_default_signed = 279,   // opt_signedness_default_signed
        S_opt_signedness_default_unsigned = 280, // opt_signedness_default_unsigned
        S_dpi_function_arg = 281,                // dpi_function_arg
        S_opt_dpi_function_args = 282,           // opt_dpi_function_args
        S_dpi_function_args = 283,               // dpi_function_args
        S_opt_automatic = 284,                   // opt_automatic
        S_opt_lifetime = 285,                    // opt_lifetime
        S_task_func_args_opt = 286,              // task_func_args_opt
        S_287_30 = 287,                          // $@30
        S_288_31 = 288,                          // $@31
        S_task_func_args = 289,                  // task_func_args
        S_task_func_port = 290,                  // task_func_port
        S_291_32 = 291,                          // $@32
        S_292_33 = 292,                          // $@33
        S_task_func_body = 293,                  // task_func_body
        S_specify_block = 294,                   // specify_block
        S_specify_item_list = 295,               // specify_item_list
        S_specify_item = 296,                    // specify_item
        S_specify_opt_triple = 297,              // specify_opt_triple
        S_specify_if = 298,                      // specify_if
        S_specify_condition = 299,               // specify_condition
        S_specify_target = 300,                  // specify_target
        S_specify_edge = 301,                    // specify_edge
        S_specify_rise_fall = 302,               // specify_rise_fall
        S_specify_triple = 303,                  // specify_triple
        S_ignored_specify_block = 304,           // ignored_specify_block
        S_ignored_specify_item_opt = 305,        // ignored_specify_item_opt
        S_ignored_specify_item = 306,            // ignored_specify_item
        S_specparam_declaration = 307,           // specparam_declaration
        S_specparam_range = 308,                 // specparam_range
        S_list_of_specparam_assignments = 309,   // list_of_specparam_assignments
        S_specparam_assignment = 310,            // specparam_assignment
        S_path_declaration = 311,                // path_declaration
        S_simple_path_declaration = 312,         // simple_path_declaration
        S_state_dependent_path_declaration = 313, // state_dependent_path_declaration
        S_module_path_expression = 314,          // module_path_expression
        S_module_path_primary = 315,             // module_path_primary
        S_number = 316,                          // number
        S_path_delay_value = 317,                // path_delay_value
        S_list_of_path_delay_extra_expressions = 318, // list_of_path_delay_extra_expressions
        S_specify_edge_identifier = 319,         // specify_edge_identifier
        S_parallel_path_description = 320,       // parallel_path_description
        S_full_path_description = 321,           // full_path_description
        S_list_of_path_inputs = 322,             // list_of_path_inputs
        S_more_path_inputs = 323,                // more_path_inputs
        S_list_of_path_outputs = 324,            // list_of_path_outputs
        S_opt_polarity_operator = 325,           // opt_polarity_operator
        S_specify_input_terminal_descriptor = 326, // specify_input_terminal_descriptor
        S_specify_output_terminal_descriptor = 327, // specify_output_terminal_descriptor
        S_system_timing_declaration = 328,       // system_timing_declaration
        S_system_timing_arg = 329,               // system_timing_arg
        S_system_timing_args = 330,              // system_timing_args
        S_ignspec_constant_expression = 331,     // ignspec_constant_expression
        S_ignspec_expr = 332,                    // ignspec_expr
        S_ignspec_id = 333,                      // ignspec_id
        S_334_34 = 334,                          // $@34
        S_param_signed = 335,                    // param_signed
        S_param_integer = 336,                   // param_integer
        S_param_real = 337,                      // param_real
        S_param_range = 338,                     // param_range
        S_param_integer_type = 339,              // param_integer_type
        S_param_range_type = 340,                // param_range_type
        S_param_implicit_type = 341,             // param_implicit_type
        S_param_type = 342,                      // param_type
        S_param_decl = 343,                      // param_decl
        S_344_35 = 344,                          // $@35
        S_localparam_decl = 345,                 // localparam_decl
        S_346_36 = 346,                          // $@36
        S_param_decl_list = 347,                 // param_decl_list
        S_single_param_decl = 348,               // single_param_decl
        S_single_param_decl_ident = 349,         // single_param_decl_ident
        S_defparam_decl = 350,                   // defparam_decl
        S_defparam_decl_list = 351,              // defparam_decl_list
        S_single_defparam_decl = 352,            // single_defparam_decl
        S_enum_type = 353,                       // enum_type
        S_354_37 = 354,                          // $@37
        S_enum_base_type = 355,                  // enum_base_type
        S_type_atom = 356,                       // type_atom
        S_type_vec = 357,                        // type_vec
        S_type_signing = 358,                    // type_signing
        S_enum_name_list = 359,                  // enum_name_list
        S_enum_name_decl = 360,                  // enum_name_decl
        S_opt_enum_init = 361,                   // opt_enum_init
        S_enum_var_list = 362,                   // enum_var_list
        S_enum_var = 363,                        // enum_var
        S_enum_decl = 364,                       // enum_decl
        S_struct_decl = 365,                     // struct_decl
        S_366_38 = 366,                          // $@38
        S_struct_type = 367,                     // struct_type
        S_368_39 = 368,                          // $@39
        S_struct_union = 369,                    // struct_union
        S_struct_body = 370,                     // struct_body
        S_opt_packed = 371,                      // opt_packed
        S_opt_signed_struct = 372,               // opt_signed_struct
        S_struct_member_list = 373,              // struct_member_list
        S_struct_member = 374,                   // struct_member
        S_member_name_list = 375,                // member_name_list
        S_member_name = 376,                     // member_name
        S_377_40 = 377,                          // $@40
        S_struct_member_type = 378,              // struct_member_type
        S_379_41 = 379,                          // $@41
        S_member_type_token = 380,               // member_type_token
        S_381_42 = 381,                          // $@42
        S_382_43 = 382,                          // $@43
        S_member_type = 383,                     // member_type
        S_struct_var_list = 384,                 // struct_var_list
        S_struct_var = 385,                      // struct_var
        S_wire_decl = 386,                       // wire_decl
        S_387_44 = 387,                          // $@44
        S_388_45 = 388,                          // $@45
        S_389_46 = 389,                          // $@46
        S_390_47 = 390,                          // $@47
        S_opt_supply_wires = 391,                // opt_supply_wires
        S_wire_name_list = 392,                  // wire_name_list
        S_wire_name_and_opt_assign = 393,        // wire_name_and_opt_assign
        S_wire_name = 394,                       // wire_name
        S_assign_stmt = 395,                     // assign_stmt
        S_assign_expr_list = 396,                // assign_expr_list
        S_assign_expr = 397,                     // assign_expr
        S_type_name = 398,                       // type_name
        S_typedef_decl = 399,                    // typedef_decl
        S_typedef_base_type = 400,               // typedef_base_type
        S_enum_struct_type = 401,                // enum_struct_type
        S_cell_stmt = 402,                       // cell_stmt
        S_403_48 = 403,                          // $@48
        S_404_49 = 404,                          // $@49
        S_tok_prim_wrapper = 405,                // tok_prim_wrapper
        S_cell_list = 406,                       // cell_list
        S_single_cell = 407,                     // single_cell
        S_single_cell_no_array = 408,            // single_cell_no_array
        S_409_50 = 409,                          // $@50
        S_single_cell_arraylist = 410,           // single_cell_arraylist
        S_411_51 = 411,                          // $@51
        S_cell_list_no_array = 412,              // cell_list_no_array
        S_prim_list = 413,                       // prim_list
        S_single_prim = 414,                     // single_prim
        S_415_52 = 415,                          // $@52
        S_cell_parameter_list_opt = 416,         // cell_parameter_list_opt
        S_cell_parameter_list = 417,             // cell_parameter_list
        S_cell_parameter = 418,                  // cell_parameter
        S_cell_port_list = 419,                  // cell_port_list
        S_cell_port_list_rules = 420,            // cell_port_list_rules
        S_cell_port = 421,                       // cell_port
        S_always_comb_or_latch = 422,            // always_comb_or_latch
        S_always_or_always_ff = 423,             // always_or_always_ff
        S_always_stmt = 424,                     // always_stmt
        S_425_53 = 425,                          // $@53
        S_426_54 = 426,                          // $@54
        S_427_55 = 427,                          // $@55
        S_428_56 = 428,                          // $@56
        S_always_cond = 429,                     // always_cond
        S_always_events = 430,                   // always_events
        S_always_event = 431,                    // always_event
        S_opt_label = 432,                       // opt_label
        S_opt_sva_label = 433,                   // opt_sva_label
        S_opt_property = 434,                    // opt_property
        S_modport_stmt = 435,                    // modport_stmt
        S_436_57 = 436,                          // $@57
        S_437_58 = 437,                          // $@58
        S_modport_args_opt = 438,                // modport_args_opt
        S_modport_args = 439,                    // modport_args
        S_modport_arg = 440,                     // modport_arg
        S_modport_member = 441,                  // modport_member
        S_modport_type_token = 442,              // modport_type_token
        S_assert = 443,                          // assert
        S_assert_property = 444,                 // assert_property
        S_simple_behavioral_stmt = 445,          // simple_behavioral_stmt
        S_asgn_binop = 446,                      // asgn_binop
        S_inc_or_dec_op = 447,                   // inc_or_dec_op
        S_for_initialization = 448,              // for_initialization
        S_behavioral_stmt = 449,                 // behavioral_stmt
        S_450_59 = 450,                          // $@59
        S_451_60 = 451,                          // $@60
        S_452_61 = 452,                          // $@61
        S_453_62 = 453,                          // $@62
        S_454_63 = 454,                          // $@63
        S_455_64 = 455,                          // $@64
        S_456_65 = 456,                          // $@65
        S_457_66 = 457,                          // $@66
        S_458_67 = 458,                          // $@67
        S_459_68 = 459,                          // $@68
        S_460_69 = 460,                          // $@69
        S_461_70 = 461,                          // $@70
        S_if_attr = 462,                         // if_attr
        S_case_attr = 463,                       // case_attr
        S_case_type = 464,                       // case_type
        S_opt_synopsys_attr = 465,               // opt_synopsys_attr
        S_behavioral_stmt_list = 466,            // behavioral_stmt_list
        S_optional_else = 467,                   // optional_else
        S_468_71 = 468,                          // $@71
        S_case_body = 469,                       // case_body
        S_case_item = 470,                       // case_item
        S_471_72 = 471,                          // $@72
        S_472_73 = 472,                          // $@73
        S_gen_case_body = 473,                   // gen_case_body
        S_gen_case_item = 474,                   // gen_case_item
        S_475_74 = 475,                          // $@74
        S_476_75 = 476,                          // $@75
        S_case_select = 477,                     // case_select
        S_case_expr_list = 478,                  // case_expr_list
        S_rvalue = 479,                          // rvalue
        S_lvalue = 480,                          // lvalue
        S_lvalue_concat_list = 481,              // lvalue_concat_list
        S_opt_arg_list = 482,                    // opt_arg_list
        S_arg_list = 483,                        // arg_list
        S_arg_list2 = 484,                       // arg_list2
        S_single_arg = 485,                      // single_arg
        S_module_gen_body = 486,                 // module_gen_body
        S_gen_stmt_or_module_body_stmt = 487,    // gen_stmt_or_module_body_stmt
        S_genvar_identifier = 488,               // genvar_identifier
        S_genvar_initialization = 489,           // genvar_initialization
        S_gen_stmt = 490,                        // gen_stmt
        S_491_76 = 491,                          // $@76
        S_492_77 = 492,                          // $@77
        S_493_78 = 493,                          // $@78
        S_494_79 = 494,                          // $@79
        S_495_80 = 495,                          // $@80
        S_gen_block = 496,                       // gen_block
        S_497_81 = 497,                          // $@81
        S_498_82 = 498,                          // $@82
        S_gen_stmt_block = 499,                  // gen_stmt_block
        S_500_83 = 500,                          // $@83
        S_opt_gen_else = 501,                    // opt_gen_else
        S_expr = 502,                            // expr
        S_basic_expr = 503,                      // basic_expr
        S_504_84 = 504,                          // @84
        S_concat_list = 505,                     // concat_list
        S_assignment_pattern_list = 506,         // assignment_pattern_list
        S_integral_number = 507                  // integral_number
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
    {
      case symbol_kind::S_al_t: // al_t
      case symbol_kind::S_attr: // attr
      case symbol_kind::S_if_attr: // if_attr
      case symbol_kind::S_case_attr: // case_attr
        value.move< al_t > (std::move (that.value));
        break;

      case symbol_kind::S_ast_node_type_t: // ast_node_type_t
      case symbol_kind::S_asgn_binop: // asgn_binop
      case symbol_kind::S_inc_or_dec_op: // inc_or_dec_op
        value.move< ast_node_type_t > (std::move (that.value));
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
        value.move< ast_t > (std::move (that.value));
        break;

      case symbol_kind::S_boolean_t: // "boolean"
      case symbol_kind::S_opt_signedness_default_signed: // opt_signedness_default_signed
      case symbol_kind::S_opt_signedness_default_unsigned: // opt_signedness_default_unsigned
      case symbol_kind::S_always_comb_or_latch: // always_comb_or_latch
      case symbol_kind::S_always_or_always_ff: // always_or_always_ff
      case symbol_kind::S_opt_property: // opt_property
        value.move< boolean_t > (std::move (that.value));
        break;

      case symbol_kind::S_ch_t: // "invalid token"
      case symbol_kind::S_specify_edge: // specify_edge
        value.move< ch_t > (std::move (that.value));
        break;

      case symbol_kind::S_integer_t: // "integer"
      case symbol_kind::S_integer_atom_type: // integer_atom_type
      case symbol_kind::S_integer_vector_type: // integer_vector_type
        value.move< integer_t > (std::move (that.value));
        break;

      case symbol_kind::S_specify_rise_fall_ptr_t: // "specify rise and fall"
      case symbol_kind::S_specify_rise_fall: // specify_rise_fall
        value.move< specify_rise_fall_ptr_t > (std::move (that.value));
        break;

      case symbol_kind::S_specify_target_ptr_t: // "specify target"
      case symbol_kind::S_specify_target: // specify_target
        value.move< specify_target_ptr_t > (std::move (that.value));
        break;

      case symbol_kind::S_specify_triple_ptr_t: // "specify triple"
      case symbol_kind::S_specify_opt_triple: // specify_opt_triple
      case symbol_kind::S_specify_triple: // specify_triple
        value.move< specify_triple_ptr_t > (std::move (that.value));
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
        value.move< string_t > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, al_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const al_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ast_node_type_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ast_node_type_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ast_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ast_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, boolean_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const boolean_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ch_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ch_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, integer_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const integer_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, specify_rise_fall_ptr_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const specify_rise_fall_ptr_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, specify_target_ptr_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const specify_target_ptr_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, specify_triple_ptr_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const specify_triple_ptr_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, string_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const string_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_al_t: // al_t
      case symbol_kind::S_attr: // attr
      case symbol_kind::S_if_attr: // if_attr
      case symbol_kind::S_case_attr: // case_attr
        value.template destroy< al_t > ();
        break;

      case symbol_kind::S_ast_node_type_t: // ast_node_type_t
      case symbol_kind::S_asgn_binop: // asgn_binop
      case symbol_kind::S_inc_or_dec_op: // inc_or_dec_op
        value.template destroy< ast_node_type_t > ();
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
        value.template destroy< ast_t > ();
        break;

      case symbol_kind::S_boolean_t: // "boolean"
      case symbol_kind::S_opt_signedness_default_signed: // opt_signedness_default_signed
      case symbol_kind::S_opt_signedness_default_unsigned: // opt_signedness_default_unsigned
      case symbol_kind::S_always_comb_or_latch: // always_comb_or_latch
      case symbol_kind::S_always_or_always_ff: // always_or_always_ff
      case symbol_kind::S_opt_property: // opt_property
        value.template destroy< boolean_t > ();
        break;

      case symbol_kind::S_ch_t: // "invalid token"
      case symbol_kind::S_specify_edge: // specify_edge
        value.template destroy< ch_t > ();
        break;

      case symbol_kind::S_integer_t: // "integer"
      case symbol_kind::S_integer_atom_type: // integer_atom_type
      case symbol_kind::S_integer_vector_type: // integer_vector_type
        value.template destroy< integer_t > ();
        break;

      case symbol_kind::S_specify_rise_fall_ptr_t: // "specify rise and fall"
      case symbol_kind::S_specify_rise_fall: // specify_rise_fall
        value.template destroy< specify_rise_fall_ptr_t > ();
        break;

      case symbol_kind::S_specify_target_ptr_t: // "specify target"
      case symbol_kind::S_specify_target: // specify_target
        value.template destroy< specify_target_ptr_t > ();
        break;

      case symbol_kind::S_specify_triple_ptr_t: // "specify triple"
      case symbol_kind::S_specify_opt_triple: // specify_opt_triple
      case symbol_kind::S_specify_triple: // specify_triple
        value.template destroy< specify_triple_ptr_t > ();
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
        value.template destroy< string_t > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

      /// The user-facing name of this symbol.
      const char *name () const YY_NOEXCEPT
      {
        return parser::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, al_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const al_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, ast_node_type_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const ast_node_type_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, ast_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const ast_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, boolean_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const boolean_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, ch_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const ch_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, integer_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const integer_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, specify_rise_fall_ptr_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const specify_rise_fall_ptr_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, specify_target_ptr_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const specify_target_ptr_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, specify_triple_ptr_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const specify_triple_ptr_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, string_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const string_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
    };

    /// Build a parser object.
    parser (YOSYS_NAMESPACE_PREFIX VERILOG_FRONTEND::VerilogLexer* lexer_yyarg, YOSYS_NAMESPACE_PREFIX VERILOG_FRONTEND::ParseState* extra_yyarg, YOSYS_NAMESPACE_PREFIX VERILOG_FRONTEND::ParseMode* mode_yyarg);
    virtual ~parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    parser (const parser&) = delete;
    /// Non copyable.
    parser& operator= (const parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if FRONTEND_VERILOG_YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static const char *symbol_name (symbol_kind_type yysymbol);

    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FRONTEND_VERILOG_YYEOF (location_type l)
      {
        return symbol_type (token::FRONTEND_VERILOG_YYEOF, std::move (l));
      }
#else
      static
      symbol_type
      make_FRONTEND_VERILOG_YYEOF (const location_type& l)
      {
        return symbol_type (token::FRONTEND_VERILOG_YYEOF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FRONTEND_VERILOG_YYerror (location_type l)
      {
        return symbol_type (token::FRONTEND_VERILOG_YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_FRONTEND_VERILOG_YYerror (const location_type& l)
      {
        return symbol_type (token::FRONTEND_VERILOG_YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FRONTEND_VERILOG_YYUNDEF (location_type l)
      {
        return symbol_type (token::FRONTEND_VERILOG_YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_FRONTEND_VERILOG_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::FRONTEND_VERILOG_YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_string_t (string_t v, location_type l)
      {
        return symbol_type (token::string_t, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_string_t (const string_t& v, const location_type& l)
      {
        return symbol_type (token::string_t, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ast_t (ast_t v, location_type l)
      {
        return symbol_type (token::ast_t, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ast_t (const ast_t& v, const location_type& l)
      {
        return symbol_type (token::ast_t, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_al_t (al_t v, location_type l)
      {
        return symbol_type (token::al_t, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_al_t (const al_t& v, const location_type& l)
      {
        return symbol_type (token::al_t, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_specify_target_ptr_t (specify_target_ptr_t v, location_type l)
      {
        return symbol_type (token::specify_target_ptr_t, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_specify_target_ptr_t (const specify_target_ptr_t& v, const location_type& l)
      {
        return symbol_type (token::specify_target_ptr_t, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_specify_triple_ptr_t (specify_triple_ptr_t v, location_type l)
      {
        return symbol_type (token::specify_triple_ptr_t, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_specify_triple_ptr_t (const specify_triple_ptr_t& v, const location_type& l)
      {
        return symbol_type (token::specify_triple_ptr_t, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_specify_rise_fall_ptr_t (specify_rise_fall_ptr_t v, location_type l)
      {
        return symbol_type (token::specify_rise_fall_ptr_t, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_specify_rise_fall_ptr_t (const specify_rise_fall_ptr_t& v, const location_type& l)
      {
        return symbol_type (token::specify_rise_fall_ptr_t, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_boolean_t (boolean_t v, location_type l)
      {
        return symbol_type (token::boolean_t, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_boolean_t (const boolean_t& v, const location_type& l)
      {
        return symbol_type (token::boolean_t, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ch_t (ch_t v, location_type l)
      {
        return symbol_type (token::ch_t, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ch_t (const ch_t& v, const location_type& l)
      {
        return symbol_type (token::ch_t, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_integer_t (integer_t v, location_type l)
      {
        return symbol_type (token::integer_t, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_integer_t (const integer_t& v, const location_type& l)
      {
        return symbol_type (token::integer_t, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ast_node_type_t (ast_node_type_t v, location_type l)
      {
        return symbol_type (token::ast_node_type_t, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ast_node_type_t (const ast_node_type_t& v, const location_type& l)
      {
        return symbol_type (token::ast_node_type_t, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_STRING (string_t v, location_type l)
      {
        return symbol_type (token::TOK_STRING, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_STRING (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_STRING, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ID (string_t v, location_type l)
      {
        return symbol_type (token::TOK_ID, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ID (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_ID, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_CONSTVAL (string_t v, location_type l)
      {
        return symbol_type (token::TOK_CONSTVAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_CONSTVAL (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_CONSTVAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_REALVAL (string_t v, location_type l)
      {
        return symbol_type (token::TOK_REALVAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_REALVAL (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_REALVAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_PRIMITIVE (string_t v, location_type l)
      {
        return symbol_type (token::TOK_PRIMITIVE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_PRIMITIVE (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_PRIMITIVE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SVA_LABEL (string_t v, location_type l)
      {
        return symbol_type (token::TOK_SVA_LABEL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SVA_LABEL (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_SVA_LABEL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SPECIFY_OPER (string_t v, location_type l)
      {
        return symbol_type (token::TOK_SPECIFY_OPER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SPECIFY_OPER (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_SPECIFY_OPER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_MSG_TASKS (string_t v, location_type l)
      {
        return symbol_type (token::TOK_MSG_TASKS, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_MSG_TASKS (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_MSG_TASKS, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_BASE (string_t v, location_type l)
      {
        return symbol_type (token::TOK_BASE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_BASE (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_BASE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_BASED_CONSTVAL (string_t v, location_type l)
      {
        return symbol_type (token::TOK_BASED_CONSTVAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_BASED_CONSTVAL (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_BASED_CONSTVAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_UNBASED_UNSIZED_CONSTVAL (string_t v, location_type l)
      {
        return symbol_type (token::TOK_UNBASED_UNSIZED_CONSTVAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_UNBASED_UNSIZED_CONSTVAL (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_UNBASED_UNSIZED_CONSTVAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_USER_TYPE (string_t v, location_type l)
      {
        return symbol_type (token::TOK_USER_TYPE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_USER_TYPE (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_USER_TYPE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_PKG_USER_TYPE (string_t v, location_type l)
      {
        return symbol_type (token::TOK_PKG_USER_TYPE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_PKG_USER_TYPE (const string_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_PKG_USER_TYPE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ASSERT (location_type l)
      {
        return symbol_type (token::TOK_ASSERT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ASSERT (const location_type& l)
      {
        return symbol_type (token::TOK_ASSERT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ASSUME (location_type l)
      {
        return symbol_type (token::TOK_ASSUME, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ASSUME (const location_type& l)
      {
        return symbol_type (token::TOK_ASSUME, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_RESTRICT (location_type l)
      {
        return symbol_type (token::TOK_RESTRICT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_RESTRICT (const location_type& l)
      {
        return symbol_type (token::TOK_RESTRICT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_COVER (location_type l)
      {
        return symbol_type (token::TOK_COVER, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_COVER (const location_type& l)
      {
        return symbol_type (token::TOK_COVER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_FINAL (location_type l)
      {
        return symbol_type (token::TOK_FINAL, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_FINAL (const location_type& l)
      {
        return symbol_type (token::TOK_FINAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ATTR_BEGIN (location_type l)
      {
        return symbol_type (token::ATTR_BEGIN, std::move (l));
      }
#else
      static
      symbol_type
      make_ATTR_BEGIN (const location_type& l)
      {
        return symbol_type (token::ATTR_BEGIN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ATTR_END (location_type l)
      {
        return symbol_type (token::ATTR_END, std::move (l));
      }
#else
      static
      symbol_type
      make_ATTR_END (const location_type& l)
      {
        return symbol_type (token::ATTR_END, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DEFATTR_BEGIN (location_type l)
      {
        return symbol_type (token::DEFATTR_BEGIN, std::move (l));
      }
#else
      static
      symbol_type
      make_DEFATTR_BEGIN (const location_type& l)
      {
        return symbol_type (token::DEFATTR_BEGIN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DEFATTR_END (location_type l)
      {
        return symbol_type (token::DEFATTR_END, std::move (l));
      }
#else
      static
      symbol_type
      make_DEFATTR_END (const location_type& l)
      {
        return symbol_type (token::DEFATTR_END, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_MODULE (location_type l)
      {
        return symbol_type (token::TOK_MODULE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_MODULE (const location_type& l)
      {
        return symbol_type (token::TOK_MODULE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ENDMODULE (location_type l)
      {
        return symbol_type (token::TOK_ENDMODULE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ENDMODULE (const location_type& l)
      {
        return symbol_type (token::TOK_ENDMODULE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_PARAMETER (location_type l)
      {
        return symbol_type (token::TOK_PARAMETER, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_PARAMETER (const location_type& l)
      {
        return symbol_type (token::TOK_PARAMETER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_LOCALPARAM (location_type l)
      {
        return symbol_type (token::TOK_LOCALPARAM, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_LOCALPARAM (const location_type& l)
      {
        return symbol_type (token::TOK_LOCALPARAM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_DEFPARAM (location_type l)
      {
        return symbol_type (token::TOK_DEFPARAM, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_DEFPARAM (const location_type& l)
      {
        return symbol_type (token::TOK_DEFPARAM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_PACKAGE (location_type l)
      {
        return symbol_type (token::TOK_PACKAGE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_PACKAGE (const location_type& l)
      {
        return symbol_type (token::TOK_PACKAGE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ENDPACKAGE (location_type l)
      {
        return symbol_type (token::TOK_ENDPACKAGE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ENDPACKAGE (const location_type& l)
      {
        return symbol_type (token::TOK_ENDPACKAGE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_PACKAGESEP (location_type l)
      {
        return symbol_type (token::TOK_PACKAGESEP, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_PACKAGESEP (const location_type& l)
      {
        return symbol_type (token::TOK_PACKAGESEP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_INTERFACE (location_type l)
      {
        return symbol_type (token::TOK_INTERFACE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_INTERFACE (const location_type& l)
      {
        return symbol_type (token::TOK_INTERFACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ENDINTERFACE (location_type l)
      {
        return symbol_type (token::TOK_ENDINTERFACE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ENDINTERFACE (const location_type& l)
      {
        return symbol_type (token::TOK_ENDINTERFACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_MODPORT (location_type l)
      {
        return symbol_type (token::TOK_MODPORT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_MODPORT (const location_type& l)
      {
        return symbol_type (token::TOK_MODPORT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_VAR (location_type l)
      {
        return symbol_type (token::TOK_VAR, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_VAR (const location_type& l)
      {
        return symbol_type (token::TOK_VAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_WILDCARD_CONNECT (location_type l)
      {
        return symbol_type (token::TOK_WILDCARD_CONNECT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_WILDCARD_CONNECT (const location_type& l)
      {
        return symbol_type (token::TOK_WILDCARD_CONNECT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_INPUT (location_type l)
      {
        return symbol_type (token::TOK_INPUT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_INPUT (const location_type& l)
      {
        return symbol_type (token::TOK_INPUT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_OUTPUT (location_type l)
      {
        return symbol_type (token::TOK_OUTPUT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_OUTPUT (const location_type& l)
      {
        return symbol_type (token::TOK_OUTPUT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_INOUT (location_type l)
      {
        return symbol_type (token::TOK_INOUT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_INOUT (const location_type& l)
      {
        return symbol_type (token::TOK_INOUT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_WIRE (location_type l)
      {
        return symbol_type (token::TOK_WIRE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_WIRE (const location_type& l)
      {
        return symbol_type (token::TOK_WIRE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_WAND (location_type l)
      {
        return symbol_type (token::TOK_WAND, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_WAND (const location_type& l)
      {
        return symbol_type (token::TOK_WAND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_WOR (location_type l)
      {
        return symbol_type (token::TOK_WOR, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_WOR (const location_type& l)
      {
        return symbol_type (token::TOK_WOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_REG (location_type l)
      {
        return symbol_type (token::TOK_REG, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_REG (const location_type& l)
      {
        return symbol_type (token::TOK_REG, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_LOGIC (location_type l)
      {
        return symbol_type (token::TOK_LOGIC, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_LOGIC (const location_type& l)
      {
        return symbol_type (token::TOK_LOGIC, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_INTEGER (location_type l)
      {
        return symbol_type (token::TOK_INTEGER, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_INTEGER (const location_type& l)
      {
        return symbol_type (token::TOK_INTEGER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SIGNED (location_type l)
      {
        return symbol_type (token::TOK_SIGNED, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SIGNED (const location_type& l)
      {
        return symbol_type (token::TOK_SIGNED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ALWAYS (location_type l)
      {
        return symbol_type (token::TOK_ALWAYS, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ALWAYS (const location_type& l)
      {
        return symbol_type (token::TOK_ALWAYS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_INITIAL (location_type l)
      {
        return symbol_type (token::TOK_INITIAL, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_INITIAL (const location_type& l)
      {
        return symbol_type (token::TOK_INITIAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ALWAYS_FF (location_type l)
      {
        return symbol_type (token::TOK_ALWAYS_FF, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ALWAYS_FF (const location_type& l)
      {
        return symbol_type (token::TOK_ALWAYS_FF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ALWAYS_COMB (location_type l)
      {
        return symbol_type (token::TOK_ALWAYS_COMB, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ALWAYS_COMB (const location_type& l)
      {
        return symbol_type (token::TOK_ALWAYS_COMB, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ALWAYS_LATCH (location_type l)
      {
        return symbol_type (token::TOK_ALWAYS_LATCH, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ALWAYS_LATCH (const location_type& l)
      {
        return symbol_type (token::TOK_ALWAYS_LATCH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_BEGIN (location_type l)
      {
        return symbol_type (token::TOK_BEGIN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_BEGIN (const location_type& l)
      {
        return symbol_type (token::TOK_BEGIN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_END (location_type l)
      {
        return symbol_type (token::TOK_END, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_END (const location_type& l)
      {
        return symbol_type (token::TOK_END, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_IF (location_type l)
      {
        return symbol_type (token::TOK_IF, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_IF (const location_type& l)
      {
        return symbol_type (token::TOK_IF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ELSE (location_type l)
      {
        return symbol_type (token::TOK_ELSE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ELSE (const location_type& l)
      {
        return symbol_type (token::TOK_ELSE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_IFNONE (location_type l)
      {
        return symbol_type (token::TOK_IFNONE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_IFNONE (const location_type& l)
      {
        return symbol_type (token::TOK_IFNONE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_FOR (location_type l)
      {
        return symbol_type (token::TOK_FOR, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_FOR (const location_type& l)
      {
        return symbol_type (token::TOK_FOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_WHILE (location_type l)
      {
        return symbol_type (token::TOK_WHILE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_WHILE (const location_type& l)
      {
        return symbol_type (token::TOK_WHILE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_REPEAT (location_type l)
      {
        return symbol_type (token::TOK_REPEAT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_REPEAT (const location_type& l)
      {
        return symbol_type (token::TOK_REPEAT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_DPI_FUNCTION (location_type l)
      {
        return symbol_type (token::TOK_DPI_FUNCTION, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_DPI_FUNCTION (const location_type& l)
      {
        return symbol_type (token::TOK_DPI_FUNCTION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_POSEDGE (location_type l)
      {
        return symbol_type (token::TOK_POSEDGE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_POSEDGE (const location_type& l)
      {
        return symbol_type (token::TOK_POSEDGE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_NEGEDGE (location_type l)
      {
        return symbol_type (token::TOK_NEGEDGE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_NEGEDGE (const location_type& l)
      {
        return symbol_type (token::TOK_NEGEDGE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_OR (location_type l)
      {
        return symbol_type (token::TOK_OR, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_OR (const location_type& l)
      {
        return symbol_type (token::TOK_OR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_AUTOMATIC (location_type l)
      {
        return symbol_type (token::TOK_AUTOMATIC, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_AUTOMATIC (const location_type& l)
      {
        return symbol_type (token::TOK_AUTOMATIC, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_CASE (location_type l)
      {
        return symbol_type (token::TOK_CASE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_CASE (const location_type& l)
      {
        return symbol_type (token::TOK_CASE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_CASEX (location_type l)
      {
        return symbol_type (token::TOK_CASEX, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_CASEX (const location_type& l)
      {
        return symbol_type (token::TOK_CASEX, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_CASEZ (location_type l)
      {
        return symbol_type (token::TOK_CASEZ, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_CASEZ (const location_type& l)
      {
        return symbol_type (token::TOK_CASEZ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ENDCASE (location_type l)
      {
        return symbol_type (token::TOK_ENDCASE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ENDCASE (const location_type& l)
      {
        return symbol_type (token::TOK_ENDCASE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_DEFAULT (location_type l)
      {
        return symbol_type (token::TOK_DEFAULT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_DEFAULT (const location_type& l)
      {
        return symbol_type (token::TOK_DEFAULT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_FUNCTION (location_type l)
      {
        return symbol_type (token::TOK_FUNCTION, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_FUNCTION (const location_type& l)
      {
        return symbol_type (token::TOK_FUNCTION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ENDFUNCTION (location_type l)
      {
        return symbol_type (token::TOK_ENDFUNCTION, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ENDFUNCTION (const location_type& l)
      {
        return symbol_type (token::TOK_ENDFUNCTION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_TASK (location_type l)
      {
        return symbol_type (token::TOK_TASK, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_TASK (const location_type& l)
      {
        return symbol_type (token::TOK_TASK, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ENDTASK (location_type l)
      {
        return symbol_type (token::TOK_ENDTASK, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ENDTASK (const location_type& l)
      {
        return symbol_type (token::TOK_ENDTASK, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SPECIFY (location_type l)
      {
        return symbol_type (token::TOK_SPECIFY, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SPECIFY (const location_type& l)
      {
        return symbol_type (token::TOK_SPECIFY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_IGNORED_SPECIFY (location_type l)
      {
        return symbol_type (token::TOK_IGNORED_SPECIFY, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_IGNORED_SPECIFY (const location_type& l)
      {
        return symbol_type (token::TOK_IGNORED_SPECIFY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ENDSPECIFY (location_type l)
      {
        return symbol_type (token::TOK_ENDSPECIFY, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ENDSPECIFY (const location_type& l)
      {
        return symbol_type (token::TOK_ENDSPECIFY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SPECPARAM (location_type l)
      {
        return symbol_type (token::TOK_SPECPARAM, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SPECPARAM (const location_type& l)
      {
        return symbol_type (token::TOK_SPECPARAM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SPECIFY_AND (location_type l)
      {
        return symbol_type (token::TOK_SPECIFY_AND, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SPECIFY_AND (const location_type& l)
      {
        return symbol_type (token::TOK_SPECIFY_AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_IGNORED_SPECIFY_AND (location_type l)
      {
        return symbol_type (token::TOK_IGNORED_SPECIFY_AND, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_IGNORED_SPECIFY_AND (const location_type& l)
      {
        return symbol_type (token::TOK_IGNORED_SPECIFY_AND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_GENERATE (location_type l)
      {
        return symbol_type (token::TOK_GENERATE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_GENERATE (const location_type& l)
      {
        return symbol_type (token::TOK_GENERATE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ENDGENERATE (location_type l)
      {
        return symbol_type (token::TOK_ENDGENERATE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ENDGENERATE (const location_type& l)
      {
        return symbol_type (token::TOK_ENDGENERATE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_GENVAR (location_type l)
      {
        return symbol_type (token::TOK_GENVAR, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_GENVAR (const location_type& l)
      {
        return symbol_type (token::TOK_GENVAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_REAL (location_type l)
      {
        return symbol_type (token::TOK_REAL, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_REAL (const location_type& l)
      {
        return symbol_type (token::TOK_REAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SYNOPSYS_FULL_CASE (location_type l)
      {
        return symbol_type (token::TOK_SYNOPSYS_FULL_CASE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SYNOPSYS_FULL_CASE (const location_type& l)
      {
        return symbol_type (token::TOK_SYNOPSYS_FULL_CASE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SYNOPSYS_PARALLEL_CASE (location_type l)
      {
        return symbol_type (token::TOK_SYNOPSYS_PARALLEL_CASE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SYNOPSYS_PARALLEL_CASE (const location_type& l)
      {
        return symbol_type (token::TOK_SYNOPSYS_PARALLEL_CASE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SUPPLY0 (location_type l)
      {
        return symbol_type (token::TOK_SUPPLY0, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SUPPLY0 (const location_type& l)
      {
        return symbol_type (token::TOK_SUPPLY0, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SUPPLY1 (location_type l)
      {
        return symbol_type (token::TOK_SUPPLY1, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SUPPLY1 (const location_type& l)
      {
        return symbol_type (token::TOK_SUPPLY1, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_TO_SIGNED (location_type l)
      {
        return symbol_type (token::TOK_TO_SIGNED, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_TO_SIGNED (const location_type& l)
      {
        return symbol_type (token::TOK_TO_SIGNED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_TO_UNSIGNED (location_type l)
      {
        return symbol_type (token::TOK_TO_UNSIGNED, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_TO_UNSIGNED (const location_type& l)
      {
        return symbol_type (token::TOK_TO_UNSIGNED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_POS_INDEXED (location_type l)
      {
        return symbol_type (token::TOK_POS_INDEXED, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_POS_INDEXED (const location_type& l)
      {
        return symbol_type (token::TOK_POS_INDEXED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_NEG_INDEXED (location_type l)
      {
        return symbol_type (token::TOK_NEG_INDEXED, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_NEG_INDEXED (const location_type& l)
      {
        return symbol_type (token::TOK_NEG_INDEXED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_PROPERTY (location_type l)
      {
        return symbol_type (token::TOK_PROPERTY, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_PROPERTY (const location_type& l)
      {
        return symbol_type (token::TOK_PROPERTY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ENUM (location_type l)
      {
        return symbol_type (token::TOK_ENUM, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ENUM (const location_type& l)
      {
        return symbol_type (token::TOK_ENUM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_TYPEDEF (location_type l)
      {
        return symbol_type (token::TOK_TYPEDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_TYPEDEF (const location_type& l)
      {
        return symbol_type (token::TOK_TYPEDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_RAND (location_type l)
      {
        return symbol_type (token::TOK_RAND, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_RAND (const location_type& l)
      {
        return symbol_type (token::TOK_RAND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_CONST (location_type l)
      {
        return symbol_type (token::TOK_CONST, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_CONST (const location_type& l)
      {
        return symbol_type (token::TOK_CONST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_CHECKER (location_type l)
      {
        return symbol_type (token::TOK_CHECKER, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_CHECKER (const location_type& l)
      {
        return symbol_type (token::TOK_CHECKER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ENDCHECKER (location_type l)
      {
        return symbol_type (token::TOK_ENDCHECKER, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ENDCHECKER (const location_type& l)
      {
        return symbol_type (token::TOK_ENDCHECKER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_EVENTUALLY (location_type l)
      {
        return symbol_type (token::TOK_EVENTUALLY, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_EVENTUALLY (const location_type& l)
      {
        return symbol_type (token::TOK_EVENTUALLY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_INCREMENT (location_type l)
      {
        return symbol_type (token::TOK_INCREMENT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_INCREMENT (const location_type& l)
      {
        return symbol_type (token::TOK_INCREMENT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_DECREMENT (location_type l)
      {
        return symbol_type (token::TOK_DECREMENT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_DECREMENT (const location_type& l)
      {
        return symbol_type (token::TOK_DECREMENT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_UNIQUE (location_type l)
      {
        return symbol_type (token::TOK_UNIQUE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_UNIQUE (const location_type& l)
      {
        return symbol_type (token::TOK_UNIQUE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_UNIQUE0 (location_type l)
      {
        return symbol_type (token::TOK_UNIQUE0, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_UNIQUE0 (const location_type& l)
      {
        return symbol_type (token::TOK_UNIQUE0, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_PRIORITY (location_type l)
      {
        return symbol_type (token::TOK_PRIORITY, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_PRIORITY (const location_type& l)
      {
        return symbol_type (token::TOK_PRIORITY, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_STRUCT (location_type l)
      {
        return symbol_type (token::TOK_STRUCT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_STRUCT (const location_type& l)
      {
        return symbol_type (token::TOK_STRUCT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_PACKED (location_type l)
      {
        return symbol_type (token::TOK_PACKED, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_PACKED (const location_type& l)
      {
        return symbol_type (token::TOK_PACKED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_UNSIGNED (location_type l)
      {
        return symbol_type (token::TOK_UNSIGNED, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_UNSIGNED (const location_type& l)
      {
        return symbol_type (token::TOK_UNSIGNED, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_INT (location_type l)
      {
        return symbol_type (token::TOK_INT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_INT (const location_type& l)
      {
        return symbol_type (token::TOK_INT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_BYTE (location_type l)
      {
        return symbol_type (token::TOK_BYTE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_BYTE (const location_type& l)
      {
        return symbol_type (token::TOK_BYTE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SHORTINT (location_type l)
      {
        return symbol_type (token::TOK_SHORTINT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SHORTINT (const location_type& l)
      {
        return symbol_type (token::TOK_SHORTINT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_LONGINT (location_type l)
      {
        return symbol_type (token::TOK_LONGINT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_LONGINT (const location_type& l)
      {
        return symbol_type (token::TOK_LONGINT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_VOID (location_type l)
      {
        return symbol_type (token::TOK_VOID, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_VOID (const location_type& l)
      {
        return symbol_type (token::TOK_VOID, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_UNION (location_type l)
      {
        return symbol_type (token::TOK_UNION, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_UNION (const location_type& l)
      {
        return symbol_type (token::TOK_UNION, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_BIT_OR_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_BIT_OR_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_BIT_OR_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_BIT_OR_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_BIT_AND_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_BIT_AND_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_BIT_AND_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_BIT_AND_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_BIT_XOR_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_BIT_XOR_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_BIT_XOR_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_BIT_XOR_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ADD_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_ADD_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ADD_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_ADD_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SUB_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_SUB_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SUB_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_SUB_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_DIV_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_DIV_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_DIV_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_DIV_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_MOD_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_MOD_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_MOD_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_MOD_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_MUL_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_MUL_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_MUL_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_MUL_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SHL_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_SHL_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SHL_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_SHL_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SHR_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_SHR_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SHR_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_SHR_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SSHL_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_SSHL_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SSHL_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_SSHL_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SSHR_ASSIGN (location_type l)
      {
        return symbol_type (token::TOK_SSHR_ASSIGN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SSHR_ASSIGN (const location_type& l)
      {
        return symbol_type (token::TOK_SSHR_ASSIGN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_BIND (location_type l)
      {
        return symbol_type (token::TOK_BIND, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_BIND (const location_type& l)
      {
        return symbol_type (token::TOK_BIND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_TIME_SCALE (location_type l)
      {
        return symbol_type (token::TOK_TIME_SCALE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_TIME_SCALE (const location_type& l)
      {
        return symbol_type (token::TOK_TIME_SCALE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_IMPORT (location_type l)
      {
        return symbol_type (token::TOK_IMPORT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_IMPORT (const location_type& l)
      {
        return symbol_type (token::TOK_IMPORT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_EXCL (location_type l)
      {
        return symbol_type (token::TOK_EXCL, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_EXCL (const location_type& l)
      {
        return symbol_type (token::TOK_EXCL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_HASH (location_type l)
      {
        return symbol_type (token::TOK_HASH, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_HASH (const location_type& l)
      {
        return symbol_type (token::TOK_HASH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_PERC (location_type l)
      {
        return symbol_type (token::TOK_PERC, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_PERC (const location_type& l)
      {
        return symbol_type (token::TOK_PERC, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_AMP (location_type l)
      {
        return symbol_type (token::TOK_AMP, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_AMP (const location_type& l)
      {
        return symbol_type (token::TOK_AMP, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_LPAREN (location_type l)
      {
        return symbol_type (token::TOK_LPAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_LPAREN (const location_type& l)
      {
        return symbol_type (token::TOK_LPAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_RPAREN (location_type l)
      {
        return symbol_type (token::TOK_RPAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_RPAREN (const location_type& l)
      {
        return symbol_type (token::TOK_RPAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_ASTER (location_type l)
      {
        return symbol_type (token::TOK_ASTER, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_ASTER (const location_type& l)
      {
        return symbol_type (token::TOK_ASTER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_PLUS (location_type l)
      {
        return symbol_type (token::TOK_PLUS, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_PLUS (const location_type& l)
      {
        return symbol_type (token::TOK_PLUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_COMMA (location_type l)
      {
        return symbol_type (token::TOK_COMMA, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_COMMA (const location_type& l)
      {
        return symbol_type (token::TOK_COMMA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_MINUS (location_type l)
      {
        return symbol_type (token::TOK_MINUS, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_MINUS (const location_type& l)
      {
        return symbol_type (token::TOK_MINUS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_DOT (location_type l)
      {
        return symbol_type (token::TOK_DOT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_DOT (const location_type& l)
      {
        return symbol_type (token::TOK_DOT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SLASH (location_type l)
      {
        return symbol_type (token::TOK_SLASH, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SLASH (const location_type& l)
      {
        return symbol_type (token::TOK_SLASH, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_COL (location_type l)
      {
        return symbol_type (token::TOK_COL, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_COL (const location_type& l)
      {
        return symbol_type (token::TOK_COL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_SEMICOL (location_type l)
      {
        return symbol_type (token::TOK_SEMICOL, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_SEMICOL (const location_type& l)
      {
        return symbol_type (token::TOK_SEMICOL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_LT (location_type l)
      {
        return symbol_type (token::TOK_LT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_LT (const location_type& l)
      {
        return symbol_type (token::TOK_LT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_EQ (location_type l)
      {
        return symbol_type (token::TOK_EQ, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_EQ (const location_type& l)
      {
        return symbol_type (token::TOK_EQ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_GT (location_type l)
      {
        return symbol_type (token::TOK_GT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_GT (const location_type& l)
      {
        return symbol_type (token::TOK_GT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_QUE (location_type l)
      {
        return symbol_type (token::TOK_QUE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_QUE (const location_type& l)
      {
        return symbol_type (token::TOK_QUE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_AT (location_type l)
      {
        return symbol_type (token::TOK_AT, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_AT (const location_type& l)
      {
        return symbol_type (token::TOK_AT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_LBRA (location_type l)
      {
        return symbol_type (token::TOK_LBRA, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_LBRA (const location_type& l)
      {
        return symbol_type (token::TOK_LBRA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_RBRA (location_type l)
      {
        return symbol_type (token::TOK_RBRA, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_RBRA (const location_type& l)
      {
        return symbol_type (token::TOK_RBRA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_CARET (location_type l)
      {
        return symbol_type (token::TOK_CARET, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_CARET (const location_type& l)
      {
        return symbol_type (token::TOK_CARET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_UNDER (location_type l)
      {
        return symbol_type (token::TOK_UNDER, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_UNDER (const location_type& l)
      {
        return symbol_type (token::TOK_UNDER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_LCURL (location_type l)
      {
        return symbol_type (token::TOK_LCURL, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_LCURL (const location_type& l)
      {
        return symbol_type (token::TOK_LCURL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_PIPE (location_type l)
      {
        return symbol_type (token::TOK_PIPE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_PIPE (const location_type& l)
      {
        return symbol_type (token::TOK_PIPE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_RCURL (location_type l)
      {
        return symbol_type (token::TOK_RCURL, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_RCURL (const location_type& l)
      {
        return symbol_type (token::TOK_RCURL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_TILDE (location_type l)
      {
        return symbol_type (token::TOK_TILDE, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_TILDE (const location_type& l)
      {
        return symbol_type (token::TOK_TILDE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_n (location_type l)
      {
        return symbol_type (token::TOK_n, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_n (const location_type& l)
      {
        return symbol_type (token::TOK_n, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_p (location_type l)
      {
        return symbol_type (token::TOK_p, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_p (const location_type& l)
      {
        return symbol_type (token::TOK_p, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_x (location_type l)
      {
        return symbol_type (token::TOK_x, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_x (const location_type& l)
      {
        return symbol_type (token::TOK_x, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TOK_z (location_type l)
      {
        return symbol_type (token::TOK_z, std::move (l));
      }
#else
      static
      symbol_type
      make_TOK_z (const location_type& l)
      {
        return symbol_type (token::TOK_z, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_LOR (location_type l)
      {
        return symbol_type (token::OP_LOR, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_LOR (const location_type& l)
      {
        return symbol_type (token::OP_LOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_LAND (location_type l)
      {
        return symbol_type (token::OP_LAND, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_LAND (const location_type& l)
      {
        return symbol_type (token::OP_LAND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_NOR (location_type l)
      {
        return symbol_type (token::OP_NOR, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_NOR (const location_type& l)
      {
        return symbol_type (token::OP_NOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_XNOR (location_type l)
      {
        return symbol_type (token::OP_XNOR, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_XNOR (const location_type& l)
      {
        return symbol_type (token::OP_XNOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_NAND (location_type l)
      {
        return symbol_type (token::OP_NAND, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_NAND (const location_type& l)
      {
        return symbol_type (token::OP_NAND, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_EQ (location_type l)
      {
        return symbol_type (token::OP_EQ, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_EQ (const location_type& l)
      {
        return symbol_type (token::OP_EQ, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_NE (location_type l)
      {
        return symbol_type (token::OP_NE, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_NE (const location_type& l)
      {
        return symbol_type (token::OP_NE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_EQX (location_type l)
      {
        return symbol_type (token::OP_EQX, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_EQX (const location_type& l)
      {
        return symbol_type (token::OP_EQX, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_NEX (location_type l)
      {
        return symbol_type (token::OP_NEX, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_NEX (const location_type& l)
      {
        return symbol_type (token::OP_NEX, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_LE (location_type l)
      {
        return symbol_type (token::OP_LE, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_LE (const location_type& l)
      {
        return symbol_type (token::OP_LE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_GE (location_type l)
      {
        return symbol_type (token::OP_GE, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_GE (const location_type& l)
      {
        return symbol_type (token::OP_GE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_SHL (location_type l)
      {
        return symbol_type (token::OP_SHL, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_SHL (const location_type& l)
      {
        return symbol_type (token::OP_SHL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_SHR (location_type l)
      {
        return symbol_type (token::OP_SHR, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_SHR (const location_type& l)
      {
        return symbol_type (token::OP_SHR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_SSHL (location_type l)
      {
        return symbol_type (token::OP_SSHL, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_SSHL (const location_type& l)
      {
        return symbol_type (token::OP_SSHL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_SSHR (location_type l)
      {
        return symbol_type (token::OP_SSHR, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_SSHR (const location_type& l)
      {
        return symbol_type (token::OP_SSHR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_POW (location_type l)
      {
        return symbol_type (token::OP_POW, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_POW (const location_type& l)
      {
        return symbol_type (token::OP_POW, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OP_CAST (location_type l)
      {
        return symbol_type (token::OP_CAST, std::move (l));
      }
#else
      static
      symbol_type
      make_OP_CAST (const location_type& l)
      {
        return symbol_type (token::OP_CAST, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UNARY_OPS (location_type l)
      {
        return symbol_type (token::UNARY_OPS, std::move (l));
      }
#else
      static
      symbol_type
      make_UNARY_OPS (const location_type& l)
      {
        return symbol_type (token::UNARY_OPS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FAKE_THEN (location_type l)
      {
        return symbol_type (token::FAKE_THEN, std::move (l));
      }
#else
      static
      symbol_type
      make_FAKE_THEN (const location_type& l)
      {
        return symbol_type (token::FAKE_THEN, l);
      }
#endif


    class context
    {
    public:
      context (const parser& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const parser& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    parser (const parser&);
    /// Non copyable.
    parser& operator= (const parser&);
#endif

    /// Check the lookahead yytoken.
    /// \returns  true iff the token will be eventually shifted.
    bool yy_lac_check_ (symbol_kind_type yytoken) const;
    /// Establish the initial context if no initial context currently exists.
    /// \returns  true iff the token will be eventually shifted.
    bool yy_lac_establish_ (symbol_kind_type yytoken);
    /// Discard any previous initial lookahead context because of event.
    /// \param event  the event which caused the lookahead to be discarded.
    ///               Only used for debbuging output.
    void yy_lac_discard_ (const char* event);

    /// Stored state numbers (used for stacks).
    typedef short state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const short yypact_ninf_;
    static const short yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;



    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const short yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const short yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const short yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const short yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if FRONTEND_VERILOG_YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;
    /// The stack for LAC.
    /// Logically, the yy_lac_stack's lifetime is confined to the function
    /// yy_lac_check_. We just store it as a member of this class to hold
    /// on to the memory and to avoid frequent reallocations.
    /// Since yy_lac_check_ is const, this member must be mutable.
    mutable std::vector<state_type> yylac_stack_;
    /// Whether an initial LAC context was established.
    bool yy_lac_established_;


    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 4086,     ///< Last index in yytable_.
      yynnts_ = 317,  ///< Number of nonterminal symbols.
      yyfinal_ = 3 ///< Termination state number.
    };


    // User arguments.
    YOSYS_NAMESPACE_PREFIX VERILOG_FRONTEND::VerilogLexer* lexer;
    YOSYS_NAMESPACE_PREFIX VERILOG_FRONTEND::ParseState* extra;
    YOSYS_NAMESPACE_PREFIX VERILOG_FRONTEND::ParseMode* mode;

  };

  inline
  parser::symbol_kind_type
  parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const unsigned char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190
    };
    // Last valid token kind.
    const int code_max = 445;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

  // basic_symbol.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_al_t: // al_t
      case symbol_kind::S_attr: // attr
      case symbol_kind::S_if_attr: // if_attr
      case symbol_kind::S_case_attr: // case_attr
        value.copy< al_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ast_node_type_t: // ast_node_type_t
      case symbol_kind::S_asgn_binop: // asgn_binop
      case symbol_kind::S_inc_or_dec_op: // inc_or_dec_op
        value.copy< ast_node_type_t > (YY_MOVE (that.value));
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
        value.copy< ast_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_boolean_t: // "boolean"
      case symbol_kind::S_opt_signedness_default_signed: // opt_signedness_default_signed
      case symbol_kind::S_opt_signedness_default_unsigned: // opt_signedness_default_unsigned
      case symbol_kind::S_always_comb_or_latch: // always_comb_or_latch
      case symbol_kind::S_always_or_always_ff: // always_or_always_ff
      case symbol_kind::S_opt_property: // opt_property
        value.copy< boolean_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ch_t: // "invalid token"
      case symbol_kind::S_specify_edge: // specify_edge
        value.copy< ch_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_integer_t: // "integer"
      case symbol_kind::S_integer_atom_type: // integer_atom_type
      case symbol_kind::S_integer_vector_type: // integer_vector_type
        value.copy< integer_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_specify_rise_fall_ptr_t: // "specify rise and fall"
      case symbol_kind::S_specify_rise_fall: // specify_rise_fall
        value.copy< specify_rise_fall_ptr_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_specify_target_ptr_t: // "specify target"
      case symbol_kind::S_specify_target: // specify_target
        value.copy< specify_target_ptr_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_specify_triple_ptr_t: // "specify triple"
      case symbol_kind::S_specify_opt_triple: // specify_opt_triple
      case symbol_kind::S_specify_triple: // specify_triple
        value.copy< specify_triple_ptr_t > (YY_MOVE (that.value));
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
        value.copy< string_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  parser::symbol_kind_type
  parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_al_t: // al_t
      case symbol_kind::S_attr: // attr
      case symbol_kind::S_if_attr: // if_attr
      case symbol_kind::S_case_attr: // case_attr
        value.move< al_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_ast_node_type_t: // ast_node_type_t
      case symbol_kind::S_asgn_binop: // asgn_binop
      case symbol_kind::S_inc_or_dec_op: // inc_or_dec_op
        value.move< ast_node_type_t > (YY_MOVE (s.value));
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
        value.move< ast_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_boolean_t: // "boolean"
      case symbol_kind::S_opt_signedness_default_signed: // opt_signedness_default_signed
      case symbol_kind::S_opt_signedness_default_unsigned: // opt_signedness_default_unsigned
      case symbol_kind::S_always_comb_or_latch: // always_comb_or_latch
      case symbol_kind::S_always_or_always_ff: // always_or_always_ff
      case symbol_kind::S_opt_property: // opt_property
        value.move< boolean_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_ch_t: // "invalid token"
      case symbol_kind::S_specify_edge: // specify_edge
        value.move< ch_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_integer_t: // "integer"
      case symbol_kind::S_integer_atom_type: // integer_atom_type
      case symbol_kind::S_integer_vector_type: // integer_vector_type
        value.move< integer_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_specify_rise_fall_ptr_t: // "specify rise and fall"
      case symbol_kind::S_specify_rise_fall: // specify_rise_fall
        value.move< specify_rise_fall_ptr_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_specify_target_ptr_t: // "specify target"
      case symbol_kind::S_specify_target: // specify_target
        value.move< specify_target_ptr_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_specify_triple_ptr_t: // "specify triple"
      case symbol_kind::S_specify_opt_triple: // specify_opt_triple
      case symbol_kind::S_specify_triple: // specify_triple
        value.move< specify_triple_ptr_t > (YY_MOVE (s.value));
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
        value.move< string_t > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  inline
  parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  inline
  parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  inline
  parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  inline
  parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  inline
  void
  parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  inline
  void
  parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  inline
  parser::symbol_kind_type
  parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  inline
  parser::symbol_kind_type
  parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


} // frontend_verilog_yy
#line 5552 "verilog_parser.tab.hh"


// "%code provides" blocks.
#line 61 "C:/Users/REDACTED/AppData/Local/Temp/fpga-regen50-q31xuiid/source/frontends/verilog/verilog_parser.y"

	USING_YOSYS_NAMESPACE;
	using namespace AST;
	using namespace VERILOG_FRONTEND;
	using parser = frontend_verilog_yy::parser;
	YOSYS_NAMESPACE_BEGIN
	namespace VERILOG_FRONTEND {
		typedef std::map<std::string, AST::AstNode*> UserTypeMap;
		struct ParseState {
			int port_counter;
			dict<std::string, int> port_stubs;
			std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>> attr_list;
			dict<IdString, std::unique_ptr<AstNode>> default_attr_list;
			std::stack<std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>>> attr_list_stack;
			std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>> albuf;
			std::vector<UserTypeMap> user_type_stack;
			dict<std::string, AstNode*> pkg_user_types;
			std::vector<AstNode*> ast_stack;
			std::unique_ptr<AstNode> astbuf1, astbuf2, astbuf3;
			AstNode* cell_hack;
			AstNode* member_hack;
			struct AstNode *current_function_or_task;
			struct AstNode *current_ast, *current_ast_mod;
			int current_function_or_task_port_id;
			std::vector<char> case_type_stack;
			bool do_not_require_port_stubs;
			bool current_wire_rand, current_wire_const, current_wire_automatic;
			bool current_modport_input, current_modport_output;
			bool default_nettype_wire = true;
			std::istream* lexin;

			AstNode* saveChild(std::unique_ptr<AstNode> child);
			AstNode* pushChild(std::unique_ptr<AstNode> child);
			void addWiretypeNode(std::string *name, AstNode* node);
			void addTypedefNode(std::string *name, std::unique_ptr<AstNode> node);
			void enterTypeScope();
			void exitTypeScope();
			bool isInLocalScope(const std::string *name);
			void rewriteGenForDeclInit(AstNode *loop);
			void ensureAsgnExprAllowed(const parser::location_type loc, bool sv_mode);
			const AstNode *addIncOrDecStmt(std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>> stmt_attr,
									std::unique_ptr<AstNode> lhs,
									std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>> op_attr, AST::AstNodeType op,
									parser::location_type loc);
			std::unique_ptr<AstNode> addIncOrDecExpr(std::unique_ptr<AstNode> lhs,
								 std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>> attr,
								 AST::AstNodeType op, parser::location_type loc, bool undo, bool sv_mode);
			// add a binary operator assignment statement, e.g., a += b
			std::unique_ptr<AstNode> addAsgnBinopStmt(std::unique_ptr<dict<IdString, std::unique_ptr<AstNode>>> attr,
								  std::unique_ptr<AstNode> eq_lhs, AST::AstNodeType op, std::unique_ptr<AstNode> rhs);
		};
		struct ParseMode {
			bool noassert = false;
			bool noassume = false;
			bool norestrict = false;
			bool sv = false;
			bool formal = false;
			bool lib = false;
			bool specify = false;
			bool assume_asserts = false;
			bool assert_assumes = false;
		};
	};
	YOSYS_NAMESPACE_END

#line 5622 "verilog_parser.tab.hh"


#endif // !YY_FRONTEND_VERILOG_YY_VERILOG_PARSER_TAB_HH_INCLUDED
