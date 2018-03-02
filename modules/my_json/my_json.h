#ifndef MY_JSON_H
#define MY_JSON_H

#include "core/variant.h"
#include "reference.h"
#include "variant_parser.h"
#include "io/json.h"
#include "dictionary.h"

class MyJson : public Resource{
    GDCLASS(MyJson, Resource);

protected:
    static void _bind_methods() {}

private:
    Dictionary dict;
public:
	Error set_file(const String &p_path){
		
	
		Error error_file;
		FileAccess *file = FileAccess::open(p_path, FileAccess::READ, &error_file);

		String buf = String("");
		while(!file->eof_reached()){
			buf += file->get_line();
		}

		String err_string;
		int err_line;
		JSON cmd;
		Variant ret;
		Error err = cmd.parse( buf, ret, err_string, err_line);
		file -> close();
		dict = Dictionary(ret);

		return OK;
	}
	//overload the to string so it gdscript prints
	//something other than the Classname and a number
	operator String() const {
		JSON a; 
		return a.print(dict);
	}
    MyJson() {};
    ~MyJson() {};
};

#endif