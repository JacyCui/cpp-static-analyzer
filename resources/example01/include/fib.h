#ifndef STATIC_ANALYZER_FIB_H
#define STATIC_ANALYZER_FIB_H

namespace example01 {

    class AbstractFib {
    public:
        virtual int fib(int i) = 0;

        virtual int getNum() = 0;
    };

    class Fib : public AbstractFib {
    private:
        int n;
    public:
        Fib(int n);

        virtual int fib(int i);

        virtual int getNum();
    };

}

#endif //STATIC_ANALYZER_FIB_H
