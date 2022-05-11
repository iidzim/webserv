#include "../includes/mimeTypes.hpp"

mimeTypes::mimeTypes(){
    setTypes();
}
mimeTypes::~mimeTypes(){}

std::string mimeTypes::getType(std::string &filename){
    size_t position = filename.find(".");
    if (position != std::string::npos){
        std::map<std::string, std::string>::iterator it = _types.find(filename.substr(position));
        if (it != _types.end())
            return it->second;
    }
    return ("text/html");
}

std::string mimeTypes::getExtention(std::string &type){
    for (std::map<std::string, std::string>::iterator it = _types.begin(); it != _types.end(); it++){
        if (it->second == type)
            return (it->first);
    }
    return (".html");

}
void mimeTypes::setTypes(){
    _types[".aac"] = "audio/aac";
    _types[".abw"] = "application/x-abiword";
    _types[".arc"] = "application/x-freearc";
    _types[".avi"] = "video/x-msvideo";
    _types[".azw"] = "application/vnd.amazon.ebook";
    _types[".bin"] = "application/octet-stream";
    _types[".bmp"] = "image/bmp";
    _types[".bz"] = "application/x-bzip";
    _types[".bz2"] = "application/x-bzip2";
    _types[".csh"] = "application/x-csh";
    _types[".css"] = "text/css";
    _types[".csv"] = "text/csv";
    _types[".doc"] = "application/msword";
    _types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    _types[".eot"] = "application/vnd.ms-fontobject";
    _types[".epub"] = "application/epub+zip";
    _types[".gz"] = "application/gzip";
    _types[".gif"] = "image/gif";
    _types[".htm"] = "text/html";
    _types[".html"] = "text/html";
    _types[".html"] = "text/html; charset=UTF-8";
    _types[".ico"] = "image/vnd.microsoft.icon";
    _types[".ics"] = "text/calendar";
    _types[".jar"] = "application/java-archive";
    _types[".jpeg"] = "image/jpeg";
    _types[".jpg"] = "image/jpeg";
    _types[".js"] = "text/javascript";
    _types[".json"] = "application/json";
    _types[".jsonld"] = "application/ld+json";
    _types[".mid"] = "audio/midi audio/x-midi";
    _types[".midi"] = "audio/midi audio/x-midi";
    _types[".mjs"] = "text/javascript";
    _types[".mp3"] = "audio/mpeg";
    _types[".mpeg"] = "video/mpeg";
    _types[".mpkg"] = "application/vnd.apple.installer+xml";
    _types[".odp"] = "application/vnd.oasis.opendocument.presentation";
    _types[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    _types[".odt"] = "application/vnd.oasis.opendocument.text";
    _types[".oga"] = "audio/ogg";
    _types[".ogv"] = "video/ogg";
    _types[".ogx"] = "application/ogg";
    _types[".opus"] = "audio/opus";
    _types[".otf"] = "font/otf";
    _types[".png"] = "image/png";
    _types[".PNG"] = "image/png";
    _types[".pdf"] = "application/pdf";
    _types[".php"] = "application/x-httpd-php";
    _types[".ppt"] = "application/vnd.ms-powerpoint";
    _types[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    _types[".rar"] = "application/vnd.rar";
    _types[".rtf"] = "application/rtf";
    _types[".sh"] = "application/x-sh";
    _types[".svg"] = "image/svg+xml";
    _types[".swf"] = "application/x-shockwave-flash";
    _types[".tar"] = "application/x-tar";
    _types[".tif"] = "image/tiff";
    _types[".tiff"] = "image/tiff";
    _types[".ts"] = "video/mp2t";
    _types[".ttf"] = "font/ttf";
    _types[".txt"] = "text/plain";
    _types[".vsd"] = "application/vnd.visio";
    _types[".wav"] = "audio/wav";
    _types[".weba"] = "audio/webm";
    _types[".webm"] = "video/webm";
    _types[".webp"] = "image/webp";
    _types[".woff"] = "font/woff";
    _types[".woff2"] = "font/woff2";
    _types[".xhtml"] = "application/xhtml+xml";
    _types[".xls"] = "application/vnd.ms-excel";
    _types[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    _types[".xml"] = "application/xml";
    _types[".xul"] = "application/vnd.mozilla.xul+xml";
    _types[".zip"] = "application/zip";
    _types[".3gp"] = "video/3gpp";
    _types[".3g2"] = "video/3gpp2";
    _types[".7z"] = "application/x-7z-compressed";
    

    }