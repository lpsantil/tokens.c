#include <stdio.h>    // printf
#include <assert.h>   // assert
#include <string.h>
#include <math.h>     // isfinite
#include <stdlib.h>   // exit
#include <stdbool.h>  // true
#include <unistd.h>
#include <fcntl.h>    // open
#include <sys/stat.h>

int lines = 1,
    col = 0;

void
error( char* str )
{
   printf( "%s\n", str );
   exit( -1 );
}

void
printToken( char* type, char* value, int from, int to )
{
   printf( "(%i,%i),%s,%.*s\n", lines, col, type, to - from, value + from );
}

int
indexOf( char* str, char c )
{
   int idx = 0;

   while( '\0' != str[ idx ] )
   {
      if( str[ idx ] == c ) return( idx );

      idx++;
   }

   return( -1 );
}

void
tokens( char* str, char* prefix, char* suffix )
{
   char c,                      // Current char */
        q;                      // Quote char */
   int from,                    // Index to start of token */
       i = 0,                   // Index of current char */
       length = strlen( str );
   char* endp;
   double m;

   // Begin tokenization
   if( 0 == length ) return; // If the source string is empty, return nothing.

   // If prefix and suffix strings are not provided, supply defaults.
   if( NULL == prefix ) prefix = "!&-=+<>|";

   if( NULL == suffix ) suffix = "!&-=+<>|";

   // Loop through this text, one character at a time.
   c = str[ i ];
   while( c )
   {
      from = i;

// Ignore whitespace.
      if( c <= ' ' )
      {
         if( '\n' == c ) { lines++; col = 0; }
         i++; col++;
         c = str[ i ];
      }
// name.
      else if( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ||
               ( c == '_' ) || ( c == '$' ) )
      {
         from = i;
         i++; col++;
         while( true )
         {
             c = str[ i ];
             if( ( c >= 'a' && c <= 'z' ) ||
                 ( c >= 'A' && c <= 'Z' ) ||
                 ( c >= '0' && c <= '9' ) ||
                 ( c == '_' ) ||
                 ( c == '$' ) )
             {
                i++; col++;
             }
             else
             {
                break;
             }
         }
         printToken( "name", str, from, i );
      }
// number.
// A number cannot start with a decimal point. It must start with a digit,
// possibly '0'.
      else if( c >= '0' && c <= '9' )
      {
         from = i;
         i++; col++;

// Look for more digits.
         while( true )
         {
            c = str[ i ];
            if( c < '0' || c > '9' ) break;

            i++; col++;
         }
// Look for a decimal fraction part.
         if( c == '.' )
         {
            i++; col++;

            while( true )
            {
               c = str[ i ];
               if( c < '0' || c > '9' ) break;

               i++; col++;
            }
         }
// Look for an exponent part.
         if( c == 'e' || c == 'E' )
         {
            i++; col++;
            c = str[ i ];
            if( c == '-' || c == '+' )
            {
               i++; col++;
               c = str[ i ];
            }
            if( c < '0' || c > '9' )
            {
               error( "Bad exponent" );
            }
            do
            {
               i++; col++;
               c = str[ i ];
            } while( c >= '0' && c <= '9' );
         }
// Make sure the next character is not a letter.
         if( c >= 'a' && c <= 'z' )
         {
            // i++; col++;
            error( "Bad number" );
         }
// Convert the string value to a number. If it is finite, then it is a good
// token.
         m = strtod( &str[ from ], &endp );
         if( ( &str[ from ] == endp ) || !isfinite( m ) ) error( "Bad number" );

         printToken( "number", str, from, i );
// string
      }
      else if( c == '\'' || c == '"')
      {
         from = i;
         q = c;
         i++; col++;
         while( true )
         {
            c = str[ i ];
            if( c < ' ' ) error( "Bad character in string" );
// Look for the closing quote.
            if( c == q ) break;
// Look for escapement.
            if( c == '\\' )
            {
               i++; col++;
               if( i >= length )
               {
                  error( "Unterminated string" );
               }
               c = str[ i ];
               /*
                    switch (c) {
                    case 'b':
                        c = '\b';
                        break;
                    case 'f':
                        c = '\f';
                        break;
                    case 'n':
                        c = '\n';
                        break;
                    case 'r':
                        c = '\r';
                        break;
                    case 't':
                        c = '\t';
                        break;
                    case 'u':
                        if (i >= length) {
                            make('string', str).error("Unterminated string");
                        }
                        c = parseInt(this.substr(i + 1, 4), 16);
                        if (!isFinite(c) || c < 0) {
                            make('string', str).error("Unterminated string");
                        }
                        c = String.fromCharCode(c);
                        i += 4;
                        break;
                    }
                */
               /*if( c == 'b' ){ c = '\b'; break; }*/
            }
            i++; col++;
         }
         i++; col++;
         printToken( "string", str, from, i );
         c = str[ i ];
      }
// comment.
      else if( c == '/' && str[ i + 1 ] == '/' )
      {
         i++; col++;
         while( true )
         {
            c = str[ i ];
            if( c == '\n' || c == '\r' || c == '\0' ) break;
            i++; //lines++; col = 0;
         }
      }
// combining
      else if( indexOf( prefix, c ) >= 0 )
      {
         from = i;
         i++; col++;
         while( true )
         {
            c = str[ i ];
            if( i >= length || indexOf( suffix, c ) < 0 ) break;
            i++; col++;
         }
         printToken( "operator", str, from, i );
      }
// single-character operator
      else
      {
         i++; col++;
         printToken( "operator", str, from, from + 1 );
         c = str[ i ];
      }
      c = str[ i ];
   }
}

int main( int argc, char** argv )
{
   int fd = open( argv[ 1 ], O_RDONLY ),
       flen = lseek( fd, 0, SEEK_END );
   lseek( fd, 0, SEEK_SET );
   char buf[ flen ];
   int r = read( fd, buf, flen );
   buf[ flen ] = '\0';
   printf( "%s(%i,%i)\n", argv[ 1 ], flen, r );
   tokens( buf, NULL, NULL );
   return( 0 );
}

