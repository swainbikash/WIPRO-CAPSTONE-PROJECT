#include <bits/stdc++.h>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

string cwd = fs::current_path().string();

void print_help() {
    cout << "Simple File Explorer - commands:\\n";
    cout << "  ls [path]                List files in directory\\n";
    cout << "  cd <path>                Change directory\\n";
    cout << "  pwd                      Print current directory\\n";
    cout << "  cat <file>               Print file contents\\n";
    cout << "  view <file>              Same as cat\\n";
    cout << "  rm <file|dir>            Remove file or empty directory\\n";
    cout << "  mkdir <dir>              Create directory\\n";
    cout << "  touch <file>             Create empty file or update timestamp\\n";
    cout << "  cp <src> <dst>           Copy file (or directory recursively)\\n";
    cout << "  mv <src> <dst>           Move/rename\\n";
    cout << "  search <pattern>         Search filenames under current directory (simple substring)\\n";
    cout << "  info <path>              Show file info (size, perms, type)\\n";
    cout << "  chmod <mode> <path>      Change permission (octal, e.g. 755)\\n";
    cout << "  help                     Show this help\\n";
    cout << "  exit                     Exit program\\n";
}

void cmd_ls(const vector<string>& args) {
    fs::path p = args.size()>=2 ? fs::path(args[1]) : fs::path(cwd);
    try {
        if(!fs::exists(p)) { cout << "Path not found: " << p << "\\n"; return; }
        for(auto &entry : fs::directory_iterator(p)) {
            auto name = entry.path().filename().string();
            auto perms = entry.is_directory() ? "d" : "-";
            cout << perms << " " << name << "\\n";
        }
    } catch(exception &e) {
        cout << "ls error: " << e.what() << "\\n";
    }
}

void cmd_cd(const vector<string>& args) {
    if(args.size()<2) { cout << "Usage: cd <path>\\n"; return; }
    fs::path p = args[1];
    try {
        fs::current_path(p);
        cwd = fs::current_path().string();
    } catch(exception &e) {
        cout << "cd error: " << e.what() << "\\n";
    }
}

void cmd_pwd() {
    cout << cwd << "\\n";
}

void cmd_cat(const vector<string>& args) {
    if(args.size()<2) { cout << "Usage: cat <file>\\n"; return; }
    fs::path p = args[1];
    try {
        if(!fs::exists(p)) { cout << "File not found\\n"; return; }
        if(fs::is_directory(p)) { cout << "Is a directory\\n"; return; }
        ifstream in(p, ios::binary);
        cout << in.rdbuf();
    } catch(exception &e) {
        cout << "cat error: " << e.what() << "\\n";
    }
}

void cmd_rm(const vector<string>& args) {
    if(args.size()<2) { cout << "Usage: rm <path>\\n"; return; }
    fs::path p = args[1];
    try {
        if(!fs::exists(p)) { cout << "Not found\\n"; return; }
        if(fs::is_directory(p)) {
            // remove directory only if empty
            if(fs::is_empty(p)) {
                fs::remove(p);
                cout << "Directory removed\\n";
            } else {
                cout << "Directory not empty. Use rm -r from shell.\\n";
            }
        } else {
            fs::remove(p);
            cout << "File removed\\n";
        }
    } catch(exception &e) {
        cout << "rm error: " << e.what() << "\\n";
    }
}

void cmd_mkdir(const vector<string>& args) {
    if(args.size()<2) { cout << "Usage: mkdir <dir>\\n"; return; }
    fs::path p = args[1];
    try {
        fs::create_directories(p);
        cout << "Directory created\\n";
    } catch(exception &e) {
        cout << "mkdir error: " << e.what() << "\\n";
    }
}

void cmd_touch(const vector<string>& args) {
    if(args.size()<2) { cout << "Usage: touch <file>\\n"; return; }
    fs::path p = args[1];
    try {
        ofstream out(p, ios::app);
        out.close();
        cout << "Touched " << p << "\\n";
    } catch(exception &e) {
        cout << "touch error: " << e.what() << "\\n";
    }
}

void copy_recursive(const fs::path &src, const fs::path &dst) {
    if(fs::is_directory(src)) {
        fs::create_directories(dst);
        for(const auto &entry: fs::directory_iterator(src)) {
            copy_recursive(entry.path(), dst / entry.path().filename());
        }
    } else {
        fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
    }
}

