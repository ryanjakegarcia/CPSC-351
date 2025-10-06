/**
* PIDManager C++ implementation
* Author: Ryan Garcia & Dhuha Abdulhussein
* Created: 9/20/2025
* Last updated: 10/6/2025
**/

#include<iostream>
#include<vector>
#include<unistd.h>
#include<sys/wait.h>
#include <cstring>
#include<string>
#include<fstream>

#define MIN_PID 100
#define MAX_PID 1000
#define TABLE_SIZE MAX_PID - MIN_PID + 1
#define BUFFER_SIZE 256
#define READ_END 0
#define WRITE_END 1

//pid_manager class by Ryan Garcia & Dhuha Abdulhussein
class pid_manager{
    private:
        std::vector<bool> pid_map;
        bool initialized;

    public:
        pid_manager(){
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
    pid_manager manager;
    manager.allocate_map(); //Required test 1, calling allocate_map()

    for(int i = 0; i < cycle; i++){
        std::cout << "Allocated PID: " << manager.allocate_pid() << std::endl; //Required test 2 & 3, calling allocate_pid() multiple times then checking if the PID is within [100-1000]
    }

    for(int i = 0; i < cycle; i++){
        manager.release_pid(i + MIN_PID);
        std::cout << "Released PID: " << i + MIN_PID << std::endl; //Required test 4, callin release_pid() for each allocated PID
    }

    for(int i = 0; i < cycle; i++){
        std::cout << "Reallocated PID: " << manager.allocate_pid() << std::endl; //Required test 5, checking if released PID is available again
    }
}

//Edge cases class tests by Ryan Garcia
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
            pid_manager manager;
            manager.allocate_pid();
        }

        void verify_negative_pid(){
            pid_manager manager;
            manager.allocate_map();
            for(int i = MIN_PID; i < MAX_PID + 1; i++){ //Allocates entire table
                manager.allocate_pid(); 
            }

            if(manager.allocate_pid() == -1) //Attempts to allocate another PID
                std::cout << "Table full, no valid PIDs available. [-1]" << std::endl;
        }

        void release_before_init(){
            pid_manager manager;
            manager.release_pid(100);
        }

        void many_operations(){
            pid_manager manager;
            manager.allocate_map();
            int times = 100;
            
            for(int count = 0; count < times; count++){
                for(int i = MIN_PID; i < MAX_PID + 1; i++){
                    manager.allocate_pid();
                    manager.release_pid(i);
                }
            }

            std::cout << "Manager successfully allocated & deallocated PIDs " << (times * (TABLE_SIZE) * 2) << " times." << std::endl;
        }
};

// void whatif_tests() by Faris Fraihat
void whatif_tests(){
    pid_manager m1;
    int pid1 = m1.allocate_pid();
    std::cout << "test1 result: " << pid1 << std::endl;

    pid_manager m2;
    m2.release_pid(MIN_PID);
    std::cout << "test2 done" << std::endl;

    pid_manager m3;
    m3.allocate_map();
    for(int i = 0; i < 3; i++){
        int p = m3.allocate_pid();
        std::cout << "test3 got pid: " << p << std::endl;
    }

    pid_manager m4; 
    m4.allocate_map();
    int p1 = m4.allocate_pid();
    m4.release_pid(p1);
    int p2 = m4.allocate_pid();
    std::cout << "test4 first=" << p1 << " second=" << p2 << std::endl;

    pid_manager m5;
    m5.allocate_map();
    int last=-1;
    for(int i = 0; i < TABLE_SIZE; i++){
        last = m5.allocate_pid();
    }
    int extra = m5.allocate_pid();
    std::cout << "test5 last=" << last << " extra=" << extra << std::endl;

    pid_manager m6;
    m6.allocate_map();
    for(int i = 0; i < 10; i++){
        int p = m6.allocate_pid();
        m6.release_pid(p);
    }
    std::cout << "test6 done" << std::endl;
}

