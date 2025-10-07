#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

struct Treasure{
    std::string name;
    int weight, value;
    float score; //Weight per value (Lower is better)

    Treasure() : name(""), weight(-1), value(-1), score(-1) {}
    Treasure(const std::string &name, int weight, int value) : name(name), weight(weight), value(value) {
        score = weight;
        score = score / value;
    }
    Treasure(const Treasure &other){
        name = other.name;
        weight = other.weight;
        value = other.value;
        score = other.score;
    }

    bool operator<(const Treasure &rhs){
        return score < rhs.score;
    }
    Treasure operator=(const Treasure &rhs){
        name = rhs.name;
        weight = rhs.weight;
        value = rhs.value;
        score = rhs.score;
        return *this;
    }

    //Assuming input is valid
    static Treasure fromString(const std::string &str){
        size_t commaPos[2];
        commaPos[0] = str.find(',');
        commaPos[1] = str.find(',', commaPos[0]+1);
        
        std::string name = str.substr(1, commaPos[0]-2);
        int val = std::stoi(str.substr(commaPos[0]+1, commaPos[1]));
        int weight = std::stoi(str.substr(commaPos[1]+1));
        return {name, weight, val};
    }
};

struct Bag{
    const int MAX_WEIGHT;
    int weightLeft, curValue;
    std::vector<Treasure> treasure;

    Bag(int maxWeight) : MAX_WEIGHT(maxWeight), weightLeft(maxWeight), curValue(0){}
   
    //Tries adding the treasure
    //Returns whether the treasure was added
    bool add(const Treasure &t){
        //The bag as is can't hold
        if(t.weight > weightLeft){
            int removedVal = 0;
            int tempWeightLeft = weightLeft;
            int i = treasure.size()-1;
            //See if removing some treasure could allow it to fit 
            //But only if the total value of the treasure being removed is less
            for(; i >= 0; --i){ 
                removedVal += treasure[i].value;
                tempWeightLeft += treasure[i].weight;
                if(removedVal >= t.value){ //We would remove more value than we gain
                    return false;
                }
                if(tempWeightLeft >= t.weight){ //We can now hold t
                    //treasure.erase(treasure.begin() + i, treasure.end());
                    for(int j = treasure.size()-1 - i; j >= 0; --j){
                        std::cout << "- Dropped " << treasure.back().name << '\n';
                        treasure.pop_back();
                    }
                    weightLeft = tempWeightLeft + t.weight; //+ t.weight becasue it gets subtracted below
                    curValue -= removedVal;
                    break;
                }
            }
            if(i < 0){
                return false;
            }
        }
        treasure.push_back(t);
        weightLeft -= t.weight;
        curValue += t.value;
        return true;
    }
};

int main(){
    const char *CAVE_FILE = "./cave.csv";
    const size_t TREASURE_COUNT = 1000;
    const int MAX_BAG_WEIGHT = 4000;
    std::ifstream caveStream {CAVE_FILE};

    if(!caveStream.is_open()){
        std::cout << "ERROR: Couldn't open cave file\n";
        return 1;
    }

    std::vector<Treasure> treasures;
    treasures.reserve(TREASURE_COUNT);

    std::string line;
    int minWeight = MAX_BAG_WEIGHT;
    Treasure t;
    while(caveStream.good()){
        std::getline(caveStream, line);
        if(line.size() == 0){
            break;
        }

        t = Treasure::fromString(line);
        if(t.weight < minWeight){
            minWeight = t.weight;
        }

        treasures.push_back(t);
    }
    caveStream.close();

    std::sort(treasures.begin(), treasures.end()); //N * log(N)

    Bag bag {MAX_BAG_WEIGHT};
    std::cout << "Looting...\n";
    for(const Treasure &t : treasures){ //N
        if(bag.add(t)){
            std::cout << "+ Took " << t.name << '\n';//" (S: " << t.score << " W: " << bag.weightLeft << " V: " << bag.curValue << ")\n";
            if(bag.weightLeft < minWeight){
                break; 
            }
        }
    }
    std::cout << "Weight left in bag: " << bag.weightLeft << '\n';
    std::cout << "Treasure Value: " << bag.curValue << '\n';
    return 0;
}