void cmd_cp(const vector<string>& args) {
    if(args.size()<3) { cout << "Usage: cp <src> <dst>\\n"; return; }
    fs::path src = args[1], dst = args[2];
    try {
        if(!fs::exists(src)) { cout << "Source not found\\n"; return; }
        if(fs::is_directory(src)) {
            copy_recursive(src, dst);
        } else {
            if(fs::is_directory(dst)) dst /= src.filename();
            fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
        }
        cout << "Copied\\n";
    } catch(exception &e) {
        cout << "cp error: " << e.what() << "\\n";
    }
}

void cmd_mv(const vector<string>& args) {
    if(args.size()<3) { cout << "Usage: mv <src> <dst>\\n"; return; }
    fs::path src = args[1], dst = args[2];
    try {
        fs::rename(src, dst);
        cout << "Moved/Renamed\\n";
    } catch(exception &e) {
        cout << "mv error: " << e.what() << "\\n";
    }
}

void cmd_search(const vector<string>& args) {
    if(args.size()<2) { cout << "Usage: search <pattern>\\n"; return; }
    string pat = args[1];
    try {
        for(auto &p: fs::recursive_directory_iterator(cwd)) {
            if(p.path().filename().string().find(pat) != string::npos) {
                cout << p.path().string() << "\\n";
            }
        }
    } catch(exception &e) {
        cout << "search error: " << e.what() << "\\n";
    }
}

void cmd_info(const vector<string>& args) {
    if(args.size()<2) { cout << "Usage: info <path>\\n"; return; }
    fs::path p = args[1];
    try {
        if(!fs::exists(p)) { cout << "Not found\\n"; return; }
        cout << "Path: " << fs::absolute(p) << "\\n";
        cout << "Size: " << (fs::is_regular_file(p) ? to_string(fs::file_size(p)) : string("N/A")) << "\\n";
        cout << "Type: " << (fs::is_directory(p) ? "directory" : "file") << "\\n";
        cout << "Permissions (rwx): ";
        auto perms = fs::status(p).permissions();
        cout << ((perms & fs::perms::owner_read) != fs::perms::none ? "r" : "-");
        cout << ((perms & fs::perms::owner_write) != fs::perms::none ? "w" : "-");
        cout << ((perms & fs::perms::owner_exec) != fs::perms::none ? "x" : "-") << "\\n";
    } catch(exception &e) {
        cout << "info error: " << e.what() << "\\n";
    }
}

void cmd_chmod(const vector<string>& args) {
    if(args.size()<3) { cout << "Usage: chmod <octal> <path>\\n"; return; }
    string mode = args[1];
    fs::path p = args[2];
    try {
        int val = stoi(mode, nullptr, 8);
        fs::permissions(p, static_cast<fs::perms>(val), fs::perm_options::replace);
        cout << "chmod applied (note: limited portability)\\n";
    } catch(exception &e) {
        cout << "chmod error: " << e.what() << "\\n";
    }
}

vector<string> split_cmd(const string &line) {
    vector<string> out;
    string token;
    istringstream iss(line);
    while(iss >> token) out.push_back(token);
    return out;
}

int main(){
    cout << "Simple File Explorer (console) - type 'help' for commands\\n";
    cwd = fs::current_path().string();
    string line;
    while(true){
        cout << fs::current_path().string() << " $ ";
        if(!getline(cin, line)) break;
        auto args = split_cmd(line);
        if(args.empty()) continue;
        string cmd = args[0];
        if(cmd=="help") print_help();
        else if(cmd=="ls") cmd_ls(args);
        else if(cmd=="cd") cmd_cd(args);
        else if(cmd=="pwd") cmd_pwd();
        else if(cmd=="cat" || cmd=="view") cmd_cat(args);
        else if(cmd=="rm") cmd_rm(args);
        else if(cmd=="mkdir") cmd_mkdir(args);
        else if(cmd=="touch") cmd_touch(args);
        else if(cmd=="cp") cmd_cp(args);
        else if(cmd=="mv") cmd_mv(args);
        else if(cmd=="search") cmd_search(args);
        else if(cmd=="info") cmd_info(args);
        else if(cmd=="chmod") cmd_chmod(args);
        else if(cmd=="exit" || cmd=="quit") break;
        else cout << "Unknown command. Type 'help'.\\n";
    }
    cout << "Goodbye\\n";
    return 0;
}