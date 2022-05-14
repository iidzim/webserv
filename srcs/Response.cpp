
#include "../includes/Response.hpp"

Response::Response(request req, std::string pwd): _headers(""), _body(""), _reqInfo(req.getRequest()), _cursor(0){
	_CurrDirecory = pwd;
	_iskeepAlive = true;
	_autoIndex = false;
	_location = "";
	_path = "";
	_root = "";
}

Response::Response(): _headers(""), _body(""), _cursor(0) {
	_iskeepAlive = true;
	_autoIndex = false;
	_location = "";
	_path = "";
	_root = "";
}

Response::Response(request req, serverInfo s, std::string pwd):  _headers(""), _body(""), _reqInfo(req.getRequest()), _servInfo(s), _cursor(0) {
	_CurrDirecory = pwd;
	_iskeepAlive = true;
	_autoIndex = false;
	_location = "";
	_path = "";
	_root = "";
}

Response::Response(const Response & src){
	*this = src;
}

Response & Response::operator=(const Response & obj){
	_headers = obj._headers;
	_body = obj._body;
	_path = obj._path;
	_location = obj._location;
	_root = obj._root;
	_index = obj._index;
	_iskeepAlive = obj._iskeepAlive;
	_reqInfo = obj._reqInfo;
	_mime = obj._mime;
	_servInfo = obj._servInfo;
	_cursor = obj._cursor;
	_autoIndex = obj._autoIndex;
	_CurrDirecory = obj._CurrDirecory;
	return *this;

}

Response::~Response(){}

std::string Response::setErrorsHeaders(std::string ErrorMsg, std::string cLentgh){
	std::ostringstream headers;

	headers << "HTTP/1.1 "<<  ErrorMsg << "\r\nContent-type: text/html\r\nContent-length: ";
	headers << cLentgh;
	headers << Connection(0);
	return headers.str();
}

bool Response::isFileExist(std::string pathName){
	int fd = open(pathName.c_str(), O_RDONLY);
	if (fd < 0)
		return false;
	close(fd);
	return true;
}



void Response::errorsResponse(int statCode){
	int ret;

	_reqInfo.statusCode = statCode;
	// std::cout <<"errorpage == "std::cout <<"errorpage == ";
	if (_servInfo.errorPage.size() == 0)
		_body = _CurrDirecory + "/error_pages/" + toString(statCode) + ".html";
	else{
		// std::cout <<"errorpage == " << "_servInfo.errorPage.find(statCode)->second" << std::endl;
		if (_servInfo.errorPage.find(statCode) != _servInfo.errorPage.end()){
			int fd = open(_servInfo.errorPage.find(statCode)->second.c_str(), O_RDONLY);
			if (fd < 0)
				_body = _CurrDirecory + "/error_pages/" + toString(statCode) + ".html";
			else
				_body = _servInfo.errorPage.find(statCode)->second;
			close(fd);
		}
		else
			_body = _CurrDirecory + "/error_pages/" + toString(statCode) + ".html";
	}
	if (isFileExist(_body) == false)
		_body = ""; // 
	ret = fileSize(_body);
	if (_reqInfo.statusCode == 400)
		_headers = setErrorsHeaders("400 Bad Request", toString(ret));
	else if (_reqInfo.statusCode == 403)
		_headers = setErrorsHeaders("403 Forbidden", toString(ret));
	else if (_reqInfo.statusCode == 404)
		_headers = setErrorsHeaders("404 Not Found", toString(ret));
	else if (_reqInfo.statusCode == 405)
		_headers = setErrorsHeaders("405 Not Method Not Allowed", toString(ret));
	else if (_reqInfo.statusCode == 411)
		_headers = setErrorsHeaders("411 Length Required", toString(ret));
	else if (_reqInfo.statusCode == 413)
		_headers = setErrorsHeaders("413 Payload Too Large", toString(ret));
	else if (_reqInfo.statusCode == 500)
		_headers = setErrorsHeaders("500 Internal Server Error", toString(ret));
	else if (_reqInfo.statusCode == 501)
		_headers = setErrorsHeaders("501 Not Implemented", toString(ret));
	else if (_reqInfo.statusCode == 502)
		_headers = setErrorsHeaders("502 Bad Gateaway", toString(ret));
	else if (_reqInfo.statusCode == 505)
		_headers = setErrorsHeaders("505 HTTP Version Not Supported", toString(ret));
}

