/* summator.h */
#ifndef HILBERT_HOTEL_H
#define HILBERT_HOTEL_H

#include "object.h"
#include "list.h"
#include "variant.h"
#include "reference.h"
#include "vector.h"
#include "variant_parser.h"
#include "io/json.h"
#include "os/thread.h"
#include "os/mutex.h"
#include "dictionary.h"


class InfiniteBus : public RID_Data {
	RID self;

private:
	uint64_t prime_num;
	uint64_t num;

public:
	uint64_t next_room() {
		return prime_num * num++;
	}
    uint64_t get_bus_num() {
        return prime_num;
    }
    _FORCE_INLINE_ void set_self(const RID &p_self) { self = p_self; }
	_FORCE_INLINE_ RID get_self() const { return self; }

	InfiniteBus(uint64_t prime) : prime_num(prime), num(1) {};
	~InfiniteBus();
};


class HilbertHotel : public Object {
    GDCLASS(HilbertHotel, Object);

    static HilbertHotel *singleton;
	static void thread_func(void *p_udata);

private:
	bool thread_exited;
	mutable bool exit_thread;
	Thread *thread;
	Mutex *mutex;

public:
	static HilbertHotel *get_singleton();
	Error init();
	void lock();
	void unlock();
    void finish();

protected:
    static void _bind_methods();

private:
    RID_Owner<InfiniteBus> bus_owner;
    Set<InfiniteBus *> buses;
    
public:
    RID create_bus();
    void add(int value);
    void reset();
    int get_total() const;

    Variant parse_dict() const;
    HilbertHotel();
};

class _HilbertHotel : public Object {
    GDCLASS(_HilbertHotel, Object);

    static _HilbertHotel *singleton;
protected:
    static void _bind_methods();
public:
    RID create_bus();
    _HilbertHotel *get_singleton();
    _HilbertHotel();
};
#endif
