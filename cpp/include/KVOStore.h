#ifndef KVOSTORE_KVOSTORE_H
#define KVOSTORE_KVOSTORE_H

#include <iostream>
#include <list>
#include <fstream>
#include <unordered_map>

using namespace std;

class KVOStore{

private:

    struct KVONode{
        string key;
        string value;

        KVONode(string k, string v){
            key = k;
            value = v;
        }
    };

    mutex mtx;
    const string connString = "dbname=kvostore user=kvostoreadmin";
    int MAX_CACHE_SIZE;
    list<KVONode*> cache;
    unordered_map<string, list<KVONode*>::iterator> lookup;

    // PUT
    int putToCache(string& k, string& v);
    int putToDisk(string& k, string& v);
    void evict();

    // REMOVE
    int removeFromCache(string& k);
    int removeFromDisk(string& k);

    // GET
    string getFromCache(string& k);
    string getFromDisk(string& k);


public:

    KVOStore(int MCS);
    ~KVOStore();
    int put(string k, string v);
    int remove(string k);
    string get(string k);
    int size();
    void print();

};


#endif //KVOSTORE_KVOSTORE_H
