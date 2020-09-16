#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

void
error( char* str )
{
   printf( "%s\n", str );
   exit( -1 );
}

void
printToken( char* type, char* value, int32_t from, int32_t to )
{
   char str[ 256 ];
   int32_t len = to - from;

   assert( len > 0 );
   assert( len <= 255 );

   bcopy( &value[ from ], str, len );
   str[ len ] = '\0';

   printf( "%s,%s\n", type, str );
}

int
indexOf( char* str, char c )
{
   int idx = 0;

   while( '\0' != str[ idx ] )
   {
      if( str[ idx ] == c )
      {
         return( idx );
      }

      idx = idx + 1;
   }

   return( -1 );
}

void
tokens( char* str, char* prefix, char* suffix )
{
   char c,                         /* Current char */
        q;                         /* Quote char */
   int32_t from,                   /* Index to start of token */
           i = 0,                  /* Index of current char */
           length = strlen( str ),
           n;                      /* The number value */
   char* endp;
   double m;

   /* Begin tokenization */
   if( 0 == length ) /* If the source string is empty, return nothing. */
   {
      return;
   }
   /* If prefix and suffix strings are not provided, supply defaults. */
   if( NULL == prefix )
   {
      /*prefix = "<>+-&";*/
      prefix = "!&-=+<>|";
   }
   if( NULL == suffix )
   {
      /*suffix = "=<>&:";*/
      suffix = "!&-=+<>|";
   }

   // Loop through this text, one character at a time.
   c = str[ i ];
   while( c )
   {
      from = i;

// Ignore whitespace.
      if( c <= ' ' )
      {
         i = i + 1;
         c = str[ i ];
      }
// name.
      else if( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ||
               ( c == '_' ) || ( c == '$' ) )
      {
         from = i;
         /*str = c;*/
         /*i += 1;*/i = i + 1;
         while( true )
         {
             /*c = this.charAt(i);*/
             c = str[ i ];
             if( ( c >= 'a' && c <= 'z' ) ||
                 ( c >= 'A' && c <= 'Z' ) ||
                 ( c >= '0' && c <= '9' ) ||
                 ( c == '_' ) ||
                 ( c == '$' ) )
             {
                /*str += c; */
                /*i += 1;*/i = i + 1;
             }
             else
             {
                break;
             }
         }
         /*result.push(make('name', str));*/
         printToken( "name", str, from, i );
      }
// number.
// A number cannot start with a decimal point. It must start with a digit,
// possibly '0'.
      else if( c >= '0' && c <= '9' )
      {
         /*str = c;*/
         from = i;
         /*i += 1;*/i = i + 1;

// Look for more digits.
         while( true )
         {
            /*c = this.charAt(i);*/
            c = str[ i ];
            if( c < '0' || c > '9' )
            {
               break;
            }
            /*i += 1;*/i = i + 1;
            /*str += c;*/
         }
// Look for a decimal fraction part.
         if( c == '.' )
         {
            /*i += 1;*/i = i + 1;
            /*str += c;*/
            while( true )
            {
               /*c = this.charAt(i);*/
               c = str[ i ];
               if( c < '0' || c > '9' )
               {
                  break;
               }
               /*i += 1;*/i = i + 1;
               /*str += c;*/
            }
         }
// Look for an exponent part.
         if( c == 'e' || c == 'E' )
         {
            /*i += 1;*/i = i + 1;
            /*str += c;*/
            /*c = this.charAt(i);*/
            c = str[ i ];
            if( c == '-' || c == '+' )
            {
               /*i += 1;*/i = i + 1;
               /*str += c;*/
               /*c = this.charAt(i);*/
               c = str[ i ];
            }
            if( c < '0' || c > '9' )
            {
               /*make('number', str).error("Bad exponent");*/
               error( "Bad exponent" );
            }
            do
            {
               /*i += 1;*/i = i + 1;
               /*str += c;*/
               /*c = this.charAt(i);*/
               c = str[ i ];
            } while( c >= '0' && c <= '9' );
         }
// Make sure the next character is not a letter.
         if( c >= 'a' && c <= 'z' )
         {
            /*str += c;*/
            /*i += 1;*/i = i + 1;
            /*make('number', str).error("Bad number");*/
            error( "Bad number" );
         }

// Convert the string value to a number. If it is finite, then it is a good
// token.
         /*n = +str;
         if(isFinite(n)) {
             result.push(make('number', n));
         } else {
                make('number', str).error("Bad number");
         }
         */
         m = strtod( &str[ from ], &endp );
         if( ( &str[ from ] == endp ) || !isfinite( m ) )
         {
            error( "Bad number" );
	}
	printToken( "number", str, from, i );
// string
      }
      else if( c == '\'' || c == '"')
      {
         from = i;
         /*str = '';*/
         q = c;
         /*i += 1;*/i = i + 1;
         while( true )
         {
            /*c = this.charAt(i);*/
            c = str[ i ];
            if( c < ' ' )
            {
               /*
               make('string', str).error(
                    (c === '\n' || c === '\r' || c === '')
                            ? "Unterminated string."
                            : "Control character in string.",
                        make('', str)
                    );
               */
               error( "Bad character in string" );
            }
// Look for the closing quote.
            if( c == q )
            {
               break;
            }
// Look for escapement.
            if( c == '\\' )
            {
               /*i += 1;*/i = i + 1;
               if( i >= length )
               {
                  /*make('string', str).error("Unterminated string");*/
                  error( "Unterminated string" );
               }
               /*c = this.charAt(i);*/
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
            /*str += c;*/
            /*i += 1;*/i = i + 1;
         }
         /*i += 1;*/i = i + 1;
         /*result.push(make('string', str));*/
         printToken( "string", str, from, i );
         /*c = this.charAt(i);*/
         c = str[ i ];
      }
// comment.
      else if( c == '/' && str[ i + 1 ] == '/' )
      {
         /*i += 1;*/i = i + 1;
         while( true )
         {
            /*c = this.charAt(i);*/c = str[ i ];
            if( c == '\n' || c == '\r' || c == '\0' )
            {
               break;
            }
            /*i += 1;*/i = i + 1;
         }
      }
// combining
      else if( indexOf( prefix, c ) >= 0 )
      {
         from = i;
         /*str = c;*/
         /*i += 1;*/i = i + 1;
         while( true )
         {
            /*c = this.charAt(i);*/c = str[ i ];
            if( i >= length || indexOf( suffix, c ) < 0 )
            {
               break;
            }
            /*str += c;*/
            /*i += 1;*/i = i + 1;
         }
         /*result.push(make('operator', str));*/printToken( "operator", str, from, i );
      }
// single-character operator
      else
      {
         /*i += 1;*/i = i + 1;
         /*result.push(make('operator', c));*/
         printToken( "operator", str, from, from + 1 );
         /*c = this.charAt(i);*/c = str[ i ];
      }
      /*c = this.charAt(i);*/c = str[ i ];
   }
   /*return result;*/
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

