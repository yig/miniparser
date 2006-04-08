#ifndef __MiniParser_hpp__
#define __MiniParser_hpp__

#include <string>
#include <map>
class MiniParserNode ;

class MiniParser
{
public:
    typedef double real ;
    typedef std::map< std::string, real > IDMap ;
    
    // There is no order of operations (everything is evaluated left-associative),
    // so use lots of parentheses.
    MiniParser( const std::string& expression ) ;
    virtual ~MiniParser() ;
    
    real eval( const IDMap& ids ) const ;
    
private:
    MiniParserNode* mParseTree ;
};

#endif /* __MiniParser_hpp__ */
