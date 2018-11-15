import numpy as np
from PyKVOStore import PyKVOStore


store = PyKVOStore(10)
store.put("a", "13")
store.put("b", "2")
# store.put("c", "3")

# print(store.size())

print(store.get("a"))
print(store.get("b"))
print(store.get("c"))

