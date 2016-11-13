#pragma once

class Delete {
public:
	template<typename T>
	static void deleteSingle(T*&p) {
		if (p != nullptr) {
			delete p;
			p = nullptr;
		}
	}

	template<typename T>
	static void deleteAry(T*&p) {
		if (p != nullptr) {
			delete[] p;
			p = nullptr;
		}
	}
};