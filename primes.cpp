#include<iostream>
#include<cmath>
#include<vector>
#include<chrono>

bool is_prime(int number){
    if(number <= 1)
        return false;

    if(number == 2 || number == 3)
        return true;

    if(number % 2 == 0 || number % 3 == 0)
        return false;

    for(int i = 5; i <= sqrt(number); i += 6){
        if(number % i == 0 || number % (i + 2) == 0)
            return false;
    }

    return true;
}

std::vector<int> generate_primes(int begin, int end){

    std::vector<int> primes;
    primes.reserve(end - begin);

    for(int i = begin; i < end; i++){
        if(is_prime(i))
            primes.push_back(i);
    }

    return primes;
}

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cerr << "Argument invalid. Please enter a  positive integer argument." << std::endl;
        return 1;
    } else {
        int limit = atoi(argv[1]);
        if(limit < 1){
            std::cerr << "Argument invalid. Please enter a positive interger argument." << std::endl;
            return 1;
        } else if(limit <= 3){
            if(limit == 1)
                std::cout << "Primes up to " << limit << ": None" << std::endl;
            else if(limit == 2)
                std::cout << "Primes up to " << limit << ": 2" << std::endl;
            else
                std::cout << "Primes up to " << limit << ": 2, 3" << std::endl;
            return 0;
        }
        int block = limit/4;
        int final = 0;
        std::vector<int> primes;
        primes.reserve(limit);
        
        auto start_time = std::chrono::high_resolution_clock::now();

        for(int i = 1; i < limit - limit % block; i += block){
            std::vector<int> temp_primes = generate_primes(i, i + block);
            primes.insert(primes.end(), temp_primes.begin(), temp_primes.end());
            final = i + block;
        }

        while(final <= limit){
            if(is_prime(final))
                primes.push_back(final);

            final++;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto time_elapsed = std::chrono::duration<double>(end_time - start_time).count();

        std::cout << "Primes up to " << limit << ": ";

        for(int prime : primes){
            std::cout << prime << ", ";
        }

        std::cout << "\nTime to calculate with single-thread: " << time_elapsed << " seconds." << std::endl;

        return 0;
    }
}