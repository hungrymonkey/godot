/* talking_tree_storage.h */
#ifndef TALKING_TREE_STORAGE_H
#define TALKING_TREE_STORAGE_H

#include "object.h"
#include "variant.h"
#include "core/os/thread.h"
#include "core/os/mutex.h"

#include "reference.h"
#include "os/file_access.h"
#include "ustring.h"

#include "ogg/ogg.h"
#include "treecursion_writer.h"

#include "treecursion_queue_lockless.h"
#include "io/treecursion_types.h"
#include "servers/treecursion_server.h"
//#include <opus.h>
//#include <opus_multistream.h>

class TalkingTreeStorage : public Object {
	GDCLASS(TalkingTreeStorage, Object);

	static TalkingTreeStorage *_singleton;
	static void thread_func(void *p_udata);
public:
	static TalkingTreeStorage *get_singleton();
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
	bool is_empty() const;
	Ref<TreecursionWriteTask> dequeue();
	void enqueue(TreecursionWriteTask * write);
	void flush();
	void new_file();
	void start_recording();
	void set_active(bool);
	void write_engine_init(const Dictionary & dict);
	void close_file();
	bool is_running() const;
	TalkingTreeStorage();
	~TalkingTreeStorage();

private:
	TreecursionQueue<TreecursionWriteTask, 1000000> game_queue;
	//void write_packet(TreecursionWriteTask * packet);
	enum {
		//https://mf4.xiph.org/jenkins/view/opus/job/opus/ws/doc/html/group__opus__multistream.html
		SAMPLE_RATE = 48000,
	};

	enum recording_state {
		INIT,
		RECORDING,
		STOPPED,
		FINISHED,
		ERROR
	};
	/*
	 *
	 *  GDSCript may change the variable
	 */
	//volatile enum recording_state _recording_state;
	enum recording_state _recording_state;

	//OpusMSEncoder *_opusEncoder;
	TreecursionWriter *_treecursion;
	void write_task(const TreecursionWriteTask *write);
};

class _TalkingTreeStorage : public Object {
	GDCLASS(_TalkingTreeStorage, Object);

	friend class TalkingTreeStorage;

	static _TalkingTreeStorage *_singleton;
protected:
	
	static void _bind_methods();
	
public:
	static _TalkingTreeStorage *get_singleton() { return _singleton; }
	void start_recording();
	void write_engine_init(const Dictionary & dict);
	void set_file_name(String name);
	void new_file();
	_TalkingTreeStorage();
	~_TalkingTreeStorage();
};

#endif