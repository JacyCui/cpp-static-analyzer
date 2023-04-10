#ifndef STATIC_ANALYZER_FACTOR_H
#define STATIC_ANALYZER_FACTOR_H

class AbstractFactor {
public:
    virtual int factor(int i) = 0;
    virtual int getNum() = 0;
};

class Factor: public AbstractFactor {
private:
    int n;
public:
    Factor(int n);
    virtual int factor(int i);
    virtual int getNum();
};

#endif //STATIC_ANALYZER_FACTOR_H
