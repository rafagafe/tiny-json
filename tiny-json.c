
/*
 * Developed by Rafa Garcia <rafagarcia77@gmail.com>
 *
 * tiny-json.c is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * tiny-json.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <string.h>
#include "tiny-json.h"

/** Structure to handle a heap of JSON properties. */
typedef struct jsonPool_s {
    json_t* const mem;      /**< Pointer to array of json properties.      */
    unsigned int const qty; /**< Length of the array of json properties.   */
    unsigned int nextFree;  /**< The index of the next free json property. */
} jsonPool_t;

/* Search a property by its name in a JSON object. */
json_t const* json_getProperty( json_t const* obj, char const* property ) {
    json_t const* sibling;
    for( sibling = obj->u.child; sibling; sibling = sibling->sibling )
        if ( sibling->name && !strcmp( sibling->name, property ) )
            return sibling;
    return 0;
}

/* Internal prototypes: */
static char* _goWhiteSpace( char* str );
static char* _goNum( char* str );
static json_t* _poolInit( jsonPool_t* pool );
static json_t* _poolNew( jsonPool_t* pool );
static char* _objValue( char* ptr, json_t* obj, jsonPool_t* pool );
static char* _setToNull( char* ch );

/* Parse a string to get a json. */
json_t const* json_create( char* str, json_t mem[], unsigned int qty ) {
    char* ptr = _goWhiteSpace( str );
    if ( *ptr != '{') return 0;
    jsonPool_t pool = { .mem = mem, .qty = qty };
    json_t* obj = _poolInit( &pool );
    obj->name    = 0;
    obj->sibling = 0;
    obj->u.child = 0;
    ptr = _objValue( ptr, obj, &pool );
    if ( !ptr ) return 0;
    return obj;
}

/** Get a special character with its escape character. Examples:
  * 'b' -> '\b', 'n' -> '\n', 't' -> '\t'
  * @param ch The escape character.
  * @return  The character code. */
static char _getEscape( char ch ) {
    static struct { char ch; char code; } const pair[] = {
        { '\"', '\"' },
        { '\\', '\\' },
        { '/',  '/'  },
        { 'b',  '\b' },
        { 'f',  '\f' },
        { 'n',  '\n' },
        { 'r',  '\r' },
        { 't',  '\t' },
    };
    unsigned int i;
    for( i = 0; i < sizeof pair / sizeof *pair; ++i )
        if ( pair[i].ch == ch )
            return pair[i].code;
    return '\0';
}

/** Check if a character is a hexadecimal digit. */
static bool _isHexaDigit( unsigned char nibble ) {
    if ( nibble <  '0' ) return false;
    if ( nibble <= '9' ) return true;
    if ( nibble <  'A' ) return false;
    if ( nibble <= 'F' ) return true;
    if ( nibble <  'a' ) return false;
    if ( nibble <= 'f' ) return true;
    return false;
}

/** Parse 4 charcters.
  * @Param str Pointer to  first digit.
  * @retval '?' If the four characters are hexadecimal digits.
  * @retcal '\0' In other cases. */
static char _getCharFromUnicode( char const* str ) {
    unsigned int i;
    for( i = 0; i < 4; ++i )
        if ( !_isHexaDigit( str[i] ) )
            return '\0';
    return '?';
}

/** Parse a string and replace the scape characters by their meaning characters.
  * This parser stops when finds the character '\"'. Then replaces '\"' by '\0'.
  * @param str Pointer to first character.
  * @retval Pointer to first non white space after the string. If success.
  * @retval Null pointer if any error occur. */
static char* _parseString( char* str ) {
    char* head = str;
    char* tail = str;
    for( ; *head >= ' '; ++head, ++tail ) {
        if ( *head == '\"' ) {
            *tail = '\0';
            return ++head;
        }
        if ( *head == '\\' ) {
            if ( *++head == 'u' ) {
                char const ch = _getCharFromUnicode( ++head );
                if ( ch == '\0' ) return 0;
                *tail = ch;
                head += 3;
            }
            else {
                char const esc = _getEscape( *head );
                if ( esc == '\0' ) return 0;
                *tail = esc;
            }
        }
        else *tail = *head;
    }
    return 0;
}

/** Parse a string to get the name of a property.
  * @param str Pointer to first character.
  * @param property The property to assign the name.
  * @retval Pointer to first of property value. If success.
  * @retval Null pointer if any error occur. */
static char* _propertyName( char* ptr, json_t* property ) {
    property->name = ++ptr;
    ptr = _parseString( ptr );
    if ( !ptr ) return 0;
    ptr = _goWhiteSpace( ptr );
    if ( !ptr ) return 0;
    if ( *ptr++ != ':' ) return 0;
    return _goWhiteSpace( ptr );
}

