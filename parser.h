#pragma once
#include <vector>
#include <string>
#include <cstring>
#include "archiver.h"
#include "exceptions.h"
#include <iostream>

using Options = std::vector<std::string>;
using ParsedData = std::pair<ArchiverUnits, Options>;

class Parser {
protected:
    int argc_;
    char** argv_;
    bool succeed_;

public:
    Parser(int argc, char** argv) {
        argc_ = argc;
        argv_ = argv;
    }
    inline ParsedData Parse() {
        succeed_ = false;
        if (argc_ == 1) {
            throw UndefinedRequestException();
        }
        Options o;
        for (size_t i = 2; i < static_cast<size_t>(argc_); ++i) {
            o.push_back(argv_[i]);
        }
        ArchiverUnits unit = ArchiverUnits::help;
        if (strcmp(argv_[1], "-c") == 0) {
            unit = ArchiverUnits::archive;
        } else if (strcmp(argv_[1], "-d") == 0) {
            unit = ArchiverUnits::unarchive;
        } else if (strcmp(argv_[1], "-h") == 0) {
            unit = ArchiverUnits::help;
        } else {
            return {};
        }
        succeed_ = true;
        return std::make_pair(unit, o);
    }
    inline bool ParseSuccess() const {
        return succeed_;
    }
};