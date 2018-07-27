#include "reference.h"
#include "vector.h"


class VarInt : public Reference {
	GDCLASS(VarInt,Reference);

	public:
	VarInt(const uint8_t *encoded);
	
	VarInt(Vector<uint8_t> encoded);
	
	VarInt(int64_t value);
	
	int64_t getValue() const {
		return this->value;
	}
	
	Vector<uint8_t> getEncoded() const;
	
	private:
	const int64_t value;
	
	int64_t parseVariant(const uint8_t *buffer);
};
