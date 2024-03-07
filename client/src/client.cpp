#include <iostream>

#include "client.hpp"

Client::Client(QObject* parent) : QObject(parent)
{
}

void Client::run()
{
    std::cout << "Hello, World!" << std::endl;
    emit finished();
}
