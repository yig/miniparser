#include "MiniParser.hpp"

#include <iostream>
#include <sstream>
#include <map>
#include <cmath>

namespace
{
    typedef MiniParserNode Node ;
    
    // This is it!
    Node* Parse( const std::string& str ) ;
}

class MiniParserNode
{
public:
    typedef MiniParser::IDMap IDMap ;
    typedef MiniParser::real real ;
    
    static MiniParserNode* FromString( const std::string& str )
    {
        return Parse( str ) ;
    }
    
    virtual ~MiniParserNode() {}
    
    virtual real eval( const IDMap& ids ) const = 0 ;
};

namespace
{
typedef MiniParserNode::real real ;

class NodeConstantValue : public Node
{
public:
    NodeConstantValue( real val ) : mVal( val ) {}
    
    real eval( const IDMap& ids ) const
    {
        return mVal ;
    }
    
private:
    real mVal ;
};

class NodeVariableValue : public Node
{
public:
    NodeVariableValue( const std::string& var ) : mVar( var ) {}
    
    real eval( const IDMap& ids ) const
    {
        IDMap::const_iterator var = ids.find( mVar ) ;
        if( ids.end() == var )
        {
            std::cerr << "unknown variable: " << mVar << std::endl ;
            return -1. ;
        }
        return var->second ;
    }
    
private:
    const std::string mVar ;
};

class NodePlus : public Node
{
public:
    NodePlus( Node* lhs, Node* rhs ) : mLHS( lhs ), mRHS( rhs ) { assert( lhs && rhs ) ; }
    ~NodePlus() { delete mLHS ; delete mRHS ; }
    
    real eval( const IDMap& ids ) const
    {
        return mLHS->eval( ids ) + mRHS->eval( ids ) ;
    }
    
private:
    Node* mLHS ;
    Node* mRHS ;
};
class NodeMinus : public Node
{
public:
    NodeMinus( Node* lhs, Node* rhs ) : mLHS( lhs ), mRHS( rhs ) { assert( lhs && rhs ) ; }
    ~NodeMinus() { delete mLHS ; delete mRHS ; }
    
    real eval( const IDMap& ids ) const
    {
        return mLHS->eval( ids ) - mRHS->eval( ids ) ;
    }
    
private:
    Node* mLHS ;
    Node* mRHS ;
};
class NodeTimes : public Node
{
public:
    NodeTimes( Node* lhs, Node* rhs ) : mLHS( lhs ), mRHS( rhs ) { assert( lhs && rhs ) ; }
    ~NodeTimes() { delete mLHS ; delete mRHS ; }
    
    real eval( const IDMap& ids ) const
    {
        return mLHS->eval( ids ) * mRHS->eval( ids ) ;
    }
    
private:
    Node* mLHS ;
    Node* mRHS ;
};
class NodeDividedBy : public Node
{
public:
    NodeDividedBy( Node* lhs, Node* rhs ) : mLHS( lhs ), mRHS( rhs ) { assert( lhs && rhs ) ; }
    ~NodeDividedBy() { delete mLHS ; delete mRHS ; }
    
    real eval( const IDMap& ids ) const
    {
        return mLHS->eval( ids ) / mRHS->eval( ids ) ;
    }
    
private:
    Node* mLHS ;
    Node* mRHS ;
};
class NodePow : public Node
{
public:
    NodePow( Node* lhs, Node* rhs ) : mLHS( lhs ), mRHS( rhs ) { assert( lhs && rhs ) ; }
    ~NodePow() { delete mLHS ; delete mRHS ; }
    
    real eval( const IDMap& ids ) const
    {
        return pow( mLHS->eval( ids ), mRHS->eval( ids ) ) ;
    }
    
private:
    Node* mLHS ;
    Node* mRHS ;
};
class NodeNegate : public Node
{
public:
    NodeNegate( Node* node ) : mNode( node ) { assert( node ) ; }
    ~NodeNegate() { delete mNode ; }
    
    real eval( const IDMap& ids ) const
    {
        return -mNode->eval( ids ) ;
    }
    
private:
    Node* mNode ;
};

class MiniTokenizer
{
public:
    MiniTokenizer( std::istream& in ) : mIn( in ) { mIn >> std::ws ; advance() ; }
    
