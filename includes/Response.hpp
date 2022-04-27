/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iidzim <iidzim@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/08 16:51:13 by iidzim            #+#    #+#             */
/*   Updated: 2022/04/27 01:10:20 by iidzim           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "configurationReader.hpp"
#include "Request.hpp"
#include <sys/wait.h>
#include "mimeTypes.hpp"
#include "configurationReader.hpp"

class Response{

    private:
        std::string                         _headers;
        std::string                         _body;
        std::string                         _fileName;
        bool                                _iskeepAlive;
        s_requestInfo                       _reqInfo;
        mimeTypes                            _mime;
        serverInfo                          _servInfo;
        int                                 _cursor;
    public:
        Response();
        Response(request req, serverInfo serv);
        ~Response();
        std::pair<std::string, std::string> get_response();
        std::string setErrorsHeaders(std::string , std::string);
        void stringfyHeaders();
        void errorsResponse(int);
        int getContentlength();
        std::string getBody();
        std::string getHeaders();
        void GetandPostMethod();
        void DeleteMethod();
        bool IsKeepAlive();
        int response_size();
        template<class T>
        std::string toString(T);
        bool isFileExist(std::string );
        std::string findMimeType(std::string);
        int fileSize(std::string);

        //!!
        bool is_complete(int len);
        int get_cursor();

};

#endif