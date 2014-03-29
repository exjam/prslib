#ifndef PRS_STRING_PARSER_H
#define PRS_STRING_PARSER_H

/**
 * Matches a whole string
 *  returns String type
 *
 * Created with string_("string")
 */

#include "base_parser.h"
#include "optional.h"
#include <vector>
#include <tuple>

namespace prs
{

template<typename String>
struct string_parser : public parser<string_parser<String>>
{
   string_parser(String str) :
      str(str)
   {
   }

   using value_type = String;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context&, value_type &myv) const
   {
      Iterator begin = first;

      for (String strItr = str; *strItr != 0; ++strItr, ++first) {
         if (first == last || *first != *strItr) {
            first = begin;
            return false;
         }
      }

      myv = str;
      return true;
   }

   String str;
};

template<typename String>
inline auto string_(String ch)
   -> string_parser<String>
{
   return string_parser<String> { ch };
}


/*
 * Some helper functions to convert built-in AST types to a string
 */
template<typename String>
inline String ast_to_string(const char &val)
{
   return String(1, static_cast<typename String::value_type>(val));
}

template<typename String>
inline String ast_to_string(const wchar_t &val)
{
   return String(1, static_cast<typename String::value_type>(val));
}

template<typename String>
inline String ast_to_string(const int &val)
{
   return String(1, static_cast<typename String::value_type>(val));
}

template<typename String>
inline String ast_to_string(const bool &)
{
   /* Comes from a NOT parser, ignore... */
   return String { };
}

template<typename String, typename Value>
inline String ast_to_string(const Value &val)
{
   return String { val };
}

template<typename String, typename Option>
inline String ast_to_string(const optional<Option> &opt)
{
   return opt ? ast_to_string<String>(*opt) : String { };
}

template<typename String, std::size_t N = 0, typename... T>
inline auto ast_to_string(const std::tuple<T...>&)
   -> typename std::enable_if<N == sizeof...(T), String>::type
{
   return String { };
}

template<typename String, std::size_t N = 0, typename... T>
inline auto ast_to_string(const std::tuple<T...>& tuple)
   -> typename std::enable_if<N < sizeof...(T), String>::type
{
   return ast_to_string<String>(std::get<N>(tuple))
        + ast_to_string<String, N + 1, T...>(tuple);
}

template<typename String, typename SubType>
inline String ast_to_string(const std::vector<SubType> &xs)
{
   String str;

   /* Can't use String{ xs.begin(), xs.end() } if SubType != string::val_type */
   for (auto x : xs) {
      str.append(ast_to_string<String>(x));
   }

   return str;
}

} // namespace prs

#endif // PRS_STRING_PARSER_H
