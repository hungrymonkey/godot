#include "core/project_settings.h"

#include "treecursion_server.h"

TreecursionServer *TreecursionServer::singleton = NULL;

TreecursionServer *TreecursionServer::get_singleton() {
	return singleton;
}

void TreecursionServer::enqueue(TreecursionWriteTask * task){
	for(int i = 0; i < TreecursionServerManager::get_servers_count(); i++){
		TreecursionServerManager::get_saver(i)->enqueue(task);
	}
}
TreecursionSaver *TreecursionServerManager::get_saver(int p_saver){
	ERR_FAIL_INDEX_V(p_saver, get_servers_count(), NULL);
	//return treecursion_servers[p_saver];
	return NULL;
}

Vector<TreecursionServerManager::ClassInfo> TreecursionServerManager::treecursion_servers;
const String TreecursionServerManager::setting_property_name("io/treecursion_engine");

int TreecursionServerManager::get_servers_count() {
	return treecursion_servers.size();
}

String TreecursionServerManager::get_server_name(int p_id) {
	ERR_FAIL_INDEX_V(p_id, get_servers_count(), "");
	return treecursion_servers[p_id].name;
}

void TreecursionServerManager::on_servers_changed() {

	String treecursion_servers("DEFAULT");
	for (int i = get_servers_count() - 1; 0 <= i; --i) {
		treecursion_servers += "," + get_server_name(i);
	}
	ProjectSettings::get_singleton()->set_custom_property_info(setting_property_name, PropertyInfo(Variant::STRING, setting_property_name, PROPERTY_HINT_ENUM, treecursion_servers));

}


int TreecursionServerManager::find_server_id(const String &p_name) {
	for (int i = treecursion_servers.size() - 1; 0 <= i; --i) {
		if (p_name == treecursion_servers[i].name) {
			return i;
		}
	}
	return -1;
}	

void TreecursionServerManager::register_server(const String &p_name, CreateTreecursionSaverCallback p_creat_callback) {
	ERR_FAIL_COND(!p_creat_callback);
	ERR_FAIL_COND(find_server_id(p_name) != -1);
	treecursion_servers.push_back(ClassInfo(p_name, p_creat_callback));
	on_servers_changed();
}
