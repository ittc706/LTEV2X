#pragma once

class Delete {
public:
	/*
	* 删除函数，删除后会置空指针
	*/
	template<typename T>
	static void safeDelete(T*&t_Point, bool t_IsArray = false) {
		if (t_Point != nullptr) {
			if (t_IsArray)
				delete[] t_Point;
			else
				delete t_Point;
			t_Point = nullptr;
		}
	}
};