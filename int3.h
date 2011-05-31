#ifndef INT3_H_
#define INT3_H_

struct int3 {
	int x, y, z;

	int3() : x(0), y(0), z(0) { }
	int3(int __z, int __y, int __x) : x(__x), y(__y), z(__z) { }

	bool operator<(const int3& oi) const {
		bool result = false;
		if (z < oi.z || y < oi.y || x < oi.x) result = true;

		return result;
	}
};

#endif /* INT3_H_ */
