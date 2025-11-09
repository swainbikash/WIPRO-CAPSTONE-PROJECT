#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iomanip>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <limits>
#include <ctime>

using namespace std;

// ---------------------- Function Prototypes ----------------------
void listFiles(const string &path);
void createFile(const string &filename);
void deleteFile(const string &filename);
void copyFile(const string &source, const string &destination);
void moveFile(const string &oldName, const string &newName);
void searchFiles(const string &path, const string &keyword);
void showPermissions(const string &filename);
void changePermissions(const string &filename);
void writeToFile(const string &filename);
void openFile(const string &filename);
void createFolder(const string &foldername);
void deleteDirectory(const string &path);
void showDetails(const string &filename);

// ---------------------- List files in directory ----------------------
void listFiles(const string &path) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path.c_str())) == NULL) {
        perror("Unable to open directory");
        return;
    }

    cout << "\n Files in: " << path << "\n";
    cout << "-----------------------------------\n";

    while ((entry = readdir(dir)) != NULL) {
        string name = entry->d_name;
        if (name == "." || name == "..") continue;

        string fullPath = path + "/" + name;
        struct stat st;
        stat(fullPath.c_str(), &st);

        cout << setw(25) << left << name;
        cout << (S_ISDIR(st.st_mode) ? " [DIR]" : " [FILE]") << endl;
    }

    closedir(dir);
}

// ---------------------- Create file ----------------------
void createFile(const string &filename) {
    ofstream file(filename);
    if (!file) {
        perror("Error creating file");
        return;
    }
    cout << " File created: " << filename << endl;
}

// ---------------------- Create directory ----------------------
void createFolder(const string &foldername) {
#ifdef _WIN32
    if (mkdir(foldername.c_str()) != 0)
#else
    if (mkdir(foldername.c_str(), 0777) != 0)
#endif
        perror("Error creating directory");
    else
        cout << " Directory created: " << foldername << endl;
}

// ---------------------- Delete directory recursively ----------------------
void deleteDirectory(const string &path) {
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        string name = entry->d_name;
        if (name == "." || name == "..") continue;
        string fullPath = path + "/" + name;

        struct stat st;
        stat(fullPath.c_str(), &st);

        if (S_ISDIR(st.st_mode))
            deleteDirectory(fullPath);
        else
            remove(fullPath.c_str());
    }
    closedir(dir);
    rmdir(path.c_str());
    cout << " Directory deleted: " << path << endl;
}

// ---------------------- Delete file ----------------------
void deleteFile(const string &filename) {
    if (remove(filename.c_str()) != 0)
        perror("Error deleting file");
    else
        cout << " File deleted: " << filename << endl;
}

// ---------------------- Copy file ----------------------
void copyFile(const string &source, const string &destination) {
    ifstream src(source, ios::binary);
    ofstream dest(destination, ios::binary);
    if (!src || !dest) {
        perror("Error copying file");
        return;
    }
    dest << src.rdbuf();
    cout << " File copied: " << source << " → " << destination << endl;
}

// ---------------------- Move or Rename file ----------------------
void moveFile(const string &oldName, const string &newName) {
    if (rename(oldName.c_str(), newName.c_str()) != 0)
        perror("Error moving file");
    else
        cout << " File moved: " << oldName << " → " << newName << endl;
}

// ---------------------- Search files recursively ----------------------
void searchFiles(const string &path, const string &keyword) {
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(path.c_str())) == NULL) return;

    while ((entry = readdir(dir)) != NULL) {
        string name = entry->d_name;
        if (name == "." || name == "..") continue;

        string fullPath = path + "/" + name;
        struct stat st;
        stat(fullPath.c_str(), &st);

        if (name.find(keyword) != string::npos)
            cout << " Found: " << fullPath << endl;

        if (S_ISDIR(st.st_mode))
            searchFiles(fullPath, keyword);
    }
    closedir(dir);
}

