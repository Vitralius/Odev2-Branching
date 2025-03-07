#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <queue>
#include <stack>

using namespace std;

#define COUNTER 10 // Number of philosophers
#define LEFT_ONE (nth_philosopher + COUNTER - 1) % COUNTER // Refers to the philosopher sitting to the left of current philosopher.
#define RIGHT_ONE (nth_philosopher + 1) % COUNTER // Refers to the philosopher sitting to the right of current philosopher.

string state_of_philosopher[COUNTER];
int philosophers[COUNTER];
stack<int> waiting_stack;


mutex Mutex;
condition_variable condition[COUNTER];

void Dining_Time(int nth_philosopher)
{
    if (state_of_philosopher[nth_philosopher] == "HUNGRY" && state_of_philosopher[LEFT_ONE] != "EATING" && state_of_philosopher[RIGHT_ONE] != "EATING" && waiting_stack.top()==nth_philosopher)
    {
        state_of_philosopher[nth_philosopher] = "EATING";

        cout << nth_philosopher + 1 << ". philosopher takes forks from " << LEFT_ONE + 1 << ". and " << nth_philosopher + 1 << ". philosophers" << endl;
        cout << nth_philosopher + 1 << ". philosopher is EATING" << endl;

        waiting_stack.pop();

        condition[nth_philosopher].notify_one();
    }
}

void Take_Fork(int nth_philosopher)
{
    unique_lock<mutex> lock(Mutex);

    state_of_philosopher[nth_philosopher] = "HUNGRY";
    // waiting_stack.push(nth_philosopher);

    cout << nth_philosopher + 1 << ". philosopher is now HUNGRY" << endl;

    Dining_Time(nth_philosopher);

    while (state_of_philosopher[nth_philosopher] != "EATING")
    {
        condition[nth_philosopher].wait(lock);
    }

    Dining_Time(nth_philosopher);
    this_thread::sleep_for(chrono::seconds(2));
}

void Give_Fork(int nth_philosopher)
{
    unique_lock<mutex> lock(Mutex);

    state_of_philosopher[nth_philosopher] = "THINKING";

    cout << nth_philosopher + 1 << ". philosopher gives forks back to the table" << endl;
    cout << nth_philosopher + 1 << ". philosopher is now THINKING" << endl;


    waiting_stack.push(LEFT_ONE);
    Dining_Time(LEFT_ONE);
    waiting_stack.push(RIGHT_ONE);
    Dining_Time(RIGHT_ONE);
}

void Philosopher(int nth_philosopher)
{
    while (true)
    {
        this_thread::sleep_for(chrono::seconds(1));
        Take_Fork(nth_philosopher);
        this_thread::sleep_for(chrono::seconds(1));
        Give_Fork(nth_philosopher);
    }
}

int main()
{
    for(int i=0; i<COUNTER; i++)
    {
        philosophers[i] = i;
        waiting_stack.push(i);
    }
    thread Threads[COUNTER];

    for (int i = 0; i < COUNTER; i++)
    {
        state_of_philosopher[i] = "THINKING";
        Threads[i] = thread(Philosopher, philosophers[i]);
        cout << i + 1 << ". philosopher is THINKING" << endl;
    }

    for (int i = 0; i < COUNTER; i++)
    {
        Threads[i].join();
    }

    return 0;
}