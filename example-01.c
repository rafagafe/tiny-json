
/*
 * example2.c is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * example2.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * In this example the JSON library is used to analyze an object that some
 * properties are expected.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tiny-json.h"

/* Parser a json string. */
int main( void ) {
    char const* msg = "{\n"
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
    char aux[512];
    strcpy( aux, msg );
    puts( aux );
    json_t mem[32];
    json_t const* json = json_create( aux, mem, sizeof mem / sizeof *mem );
    if ( !json ) {
        puts("Error json create.");
        return EXIT_FAILURE;
    }

    json_t const* firstName = json_getProperty( json, "firstName" );
    if ( !firstName || JSON_TEXT != json_getType( firstName ) ) {
        puts("Error, the first name property is not found.");
        return EXIT_FAILURE;
    }
    char const* firstNameVal = json_getValue( firstName );
    if ( firstNameVal ) printf( "Fist Name: %s.\n", firstNameVal );

    json_t const* lastName = json_getProperty( json, "lastName" );
    if ( !lastName || JSON_TEXT != json_getType( lastName ) ) {
        puts("Error, the last name property is not found.");
        return EXIT_FAILURE;
    }
    char const* lastNameVal = json_getValue( lastName );
    if ( lastName ) printf( "Last Name: %s.\n", lastNameVal );

    json_t const* age = json_getProperty( json, "age" );
    if ( !age || JSON_INTEGER != json_getType( age ) ) {
        puts("Error, the age property is not found.");
        return EXIT_FAILURE;
    }
    int const ageVal = (int)json_getInteger( age );
    printf( "Age: %d.\n", ageVal );

    json_t const* phoneList = json_getProperty( json, "phoneList" );
    if ( !phoneList || JSON_ARRAY != json_getType( phoneList ) ) {
        puts("Error, the phone list property is not found.");
        return EXIT_FAILURE;
    }

    json_t const* phone;
    for( phone = json_getChild( phoneList ); phone != 0; phone = json_getSibling( phone ) ) {
        if ( JSON_OBJ == json_getType( phone ) ) {
            json_t const* number = json_getProperty( phone, "number" );
            if ( number ) {
                char const* numberVal = json_getValue( number );
                if ( numberVal ) printf( "Number: %s.\n", numberVal );
            }
        }
    }

    return EXIT_SUCCESS;
}