// ---------------------- Show file permissions ----------------------
void showPermissions(const string &filename) {
    struct stat fileStat;
    if (stat(filename.c_str(), &fileStat) < 0) {
        perror("Unable to read file permissions");
        return;
    }

    cout << "\nPermissions for: " << filename << endl;
    cout << "-----------------------------------\n";
    cout << ((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    cout << ((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    cout << ((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    cout << " ";
    cout << ((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    cout << ((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    cout << ((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    cout << " ";
    cout << ((fileStat.st_mode & S_IROTH) ? "r" : "-");
    cout << ((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    cout << ((fileStat.st_mode & S_IXOTH) ? "x" : "-");
    cout << endl;
}

// ---------------------- Change file permissions ----------------------
void changePermissions(const string &filename) {
#ifdef _WIN32
    cout << "Changing file permissions is not supported on Windows.\n";
#else
    int perms;
    cout << "Enter new permission in numeric form (e.g., 755): ";
    cin >> perms;

    mode_t mode = 0;
    int owner = perms / 100;
    int group = (perms / 10) % 10;
    int others = perms % 10;

    mode |= (owner & 4 ? S_IRUSR : 0);
    mode |= (owner & 2 ? S_IWUSR : 0);
    mode |= (owner & 1 ? S_IXUSR : 0);
    mode |= (group & 4 ? S_IRGRP : 0);
    mode |= (group & 2 ? S_IWGRP : 0);
    mode |= (group & 1 ? S_IXGRP : 0);
    mode |= (others & 4 ? S_IROTH : 0);
    mode |= (others & 2 ? S_IWOTH : 0);
    mode |= (others & 1 ? S_IXOTH : 0);

    if (chmod(filename.c_str(), mode) != 0)
        perror("Error changing permissions");
    else
        cout << " Permissions changed successfully.\n";
#endif
}

// ---------------------- Show file details ----------------------
void showDetails(const string &filename) {
    struct stat st;
    if (stat(filename.c_str(), &st) != 0) {
        perror("Error");
        return;
    }

    cout << "\nDetails of " << filename << ":\n";
    cout << "-----------------------------------\n";
    cout << "Size: " << st.st_size << " bytes" << endl;
    cout << "Last Modified: " << ctime(&st.st_mtime);
}

// ---------------------- Write data to file ----------------------
void writeToFile(const string &filename) {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    ofstream file(filename, ios::app);
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    cout << "Enter text to write: ";
    string data;
    getline(cin, data);

    file << data << endl;
    cout << " Data written successfully.\n";
}

// ---------------------- Open and read file ----------------------
void openFile(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        perror("Error opening file");
        return;
    }

    cout << "\n Contents of " << filename << ":\n";
    cout << "-----------------------------------\n";

    string line;
    while (getline(file, line)) {
        cout << line << endl;
    }

    cout << "-----------------------------------\n";
    file.close();
}

// ---------------------- Main Menu ----------------------
int main() {
    char cwd[1024];

    while (true) {
        system("clear"); // Clears screen each time
        getcwd(cwd, sizeof(cwd));
        string currentPath = cwd;

        listFiles(currentPath);

        cout << "\nOptions:\n";
        cout << "A. Enter directory\n";
        cout << "B. Go back\n";
        cout << "C. Create file\n";
        cout << "D. Delete file\n";
        cout << "E. Copy file\n";
        cout << "F. Move file/Rename file\n";
        cout << "G. Search files\n";
        cout << "H. Show file permissions\n";
        cout << "I. Change file permissions\n";
        cout << "J. Open file\n";
        cout << "K. Write to file\n";
        cout << "L. Create directory\n";
        cout << "M. Delete directory\n";
        cout << "N. Show file details\n";
        cout << "O. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        if (choice == 'A') {
            string dirName; cout << "Enter directory name: "; cin >> dirName;
            if (chdir(dirName.c_str()) != 0) perror("Failed to enter directory");
        } else if (choice == 'B') {
            if (chdir("..") != 0) perror("Failed to go back");
        } else if (choice == 'C') {
            string filename; cout << "Enter new file name: "; cin >> filename;
            createFile(filename);
        } else if (choice == 'D') {
            string filename; cout << "Enter file name to delete: "; cin >> filename;
            deleteFile(filename);
        } else if (choice == 'E') {
            string src, dest; cout << "Enter source: "; cin >> src;
            cout << "Enter destination: "; cin >> dest;
            copyFile(src, dest);
        } else if (choice == 'F') {
            string oldName, newName;
            cout << "Enter old name: "; cin >> oldName;
            cout << "Enter new name: "; cin >> newName;
            moveFile(oldName, newName);
        } else if (choice == 'G') {
            string keyword; cout << "Enter search keyword: "; cin >> keyword;
            searchFiles(currentPath, keyword);
        } else if (choice == 'H') {
            string filename; cout << "Enter file name: "; cin >> filename;
            showPermissions(filename);
        } else if (choice == 'I') {
            string filename; cout << "Enter file name: "; cin >> filename;
            changePermissions(filename);
        } else if (choice == 'J') {
            string filename; cout << "Enter file name to open: "; cin >> filename;
            openFile(filename);
        } else if (choice == 'K') {
            string filename; cout << "Enter file name to write: "; cin >> filename;
            writeToFile(filename);
        } else if (choice == 'L') {
            string foldername; cout << "Enter directory name to create: "; cin >> foldername;
            createFolder(foldername);
        } else if (choice == 'M') {
            string foldername; cout << "Enter directory name to delete: "; cin >> foldername;
            deleteDirectory(foldername);
        } else if (choice == 'N') {
            string filename; cout << "Enter file name to view details: "; cin >> filename;
            showDetails(filename);
        } else if (choice == 'O') {
            cout << "Exiting File Explorer...\n";
            break;
        } else cout << "Invalid choice.\n";

        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }

    return 0;
}
