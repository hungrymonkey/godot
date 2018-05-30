#ifndef COMMAND_SERVER_H
#define COMMAND_SERVER_H


#include "object.h"
#include "ustring.h"
#include  "io/treecursion_types.h"

class TreecursionSaver : public Object {
	GDCLASS(TreecursionSaver, Object);

	static TreecursionSaver *singleton;
protected:
	static void _bind_methods();

public:
	static TreecursionSaver *get_singleton();
	virtual void flush() = 0;
	virtual void enqueue(TreecursionWriteTask * task) = 0;
};



typedef TreecursionSaver *(*CreateTreecursionSaverCallback)();


class TreecursionServerManager {
	struct ClassInfo {
		String name;
		CreateTreecursionSaverCallback create_callback;

		ClassInfo() :
				name(""),
				create_callback(NULL) {}

		ClassInfo(String p_name, CreateTreecursionSaverCallback p_create_callback) :
				name(p_name),
				create_callback(p_create_callback) {}

		ClassInfo(const ClassInfo &p_ci) :
				name(p_ci.name),
				create_callback(p_ci.create_callback) {}
	};

	static Vector<ClassInfo> treecursion_servers;
	static int default_server_id;
	static int default_server_priority;

public:
	static const String setting_property_name;

private:
	static void on_servers_changed();

public:
	static void register_server(const String &p_name, CreateTreecursionSaverCallback p_creat_callback);
	static void set_default_server(const String &p_name, int p_priority = 0);
	static int find_server_id(const String &p_name);
	static int get_servers_count();
	static TreecursionSaver *get_saver(int p_driver);
	static String get_server_name(int p_id);
	static TreecursionSaver *new_server(const String &p_name);
};

class TreecursionServer : public Object {
	GDCLASS(TreecursionServer, Object);

	//friend class TreecursionServerManager;

	static TreecursionServer *singleton;
protected:
	static void _bind_methods();

public:
	static TreecursionServer *get_singleton();
	void enqueue(TreecursionWriteTask * task);
};
#endif