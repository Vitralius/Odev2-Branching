#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>

using namespace std;

#define CUSTOMER_COUNTER 20 // Number of customers
#define CHAIR_COUNTER 4 // Number of chairs
 
// A barbershop consists of a waiting room with n chairs and the barber room containing the barber
// chair. If there are no customers to be served, the barber goes to sleep. If a customer enters the
// barbershop and all chairs are occupied, then the customer leaves the shop. If the barber is busy but
// chairs are available, then the customer sits in one of the free chairs. If the barber is asleep, the
// customer wakes up the barber.

// Customer => WAITING, LEAVED, SERVED  
// Barber => SLEEPING, AWAKENED
string state_of_customer[CUSTOMER_COUNTER];
string state_of_barber = "SLEEPING"; //Initial state of the barber

mutex Mutex;
condition_variable customer_condition;
condition_variable barber_condition;
int waiting = 0; // Number of customers that waiting
vector<int> LEAVED_COUNTER; // Number of customers that leaved the barber

void Barber() 
{
    while (true) 
    {
        unique_lock<mutex> lock(Mutex);
        while(waiting==0)
        {   
            barber_condition.notify_one();
            state_of_barber="SLEEPING";
            cout << "Barber is sleeping" << endl;
            customer_condition.wait(lock); 
        }

        if(waiting>0)
        {
            waiting--;
            cout << "Barber is cutting hair of customer" << endl;
            this_thread::sleep_for(chrono::seconds(4));
            barber_condition.notify_one();
        }
    }
}

void customer(int nth_customer) 
{
    unique_lock<mutex> lock(Mutex);
    if (waiting < CHAIR_COUNTER) {
        state_of_customer[nth_customer] = "ENTERED";
        cout << nth_customer + 1 << ". customer is ENTERED" << endl;
        if(state_of_barber=="SLEEPING")
        {
            state_of_barber="AWAKENED";
            cout << nth_customer + 1 << ". AWAKENED the barber" << endl;
        }
        waiting++;
        customer_condition.notify_one();
        cout << "Customer " << nth_customer+1 << " is waiting." << endl;
        state_of_customer[nth_customer] = "WAITING";
        barber_condition.wait(lock);
        cout << nth_customer+1 << ". customer is getting a haircut." << endl;
        state_of_customer[nth_customer] = "SERVED";
        cout << "HERE" << endl;
        this_thread::sleep_for(chrono::seconds(1));
    } else {
        cout << "Shop is full. " << nth_customer+1 << ". customer leaves." << endl;
        LEAVED_COUNTER.push_back(nth_customer);
        cout << "THERE" << endl;
        state_of_customer[nth_customer] = "LEAVED";
        
    }
}


int main()
{
    {
        thread barberThread(Barber);
        vector<thread> customerThreads;
        vector<thread> leavedThreads;

        for (int i = 0; i < CUSTOMER_COUNTER; i++)
        {
            customerThreads.emplace_back(customer, i);
            this_thread::sleep_for(chrono::seconds(1));
        }
    
        for (int i = 0; i < CUSTOMER_COUNTER; i++)
        {
            customerThreads[i].join();
        }
 
        for (int i = 0; i < (int)LEAVED_COUNTER.size(); i++)
        {   
            leavedThreads.emplace_back(customer, LEAVED_COUNTER[i]);
            this_thread::sleep_for(chrono::seconds(1));
        }

        for (int i = 0; i < (int)LEAVED_COUNTER.size(); i++)
        {
            leavedThreads[i].join();
        }

        barberThread.detach();
    }

    return 0;
}