# distutils: language = c++
# distutils: include_dirs = ['../cpp/include', '../cpp/ext/libpqxx/include', '../cpp/ext/libpqxx/build/include']
# distutils: library_dirs = ['../cpp/ext/libpqxx/build/src/dylib']
# distutils: libraries = ['pqxx']

from libcpp.string cimport string

cdef extern from "../cpp/include/KVOStore.h":
	cdef cppclass KVOStore:
		KVOStore(int) except +
		int put(string, string)
		int remove(string)
		string get(string)
		int size()
		# void print()

cdef extern from "../cpp/src/KVOStore.cpp":
	pass



