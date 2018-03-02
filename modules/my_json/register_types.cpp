/* register_types.cpp */

#include "register_types.h"
#include "class_db.h"

#include "my_json_loader.h"
#include "my_json.h"
static ResourceFormatLoaderMyJson *my_json_loader = NULL;
void register_my_json_types() {
	my_json_loader = memnew(ResourceFormatLoaderMyJson);
    ResourceLoader::add_resource_format_loader(my_json_loader);
	ClassDB::register_class<MyJson>();
}

void unregister_my_json_types() {
	memdelete(my_json_loader);
   //nothing to do here
}
