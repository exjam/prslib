#ifndef PRS_ONE_OR_MORE_PARSER_H
#define PRS_ONE_OR_MORE_PARSER_H

/**
 * Matches 1...n of x
 *  returns std::vector<x>
 *
 * Create with operator +x
 */

#include "base_parser.h"
#include "whitespace_parser.h"
#include "util.h"
#include <vector>

namespace prs
{

template<typename Repeat>
struct one_or_more_parser : public parser<one_or_more_parser<Repeat>>
{
   one_or_more_parser(Repeat repeat) :
      repeat(repeat)
   {
   }

   using value_type = std::vector<typename Repeat::value_type>;

   template<typename Iterator, typename Context, bool Atomic>
   bool parse(Iterator &first, Iterator last, Context& ctx, value_type &myv) const
   {
      typename Repeat::value_type v;
      myv = value_type();

      if (!Atomic) {
         whitespace_parser::parse(first, last, ctx);
      }

      if (!repeat.template parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
         return false;
      }

      myv.push_back(v);
      v = typename Repeat::value_type { };

      while(repeat.template parse<Iterator, Context, Atomic>(first, last, ctx, v)) {
         myv.push_back(v);
         v = typename Repeat::value_type { };

         if (!Atomic) {
            whitespace_parser::parse(first, last, ctx);
         }
      }

      return true;
   }

   Repeat repeat;
};

template<typename Parser>
inline auto make_one_or_more_parser(const Parser &parser)
   -> one_or_more_parser<decltype(make_rule_or_parser(parser))>
{
   return { make_rule_or_parser(parser) };
}

template<typename Parser>
inline auto operator +(const Parser &parser)
   -> typename std::enable_if<
         std::identity<decltype(is_parser_or_rule<Parser>())>::type::value,
         decltype(make_one_or_more_parser(parser))
      >::type
{
   return make_one_or_more_parser(parser);
}

} // namespace prs

#endif // PRS_ONE_OR_MORE_PARSER_H