std::string Response::Connection(int flag){
	std::string ret = "\r\nConnection: ";
	std::map<std::string, std::string>::iterator it = _reqInfo.headers.find("connection");
	if( it != _reqInfo.headers.end()){
		std::string connect = it->second;
		if (connect == "keep-alive") {
			connect = "Keep-Alive";
		}
		else {
			connect = "Closed";
			_iskeepAlive = false;
		}
		ret += connect;
	}
	else
		ret += "Keep-Alive";
	if (flag == 0)
		ret +="\r\n\r\n";
	return ret;
}

void Response::DeleteMethod(){
	int fd;
	std::ostringstream headers;
	std::vector<std::string> forbiddenFiles;
	forbiddenFiles.push_back(_CurrDirecory +"/srcs/");
	forbiddenFiles.push_back(_CurrDirecory +"/includes/");
	forbiddenFiles.push_back(_CurrDirecory +"/error_pages/");
	forbiddenFiles.push_back(_CurrDirecory +"/configFile/");
	forbiddenFiles.push_back(_CurrDirecory +"/main.cpp");
	forbiddenFiles.push_back(_CurrDirecory +"/Makefile");
	forbiddenFiles.push_back(_CurrDirecory +"/var/www/html/");
	for (std::vector<std::string>::iterator it = forbiddenFiles.begin(); it != forbiddenFiles.end(); it++){
		if (_path.find(*it) == 0){
			errorsResponse(403);
			return ;
		}
	}
	fd = unlink(_path.c_str());
	if (fd < 0){
		if (errno == EACCES)
			errorsResponse(403);
		return ;
	}
	else {
		headers << "HTTP/1.1 204 No Content";
		headers << Connection(0);
		_body = "";
	}
	_headers = headers.str();
}

void Response::setOkHeaders(std::string mType, std::string body){
	_headers = "HTTP/1.1 200 OK\r\nContent-type: " + mType + "\r\nContent-length: " + toString(fileSize(body));
	_headers += Connection(0);
}

