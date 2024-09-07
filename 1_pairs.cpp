#include <iostream>
#include <utility> // for pairs
#include <vector>
using namespace std;


void explainPair() {
    pair<int, int> p = {1, 3};
    cout << p.first << " " << p.second << endl;

    pair<int, pair<int, int>> a = {1, {3, 4}};

    cout << a.first << " " << a.second.second << " " << a.second.first << endl;

    pair<int, int> arr[] = { {1,2}, {2,5}, {5,1}};
    cout << arr[1].second << endl;
}

void explainVector() {
    vector<int> v;

    v.push_back(1);
    v.emplace_back(2); // faster than push back

    vector<pair<int, int>> vec;

    vec.push_back({1,2});  // give the curcly braces to enter a pair
    vec.emplace_back(1,2); // automaticaly asumes it to be a pair

    vector<int> v2(5, 100); // gives vector of leght 5 with each element 100 

    vector<int> v3(5); // container of size 5 is declared with garbage value

    vector<int> vec1(5, 20); // container of 5 instances of number 20
    vector<int> vec2(vec1); //vec2 will be another container of 5 instances of 20

    vector<int>::iterator it = v2.begin(); // iterators points to the memory address
    it++; // now at next address. vector has contagious memory
    cout << *(it) << " " << endl;

}

int main(){
    explainPair();
    return 0; 
}