#include "MiniParser.hpp"

#include <iostream>
#include <cstdlib>

class myfunc
{
public:
    myfunc() : mp( "x + (y**2)" ) {}
    
    double operator()( double x, double y )
    {
        MiniParser::IDMap ids ;
        ids[ "x" ] = x ;
        ids[ "y" ] = y ;
        
        return mp.eval( ids ) ;
    }
    
private:
    MiniParser mp ;
};

int main( int argc, char* argv[] )
{
    if( argc == 3 )
    {
        myfunc func ;
        double x = atof( argv[1] ) ;
        double y = atof( argv[2] ) ;
        std::cout << "f( " << x << ", " << y << " ): " << func( x, y ) << std::endl ;
    }
    else if( argc > 3 )
    {
        std::cout << "Parsing: " << argv[1] << std::endl ;
        MiniParser mp( argv[1] ) ;
        
        MiniParser::IDMap ids ;
        char var[] = "a" ;
        for( int i = 2 ; i < argc ; ++i )
        {
            var[0] = 'a' + i-2 ;
            
            ids[ var ] = atof( argv[i] ) ;
        }
        
        std::cout << "eval( " ;
        for(
            MiniParser::IDMap::const_iterator idit = ids.begin() ;
            idit != ids.end() ;
            ++idit
            )
        {
            if( idit != ids.begin() )
                std::cout << ", " ;
            
            std::cout << idit->first << " = " << idit->second ;
        }
        std::cout << " ): " << mp.eval( ids ) << std::endl ;
    }
    else
    {
        std::cerr << "Usage: " << argv[0] << " x y" << std::endl ;
        std::cerr << "\tor" << std::endl ;
        std::cerr << "Usage: " << argv[0] << " expression [a [b [c [...]]]]" << std::endl ;
        std::cerr << "(Remember: order-of-operations is left-associative! use parentheses!)" << std::endl ;
        exit( -1 ) ;
    }
    
    exit( 0 ) ;
}
