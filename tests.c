/*
 * Developed by Rafa Garcia <rafagarcia77@gmail.com>
 *
 * tests.c is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tiny-json.h is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with tests.c.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "tiny-json.h"

#define done() return 0
#define fail() return __LINE__
static unsigned int checkqty = 0;
#define check( x ) do { ++checkqty; if (!(x)) fail(); } while ( 0 )

static int empty( void ) {
    json_t pool[6];
    unsigned const qty = sizeof pool / sizeof *pool;
    {
        char str[] = "{}";
        json_t const* json = json_create( str, pool, qty );
        check( json );
        json_t const* child = json_getChild( json );
        check( child == NULL );
    }
    {
        char str[] = "{\"a\":[]}";
        json_t const* json = json_create( str, pool, qty );
        check( json );
        json_t const* child = json_getChild( json );
        check( child );
        char const* childname = json_getName( child );
        check( childname );
        check( !strcmp( childname, "a" ) );
        check( json_getType( child ) == JSON_ARRAY );
        check( !json_getChild( child ) );
    }
    {
        char str[] = "{\"a\":[{},{}]}";
        json_t const* json = json_create( str, pool, qty );
        check( json );
        json_t const* child = json_getChild( json );
        check( child );
        char const* childname = json_getName( child );
        check( childname );
        check( !strcmp( childname, "a" ) );
        check( json_getType( child ) == JSON_ARRAY );
        int num = 0;
        for( json_t const* i = json_getChild( child ); i; ++num, i = json_getSibling( i ) ) {
            check( i );
            check( json_getType( i ) == JSON_OBJ );
            check( !json_getChild( i ) );
        }
        check( num == 2 );
    }
    done();
}

static int primitive( void ) {
    json_t pool[8];
    unsigned const qty = sizeof pool / sizeof *pool;
    {
        char str[] = "{"
                         "\"max\":        9223372036854775807,"
                         "\"min\":        -9223372036854775808,"
                         "\"boolvar0\":   false,"
                         "\"boolvar1\":   true,"
                         "\"nullvar\":    null,"
                         "\"scientific\": 5368.32e-3,"
                         "\"real\":       -0.25,"
                     "}";

        json_t const* json = json_create( str, pool, qty );
        check( json );

        json_t const* boolvar0 = json_getProperty( json, "boolvar0" );
        check( boolvar0 );
        check( JSON_BOOLEAN == json_getType( boolvar0 ) );
        check( !strcmp( "false", json_getValue( boolvar0 ) ) );
        check( false == json_getBoolean( boolvar0 ) );

        json_t const* boolvar1 = json_getProperty( json, "boolvar1" );
        check( boolvar1 );
        check( JSON_BOOLEAN == json_getType( boolvar1 ) );
        check( !strcmp( "true", json_getValue( boolvar1 ) ) );
        check( true == json_getBoolean( boolvar1 ) );

        json_t const* nullvar = json_getProperty( json, "nullvar" );
        check( nullvar );
        check( JSON_NULL == json_getType( nullvar ) );
        check( !strcmp( "null", json_getValue( nullvar ) ) );

        json_t const* max = json_getProperty( json, "max" );
        check( max );
        check( JSON_INTEGER == json_getType( max ) );
        check( !strcmp( "9223372036854775807", json_getValue( max ) ) );
        check( INT64_MAX == json_getInteger( max ) );

        json_t const* min = json_getProperty( json, "min" );
        check( max );
        check( JSON_INTEGER == json_getType( max ) );
        check( !strcmp( "-9223372036854775808", json_getValue( min ) ) );
        check( INT64_MIN == json_getInteger( min ) );

        json_t const* real = json_getProperty( json, "real" );
        check( real );
        check( JSON_REAL == json_getType( real ) );
        check( !strcmp( "-0.25", json_getValue( real ) ) );
        check( -0.25 == json_getReal( real ) );

        json_t const* scientific = json_getProperty( json, "scientific" );
        check( scientific );
        check( JSON_REAL == json_getType( scientific ) );
        check( !strcmp( "5368.32e-3", json_getValue( scientific ) ) );
        check( 5368.32e-3 == json_getReal( scientific ) );

    }

    done();
}

static int text( void ) {
    json_t pool[2];
    unsigned const qty = sizeof pool / sizeof *pool;

    char str[] = "{\"a\":\"\\tThis text: \\\"Hello\\\".\\n\"}";

    json_t const* json = json_create( str, pool, qty );
    check( json );

    json_t const* a = json_getProperty( json, "a" );
    check( a );
    check( JSON_TEXT == json_getType( a ) );
    check( !strcmp( "\tThis text: \"Hello\".\n", json_getValue( a ) ) );


    done();
}

static int goodformats( void ) {
    json_t pool[4];
    unsigned const qty = sizeof pool / sizeof *pool;
    {
        char str[] = "{\"qwerty\":false,}";
        json_t const* root = json_create( str, pool, qty );
        check( root );
    }
    {
        char str[] = "{\"a\":[0,]}";
        json_t const* root = json_create( str, pool, qty );
        check( root );
    }
    {
        char str[] = "{\"a\":[0],}";
        json_t const* root = json_create( str, pool, qty );
        check( root );
    }
    {
        char str[] = "{\"qwerty\":654,}";
        json_t const* root = json_create( str, pool, qty );
        check( root );
    }
    {
        char str[] = "{\"qwerty\":\"asdfgh\",}";
        json_t const* root = json_create( str, pool, qty );
        check( root );
    }

    {
        char str[] = "{,\"qwerty\":\"asdfgh\",}";
        json_t const* root = json_create( str, pool, qty );
        check( root );
    }
    {
        char str[] = "{,\"a\":1, , \"b\":2,,,,}";
        json_t const* root = json_create( str, pool, qty );
        check( root );
    }
    done();
}

static int array( void ) {
    json_t pool[7];
    unsigned const qty = sizeof pool / sizeof *pool;

    char str[] = "{\"array\":[ 1, true, null, \"Text\", 0.3232 ]}";

    json_t const* json = json_create( str, pool, qty );
    check( json );

    json_t const* array = json_getProperty( json, "array" );
    check( array );
    check( JSON_ARRAY == json_getType( array ) );

    static struct { jsonType_t type; char const* value; } const pairs[] = {
        { JSON_INTEGER, "1" }, { JSON_BOOLEAN, "true" }, { JSON_NULL, "null" },
        { JSON_TEXT, "Text" }, { JSON_REAL, "0.3232" }
    };
    unsigned const len = sizeof pairs / sizeof *pairs;
    json_t const* element = json_getChild( array );
    for( unsigned int i = 0; i < len; ++i, element = json_getSibling( element ) ) {
        check( element );
        check( pairs[i].type == json_getType( element ) );
        check( !strcmp( pairs[i].value, json_getValue( element ) ) );
    }
    check( !element );

    done();
}

int badformat( void ) {
    json_t pool[2];
    unsigned const qty = sizeof pool / sizeof *pool;
    {
        char str[] = "{\"var:true}";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    {
        char str[] = "{\"var\":tr}";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    {
        char str[] = "{\"var\":true";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    {
        char str[] = "{\"var\":true} text outside json";
        json_t const* json = json_create( str, pool, qty );
        check( json );
        json_t const* var = json_getProperty( json, "var" );
        check( var );
        check( JSON_BOOLEAN == json_getType( var ) );
        check( !strcmp( "true", json_getValue( var ) ) );
        check( true == json_getBoolean( var ) );
    }
    {
        char str[] = "{\"var\":truep}";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    {
        char str[] = "{\"var\":0s}";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    {
        char str[] = "{\"var\":9223372036854775808}";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    {
        char str[] = "{\"var\":12233720368547758080}";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    {
        char str[] = "{\"var\":-9223372036854775809}";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    {
        char str[] = "{\"var\":-12233720368547758090}";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    {
        char str[] = "{\"var\":9}";
        json_t const* json = json_create( str, pool, qty );
        check( json );
    }
    {
        char str[] = "{\"var\":,9}";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    {
        char str[] = "{\"var\":}";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    {
        char str[] = "{\"var\":,}";
        json_t const* json = json_create( str, pool, qty );
        check( !json );
    }
    done();
}

struct test {
    int(*func)(void);
    char const* name;
};

static int test_exec( struct test const* test ) {
    int const err = test->func();
    if ( err ) {
        fprintf( stderr, "%s%s%s%d%s", "Failed test: '", test->name, "' Line: ", err, ".\n" );
        return 1;
    }
    return 0;
}

static struct test const tests[] = {
    { empty,       "Empty object and array" },
    { primitive,   "Primitive properties"   },
    { text,        "Text"                   },
    { array,       "Array"                  },
    { badformat,   "Bad format"             },
    { goodformats, "Formats"                },
};

int main( void ) {
    int failed = 0;
    unsigned int const qty = sizeof tests / sizeof *tests;
    for( unsigned int i = 0; i < qty; ++i )
        failed += test_exec( tests + i );
    unsigned int const percent = 100.0 * ( qty - failed ) / qty;
    printf( "%d%s%d%s", percent, "%. ", checkqty, " checks.\n" );
    return failed;
}
