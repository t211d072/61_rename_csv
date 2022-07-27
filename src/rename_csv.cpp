//参考サイト：https://code-database.com/knowledges/122
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

using namespace std;
namespace fs = boost::filesystem;

//文字列のsplit機能
std::vector<std::string> split(std::string str, char del) {
    int first = 0;
    int last = str.find_first_of(del);
    std::vector<std::string> result;
    while (first < str.size()) {
        std::string subStr(str, first, last - first);
        result.push_back(subStr);
        first = last + 1;
        last = str.find_first_of(del, first);
        if (last == std::string::npos) {
            last = str.size();
        }
    }
    return result;
}

std::vector<std::vector<std::string> > csv2vector(std::string filename, int ignore_line_num = 0){
    //csvファイルの読み込み
    std::ifstream reading_file;
    reading_file.open(filename, std::ios::in);
    if(!reading_file){
        std::vector<std::vector<std::string> > data;
        return data;
    }
    std::string reading_line_buffer;
    //最初のignore_line_num行を空読みする
    for(int line = 0; line < ignore_line_num; line++){
        getline(reading_file, reading_line_buffer);
        if(reading_file.eof()) break;
    }

    //二次元のvectorを作成
    std::vector<std::vector<std::string> > data;
    while(std::getline(reading_file, reading_line_buffer)){
        if(reading_line_buffer.size() == 0) break;
        std::vector<std::string> temp_data;
        temp_data = split(reading_line_buffer, ',');
        data.push_back(temp_data);
    }
    return data;
}

//フルパスからファイル名を抽出する関数
inline string getFileName(const string &path)
{
    using namespace std;
    size_t pos1;

    pos1 = path.rfind('\\');
    if(pos1 != string::npos){
        return path.substr(pos1+1, path.size()-pos1-1);
    }

    pos1 = path.rfind('/');
    if(pos1 != string::npos){
        return path.substr(pos1+1, path.size()-pos1-1);
    }

    return path;
}

//ファイル名一覧をvectorに格納する関数
void getFiles(const fs::path &path, vector<string> &files)
{
    int count = 1;
    BOOST_FOREACH(const fs::path& p, std::make_pair(fs::recursive_directory_iterator(path), fs::recursive_directory_iterator())) {
        if (!fs::is_directory(p)) files.push_back(p.string());
        cout << count << " Files Loading ..." << endl;
        count++;
    }
}

int main(int argc, char *argv[]){

    const string SEARCH_FILENAME = "compare_extract_result.csv";
    const string NEGTIVE_DIRECTORY_NAME = "neg_extract";

    if(argc < 2){
        cout << "Error : Usage is... " << argv[0] << " [csvファイルが入っているディレクトリ]" << endl;
        cout << "[このプログラムの説明]" << endl;
        cout << "第一引数で指定したディレクトリ下（サブフォルダも含めて）にある" << SEARCH_FILENAME << "を検索しファイル名を変更し一つのディレクトリにまとめます" << endl;
        return -1;
    }

    /*** Enterキーが押されるまで待機 ***/
    cout << "[プログラムの説明]" << endl;
    cout << "第一引数で指定したディレクトリ下（サブフォルダも含めて）にある" << SEARCH_FILENAME << "を検索しファイル名を変更し一つのディレクトリにまとめます" << endl;
    cout << "第一引数で指定したディレクトリ下の" << NEGTIVE_DIRECTORY_NAME << "内にあるCSVファイルは別のフォルダ(result/" << NEGTIVE_DIRECTORY_NAME << ")にまとめます" << endl;
    cout << "[" << SEARCH_FILENAME << "]と[" << NEGTIVE_DIRECTORY_NAME << "]はソースコードから変更可能です" << endl;
    cout << "Enterキーを押すと処理を開始します" << endl;
    getchar();

    string path = argv[1];
    vector<string> filepath;
    vector<string> search_result_filepath;

    vector<vector<string>> output_csv;
    vector<vector<string>> output_csv_tmp;

    /*** 出力フォルダのクリーンアップ ***/
    fs::remove_all(fs::current_path()/"result");

    try{
        fs::create_directories("result/neg_extract");
    } catch(fs::filesystem_error& e){
    /*** ディレクトリが作成できなかった場合例外処理に入る ***/
        cerr << "Failed to create directory(" << e.what() << ")" << endl;
    }
    
    getFiles(argv[1], filepath);

    for(int i = 0; i < filepath.size(); i++){
        string filename = getFileName(filepath[i]);
        if(filename == SEARCH_FILENAME){
            search_result_filepath.push_back(filepath[i]);
        }
        cout << "Now Search File:" << i+1 << "/" << filepath.size() << endl;
    }

    for(int i = 0; i < search_result_filepath.size(); i++){
        fs::path tmp = search_result_filepath[i];
        fs::path p_parent_path = tmp.parent_path();
        string s_parent_path = p_parent_path.string();
        string parent_directory_name = getFileName(s_parent_path);

        fs::path p_grandparent_path = p_parent_path.parent_path();
        string s_grandparent_path = p_grandparent_path.string();
        string grandparent_directory_name = getFileName(s_grandparent_path);
        
        if(grandparent_directory_name == NEGTIVE_DIRECTORY_NAME){
            stringstream ss_parent_directory_name;
            ss_parent_directory_name << "result/neg_extract/neg_extract_" << parent_directory_name << ".csv";

            fs::copy_file(search_result_filepath[i], fs::current_path()/"result/neg_extract/compare_extract_result.csv");
            fs::rename(fs::current_path()/"result/neg_extract/compare_extract_result.csv", fs::current_path()/ss_parent_directory_name.str());
        }else{
            stringstream ss_parent_directory_name;
            ss_parent_directory_name << "result/" << parent_directory_name << ".csv";

            fs::copy_file(search_result_filepath[i], fs::current_path()/"result/compare_extract_result.csv");
            fs::rename("result/compare_extract_result.csv", fs::current_path()/ss_parent_directory_name.str());
        }
        cout << "Now Rename File:" << i+1 << "/" << search_result_filepath.size() << endl;
    }

    cout << "output done." << endl;
    
    return 0;

}