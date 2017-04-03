
/*
 * Developed by Rafa Garcia <rafagarcia77@gmail.com>
 *
 * example-01.c is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * example-01.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with example-01.c.  If not, see <http://www.gnu.org/licenses/>.
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

    json_t const* firstName = json_getProperty( json, "firstName" );
    if ( !firstName || JSON_TEXT != json_getType( firstName ) ) {
        puts("Error, the first name property is not found.");
        return EXIT_FAILURE;
    }
    char const* firstNameVal = json_getValue( firstName );
    printf( "Fist Name: %s.\n", firstNameVal );

    char const* lastName = json_getPropertyValue( json, "lastName" );
    if ( !lastName ) {
        puts("Error, the last name property is not found.");
        return EXIT_FAILURE;
    }	
    printf( "Last Name: %s.\n", lastName );

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
            char const* phoneNumber = json_getPropertyValue( phone, "number" );
            if ( phoneNumber ) printf( "Number: %s.\n", phoneNumber );
        }
    }

    return EXIT_SUCCESS;
}