/** Parse a string to get the value of a property when its type is JSON_TEXT.
  * @param str Pointer to first character ('\"').
  * @param property The property to assign the name.
  * @retval Pointer to first non white space after the string. If success.
  * @retval Null pointer if any error occur. */
static char* _textValue( char* ptr, json_t* property ) {
    ++property->u.value;
    ptr = _parseString( ++ptr );
    if ( !ptr ) return 0;
    property->type = JSON_TEXT;
    return ptr;
}

/** Compare two strings until get the null character in the second one.
  * @param ptr sub string
  * @param str main string
  * @retval Pointer to next chraracter.
  * @retval Null pointer if any error occur. */
static char* _checkStr( char* ptr, char const* str ) {
    while( *str )
        if ( *ptr++ != *str++ )
            return 0;
    return ptr;
}

/** Parser a string to get a primitive value.
  * If the first character after the value is diferent of '}' or ']' is set to '\0'.
  * @param str Pointer to first character.
  * @param property Property handler to set the value and the type, (true, false or null).
  * @param value String with the primitive literal.
  * @param type The code of the type. ( JSON_BOOLEAN or JSON_NULL )
  * @retval Pointer to first non white space after the string. If success.
  * @retval Null pointer if any error occur. */
static char* _primitiveValue( char* ptr, json_t* property, char const* value, jsonType_t type ) {
    ptr = _checkStr( ptr, value );
    if ( !ptr ) return 0;
    ptr = _setToNull( ptr );
    property->type = type;
    return ptr;
}

/** Parser a string to get a true value.
  * If the first character after the value is diferent of '}' or ']' is set to '\0'.
  * @param str Pointer to first character.
  * @param property Property handler to set the value and the type, (true, false or null).
  * @retval Pointer to first non white space after the string. If success.
  * @retval Null pointer if any error occur. */
static char* _trueValue( char* ptr, json_t* property ) {
    return _primitiveValue( ptr, property, "true", JSON_BOOLEAN );
}

/** Parser a string to get a false value.
  * If the first character after the value is diferent of '}' or ']' is set to '\0'.
  * @param str Pointer to first character.
  * @param property Property handler to set the value and the type, (true, false or null).
  * @retval Pointer to first non white space after the string. If success.
  * @retval Null pointer if any error occur. */
static char* _falseValue( char* ptr, json_t* property ) {
    return _primitiveValue( ptr, property, "false", JSON_BOOLEAN );
}

/** Parser a string to get a null value.
  * If the first character after the value is diferent of '}' or ']' is set to '\0'.
  * @param str Pointer to first character.
  * @param property Property handler to set the value and the type, (true, false or null).
  * @retval Pointer to first non white space after the string. If success.
  * @retval Null pointer if any error occur. */
static char* _nullValue( char* ptr, json_t* property ) {
    return _primitiveValue( ptr, property, "null", JSON_NULL );
}

static bool _isNum( unsigned char ch );

static char* _expValue( char* ptr, json_t* property ) {
    if ( *ptr == '-' || *ptr == '+' ) ++ptr;
    if ( !_isNum( *ptr ) ) return 0;
    ptr = _goNum( ++ptr );
    property->type = JSON_SCIENTIFIC;
    return ptr;
}

static char* _fraqValue( char* ptr, json_t* property ) {
    if ( !_isNum( *ptr ) ) return 0;
    ptr = _goNum( ++ptr );
    if ( !ptr ) return 0;
    if ( *ptr == 'e' || *ptr == 'E' ) return _expValue( ++ptr, property );
    property->type = JSON_REAL;
    return ptr;
}

/** Parser a string to get a numerial value.
  * If the first character after the value is diferent of '}' or ']' is set to '\0'.
  * @param str Pointer to first character.
  * @param property Property handler to set the value and the type:
  *         JSON_REAL, JSON_SCIENTIFIC or JSON_INTEGER.
  * @retval Pointer to first non white space after the string. If success.
  * @retval Null pointer if any error occur. */
static char* _numValue( char* ptr, json_t* property ) {
    if ( *ptr == '-' ) ++ptr;
    if ( *ptr != '0' ) {
        ptr = _goNum( ptr );
        if ( !ptr ) return 0;
    }
    else if ( _isNum( *++ptr ) ) return 0;
    if ( *ptr == '.' ) {
        ptr = _fraqValue( ++ptr, property );
        if ( !ptr ) return 0;
    }
    else if ( *ptr == 'e' || *ptr == 'E' ) {
        ptr = _expValue( ++ptr, property );
        if ( !ptr ) return 0;
    }
    else property->type = JSON_INTEGER;
    ptr = _setToNull( ptr );
    return ptr;
}

/** Add a property to a JSON object or array.
  * @param obj The handler of the JSON object or array.
  * @param property The handler of the property to be added. */