void Response::setResponse(){
    bool isLoc = false;
    DIR *folder;
    std::string cgiExt ="";
    std::pair<std::string, std::string> redirect;
    for (unsigned long  i = 0; i < _servInfo.location.size(); i++){

        if (_servInfo.location[i].uri == "/" || _reqInfo.URI == (_servInfo.location[i].uri + "/") || _reqInfo.URI == _servInfo.location[i].uri
            || (_reqInfo.URI.find(_servInfo.location[i].uri + "/") == 0 && _servInfo.location[i].uri.size() > 1)) {
            _root = _servInfo.location[i].root;
            _location = _servInfo.location[i].uri;
            isLoc = true;
            _index = _servInfo.location[i].index;
            cgiExt = _servInfo.location[i].cgi;
            redirect = _servInfo.location[i].redirect;
            if (_servInfo.location[i].autoindex == true)
                _autoIndex = true;
            if (std::find(_servInfo.location[i].allow_methods.begin(), _servInfo.location[i].allow_methods.end(), 
                _reqInfo.method) == _servInfo.location[i].allow_methods.end()){
                    _reqInfo.statusCode = 405;
                    errorsResponse(405);
                    return ;
                }
            break ;
        }
    }
    if (cgiExt.length() > 0 && cgiExt != ".py" && cgiExt != ".php"){
        errorsResponse(502);
        return ;
    }
    if (isLoc == false || _root == "" || _index.size() == 0){
        if (_index.size() == 0)
            _index = _servInfo.index;
        if (_root == "")
            _root = _servInfo.root;
        if (_reqInfo.method != "GET" && isLoc == false){
            errorsResponse(405);
            return ;
        }
    }
    if (_root == _servInfo.root && _servInfo.autoindex == true)
        _autoIndex = true;
    if (isLoc == true){
        
        std::string uri = _reqInfo.URI.substr(_location.length());
        
        if (uri[0] != '/' && _root[_root.length() -1 ] != '/')
            _root += "/";
        _path = _root + uri;
    }
    else{
        _path = _root + _reqInfo.URI;
    }

    if (redirect.second.length() != 0){
        if (_path == (_root + redirect.first) || (redirect.first == "/" && _path == _root)){
            // std::cout << "ok" << std::endl;
            _body = redirect.second;
            _headers = "HTTP/1.1 301 Moved Permanently\r\nContent-type: text/html\r\nContent-length: " + toString(fileSize(_body));
            _headers += "\r\nLocation: " + redirect.second + "/";
            _headers += Connection(0);
            return ;
        }
    }
    folder = opendir(_path.c_str());
    if (!folder){
        if (errno == EACCES)
            errorsResponse(403);
        else if (errno == ENOENT)
            errorsResponse(404);
        else if (errno == ENOTDIR){
            if (_reqInfo.method == "DELETE"){
                DeleteMethod();
                return ;
            }
            if (cgiExt.length()){
                // if ( _path.substr(_path.length() - cgiExt.length()) == cgiExt ){
                    std::pair<std::string, std::string> cgiOut;
                    std::string conn = Connection(1);
                    cgi CGI(_reqInfo, _path, cgiExt , conn);
                    CGI.executeFile(_CurrDirecory);
                    if (CGI._statusCode == 500){
                        errorsResponse(500);
                        return;
                    }
                    cgiOut = CGI.parseCgiOutput(_CurrDirecory);
                    if (cgiOut.first.find("Status: 301 Moved Permanently") == 0)
                        _headers = "HTTP/1.1 301 301 Moved Permanently";
                    else
                        _headers = "HTTP/1.1 200 OK";
                    _headers += Connection(1) + "\r\n" + cgiOut.first;
                    _body = cgiOut.second;
                // }
                // else // cgiExt != _path.substr(_path.length() - cgiExt.length())
                //     errorsResponse(502);
                // return ;
            }
            else{ // cgiExt.length  == 0
                _body = _path;
                _headers = "HTTP/1.1 200 OK\r\nContent-type: " + _mime.getType(_reqInfo.URI) + "\r\nContent-length: " + toString(fileSize(_body));
                if (_autoIndex == true)
                    _headers += "\r\nContent-Disposition: attachment";
                _headers += Connection(0);
            }
            return ;
        }
    }
    else { // folder opened 

        if (_reqInfo.method == "DELETE"){
            closedir(folder);
            errorsResponse(403);
            return ;
        }
        if (cgiExt.length() == 0){
            if (_reqInfo.URI != "/"){

                if ( _path[_path.length() - 1] != '/'){
                    _path += "/";
                    _body = _path + _index[0];
                    _headers = "HTTP/1.1 302 Found\r\nContent-type: text/html\r\nContent-length: " + toString(fileSize(_body));
                    if (_reqInfo.URI == "/")
                        _headers += "\r\nLocation: " + _reqInfo.URI;
                    else
                        _headers += "\r\nLocation: " + _reqInfo.URI + "/";
                    _headers += Connection(0);
                    closedir(folder);
                    return ;
                }
            }
            int fd =-1;
            size_t i = 0;
            for (; i < _index.size(); i++){
                fd = open((_path+"/" +_index[i]).c_str(), O_RDONLY);
                if (fd >= 0){
                    
                    _body = _path + "/" + _index[i];
                    setOkHeaders("text/html", _body);
                    close(fd);
                    closedir(folder);
                    return ;
                }
                close(fd);
            }
            if (fd < 0){
                if (_autoIndex == true){
                    autoIndex indx;
                    indx.setAutoIndexBody(folder, _path, _root, _location, _CurrDirecory);
                    if (indx.isError() == true)
                        errorsResponse(indx.getErrorCode());
                    else { // indx.isError == false
                        _body = indx.getBodyName();
                        setOkHeaders("text/html", _body);
                    }
                }
                else //autoindex == false
                    errorsResponse(404);
                closedir(folder);
                return ;
            }
        }
        else { // cgi.length != 0

            
            for (size_t i = 0; i < _index.size(); i++){
                if (_index[i].substr(_index[i].length() - cgiExt.length()) == cgiExt){
                    std::pair<std::string, std::string> cgiOut;
                    std::string conn = Connection(1);
                    cgi CGI(_reqInfo, _path + "/" +_index[i], cgiExt , conn);
                    CGI.executeFile(_CurrDirecory);
                    if (CGI._statusCode == 500){
                        errorsResponse(500);
                        return;
                    }
                    cgiOut = CGI.parseCgiOutput(_CurrDirecory);
                    _headers = "HTTP/1.1 200 OK" + Connection(1) + "\r\n" + cgiOut.first;
                    _body = cgiOut.second;
                    closedir(folder);
                    return ;
                }
            }
              // no cgi file with cgiExt is found in path
            int opn = -1;
            for (size_t i = 0; i < _index.size(); i++){
                opn = open((_path + '/' + _index[i]).c_str(), O_RDONLY);
                if (opn != -1){
                    _body = _path + "/" + _index[i];
                     setOkHeaders("text/html", _body);
                    close(opn);
                    break;
                }
                close(opn);
            }
            if (opn == -1) {
                
                if (_autoIndex == true) {
                    autoIndex indx;
                    std::cout << "_path = " <<_path << " |_root == " <<  _root << std::endl;
                    indx.setAutoIndexBody(folder, _path, _root, _location, _CurrDirecory);
                    if (indx.isError() == true)
                        errorsResponse(indx.getErrorCode());
                    else { // no error in autoindex
                        _body = indx.getBodyName();
                         setOkHeaders("text/html", _body);
                    }
                }
                else // autoindex is false
                    errorsResponse(404);
            }
            closedir(folder);
        }   
    }

}

