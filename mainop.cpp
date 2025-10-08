#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

struct Treasure{
    std::string name;
    int weight;
    float value;
    float score; //Value per weight

    Treasure() : name(""), weight(-1), value(-1), score(-1) {}
    Treasure(const std::string &name, int weight, float value) : name(name), weight(weight), value(value) {
        score = value;
        score = score / weight;
    }
    Treasure(const Treasure &other){
        name = other.name;
        weight = other.weight;
        value = other.value;
        score = other.score;
    }

    bool operator<(const Treasure &rhs){
        return score > rhs.score;
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
        float val = std::stof(str.substr(commaPos[0]+1, commaPos[1]));
        int weight = std::stoi(str.substr(commaPos[1]+1));
        return {name, weight, val};
    }
};

struct Bag{
    const int MAX_WEIGHT;
    int weightLeft;
    float value;
    std::vector<Treasure> treasure;

    Bag(int maxWeight) : MAX_WEIGHT(maxWeight), weightLeft(maxWeight), value(0){}
   
    void add(const std::vector<Treasure> &treasures){
        int i = 0;
        for(; i < treasures.size(); ++i){
            if(treasures[i].weight > weightLeft){
                break;
            }
            std::cout << "Took " << treasures[i].name << '\n';
            weightLeft -= treasures[i].weight;
            value += treasures[i].value;
        }

        treasure.reserve(i+1);
        std::copy(treasures.begin(), treasures.begin() + i, std::back_insert_iterator(treasure));

        if(i != treasures.size()){
            Treasure tPart {treasures[i].name + " Part", weightLeft, weightLeft * treasures[i].score};
            treasure.push_back(tPart);
            weightLeft = 0;
            value += tPart.value;
            std::cout << "Took " << tPart.name << '\n';
        }
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
    while(caveStream.good()){
        std::getline(caveStream, line);
        if(line.size() == 0){
            break;
        }
        treasures.push_back(Treasure::fromString(line));
    }
    caveStream.close();

    std::sort(treasures.begin(), treasures.end()); //N * log(N)

    Bag bag {MAX_BAG_WEIGHT};

    std::cout << "Looting...\n";
    bag.add(treasures);
    
    std::cout << "Weight left in bag: " << bag.weightLeft << '\n';
    std::cout << "Treasure Value: " << bag.value << '\n';
    return 0;
}