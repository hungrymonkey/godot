#include "treecursion_server.h"

TreecursionServer *TreecursionServer::singleton = NULL;

TreecursionServer *TreecursionServer::get_singleton() {
	return singleton;
}
