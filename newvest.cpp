#include "newvest.hpp"
#include <eosio/system.hpp> 

using namespace eosio;
using namespace std;

// === New Vest Vesting Token  === //

// --- Create a new token --- //
void newvest::create( const name& issuer, const asset& maximum_supply ) {
    require_auth( get_self() );

    auto sym = maximum_supply.symbol;
    check( maximum_supply.is_valid(), "⚡️ Supply is not valid." );
    check( maximum_supply.amount > 0, "⚡️ Max-supply must be positive." );

    stats statstable( get_self(), sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    check( existing == statstable.end(), "⚡️ A token with symbol already exists." );

    statstable.emplace( get_self(), [&]( auto& s ) {
        s.supply.symbol = maximum_supply.symbol;
        s.max_supply = maximum_supply;
        s.issuer = issuer;
    });
}

// --- Issue tokens to an account --- //
void newvest::issue( const name& to, const asset& quantity, const string& memo ) {
    auto sym = quantity.symbol;
    check( sym.is_valid(), "⚡️ Invalid symbol name" );
    check( memo.size() <= 256, "⚡️ Memo is too long" );

    stats statstable( get_self(), sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    check( existing != statstable.end(), "⚡️ A token with symbol does not exist, create token before issue" );
    const auto& st = *existing;
    check( to == st.issuer, "⚡️ Tokens can only be issued to issuer account" );

    require_auth( st.issuer );
    check( quantity.is_valid(), "⚡️  Invalid quantity" );
    check( quantity.amount > 0, "⚡️ You must issue positive quantity" );

    check( quantity.symbol == st.supply.symbol, "⚡️ Symbol precision mismatch" );
    check( quantity.amount <= st.max_supply.amount - st.supply.amount, "⚡️ Quantity exceeds available supply" );

    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply += quantity;
    });

    add_balance( st.issuer, quantity, st.issuer );
}

// --- Burn tokens from an account --- //
void newvest::burn( const name& burner, const asset& quantity, const string& memo ) {
    require_auth( burner );

    auto sym = quantity.symbol;
    check( sym.is_valid(), "⚡️ Invalid symbol name" );
    check( memo.size() <= 256, "⚡️ Memo is too long" );

    stats statstable( get_self(), sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    check( existing != statstable.end(), "⚡️ Token with symbol does not exist" );
    const auto& st = *existing;

    check( quantity.is_valid(), "⚡️ Invalid quantity" );
    check( quantity.amount > 0, "⚡️ Must burn positive quantity" );
    check( quantity.symbol == st.supply.symbol, "⚡️ Symbol precision mismatch" );

    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply -= quantity;
    });

    sub_balance( burner, quantity );

    burners burntable( get_self(), get_self().value );
    auto it = burntable.find( burner.value );
    if (it == burntable.end()) {
        burntable.emplace( burner, [&]( auto& b ) {
            b.burner = burner;
            b.total_burned = quantity;
            b.last_memo = memo;
        });
    } else {
        burntable.modify( it, same_payer, [&]( auto& b ) {
            b.total_burned += quantity;
            b.last_memo = memo;
        });
    }
}

// --- Transfer tokens between accounts --- //
void newvest::transfer( const name& from, const name& to, const asset& quantity, const string& memo ) {
    check( from != to, "⚡️ Cannot transfer to self" );
    require_auth( from );
    check( is_account( to ), "⚡️ To account does not exist" );
    auto sym = quantity.symbol.code();
    stats statstable( get_self(), sym.raw() );
    const auto& st = statstable.get( sym.raw() );

    require_recipient( from );
    require_recipient( to );

    check( quantity.is_valid(), "⚡️ Invalid quantity" );
    check( quantity.amount > 0, "⚡️ Must transfer positive quantity" );
    check( quantity.symbol == st.supply.symbol, "⚡️ Symbol precision mismatch" );
    check( memo.size() <= 256, "⚡️ Memo is too long" );

    auto payer = has_auth( to ) ? to : from;

    sub_balance( from, quantity );
    add_balance( to, quantity, payer );
}

