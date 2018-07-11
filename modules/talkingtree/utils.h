#include "core/hash_map.h"

template<class TKey, class TData, class Hasher = HashMapHasherDefault, class Comparator = HashMapComparatorDefault<TKey>, uint8_t MIN_HASH_TABLE_POWER = 3, uint8_t RELATIONSHIP = 8>
class BiMap {
	HashMap<TKey, TData, Hasher, Comparator, MIN_HASH_TABLE_POWER, RELATIONSHIP> forward;
	HashMap<TKey, TData, Hasher, Comparator, MIN_HASH_TABLE_POWER, RELATIONSHIP> backward;
	public:
		void clear(){
			forward.clear();
			backward.clear();
		}
		void add(const TKey &p_key, const TData &p_data) {
			backward.set(p_data, p_key);
			forward.set(p_key, p_data);
		}
		const TData &getForward(const TKey &p_key) const {
			return forward.get(p_key);
		}
		const TKey &getBackward(const TData &p_key) const {
			return backward.get(p_key);
		}
		bool eraseKey(const TKey &p_key) {
			if(!forward.has(p_key))
				return false;
			backward.erase(forward.get(p_key));
			return forward.erase(p_key);
		}
		bool eraseData(const TData &p_key) {
			if(!backward.has(p_key))
				return false;
			forward.erase(backward.get(p_key));
			return backward.erase(p_key);
		}
		const TKey *next(const TKey *p_key) const {
			return forward.next(p_key);
		}

};