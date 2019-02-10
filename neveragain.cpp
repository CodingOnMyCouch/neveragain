#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>



// Saving text to IPFS, then storing the hash returned by IPFS on the EOS blockchain
// Saving an image to IPFS, then storing the hash returned by IPFS on the EOS blockchain
// fetching the hash from the EOS blockchain, then querying IPFS for the image or text



using namespace eosio;
using namespace std;

class neveragain : public contract {
    using contract::contract;
    
public:
    neveragain(name receiver, name code, datastream<const char*> ds) :
    contract( receiver, code, ds),
    _ipfsdata(_code, _code.value){}

    [[eosio::action]] void savehash(string text_hash, string image_hash) {
       
        
        auto iter = _ipfsdata.find(0);
        if(iter != _ipfsdata.end()) {
            _ipfsdata.modify(iter, name{0}, [&](auto& row) {
                if(text_hash != "") {
                    row.text_hash = text_hash;
                    print("YOUR STORY HAS BEEN UPLOADED TO THE WORLD!");
                }
                if(image_hash != "") {
                    row.image_hash = image_hash;
                    print("YOUR STORY HAS BEEN UPLOADED TO THE WORLD!");
                }
            });
        }else{
            _ipfsdata.emplace(_self, [&](auto& row) {
                row.id = _ipfsdata.available_primary_key();
                row.text_hash = text_hash;
                row.image_hash = image_hash;
                print("YOUR STORY HAS BEEN UPLOADED TO THE WORLD!");
            });
        }

    }
    

   [[eosio::action]] void delhash(uint64_t id) {
        // cleos push action ipfs.app delhash '[0]' -p dummy
        auto iter = _ipfsdata.find(id);
        _ipfsdata.erase(iter);
    }
    

private:
  
    [[eosio::table]] struct ipfsdata {
        //name name;
        uint64_t id;
        string text_hash;
        string image_hash;
        
        auto primary_key()const { return id; }
        
        EOSLIB_SERIALIZE(ipfsdata, (id)(text_hash)(image_hash));
    };


    multi_index<"ipfsdata"_n, ipfsdata> _ipfsdata;
    
    
    
};

//EOSIO_ABI( ipfs, (savehash)(delhash) );
EOSIO_DISPATCH( neveragain, (savehash)(delhash) );

 // cleos push action ipfs.app savehash '["QmPnnvFkvSCmPvMdMtSNbXrFWQX5XYc4YkcVU4cujyyH8Z",""]' -p dummy
 // cleos push action ipfs.app savehash '["","QmPnnvFkvSCmPvMdMtSNbXrFWQX5XYc4YkcVU4cujyyH8Z"]' -p dummy
 // cleos push acction neveragain savehash '["QmPnnvFkvSCmPvMdMtSNbXrFWQX5XYc4YkcVU4cujyyH8Z",""]' -p victim
 // cleos get table ipfs.app ipfs.app ipfsdata