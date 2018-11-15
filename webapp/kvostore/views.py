from django.shortcuts import render
from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt

import sys
sys.path.append('../py')
from PyKVOStore import PyKVOStore

store = PyKVOStore(3)

def index_get(request):
	global store
	key = request.GET.get('key','')	
	response = None
	if(key==''):
		response = JsonResponse({"status": 0})
	else:  
		value = store.get(key)
		if(value == ''):
			response = JsonResponse({"status": 0})
		else:
			response = JsonResponse({"status": 1, "value": value})
	return response


def index_put(request):
	global store
	key = request.GET.get('key','')
	value = request.GET.get('value','')
	response = None
	if(key == '' or value == ''):
		response = JsonResponse({"status":0})
	else:
		store.put(key, value)
		response = JsonResponse({"status":1})
	return response

def index_delete(request):
	global store
	key = request.GET.get('key','')
	response = None
	if(key == ''):
		response = JsonResponse({"status":0})
	else:
		store.remove(key)
		response = JsonResponse({"status":1})
	return response


@csrf_exempt
def index(request):
	if request.method == 'GET':
		return index_get(request)

	elif request.method == 'PUT':
		return index_put(request)
		
	elif request.method == 'DELETE':
		return index_delete(request)
	else:
		return JsonResponse({"status": -1, "msg": "method unsupported"})


