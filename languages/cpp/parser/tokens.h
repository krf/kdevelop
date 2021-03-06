/* This file is part of KDevelop
    Copyright 2002-2005 Roberto Raggi <roberto@kdevelop.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef TOKENS_H
#define TOKENS_H

#include <cppparserexport.h>

/**@file Token kinds.*/

/**Token kinds.*/
enum TOKEN_KIND
{
    Token_EOF = 0,

    Token_K_DCOP = 1000,
    Token_Q_OBJECT,
    Token___typeof,
    Token_and,
    Token_and_eq,
    Token_arrow,
    Token_asm,
    Token_assign,
    Token_auto,
    Token_bitand,
    Token_bitor,
    Token_bool,
    Token_break,
    Token_case,
    Token_catch,
    Token_char,
    Token_char_literal,
    Token_char16_t,
    Token_char32_t,
    Token_class,
    Token_comment,
    Token_compl,
    Token_concat,
    Token_const,
    Token_constexpr,
    Token_const_cast,
    Token_continue,
    Token_decr,
    Token_decltype,
    Token_default,
    Token_delete,
    Token_do,
    Token_double,
    Token_dynamic_cast,
    Token_ellipsis,
    Token_else,
    //Token_emit, // Disable emit as its also a valid function call and the token isn't used anywhere
    Token_enum,
    Token_eq,
    Token_explicit,
    Token_export,
    Token_extern,
    Token_false,
    Token_final,
    Token_float,
    Token_for,
    Token_friend,
    Token_geq,
    Token_goto,
    Token_identifier,
    Token_if,
    Token_incr,
    Token_inline,
    Token_int,
    Token_k_dcop,
    Token_k_dcop_signals,
    Token_leq,
    Token_long,
    Token_mutable,
    Token_namespace,
    Token_new,
    Token_noexcept,
    Token_not,
    Token_not_eq,
    Token_nullptr,
    Token_number_literal,
    Token_operator,
    Token_or,
    Token_or_eq,
    Token_override,
    Token_preproc,
    Token_private,
    Token_protected,
    Token_ptrmem,
    Token_public,
    Token_register,
    Token_reinterpret_cast,
    Token_return,
    Token_scope,
    Token_leftshift,
    Token_rightshift,
    Token_short,
    Token___qt_signals__,
    Token_signed,
    Token_sizeof,
    Token___qt_slots__,
    Token_static,
    Token_static_assert,
    Token_static_cast,
    Token_string_literal,
    Token_struct,
    Token_switch,
    Token_template,
    Token_this,
    Token_thread_local,
    Token_throw,
    Token_true,
    Token_try,
    Token_typedef,
    Token_typeid,
    Token_typename,
    Token_union,
    Token_unsigned,
    Token_using,
    Token_virtual,
    Token_void,
    Token_volatile,
    Token_wchar_t,
    Token_while,
    Token_whitespaces,
    Token_xor,
    Token_xor_eq,
    Token___qt_signal__,
    Token___qt_slot__,
    Token___qt_property__,

    TOKEN_KIND_COUNT
};

/**@return human-readable name of the token.*/
KDEVCPPPARSER_EXPORT char const *token_name(int token);

/**@return text corresponding to the token.*/
KDEVCPPPARSER_EXPORT char const *token_text(int token);

#endif

