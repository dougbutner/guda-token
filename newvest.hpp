#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>

#include <string>

// === New Vest Vesting Token  === //

class [[eosio::contract]] newvest : public eosio::contract {
   public:
      using eosio::contract::contract;

      // --- Action to create a new token --- //
      ACTION create( const eosio::name&   issuer,
                     const eosio::asset&  maximum_supply);

      // --- Issue tokens to an account --- //
      ACTION issue( const eosio::name& to, const eosio::asset& quantity, const std::string& memo );

      // --- Burn tokens from an account --- //
      ACTION burn( const eosio::name& burner, const eosio::asset& quantity, const std::string& memo );

      // --- Transfer tokens between accounts --- //
      ACTION transfer( const eosio::name&    from,
                       const eosio::name&    to,
                       const eosio::asset&   quantity,
                       const std::string&  memo );

      // --- Open a token account --- //
      ACTION open( const eosio::name& owner, const eosio::symbol& symbol, const eosio::name& ram_payer );

      // --- Close a token account --- //
      ACTION close( const eosio::name& owner, const eosio::symbol& symbol );

      // --- Vest tokens --- //
      ACTION vest( const eosio::name& to,
                   const eosio::asset& quantity,
                   uint64_t vest_seconds,
                   const std::string& memo );

      // --- Claim vested tokens --- //
      ACTION claimvest( uint64_t id,
                        const eosio::asset& quantity );

      // --- Get the supply of a token --- //
      static eosio::asset get_supply( const eosio::name& token_contract_account, const eosio::symbol_code& sym_code ) {
         stats statstable( token_contract_account, sym_code.raw() );
         const auto& st = statstable.get( sym_code.raw(), "invalid supply symbol code" );
         return st.supply;
      }

      // --- Get the balance of an account --- //
      static eosio::asset get_balance( const eosio::name& token_contract_account, const eosio::name& owner, const eosio::symbol_code& sym_code ) {
         accounts accountstable( token_contract_account, owner.value );
         const auto& ac = accountstable.get( sym_code.raw(), "no balance with specified symbol" );
         return ac.balance;
      }

   private:
      // --- Store account balances --- //
      TABLE account {
         eosio::asset    balance;

         uint64_t primary_key()const { return balance.symbol.code().raw(); }
      };

      // --- Store token statistics --- //
      TABLE currency_stats {
         eosio::asset    supply;
         eosio::asset    max_supply;
         eosio::name     issuer;

         uint64_t primary_key()const { return supply.symbol.code().raw(); }
      };

      // --- Store vesting records --- //
      TABLE vest_record {
         uint64_t       id;
         eosio::asset   vested_balance;
         eosio::name    receiver;
         uint64_t       vested_until;

         uint64_t primary_key()const { return id; }
      };

      // --- Store burn records --- //
      TABLE burner_record {
         eosio::name    burner;
         eosio::asset   total_burned;
         std::string    last_memo;

         uint64_t primary_key()const { return burner.value; }
      };

      typedef eosio::multi_index< "accounts"_n, account > accounts;
      typedef eosio::multi_index< "stat"_n, currency_stats > stats;
      typedef eosio::multi_index< "vests"_n, vest_record > vests;
      typedef eosio::multi_index< "burners"_n, burner_record > burners;

      // --- Subtract balance from an account --- //
      void sub_balance( const eosio::name& owner, const eosio::asset& value );
      // --- Add balance to an account --- //
      void add_balance( const eosio::name& owner, const eosio::asset& value, const eosio::name& ram_payer );
      // --- Add vested balance to an account --- //
      void add_vested_balance( const eosio::name& owner, const eosio::asset& value, uint64_t vest_seconds, const eosio::name& ram_payer );
}; //END newvest