void Response::uploadResponse(){
	_body = _CurrDirecory + "/var/www/upload.html";
	_headers = "HTTP/1.1 201 created\r\nContent-type: text/html\r\nContent-length: " + toString(fileSize(_body));
	_headers += Connection(0);
}
std::pair<std::string, std::string> Response::get_response(){

	if (_reqInfo.method != "GET" && _reqInfo.method != "POST" && _reqInfo.method != "DELETE")
		errorsResponse(501);
	else{
		if (_reqInfo.statusCode == 200)
			setResponse();
		else if (_reqInfo.statusCode == 201)
			uploadResponse();
		else
			errorsResponse(_reqInfo.statusCode);
	}
	std::pair<std::string, std::string> p;
	// std::cout << "-----------------------------------------------" << std::endl;
	// std::cout << _headers << std::endl;
	// std::cout << _body << std::endl;
	// std::cout << "-----------------------------------------------" << std::endl;
	p.first = _headers;
	p.second = _body;
	return (p);
}

std::string Response::getBody(){
	return (_body);
}

std::string Response::getHeaders(){
	return _headers;
}

bool Response::IsKeepAlive(){
	return _iskeepAlive;
} 


int Response::response_size(){
	return (fileSize(_body) + _headers.size());
}



template<class T>
std::string toString(T i){
	std::ostringstream ret;
	ret  << i;
	return ret.str();
}




int fileSize(std::string fileName){
	int fd = open(fileName.c_str(), O_RDONLY);
	int ret = 0;
	if (fd >= 0){
		ret = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		close (fd);
	}
	return (ret);
}

bool Response::is_complete(int len, std::string filename){

	_cursor += len;
	// std::cout << "sending .... " << _cursor << std::endl;
	if ((size_t)_cursor >= fileSize(filename) + _headers.size())
		return true;
	return false;
}

int Response::get_cursor(){
	return _cursor;
}