// --- Vest tokens --- //
void newvest::vest( const name& to, const asset& quantity, uint64_t vest_seconds, const string& memo ) {
    require_auth( get_self() );
    check( is_account( to ), "⚡️ To account does not exist" );
    check( vest_seconds > 0, "⚡️ Must vest for positive time" );
    check( vest_seconds <= 10*365*24*60*60, "⚡️ Must vest for no longer than 10 years" );

    auto sym = quantity.symbol.code();
    stats statstable( get_self(), sym.raw() );
    const auto& st = statstable.get( sym.raw(), "⚡️ Token with symbol does not exist" );

    check( quantity.is_valid(), "⚡️ Invalid quantity" );
    check( quantity.amount > 0, "⚡️ Must vest positive quantity" );
    check( quantity.symbol == st.supply.symbol, "⚡️ Symbol precision mismatch" );
    check( memo.size() <= 256, "⚡️ Memo is too long" );

    sub_balance( get_self(), quantity );
    add_vested_balance( to, quantity, vest_seconds, get_self() );
}

// --- Claim vested tokens --- //
void newvest::claimvest( uint64_t id, const asset& quantity ) {
    check( quantity.is_valid(), "⚡️ Invalid quantity" );
    check( quantity.amount > 0, "⚡️ Must claim positive quantity" );

    vests vestings( get_self(), get_self().value );
    auto vest = vestings.get( id, "⚡️ Vest not found" );
    auto receiver = vest.receiver;

    require_auth( receiver );

    check( quantity.symbol == vest.vested_balance.symbol, "⚡️ Symbol precision mismatch" );
    check( vest.vested_balance.amount >= quantity.amount, "⚡️ Overdrawn balance" );
    check( current_time_point().sec_since_epoch() >= vest.vested_until, "⚡️ Vesting period not over" );

    if( vest.vested_balance.amount == quantity.amount ) {
        vestings.erase( vest );
    } else {
        vestings.modify( vest, receiver, [&]( auto& v ) {
            v.vested_balance -= quantity;
        });
    }

    add_balance( receiver, quantity, get_self() );
}

// --- Subtract balance from an account --- //
void newvest::sub_balance( const name& owner, const asset& value ) {
    accounts from_acnts( get_self(), owner.value );

    const auto& from = from_acnts.get( value.symbol.code().raw(), "⚡️ No balance object found" );
    check( from.balance.amount >= value.amount, "⚡️ Overdrawn balance" );

    from_acnts.modify( from, owner, [&]( auto& a ) {
        a.balance -= value;
    });
}

// --- Add balance to an account --- //
void newvest::add_balance( const name& owner, const asset& value, const name& ram_payer ) {
    accounts to_acnts( get_self(), owner.value );
    auto to = to_acnts.find( value.symbol.code().raw() );
    if( to == to_acnts.end() ) {
        to_acnts.emplace( ram_payer, [&]( auto& a ){
            a.balance = value;
        });
    } else {
        to_acnts.modify( to, same_payer, [&]( auto& a ) {
            a.balance += value;
        });
    }
}

// --- Add vested balance to an account --- //
void newvest::add_vested_balance( const name& owner, const asset& value, uint64_t vest_seconds, const name& ram_payer ) {
    vests vestings( get_self(), get_self().value );
    vestings.emplace( ram_payer, [&]( auto& v ) {
        v.id = vestings.available_primary_key();
        v.vested_balance = value;
        v.receiver = owner;
        v.vested_until = current_time_point().sec_since_epoch() + vest_seconds;
    });
}

// --- Open a token account --- //
void newvest::open( const name& owner, const symbol& symbol, const name& ram_payer ) {
    require_auth( ram_payer );

    check( is_account( owner ), "⚡️ Owner account does not exist" );

    auto sym_code_raw = symbol.code().raw();
    stats statstable( get_self(), sym_code_raw );
    const auto& st = statstable.get( sym_code_raw, "⚡️ Symbol does not exist" );
    check( st.supply.symbol == symbol, "⚡️ Symbol precision mismatch" );

    accounts acnts( get_self(), owner.value );
    auto it = acnts.find( sym_code_raw );
    if( it == acnts.end() ) {
        acnts.emplace( ram_payer, [&]( auto& a ){
            a.balance = asset{0, symbol};
        });
    }
}

// --- Close a token account --- //
void newvest::close( const name& owner, const symbol& symbol ) {
    require_auth( owner );

    accounts acnts( get_self(), owner.value );
    auto it = acnts.find( symbol.code().raw() );
    check( it != acnts.end(), "⚡️ Balance row already deleted or never existed. Action won't have any effect." );
    check( it->balance.amount == 0, "⚡️ Cannot close because the balance is not zero." );
    acnts.erase( it );
}
