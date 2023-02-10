
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tiny-json.h"
#include "../json-maker/json-maker.h"

int main( void ) {

  printf("tiny-json objects to json...\n");
  {
    // convert a static string to a tiny-json objects
    const char* original = "{\"ts\":\"2023-02-09T23:18:27Z\",\"dev\":true,\"info\":{\"r1\":16807,\"ra\":[282475249,1622650073,984943658,1144108930],\"r2\":470211272},\"r3\":44}";
    char *data = malloc( strlen(original)+1 );
    strcpy(data, original);

    json_t pool[16] = {0,};
    json_t const *e = json_create( data, pool, sizeof(pool)/sizeof(pool[0]) );
    assert( e );

    // test something in those tiny-json objects
    json_t const *e1 = json_getProperty( e, "dev" );
    const char* key = json_getName(e1);
    bool value = json_getBoolean(e1);
    assert( strcmp( key, "dev" ) == 0 );
    assert( value == true );

    // convert tiny-json objects back into json string
    size_t sz = 2048;
    char *json = calloc( 1, sz );
    assert(json);

    // just convert back, don't change anything
    json_ObjectsToJSON( json, &sz, e );
    assert( strcmp( json, original ) == 0 );

    // convert while replacing a key
    json_replaceInteger( json, &sz, e, "r1", 22 );
    assert( strstr( json, "\"ts\":\"2023-02-09T23:18:27Z\"") ); // no change
    assert( strstr( json, "\"r1\":22") );                       // <- changed
    assert( strstr( json, "\"dev\":true") );                    // no change

    // retain last change by reseting json input & storing tiny-objects
    strcpy(data, json);
    e = json_create( data, pool, sizeof(pool)/sizeof(pool[0]) ); 

    // convert while replacing a key
    json_replaceBoolean( json, &sz, e, "dev", false );
    assert( strstr( json, "\"ts\":\"2023-02-09T23:18:27Z\"") ); // no change
    assert( strstr( json, "\"r1\":22") );                       // still changed
    assert( strstr( json, "\"dev\":false") );                   // <- changed

    // retain last change by reseting json input & storing tiny-objects
    strcpy(data, json);
    e = json_create( data, pool, sizeof(pool)/sizeof(pool[0]) ); 

    // convert while replacing a key
    json_replaceString( json, &sz, e, "ts", "ABCDEF" );
    assert( strstr( json, "\"ts\":\"ABCDEF\"") );               // <- changed
    assert( strstr( json, "\"r1\":22") );                       // still changed
    assert( strstr( json, "\"dev\":false") );                   // still changed

    free(data);
    free(json);
  }
  printf("pass\n");
  return 0;
}

