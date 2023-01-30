#pragma once

#include <exception>

class FileReadingException : public std::exception {
public:
    const char* what() const throw() override {
        return "Process terminated due to undefined circumstances\n";
    }
};

class UndefinedRequestException : public std::exception {
public:
    const char* what() const throw() override {
        return "Your request to archiver is undefined. \n"
               "Please check archiver -h to get more information.\n";
    }
};
