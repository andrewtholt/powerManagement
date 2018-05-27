
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
        virtual void act() = 0;
};

class ld : public instruction {
    public:
        ld(char *n);
        virtual void act() ;
        virtual void dump() override;
};

class ldn : public instruction {
    public:
        ldn(char *n);
        void act();
        virtual void dump() override;
};

class out : public instruction {
    public:
        out(char *n);
        void act();
        virtual void dump() override ;

};

class Or : public instruction {
    public:
        Or(char *n);
        void act();
        virtual void dump() override ;
};

class And : public instruction {
    public:
        And(char *n);
        void act();
        virtual void dump() override ;
};

class Andn : public instruction {
    public:
        Andn(char *n);
        void act();
        virtual void dump() override ;
};

class Noop : public instruction {
    public:
        Noop(char *n);
        void act();
        virtual void dump() override ;
};

class timLd : public instruction {
    public:
        timLd(char *n);
        virtual void act() ;
        virtual void dump() override;
};

class timLdn : public instruction {
    public:
        timLdn(char *n);
        virtual void act() ;
        virtual void dump() override;
};

class timAndn : public instruction {
    public:
        timAndn(char *n);
        void act();
        virtual void dump() override ;
};

class timAnd : public instruction {
    public:
        timAnd(char *n);
        void act();
        virtual void dump() override ;
};


