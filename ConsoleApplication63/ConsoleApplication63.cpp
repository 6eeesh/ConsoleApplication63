#include <iostream>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <chrono>

using namespace std;

class AccountHolder {
public:
    AccountHolder(const string& last_name, const string& first_name, int credit_rating, const string& registration_date)
        : last_name_(last_name), first_name_(first_name), credit_rating_(credit_rating), registration_date_(registration_date) {}

    const string& get_last_name() const { return last_name_; }
    const string& get_first_name() const { return first_name_; }
    int get_credit_rating() const { return credit_rating_; }
    const string& get_registration_date() const { return registration_date_; }

private:
    string last_name_;
    string first_name_;
    int credit_rating_;
    string registration_date_;
};

class Account {
public:
    Account(const AccountHolder& account_holder, const chrono::system_clock::time_point& opening_date_time,
        const chrono::system_clock::time_point* closing_date_time = nullptr)
        : account_holder_(account_holder), opening_date_time_(opening_date_time), closing_date_time_(closing_date_time),
        balance_(0), mutex_() {}

    void deposit(double amount) {
        lock_guard<mutex> lock(mutex_);
        balance_ += amount;
    }

    bool withdraw(double amount) {
        lock_guard<mutex> lock(mutex_);
        if (balance_ >= amount) {
            balance_ -= amount;
            return true;
        }
        return false;
    }

    void transfer(Account& to_account, double amount) {
        thread([=]() {
            this_thread::sleep_for(chrono::seconds(3));
            if (withdraw(amount)) {
                to_account.deposit(amount);
            }
            else {
                throw runtime_error("Insufficient funds");
            }
            }).detach();
    }

    const AccountHolder& get_account_holder() const { return account_holder_; }
    const chrono::system_clock::time_point& get_opening_date_time() const { return opening_date_time_; }
    const chrono::system_clock::time_point* get_closing_date_time() const { return closing_date_time_; }
    double get_balance() const {
        lock_guard<mutex> lock(mutex_);
        return balance_;
    }

private:
    AccountHolder account_holder_;
    chrono::system_clock::time_point opening_date_time_;
    const chrono::system_clock::time_point* closing_date_time_;
    double balance_;
    mutex mutex_;
};

class Operation {
public:
    enum class OperationType {
        DEPOSIT,
        WITHDRAW
    };

    Operation(OperationType operation_type, const chrono::system_clock::time_point& creation_date_time, bool status)
        : operation_type_(operation_type), creation_date_time_(creation_date_time), status_(status) {}

    OperationType get_operation_type() const { return operation_type_; }
    const chrono::system_clock::time_point& get_creation_date_time() const { return creation_date_time_; }
    bool get_status() const { return status_; }

    void set_status(bool status) { status_ = status; }

private:
    OperationType operation_type_;
    chrono::system_clock::time_point creation_date_time_;
    bool status_;
};