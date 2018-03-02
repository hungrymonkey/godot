#include "my_json_loader.h"
#include "my_json.h"

ResourceFormatLoaderMyJson::ResourceFormatLoaderMyJson() {
}
RES ResourceFormatLoaderMyJson::load(const String &p_path, const String &p_original_path, Error *r_error) {
	MyJson *my = memnew(MyJson);
	if (r_error)
		*r_error = OK;
	Error err = my->set_file(p_path);
	return Ref<MyJson>(my);
}

void ResourceFormatLoaderMyJson::get_recognized_extensions(List<String> *p_extensions) const {

	p_extensions->push_back("mjson");
}
String ResourceFormatLoaderMyJson::get_resource_type(const String &p_path) const {

	if (p_path.get_extension().to_lower() == "mjson")
		return "MyJson";
	return "";
}

bool ResourceFormatLoaderMyJson::handles_type(const String &p_type) const {
	return (p_type == "MyJson");
}
