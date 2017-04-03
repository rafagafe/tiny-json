
/*
 * Developed by Rafa Garcia <rafagarcia77@gmail.com>
 *
 * example-02.c is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * example-02.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with example-02.c.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * In this example the JSON library is used to scan an object that nothing is
 * known about its properties.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tiny-json.h"

/** Get the name of a json type code.
  * @param type the json type code.
  * @return A null-terminated string with the name. */
static char const* getJsonTypeName( jsonType_t type ) {
    static struct { jsonType_t type; char const* name; } const pair[] = {
        { JSON_OBJ,     "object"  },
        { JSON_TEXT,    "text"    },
        { JSON_BOOLEAN, "boolean" },
        { JSON_INTEGER, "integer" },
        { JSON_NULL,    "null"    },
        { JSON_ARRAY,   "array"   },
    };
    unsigned int i;
    for( i = 0; i < sizeof pair / sizeof *pair; ++i )
        if ( pair[i].type == type )
            return pair[i].name;
    return "Unknown";
}

/** Print the value os a json object or array.
  * @param json The handler of the json object or array. */
static void dump( json_t const* json ) {

    jsonType_t const type = json_getType( json );
    if ( type != JSON_OBJ && type != JSON_ARRAY ) {
        puts("error");
        return;
    }

    printf( "%s", type == JSON_OBJ? " {": " [" );

    json_t const* child;
    for( child = json_getChild( json ); child != 0; child = json_getSibling( child ) ) {

        jsonType_t propertyType = json_getType( child );
        char const* name = json_getName( child );
        if ( name ) printf(" \"%s\": ", name );

        if ( propertyType == JSON_OBJ || propertyType == JSON_ARRAY )
            dump( child );

        else {
            char const* value = json_getValue( child );
            if ( value ) {
                bool const text = JSON_TEXT == json_getType( child );
                char const* fmt = text? " \"%s\"": " %s";
                printf( fmt, value );
                bool const last = !json_getSibling( child );
                if ( !last ) putchar(',');
            }
        }
    }

    printf( "%s", type == JSON_OBJ? " }": " ]" );

}

/* Parser a json string. */
int main( void ) {
    char str[] = "{\n"
        "\t\"firstName\": \"Bidhan\",\n"
        "\t\"lastName\": \"Chatterjee\",\n"
        "\t\"age\": 40,\n"
        "\t\"address\": {\n"
        "\t\t\"streetAddress\": \"144 J B Hazra Road\",\n"
        "\t\t\"city\": \"Burdwan\",\n"
        "\t\t\"state\": \"Paschimbanga\",\n"
        "\t\t\"postalCode\": \"713102\"\n"
        "\t},\n"
        "\t\"phoneList\": [\n"
        "\t\t{ \"type\": \"personal\", \"number\": \"09832209761\" },\n"
        "\t\t{ \"type\": \"fax\", \"number\": \"91-342-2567692\" }\n"
        "\t]\n"
        "}\n";
    puts( str );
    json_t mem[32];
    json_t const* json = json_create( str, mem, sizeof mem / sizeof *mem );
    if ( !json ) {
        puts("Error json create.");
        return EXIT_FAILURE;
    }
    puts("Print JSON:");
    dump( json );
    return EXIT_SUCCESS;
}
