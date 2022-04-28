#include "../includes/autoIndex.hpp"

autoIndex::autoIndex(){
    _isError = false;
}
autoIndex::~autoIndex(){}



void autoIndex::setAutoIndexBody(std::string uri, std::string pathName){
    std::ostringstream result;
    std::ofstream out("/Users/oel-yous/Desktop/webserv/includes/exemple.html");
    DIR *folder;
    struct dirent *entry;

    folder = opendir(pathName.c_str());
    if (!folder){
        _isError = true;
        if (errno == EACCES)
            _errorCode = 403;
        else if (errno == ENOENT)
            _errorCode = 404;
        return ;
    }
    result << "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><title>YDA</title></head><body><div class='page'><h1> Index of";
    result << uri <<  "/</h1>   <hr style='width:100%;text-align:left;margin-left:0'>";
    result << "<table><tr><th>Name</th><th>Last modified</th><th>Size</th></tr>";
    while ((entry=readdir(folder))){
        result << "<tr>";
        result << "<th> <a href='" << pathName + entry->d_name << "'>" << entry->d_name <<"</th>";
        result << "<th>" << lastTimeModified(pathName + "/" + entry->d_name) << "</th>";
        result << "<th>" << _fileSize << "</th>";
        result << "</tr>";
    }
    result << "<span class='title' ></span></div><style> table {border-collapse: collapse;width: 100%;}th, td {padding: 8px;text-align: left;border-bottom: 1px solid #ddd;}</style></body></html>";
    _body = result.str();
    out << _body;
    out.close();
    // set bodyfilename  _bodyName = /Users/oel-yous/Desktop/webserv/includes/exemple.html : in this case 
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