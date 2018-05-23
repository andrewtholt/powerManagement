
#pragma once

#include <sqlite3.h>
#include <string>

using namespace std;

class instruction {
    public:
        char id[32];
        char name[32];
        void dump(char *iName) ;
        virtual void dump() = 0;
        void act() {};
};

class ld : public instruction {
    public:
//        char *name = (char *)"LD";
        ld(char *n);
        void act();
        virtual void dump() override;
};

class ldn : public instruction {
    public:
//        char *name = (char *)"LDN";
        ldn(char *n);
        void act();
        void dump();
};

class out : public instruction {
    public:
        out(char *n);
        void act();
        void dump();

};

