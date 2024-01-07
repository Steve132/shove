#include<shv/iterator/generator.hpp>
#include<iostream>
#include<vector>
#include<iterator>

int main(int argc,char** argv){
    int z=0;
    auto genf=[z] () -> std::optional<int> {
            std::optional<int> op;
            return op;
        };
    shv::generator_iterator<decltype(genf)> b(std::move(genf));

    return 0;
}