//By Faris Fraihat - not working
/* void parent_handle_requests(int read_fd, int write_fd, pid_manager& manager){
    char buffer[256];
    int last_pid = -1;
    while(true){
        if(!read(read_fd, buffer, sizeof(buffer))){
            break;
        }

        if(strcmp(buffer, "Allocate") == 0){
            int new_pid = manager.allocate_pid();
            last_pid = new_pid;
            snprintf(buffer, sizeof(buffer), "%d", new_pid);
            write(write_fd, buffer, strlen(buffer) + 1);
        } else if(strcmp(buffer, "Release") == 0){
            manager.release_pid(last_pid);
            std::cout << "Parent received request to release PID: " << last_pid << std::endl;
        } else if(strcmp(buffer, "Done") == 0){
            break;
        }
    }
} */

//By Faris Fraihat - not working
/* void child_request_pids(int write_fd, int read_fd, int iteration){
    char buffer[256];
    for(int i = 0; i < iteration; ++i){
        write(write_fd, "Allocate", sizeof("Allocate"));
        read(read_fd, buffer, sizeof(buffer));
        int received_pid = atoi(buffer);
        std::cout << "Hello from child, received PID: " << received_pid << std::endl;
        write(write_fd, "Release", sizeof("Release"));
    }
} */

int main(){
    int ppipe[2], cpipe[2]; //Parent writing pipe, child writing pipe
    pid_t pid;
    
    if(pipe(ppipe) == -1){
        fprintf(stderr, "Parent pipe failed");
        return 1;
    }

    if(pipe(cpipe) == -1){
        fprintf(stderr, "Child pipe failed");
        return 1;
    }


    pid = fork();
    std::ofstream parent_file, child_file;
    parent_file.open("poutput.txt");
    child_file.open("coutput.txt");
    if(pid < 0){
        std::cerr << "Fork Failed" << std::endl;
    } else if(pid == 0){ //child
        close(ppipe[WRITE_END]);
        close(cpipe[READ_END]);
        char buffer[BUFFER_SIZE];
        int counter = 0;
        int cycle = TABLE_SIZE;
        int last_pid = -1;

        while(true){
            if(counter++ >= cycle && counter <= 2*cycle){
                snprintf(buffer, sizeof(buffer), "%d", last_pid--); //child writes pid that it wants released
                write(cpipe[WRITE_END], buffer, sizeof(buffer));
            } else if(counter > 2*cycle){
                write(cpipe[WRITE_END], "Done", sizeof("Done"));
                break;
            } else{
                write(cpipe[WRITE_END], "Allocate", sizeof("Allocate"));
            }

            read(ppipe[READ_END], buffer, sizeof(buffer));

            if(strcmp(buffer, "Waiting") != 0){
                int received_pid = atoi(buffer);
                last_pid = received_pid;
                std::cout << "Hello from child, received PID: " << received_pid << std::endl;
                child_file << "Hello from child, received PID: " << received_pid << std::endl;
            }
        }

        close(ppipe[READ_END]);
        close(cpipe[WRITE_END]);
    } else{ //parent
        close(cpipe[WRITE_END]);
        close(ppipe[READ_END]);
        pid_manager manager;
        manager.allocate_map();

        //parent_handle_requests(ppipe[READ_END], cpipe[WRITE_END], manager);
        char buffer[BUFFER_SIZE];
        while(true){
            read(cpipe[READ_END], buffer, sizeof(buffer));

            if(strcmp(buffer, "Done\0") == 0){
                break;
            } else if(strcmp(buffer, "Allocate\0") == 0){
                int new_pid = manager.allocate_pid();
                snprintf(buffer, sizeof(buffer), "%d", new_pid);
                write(ppipe[WRITE_END], buffer, strlen(buffer) + 1);
            } else {
                int released_pid = atoi(buffer);
                manager.release_pid(released_pid);
                std::cout << "Parent received request to release PID: " << released_pid << std::endl;
                parent_file << "Parent received request to release PID: " << released_pid << std::endl;
                write(ppipe[WRITE_END], "Waiting", sizeof("Waiting"));
            }
        }
        close(cpipe[READ_END]);
        close(ppipe[WRITE_END]);
        wait(NULL);
    }
    parent_file.close();
    child_file.close();
    return 0;
}