static void _add( json_t* obj, json_t* property ) {
    property->sibling = 0;
    if ( !obj->u.child ) obj->u.child = property;
    else {
        json_t* iter;
        for( iter = obj->u.child; iter->sibling; iter = iter->sibling );
        iter->sibling = property;
    }
}

/** Parser a string to get a json object value.
  * @param str Pointer to first character.
  * @param pool The handler of a json pool for creating json instances.
  * @retval Pointer to first character after the value. If success.
  * @retval Null pointer if any error occur. */
static char* _objValue( char* ptr, json_t* obj, jsonPool_t* pool ) {
    obj->type    = JSON_OBJ;
    obj->u.child = 0;
    obj->sibling = 0;
    ptr++;
    for(;;) {
        ptr = _goWhiteSpace( ptr );
        if ( !ptr ) return 0;
        char const endchar = ( obj->type == JSON_OBJ )? '}': ']';
        if ( *ptr == endchar ) {
            *ptr = '\0';
            json_t* parentObj = obj->sibling;
            if ( !parentObj ) return ++ptr;
            obj->sibling = 0;
            obj = parentObj;
            ++ptr;
            continue;
        }
        if ( *ptr == ',' ) {
            ptr = _goWhiteSpace( ++ptr );
            if ( !ptr ) return 0;
        }
        json_t* property = _poolNew( pool );
        if ( !property ) return 0;
        if( obj->type != JSON_ARRAY ) {
            if ( *ptr != '\"' ) return 0;
            ptr = _propertyName( ptr, property );
            if ( !ptr ) return 0;
        }
        else property->name = 0;
        _add( obj, property );
        property->u.value = ptr;
        switch( *ptr ) {
            case '{':
                property->type    = JSON_OBJ;
                property->u.child = 0;
                property->sibling = obj;
                obj = property;
                ++ptr;
                break;
            case '[':
                property->type    = JSON_ARRAY;
                property->u.child = 0;
                property->sibling = obj;
                obj = property;
                ++ptr;
                break;
            case '\"': ptr = _textValue( ptr, property );    break;
            case 't':  ptr = _trueValue( ptr, property );    break;
            case 'f':  ptr = _falseValue( ptr, property );   break;
            case 'n':  ptr = _nullValue( ptr, property );    break;
            default:   ptr = _numValue( ptr, property ); break;
        }
        if ( !ptr ) return 0;
    }
}

/** Initialize a json pool.
  * @param pool The handler of the pool.
  * @return a instace of a json. */
static json_t* _poolInit( jsonPool_t* pool ) {
    pool->nextFree = 1;
    return &pool->mem[0];
}

/** Create an instance of a json from a pool.
  * @param pool The handler of the pool.
  * @retval The handler of the new instance if success.
  * @retval Null pointer if the pool was empty. */
static json_t* _poolNew( jsonPool_t* pool ) {
    if ( pool->nextFree >= pool->qty ) return 0;
    return &pool->mem[pool->nextFree++];
}

/** Checks whether an character belongs to set.
  * @param ch Character value to be checked.
  * @param set Set of characters. It is just a null-terminated string.
  * @return true or false there is membership or not. */
static bool _isOneOfThem( char ch, char const* set ) {
    while( *set != '\0' )
        if ( ch == *set++ )
            return true;
    return false;
}

/** Increases a pointer while it points to a character that belongs to a set.
  * @param str The initial pointer value.
  * @param set Set of characters. It is just a null-terminated string.
  * @return The final pointer value or null pointer if the null character was found. */
static char* _goWhile( char* str, char const* set ) {
    for(; *str != '\0'; ++str ) {
        if ( !_isOneOfThem( *str, set ) )
            return str;
    }
    return 0;
}

/** Increases a pointer while it points to a white space character.
  * @param str The initial pointer value.
  * @return The final pointer value or null pointer if the null character was found. */
static char* _goWhiteSpace( char* str ) {
    return _goWhile( str, " \n\r\t\f" );
}

/** Checks if a character is a decimal digit. */
static bool _isNum( unsigned char ch ) {
    return ch >= '0' && ch <= '9';
}

/** Increases a pointer while it points to a decimal digit character.
  * @param str The initial pointer value.
  * @return The final pointer value or null pointer if the null character was found. */
static char* _goNum( char* str ) {
    for( ; *str != '\0'; ++str ) {
        if ( !_isNum( *str ) )
            return str;
    }
    return 0;
}

/** Set a char to '\0' and increase its pointer if the char is diferent to '}' or ']'.
  * @param ch Pointer to character.
  * @return  Final value pointer. */
static char* _setToNull( char* ch ) {
    if ( !_isOneOfThem( *ch, "}]" ) ) *ch++ = '\0';
    return ch;
}
