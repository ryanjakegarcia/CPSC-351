/**
* PIDManager C++ implementation
* Author: Ryan Garcia
* Created: 9/20/2025
* Last updated: 9/22/2025
**/

#include<iostream>
#include<vector>
#include<unistd.h>
#include<sys/wait.h>

#define MIN_PID 100
#define MAX_PID 1000
#define TABLE_SIZE MAX_PID - MIN_PID + 1

class PIDManager{
    private:
        std::vector<bool> pid_map;
        bool initialized;

    public:
        PIDManager(){
            pid_map.reserve(TABLE_SIZE);
            initialized = false;
        }

        int allocate_map(){
            pid_map.assign(TABLE_SIZE, false);
            initialized = true;
            return 1; //Success, after further research, std::vector::assign() is considered to never throw an exception so this allocation shouldn't fail.
        }

        int allocate_pid(){
            if(!initialized)
            {
                std::cerr << "Unable to allocate PID, map unitialized." << std::endl;
                return -1;
            }

            for(int i = 0; i < pid_map.size(); i++){
                if(!pid_map[i]){
                    pid_map[i] = true;
                    return MIN_PID + i; //first available PID is found & returned
                }
            }

            return -1; //No available PIDs
        }

        void release_pid(int pid){
            if(!initialized){
                std::cerr << "Unable to release PID: " << pid << ", map unitialized." << std::endl;
                return;
            }

            if(pid < MIN_PID || pid > MAX_PID){ //bounds checking on provided PID
                std::cerr << "Invalid PID: " << pid << std::endl;
                return;
            }

            pid_map[pid - MIN_PID] = false; //free this PID
        }
};

// required_tests() by Ryan Garcia
void required_tests(){
    int n = 0;
    int cycle = TABLE_SIZE + n; //Will allocate 'n' too many PIDs.
    PIDManager manager;
    manager.allocate_map(); //Required test 1, calling allocate_map()

    for(int i = 0; i < cycle; i++){
        std::cout << "Allocated PID: " << manager.allocate_pid() << std::endl; //Required test 2 & 3, calling allocate_pid() multiple times then checking if the PID is within [100-1000]
    }

    for(int i = MIN_PID; i < MAX_PID; i++){
        manager.release_pid(i);
        std::cout << "Released PID: " << i << std::endl; //Required test 4, callin release_pid() for each allocated PID
    }

    for(int i = MIN_PID; i < MAX_PID; i += 10){
        std::cout << "Reallocated PID: " << manager.allocate_pid() << std::endl; //Required test 5, checking if released PID is available again
    }
}

class edge_cases{
    public:
        edge_cases(){
            try_alloc_before_init();
            verify_negative_pid();
            release_before_init();
            many_operations();
        }
    private:
        void try_alloc_before_init(){
            PIDManager manager;
            manager.allocate_pid();
        }

        void verify_negative_pid(){
            PIDManager manager;
            manager.allocate_map();
            for(int i = MIN_PID; i < MAX_PID + 1; i++){ //Allocates entire table
                manager.allocate_pid(); 
            }

            if(manager.allocate_pid() == -1) //Attempts to allocate another PID
                std::cout << "Table full, no valid PIDs available. [-1]" << std::endl;
        }

        void release_before_init(){
            PIDManager manager;
            manager.release_pid(100);
        }

        void many_operations(){
            PIDManager manager;
            manager.allocate_map();
            int times = 100;
            
            for(int count = 0; count < times; count++){
                for(int i = MIN_PID; i < MAX_PID + 1; i++){
                    manager.allocate_pid();
                    manager.release_pid(i);
                }
            }

            std::cout << "Manager successfully allocated & deallocated PIDs " << (times * (TABLE_SIZE)) << " times." << std::endl;
        }
};

// void whatif_tests() by Faris Fraihat
void whatif_tests(){
    PIDManager m1;
    int pid1 = m1.allocate_pid();
    std::cout << "test1 result: " << pid1 << std::endl;

    PIDManager m2;
    m2.release_pid(MIN_PID);
    std::cout << "test2 done" << std::endl;

    PIDManager m3;
    m3.allocate_map();
    for(int i = 0; i < 3; i++){
        int p = m3.allocate_pid();
        std::cout << "test3 got pid: " << p << std::endl;
    }

    PIDManager m4; 
    m4.allocate_map();
    int p1 = m4.allocate_pid();
    m4.release_pid(p1);
    int p2 = m4.allocate_pid();
    std::cout << "test4 first=" << p1 << " second=" << p2 << std::endl;

    PIDManager m5;
    m5.allocate_map();
    int last=-1;
    for(int i = 0; i < TABLE_SIZE; i++){
        last = m5.allocate_pid();
    }
    int extra = m5.allocate_pid();
    std::cout << "test5 last=" << last << " extra=" << extra << std::endl;

    PIDManager m6;
    m6.allocate_map();
    for(int i = 0; i < 10; i++){
        int p = m6.allocate_pid();
        m6.release_pid(p);
    }
    std::cout << "test6 done" << std::endl;
}

int main(){
    pid_t pid = fork();
    
    if(pid < 0){
        std::cerr << "Fork failed!" << std::endl;
        return 1;
    }else if(pid == 0){
        std::cout << "Child process PID Manager" << std::endl;
        required_tests();
        whatif_tests();
        edge_cases edge;
        
    }else{
        wait(NULL);
        std::cout<< "Parent process PID Manager" << std::endl;
        required_tests();
        whatif_tests();
        edge_cases edge;
    }

    return 0;
}