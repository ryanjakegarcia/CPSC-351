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

int main(){
    int limit;
    std::cout << "Enter limit: ";
    std::cin >> limit;
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

    std::cout << "\nTime to calculate with single-thread: " << time_elapsed << " seconds.";
}