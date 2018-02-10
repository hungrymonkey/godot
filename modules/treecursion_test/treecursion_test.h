/* talking_tree_storage.h */
#ifndef TREECURSION_TEST_STORAGE_H
#define TREECURSION_TEST_STORAGE_H



#include "object.h"
#include "variant.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"

#include "reference.h"
#include "os/file_access.h"
#include "ustring.h"

#include "treecursion_write.h"
#include "io/treecursion_types.h"

#include "ref_ptr.h"
#include "treecursion_queue_lockless.h"

class TreecursionTestStorage : public Object {
	GDCLASS(TreecursionTestStorage, Object);

	static TreecursionTestStorage *_singleton;
	static void thread_func(void *p_udata);
public:
	static TreecursionTestStorage *get_singleton();
	void set_singleton();
	void lock();
	void unlock();
	void finish();
	Error init();

private:
	bool _thread_exited;
	mutable bool _exit_thread;
	Thread *_thread;
	Mutex *_mutex;

protected:
	static void _bind_methods();

public:
	bool is_empty();
	void new_file();
	void close_file();

	void enqueue(TreecursionWriteTask * packet);

	Ref<TreecursionWriteTask> dequeue();
	TreecursionTestStorage();
	~TreecursionTestStorage();
	

private:
	TreecursionQueue<TreecursionWriteTask, 4096> game_queue;
	void write_packet(TreecursionWriteTask * packet);
	TreecursionTestWriter *treecursion;


};

#endif
