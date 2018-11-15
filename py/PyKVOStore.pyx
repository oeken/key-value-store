# distutils: language = c++

from libcpp.string cimport string
from KVOStore cimport KVOStore

# helpers
def s2b(s):
	return s.encode('utf-8')

def b2s(b):
	return b.decode('utf-8')

# wrapper class
cdef class PyKVOStore:
	cdef KVOStore* c_store  # Hold a C++ instance which we're wrapping

	def __cinit__(self, int MAX_CACHE_SIZE):
		self.c_store = new KVOStore(MAX_CACHE_SIZE)

	# PUT
	def put(self, key, value):
		return self.cput(s2b(key), s2b(value))

	def cput(self, string key, string value):
		return self.c_store.put(key, value)

	# REMOVE
	def remove(self, key):
		return self.cremove(s2b(key))

	def cremove(self, string key):
		return self.c_store.remove(key)

	# GET
	def get(self, key):
		return b2s(self.cget(s2b(key)))

	def cget(self, string key):
		return self.c_store.get(key)

	# OTHER
	def size(self):
		return self.c_store.size()

	def __dealloc__(self):
		del self.c_store

