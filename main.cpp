#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <iterator>
#include <vector>
#include <unordered_map>
#include <string>
#include <map>


typedef std::map<std::string,std::string>LZWDictionary;
static LZWDictionary dictionaryCompress;
static LZWDictionary dictionaryDEcompress;



std::string CharTostring(char c){
    return {c};
}


std::string GetWordFromDict(const std::string &word,const LZWDictionary& asciiDict,const LZWDictionary& combineDict){
    if(asciiDict.find(word)!=asciiDict.end()){
        return asciiDict.at(word);
    }
    else if(combineDict.find(word)!= combineDict.end()){
        return combineDict.at(word);
    } else{
        return "";
    }
}

void InitDict(){
    for(uint16_t i = 0 ; i < 256;++i){
        std::string simple = CharTostring(static_cast<char>(i));
        std::string simple_with_null = CharTostring(static_cast<char>(i)) + CharTostring(static_cast<char>(0));
        dictionaryCompress[simple] = simple_with_null; //["A"] = "A\0"
        dictionaryDEcompress[simple_with_null] = simple; //["A\0"] = "A"
    }
}

///@param new_entry_str : the new entry to the dictionary
///@param dictionary : the dictionary where we add new items (dynamic change)
///@param key_a,key_b : key combination for making the unique keys
void DictAdd_keyValue(const std::string & new_entry_str,LZWDictionary& dictionary,uint32_t& key_a,uint32_t& key_b) {
    if (key_a >= 256) {
        key_a = 0;
        key_b += 1;
        if (key_b >= 256) {
            key_b = 1;
            dictionary.clear();
        }
    }
    dictionary[new_entry_str] = CharTostring(static_cast<char>(key_a)) + CharTostring(static_cast<char>(key_a));
    key_a += 1;
}

















void read_file(const std::string & file_name){
    std::ifstream infile{"../"+ file_name};
    std::string line;

    if(infile.is_open()){
        while (std::getline(infile,line)){
            std::cout<<line<<std::endl;
        }
    } else{
        std::cerr<<"Error in opening file";
    }
}

int main() {





  /*  std::string name {"tehran"};
    std::vector<int>asci_values{};
    int number_plus{256};
    for (size_t i = 0 ; i < name.length();i++){
        while (isascii(name[i])){
            name[i++];
            if(!isascii(name[i++])){
                asci_values.push_back(number_plus);
                number_plus++;
            }
        }
    }
*/
    /*std::unordered_map<std::string,int>dictionary;
    dictionary.insert(std::pair<std::string,int>("sepehr",23));
    for(auto pair : dictionary){
        std::cout<<pair.first<<pair.second;
    }

    for (size_t i : asci_values){
        std::cout<<i<<std::endl;
    }
*/


    /*for(size_t i = 0 ; i< name.size();i++){
        std::cout<<int(name[i]);
    }*/

   /*int count{0};
    int pos{0};
    for(size_t i = 0 ; i <= name.size() ; i++){
        std::string newString = name.substr(pos,count);
        count++;
        std::cout<<newString<<std::endl;
    }

*/




    return 0;
}


