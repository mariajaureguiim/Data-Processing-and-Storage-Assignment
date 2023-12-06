#include <iostream>
#include <unordered_map>
#include <stdexcept>

class InMemoryDB {
private:
    std::unordered_map<std::string, int> mainState;
    std::unordered_map<std::string, int> transactionState;
    bool inTransaction;

public:
    InMemoryDB() : inTransaction(false) {}

    int get(const std::string& key) {
        if (inTransaction && transactionState.count(key) > 0) {
            return transactionState[key];
        } else {
            return mainState[key];
        }
    }

    void put(const std::string& key, int val) {
        if (!inTransaction) {
            throw std::runtime_error("Transaction not in progress");
        }
        transactionState[key] = val;
    }

    void begin_transaction() {
        if (inTransaction) {
            throw std::runtime_error("Transaction already in progress");
        }
        inTransaction = true;
    }

    void commit() {
        if (!inTransaction) {
            throw std::runtime_error("No open transaction to commit");
        }

        for (const auto& entry : transactionState) {
            mainState[entry.first] = entry.second;
        }

        clearTransactionState();
    }

    void rollback() {
        if (!inTransaction) {
            throw std::runtime_error("No ongoing transaction to rollback");
        }

        clearTransactionState();
    }

private:
    void clearTransactionState() {
        transactionState.clear();
        inTransaction = false;
    }
};

int main() {
    InMemoryDB inmemoryDB;

    std::cout << inmemoryDB.get("A") << std::endl;  

    try {
        inmemoryDB.put("A", 5);  
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    inmemoryDB.begin_transaction();
    inmemoryDB.put("A", 5);
    std::cout << inmemoryDB.get("A") << std::endl; 
    inmemoryDB.put("A", 6);
    inmemoryDB.commit();
    std::cout << inmemoryDB.get("A") << std::endl; 

    try {
        inmemoryDB.commit();  
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        inmemoryDB.rollback(); 
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    std::cout << inmemoryDB.get("B") << std::endl; 

    inmemoryDB.begin_transaction();
    inmemoryDB.put("B", 10);
    inmemoryDB.rollback();
    std::cout << inmemoryDB.get("B") << std::endl; 

    return 0;
}
