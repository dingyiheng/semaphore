#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <iostream>

using namespace std;

class semaphore {
public:
	semaphore() :sema(1), executing(-1) {}
	int sema;
	vector<int> waiting;
	int executing;

	mutex m;
	condition_variable cv;
};

void wait(semaphore& s, int id) {
	unique_lock<mutex> lock(s.m);
	s.sema--;
	if (s.sema < 0) {
		s.waiting.push_back(id);
		s.cv.wait(lock, [&] {return id == s.executing; });
	}
	else {
		s.executing = id;
	}
}

void signal(semaphore& s,int id) {
	lock_guard<mutex> lock(s.m);
	s.sema++;

	if (s.sema <= 0) {
		s.executing = s.waiting.back();
		s.waiting.pop_back();
		s.cv.notify_all();
	}
}

void Count_down(int id, int &Count, semaphore& s) {
	while (1) {
		wait(s, id);
		if (Count >= 1) {
			
			printf("ID: %d with count %d\n", id, Count);
			Count--;
			
		}
		signal(s, id);

		if (Count < 1) {
			break;
		}
		this_thread::get_id();
	}
}

int main() {
	int Count = 10;
	semaphore s;

	thread t[5];
	for (int i = 0; i < 5; i++) {
		t[i] = thread(Count_down,i,ref(Count),ref(s));
	}
	for (int i = 0; i < 5; i++) {
		t[i].join();
	}

	return 0;
}