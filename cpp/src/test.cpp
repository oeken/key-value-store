#include <stdio.h>
#include <iostream>
#include <thread>
#include <vector>
#include "KVOStore.h"

using namespace std;

// test stuff
void test_functionality(){
    KVOStore* store = new KVOStore(3);

    store->put(string("a"), string("1"));
    store->put(string("b"), string("2"));
    store->put(string("c"), string("3"));
    store->put(string("d"), string("4"));
    store->put(string("e"), string("5"));

    store->remove("a");
    cout << store->get("d") << endl;
    cout << store->get("d") << endl;

    cout << endl;
    store->print();

    delete store;
}

void accessStore(KVOStore* store, int threadId){
    if(threadId==300){
        store->remove(string("a"));
        cout << " PUT 300: \n" << flush;
    }
    if(threadId==600){
        store->put(string("a"),string("600"));
        cout << " PUT 600: \n" << flush;
    }

    string s = to_string(threadId) + " GET: " + store->get(string("a")) + "\n";
    cout << s << flush;
}

void test_threadsafety(){
    auto store = new KVOStore(3);
    store->put(string("a"),string("1"));

    vector<thread> ts;
    for (int i = 0; i < 700; ++i) {
        ts.push_back(thread(accessStore, store, i));
    }

    for (auto& t: ts) {
        t.join();
    }

    cout << "deleting store" << endl;
    delete store;
    cout << "deleted store" << endl;

}

void test_diskfunctionality(){
	auto store = new KVOStore(3);

	// store->remove(string("a"));
	// store->remove(string("b"));
	// store->remove(string("d"));

    // store->put(string("a"),string("23"));
    // store->put(string("b"),string("2"));
    // store->put(string("c"),string("3"));
    // store->put(string("d"),string("4"));

    store->print();

    int dummy;
    scanf("%d", &dummy);

    delete store;
}

int main() {

	// test_functionality();
    // test_threadsafety();
    test_diskfunctionality();

    // cout << "olley be" << endl;

    return 0;
}


