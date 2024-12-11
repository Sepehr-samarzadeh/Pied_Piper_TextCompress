#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <vector>
#include <string>
#include <map>


enum LZW_Status{
    Success,
    Error_Empty_Dictionary,
    Error_Uncompressed_String
};

typedef std::map<std::string,std::string>LZWDictionary;
typedef std::tuple<std::string,LZW_Status>LZW_Output;
static LZWDictionary dictionaryCompress;
static LZWDictionary dictionaryDEcompress;



std::string CharTostring(char c){
    return {c};
}


std::string GetWordCodeFromDictionaries(const std::string &word, const LZWDictionary& asciiDict, const LZWDictionary& combineDict){
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
    dictionary[new_entry_str] = CharTostring(static_cast<char>(key_a)) + CharTostring(static_cast<char>(key_b));
    key_a += 1;
}

void DictAdd_KeyValue_Second(const std::string & str,LZWDictionary&dict,uint32_t&a,uint32_t&b){
    if(a>=256){
        a = 0;
        b +=1;
        if(b >= 256){
            b = 1;
            dict.clear();
        }
    }
    dict[CharTostring(static_cast<char>(a)) + CharTostring(static_cast<char>(b))] = str;
    a+=1;
}


LZW_Output Compress(const std::string & text){
    size_t len = text.size();
    if(len <=1){
        return  {"u"+text,Success};
    }

    LZWDictionary dict;
    uint32_t a = 0;
    uint32_t b = 1;
    std::string result = "c";
    size_t result_len = 1;
    std::string word = "";
    for(size_t i = 0 ; i < len ; ++i){
        char c = text[i];
        std::string wc = word + CharTostring(c);
        if(dictionaryCompress.find(wc) == dictionaryCompress.end() && dict.find(wc) == dict.end()){
            std::string write = GetWordCodeFromDictionaries(word, dictionaryCompress, dict);

            if(write.empty()){
                return {nullptr,Error_Empty_Dictionary};
            }

            result += write;
            result_len += write.size();

            if(len <= result_len){
                return {"u"+text,Success}; //TODO:signal user that its not worthy
            }
            DictAdd_keyValue(wc,dict,a,b);
            word = c;
        } else{
            word = wc;
        }
    }
    std::string finalWrite = GetWordCodeFromDictionaries(word, dictionaryCompress, dict);
    if(finalWrite.empty()){
        return {nullptr,Error_Empty_Dictionary};
    }

    result += finalWrite;
    result_len += finalWrite.size();

    if(len <= result_len){
        return {"u" + text,Success};
    }
    return {result,Success};
}

LZW_Output Decompress(const std::string & text){
    if(text.empty()){
        return {nullptr,Error_Uncompressed_String};
    }
    char check = text[0];
    if(check == 'u'){
        return {text.substr(1),Success};
    }
    else if (check != 'c'){
        return {nullptr,Error_Uncompressed_String};
    }
    std::string text_data = text.substr(1);
    size_t len = text_data.size();
    if(len < 2){
        return {nullptr,Error_Uncompressed_String};
    }
    LZWDictionary dict;
    uint32_t a = 0;
    uint32_t b = 1;
    std::string result;
    std::string last = text_data.substr(0,2);
    result = GetWordCodeFromDictionaries(last, dictionaryDEcompress, dict);
    for(size_t i = 2; i < len ; i+=2){
        std::string code = text_data.substr(i,2);
        std::string lastStr = GetWordCodeFromDictionaries(last, dictionaryDEcompress, dict);
        if(lastStr.empty()){
            return {nullptr,Error_Empty_Dictionary};
        }
        std::string toAdd = GetWordCodeFromDictionaries(code, dictionaryDEcompress, dict);
        if(!toAdd.empty()){
            result+=toAdd;
            DictAdd_KeyValue_Second(lastStr + toAdd[0],dict,a,b);
        } else{
            std::string tmp = lastStr + lastStr[0];
            result += tmp;
            DictAdd_KeyValue_Second(tmp,dict,a,b);
        }
        last = code;
    }
    return {result,Success};
}


std::string read_file(const std::string &file_name) {
    std::ifstream infile{"../" + file_name};
    std::string line;
    std::string content;

    if (infile.is_open()) {
        while (std::getline(infile, line)) {
            content += line + "\n"; // Combine lines with newline
        }
    } else {
        std::cerr << "Error in opening file" << std::endl;
    }

    return content; // Return the file content as a string
}


void PrintDictionaries(LZWDictionary & dictionary) {
    std::cout << "Dictionary Compress:" << std::endl;
    for(const auto & it : dictionary){
        std::cout<< it.first<<" "<<it.second<<std::endl;
    }
}




int main() {
    InitDict();

   /* std::map<std::string,std::string>theMap;
    theMap.insert({"sep","\0"});
    for(auto it = theMap.cbegin();it != theMap.cend(); ++it){
        std::cout <<it->first<<" "<<it->second<<std::endl;
    }*/

    //PrintDictionaries(dictionaryCompress);

   std::string file_name = "Tlor.txt";
    std::string content = read_file(file_name);

    if (content.empty()) {
        std::cerr << "file is empty or cannot be read!" << std::endl;
        return 1;
    }


    std::cout << "Original Size: " << content.size() << " bytes\n";

    auto [compressed, compress_status] = Compress(content);
    if (compress_status != Success) {
        std::cerr << "Compression failed with status: " << compress_status << std::endl;
        return 1;
    }


    std::cout << "Compressed Size: " << compressed.size() << " bytes\n";


    auto [decompressed, decompress_status] = Decompress(compressed);
    if (decompress_status != Success) {
        std::cerr << "Decompression failed with status: " << decompress_status << std::endl;
        return 1;
    }



    if (decompressed == content) {
        std::cout << "passed! Decompressed content matches the original." << std::endl;
    } else {
        std::cerr << "failed! Decompressed content does not match the original." << std::endl;
    }

    return 0;
}


































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


