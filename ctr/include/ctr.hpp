#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/name.hpp>
#include <eosio/singleton.hpp>


#include <string>




using namespace eosio;

CONTRACT ctr : public contract {
  public:
    using contract::contract;
    ctr(eosio::name receiver, eosio::name code, datastream<const char*> ds);
    
    // action to get account balance
    ACTION getbalance(eosio::name username);
    
    // action set daily limit
    // limit can only be reset after 24 hours
    // format: 23E6 == 23.0000
    ACTION setdaylimit(eosio::name owner, int64_t amount, std::string_view tkn);
    
    
    // withdraw token from your ctr account back to your blockchain
    ACTION withdraw(eosio::name contract, eosio::name from, eosio::name to, eosio::asset quantity, std::string& memo);

    
   
    [[eosio::on_notify("eosio.token::transfer")]]
    void deposit(const eosio::name from, const eosio::name to, const eosio::asset quantity, std::string& memo);
    
    
    using transfer_action = action_wrapper<"transfer"_n, &ctr::deposit>;
    
  
  private:
  
    void add_balance(eosio::name contract, eosio::name owner, eosio::asset value, eosio::name ram_payer);
    void sub_balance(eosio::name contract, eosio::name owner, eosio::asset value);
    
    TABLE account {
      name      contract;
      asset     balance;
      asset     limit;
      time_point_sec expires_at;
      
      uint64_t primary_key()const { return contract.value; }
    };

    typedef multi_index< "accounts"_n, account> accounts;
};

