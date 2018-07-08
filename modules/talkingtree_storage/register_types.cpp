#include "register_types.h"
#include "class_db.h"
#include "talking_tree_storage.h"
#include "treecursion_reader.h"
#include "talkingtree_storage_loader.h"
#include "treecursion_player.h"
#include "engine.h"


static ResourceFormatTalkingTreeStorage *talkingtree_storage_loader = NULL;

static TalkingTreeStorage *talking_tree_storage_writer = NULL;
static _TalkingTreeStorage *_talking_tree_storage = NULL;

static TreecursionPlayer *treecursion_player = NULL;
static _TreecursionPlayer *_treecursion_player = NULL;

/*
TreecursionSaver _createTreecursionServer(){
	return memnew(TalkingTreeStorage);
}*/

void register_talkingtree_storage_types(){
	//loader
	talkingtree_storage_loader = memnew(ResourceFormatTalkingTreeStorage);
    ResourceLoader::add_resource_format_loader(talkingtree_storage_loader);

	//writer
	talking_tree_storage_writer = memnew(TalkingTreeStorage);
	talking_tree_storage_writer->init();

	_talking_tree_storage = memnew(_TalkingTreeStorage);
	ClassDB::register_class<_TalkingTreeStorage>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("TalkingTreeStorage", _TalkingTreeStorage::get_singleton()));

	//reader
	ClassDB::register_class<Treecursion>();

	//submission queue
	treecursion_player = memnew(TreecursionPlayer);
	treecursion_player->init();
	_treecursion_player = memnew(_TreecursionPlayer);
	ClassDB::register_class<_TreecursionPlayer>();
	Engine::get_singleton()->add_singleton(Engine::Singleton("TreecursionPlayer", _TreecursionPlayer::get_singleton()));

}
void unregister_talkingtree_storage_types(){
	talking_tree_storage_writer->finish();
	treecursion_player->finish();

	memdelete(talkingtree_storage_loader);

	memdelete(_treecursion_player);
	memdelete(_talking_tree_storage);

	memdelete(treecursion_player);
	memdelete(talking_tree_storage_writer);
}