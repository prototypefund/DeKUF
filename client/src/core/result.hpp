#pragma once

#include <QString>
#include <type_traits>

template <typename T>
class Result {
public:
    bool success;
    QString errorMessage;
    std::optional<T> value;

    Result() : success(true) {}

    explicit Result(const T& v) : success(true), value(v) {}

    static Result<T> Failure(const QString& error) {
        Result<T> result;
        result.success = false;
        result.errorMessage = error;
        return result;
    }

    bool isSuccess() const { return success; }

    QString getErrorMessage() const { return errorMessage; }

    const T& getValue() const { return value.value(); }
    bool hasValue() const { return value.has_value(); }
};

template<>
class Result<void> {
public:
    bool success;
    QString errorMessage;

    Result() : success(true) {}

    static Result<void> Failure(const QString& error) {
        Result<void> result;
        result.success = false;
        result.errorMessage = error;
        return result;
    }

    bool isSuccess() const { return success; }

    QString getErrorMessage() const { return errorMessage; }
};
