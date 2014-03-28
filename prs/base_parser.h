#ifndef PRS_BASE_PARSER_H
#define PRS_BASE_PARSER_H

/*
 * Base class for a parser so our operator overloads only affect parser classes
 */

template<typename Derived>
struct parser
{
   const Derived & derived() const
   {
      return *static_cast<const Derived*>(this);
   }
};

#endif // PRS_BASE_PARSER_H
