#include "KVOStore.h"
#include <pqxx/pqxx>

using namespace std;


// internal
int KVOStore::putToCache(string &k, string &v){
    if(lookup.find(k) != lookup.end()) { // in the cache, update existing
        auto nodeIter = lookup[k];
        auto nodePtr = (*nodeIter);
        nodePtr->value = v; // update node value
        cache.erase(nodeIter);
        cache.push_front(nodePtr); // move forward
        lookup[k] = cache.begin(); // update nodeIter
    }else{ // not in the cache, insert new node
        if (cache.size() == MAX_CACHE_SIZE) evict(); // evict if cache is full
        auto nodePtr = new KVONode(k,v); // make new pair
        cache.push_front(nodePtr); // add to beginning of cache
        auto nodeIter = cache.begin(); // get reference
        lookup.insert(make_pair(k, nodeIter)); // put to lookup table
    }
    return 1;
}

int KVOStore::putToDisk(string& k, string& v){
    string oldVal = getFromDisk(k);

    string sql;
    if(oldVal==""){ // didnt exist
        // sql
        char buffer[2000];
        sprintf(buffer, "INSERT INTO records VALUES (\'%s\', \'%s\');", k.c_str(), v.c_str());
        sql = string(buffer); 
    }else{ // existed, only update
        // sql
        char buffer[2000];
        sprintf(buffer, "UPDATE records SET kvovalue=\'%s\' WHERE kvokey=\'%s\';", v.c_str(), k.c_str());
        sql = string(buffer); 
    }

    // connect -- exec -- commit -- disconnect
    pqxx::connection C(connString);
    pqxx::work W(C);    
    W.exec(sql);
    W.commit();
    C.disconnect();

    return 1;
}

void KVOStore::evict(){
    auto lastPtr = cache.back();
    string k = lastPtr->key;
    string v = lastPtr->value;
    cache.pop_back(); // remove from cache
    lookup.erase(k); // remove from lookup
    putToDisk(k, v); // insert to disk
}

int KVOStore::removeFromCache(string& k){
    // fetch value
    auto nodeIter = lookup[k];
    auto nodePtr = *nodeIter;
    cache.erase(nodeIter);
    lookup.erase(nodePtr->key);
    return 1;
}

int KVOStore::removeFromDisk(string& k){
    // sql
    char buffer [2000];
    sprintf(buffer, "DELETE FROM records where kvokey=\'%s\';", k.c_str());
    string sql(buffer); 

    // connect -- exec -- commit -- disconnect
    pqxx::connection C(connString);
    pqxx::work W(C);
    W.exec(sql);
    W.commit();
    C.disconnect();

    return 1;
}

string KVOStore::getFromCache(string& k){
    // fetch value
    auto nodeIter = lookup[k];
    auto nodePtr = *nodeIter;
    string v = nodePtr->value;

    // move to front
    cache.erase(nodeIter);
    cache.push_front(nodePtr);
    lookup[nodePtr->key] = cache.begin();

    return v;
}

string KVOStore::getFromDisk(string& k){
    // sql
    char buffer [2000];
    sprintf(buffer, "SELECT * FROM records WHERE kvokey=\'%s\';", k.c_str());
    string sql(buffer); 

    // connect -- exec -- commit -- disconnect
    pqxx::connection C(connString);
    pqxx::work W(C);    
    pqxx::result R = W.exec(sql);
    W.commit();
    C.disconnect();

    // return value  
    if(R.size()==0) return "";
    string value = R[0][1].c_str();
    return value;
}


// constructors
KVOStore::KVOStore(int MCS){
    MAX_CACHE_SIZE = MCS;
    list<KVONode*> cache = list<KVONode*>();
    unordered_map<string, list<KVONode*>::iterator> lookup = unordered_map<string, list<KVONode*>::iterator>();
}


KVOStore::~KVOStore(){
    while(!cache.empty()){
        auto nodePtr = cache.front();
        putToDisk(nodePtr->key, nodePtr->value);
        delete nodePtr;
        cache.pop_front();
    }
}


// interface
int KVOStore::put(string k, string v){
    mtx.lock(); // critical section: begin
    
    int status;
    if(k.empty() || v.empty()){
        status = 0;
    }else if(getFromDisk(k)==""){
        status = putToCache(k, v);
    }else{
        removeFromDisk(k);
        status = putToCache(k, v);
    }
    
    mtx.unlock(); // critical section: end
    return status;
}

int KVOStore::remove(string k){
    mtx.lock(); // critical section: begin
    
    int status;
    if(k.empty()){
        status = 0;
    }else if(lookup.find(k) != lookup.end()){ // in the cache
        status = removeFromCache(k);
    }else{ // in the disk
        status = removeFromDisk(k);
    }
    
    mtx.unlock(); // critical section: end
    return status;
}

string KVOStore::get(string k){
    mtx.lock(); // critical section: begin
    
    string v;
    if(k.empty()){
        v = "";
    }else if(lookup.find(k) != lookup.end()){ // in the cache
        v = getFromCache(k);
    }else{ // in the disk
        v = getFromDisk(k);
        if(!v.empty()){
            removeFromDisk(k);
            putToCache(k, v);    
        }
    }

    mtx.unlock(); // critical section: end
    return v;
}

int KVOStore::size(){
    mtx.lock(); // critical section: begin
    int sz = cache.size();
    mtx.unlock(); // critical section: begin
    return sz;
}

void KVOStore::print(){
    mtx.lock(); // critical section: begin
    cout << "Found " << cache.size() << " items in the cache" << endl;
    for(auto const& ptr: cache){
        cout << "[" << ptr->key << "] [" << ptr->value << "]" << endl;
    }
    mtx.unlock(); // critical section: begin
}
