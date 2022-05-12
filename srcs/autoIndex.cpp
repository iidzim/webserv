#include "../includes/autoIndex.hpp"

autoIndex::autoIndex(){
    _isError = false;
}
autoIndex::~autoIndex(){}



void autoIndex::setAutoIndexBody(DIR *folder, std::string path, std::string root, std::string location, std::string CurrPath){
    std::ostringstream result;
    // char cwd[256];
    // std::string currPath(getcwd(cwd, sizeof(cwd)));
    std::string autoIndexPath = CurrPath + "/var/www/html/autoindex.html";

    std::ofstream out(autoIndexPath);
    struct dirent *entry;
    result << "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><title>YDA</title></head><body><div class='page'><h1> Index of";
    result << path <<  "</h1>   <hr style='width:100%;text-align:left;margin-left:0'>";
    result << "<table><tr><th>Name</th><th>Last modified</th><th>Size</th></tr>";
    while ((entry=readdir(folder))){
        std::string href;
        href = location + path.substr(root.length());
        result << "<tr>";
        result << "<th> <a href='" << href;
        if (path[path.length()-1] != '/')
            result << "/";
        result <<  entry->d_name << "'>" << entry->d_name <<"</th>";
        result << "<th>" << lastTimeModified(path + "/" + entry->d_name) << "</th>";
        result << "<th>" << _fileSize << "</th>";
        result << "</tr>";
    }
    result << "<span class='title' ></span></div><style> table {border-collapse: collapse;width: 100%;}th, td {padding: 8px;text-align: left;border-bottom: 1px solid #ddd;}</style></body></html>";
    _body = result.str();
    out << _body;
    out.close();

    _bodyName = autoIndexPath;
}

std::string autoIndex::lastTimeModified(std::string fileName){
    struct stat attrib;
    stat(fileName.c_str(), &attrib);
    char date[20];

    strftime(date, 20, "%d-%m-%y %H:%M", localtime(&(attrib.st_mtime)));
    std::ostringstream size;
    size << attrib.st_size;
    _fileSize = size.str();
    std::ostringstream result;
    result  << date;
    return result.str();
}

bool autoIndex::isError(){
    return _isError;
}

int autoIndex::getErrorCode(){
    return _errorCode;
}

std::string autoIndex::getBodyName(){
    return _bodyName;
}