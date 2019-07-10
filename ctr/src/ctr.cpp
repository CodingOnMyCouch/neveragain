#include <ctr.hpp>

ctr::ctr(eosio::name receiver, eosio::name code, datastream<const char*> ds):contract(receiver, code, ds){}
   


   
void ctr::getbalance(eosio::name username)
{
  require_auth(username);
  
  accounts _balances(_self, username.value);
  
  auto it = _balances.find(username.value);
  if(it != _balances.end())
  {
    it->balance.print();
  }
}



void ctr::deposit(const eosio::name from, const eosio::name to, const eosio::asset quantity, std::string& memo)
{
  auto self = get_self(); 
  
  if(to == self){
    
    add_balance(get_first_receiver(), to, quantity, self);
    
    print("Deposit complete to : ", name{to});
  }
}

    
void ctr::setdaylimit(eosio::name owner, int64_t amount, std::string_view tkn)
{
  require_auth(owner); 
  
  accounts to_acnts(get_self(), owner.value);
  auto it = to_acnts.find(owner.value);
  
  if(it != to_acnts.end()){
   to_acnts.modify(it, owner, [&](auto& new_acc){
      new_acc.limit = eosio::asset(amount, symbol(tkn, 0));
      new_acc.expires_at = time_point_sec(current_time_point() + hours(24));
    });
  }
}


// create receipts as inline actions with notifications to the user
// indicating each payment details amd remaining balance
    


void ctr::withdraw(eosio::name contract, eosio::name from, name to, asset quantity, std::string& memo)
{
  auto self = get_self();
  require_auth(self);

    eosio::check(quantity.symbol == symbol("TOKEN", 4), "can only withdraw TOKEN token");
    eosio::check(quantity.amount > 0, "withdraw amount must be positive");

    sub_balance(contract, from, quantity);
    
    // Inline transfer
    ctr::transfer_action t_action(contract, {_self, "active"_n});
    t_action.send(_self, to, quantity, memo);
}



void ctr::add_balance(eosio::name contract, eosio::name owner, eosio::asset value, eosio::name ram_payer)
{
  accounts to_acnts(get_self(), owner.value);

  auto it = to_acnts.find(contract.value);
  
  if( it == to_acnts.end() ) {
    to_acnts.emplace( ram_payer, [&]( auto& new_acc ){
      new_acc.contract = contract;
      new_acc.balance = value;
    });
   } else {
    to_acnts.modify(it, ram_payer, [&](auto& new_acc){
      new_acc.balance += value;
    });
    
   } 
}




void ctr::sub_balance(eosio::name contract, eosio::name owner, eosio::asset value)
{
  accounts from_acnts(get_self(), owner.value);
  
  auto itr = from_acnts.find(contract.value);
  
  if(itr != from_acnts.end()){
    from_acnts.modify(itr, same_payer, [&](auto& new_acc){
      new_acc.balance -= value;
    });
  }
}