    std::string eat() { std::string result = mPeek ; advance() ; return result ; }
    const std::string& peek() { return mPeek ; }
    bool eof() { return mIn.eof() ; }
    
private:
    void advance()
    {
        if( eof() )
            mPeek = "" ;
        else
            mIn >> mPeek >> std::ws ;
    }
    
    std::istream& mIn ;
    std::string mPeek ;
};

void find_replace( std::string& in_this_string, const std::string& find, const std::string& replace )
{
    std::string::size_type pos = 0 ;
    while( std::string::npos != (pos = in_this_string.find( find, pos )) )
    {
        in_this_string.replace( pos, find.length(), replace ) ;
        pos += replace.length() ;
    }
}

Node* ParseValueNode( MiniTokenizer& mt ) ;

Node* Parse( const std::string& str )
{
    std::string input = str ;
    find_replace( input, "(", " ( " ) ;
    find_replace( input, ")", " ) " ) ;
    find_replace( input, "+", " + " ) ;
    find_replace( input, "-", " - " ) ;
    find_replace( input, "*", " * " ) ;
    find_replace( input, "/", " / " ) ;
    find_replace( input, "^", " ^ " ) ;
    find_replace( input, "*  *", "**" ) ;
    
    std::istringstream istr( "( " + input + " )" ) ;
    MiniTokenizer tt( istr ) ;
    return ParseValueNode( tt ) ;
}

Node* ParseExpressionNode( MiniTokenizer& mt ) ;
Node* ParseValueNode( MiniTokenizer& mt )
{
    std::string t = mt.eat() ;
    if( t.size() == 0 )
    {
        std::cerr << "expression cut short" << std::endl ;
        return new NodeConstantValue( -1. ) ;
    }
    
    assert( t.size() > 0 ) ;
    if( t == "(" )
    {
        Node* result = ParseExpressionNode( mt ) ;
        std::string rp = mt.eat() ;
        if( ")" != rp )
        {
            std::cerr << "expression error: expected \")\"" << std::endl ;
            delete result ;
            return new NodeConstantValue( -1. ) ;
        }
        return result ;
    }
    else if( t[0] == '-' )
    {
        return new NodeNegate( ParseValueNode( mt ) ) ;
    }
    else if( t[0] >= '0' && t[0] <= '9' )
    {
        std::istringstream ttof( t ) ;
        real val ;
        ttof >> val ;
        if( !ttof )
        {
            std::cerr << "expecting a real number but received: " << t << std::endl ;
            return new NodeConstantValue( -1. ) ;
        }
        else
        {
            return new NodeConstantValue( val ) ;
        }
    }
    else
    {
        return new NodeVariableValue( t ) ;
    }
}

Node* ParseExpressionNode( MiniTokenizer& mt )
{
    Node* lhs = ParseValueNode( mt ) ;
    // short circuit if we're a single value
    if( mt.peek() == ")" ) return lhs ;
    
    std::string op = mt.eat() ;
    Node* rhs = ParseExpressionNode( mt ) ;
    
    if( op == "+" )
    {
        return new NodePlus( lhs, rhs ) ;
    }
    else if( op == "-" )
    {
        return new NodeMinus( lhs, rhs ) ;
    }
    else if( op == "*" )
    {
        return new NodeTimes( lhs, rhs ) ;
    }
    else if( op == "/" )
    {
        return new NodeDividedBy( lhs, rhs ) ;
    }
    else if( op == "**" )
    {
        return new NodePow( lhs, rhs ) ;
    }
    else if( op == "^" )
    {
        return new NodePow( lhs, rhs ) ;
    }
    // errors
    else if( op.size() == 0 )
    {
        std::cerr << "expression cut short" << std::endl ;
        delete lhs ;
        delete rhs ;
        return new NodeConstantValue( -1. ) ;
    }
    else
    {
        std::cerr << "unknown operation: " << op << std::endl ;
        delete lhs ;
        delete rhs ;
        return new NodeConstantValue( -1. ) ;
    }
}

} // ~unnamed namespace


MiniParser::MiniParser( const std::string& expression )
    : mParseTree( NULL )
{
    mParseTree = MiniParserNode::FromString( expression ) ;
}

MiniParser::~MiniParser()
{
    delete mParseTree ;
}

real
MiniParser::eval( const IDMap& ids )
const
{
    if( !mParseTree )
    {
        std::cerr << "No parse tree!" << std::endl ;
        return -1. ;
    }
    
    return mParseTree->eval( ids ) ;
}
