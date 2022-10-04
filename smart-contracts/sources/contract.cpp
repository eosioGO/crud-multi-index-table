#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract("crudtable")]] myclass : public eosio::contract {

public:

   myclass(name receiver, name code, datastream<const char*> ds): 
	   mytable(get_self(), get_self().value),
	   contract(receiver, code, ds) 
   {}

   // interface for method "create"
   [[eosio::action]]
   int64_t intcreate (name user, std::string description) 
   {
      return this->create(user, description);
   }

   //instead of "read" method use "cleos get table"

   // interface for method "update"
   [[eosio::action]]
   void intupdate (uint64_t id, name user, std::string description)
   {
      this->update(id, user, description);
   }

   // interface for method "erase"
   [[eosio::action]]
   void interase(uint64_t id, name user) 
   {
      this->erase(id, user);   
   }

   [[eosio::action]]
   void getselfinfo()
   {
      print("This is ", get_self());
   }

private:
   struct [[eosio::table]] crudtable {

      uint64_t id;
      name user;
      std::string description;
      uint64_t primary_key() const { return id;}
      uint64_t get_secondary_1() const { return user.value;}
   };

   uint64_t create(name user, std::string description) {
      require_auth(user);
      uint64_t incr = mytable.available_primary_key();
            mytable.emplace(user, [&]( auto& row ) {
               row.id = incr;
               row.user = user;
               row.description = description;
               });
      return incr;
   }

   void update(uint64_t id, name user, std::string description){
      require_auth(user);
      auto iterator = mytable.find(id);
      check(iterator != mytable.end(), "Record does not exist");
      mytable.modify(iterator, user, [&](auto &row){
         row.user = user;
         row.description = description;
      });
   }   

   void erase(uint64_t id, name user) {
      require_auth(user);
      auto iterator = mytable.find(id);
      check(iterator != mytable.end(), "Record does not exist");
      mytable.erase(iterator);
   }   

   typedef eosio::multi_index<"crudtable"_n, crudtable,
      indexed_by<"byuser"_n, const_mem_fun<crudtable, uint64_t, &crudtable::get_secondary_1>>
   > myindex;

   myindex mytable;
};