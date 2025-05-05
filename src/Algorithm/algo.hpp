#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

class Algorithm {
public:
    virtual void readUSBPower() = 0;
    virtual void processData() = 0;
    virtual void moveMotors() = 0;
    virtual ~Algorithm() {}
};

#endif // ALGORITHM_HPP