# GUDA Token Contract 🜛

Welcome to the GUDA Token Contract repository! This contract supports the GUDA token, a unique cryptocurrency embodying a badge of honor, representing a journey through challenges and personal growth. Earn Gudabits for real-life challenges and redeem them for NFTs.

## Project Overview

- **Project Name**: GUDA
- **Website**: [GUDA](https://guda.guda.love/)
- **Description**: GUDA is more than a symbol; it is a badge of honor, a testament to your journey through the shadows and your emergence into the light. Earn Gudabits for IRL challenges that transmute your shadows, found in Discord. Every Gudabit (0.00000001 GUDA) is redeemable for an NFT via the drops.guda contract.
- **Tags**: Social, Challenge, Celebrity, Meme, NFT
- **GitHub Repository**: [GUDA Token](https://github.com/dougbutner/guda-token)
- **Social Media**:
  - [Discord](https://discord.com/invite/MrRXZYhHfp)
  - [Linktree](https://linktr.ee/gudasol)
  - [Twitter](https://twitter.com/GUDA_coin)
  - [YouTube](https://youtube.com/@gudasol)

## Features 🌟

The GUDA Token contract includes the following features:

- **Token Creation**: Create a new token with a maximum supply.
- **Minting**: Mint tokens to a specified account.
- **Burning**: Burn tokens from a specified account.
- **Transfer**: Transfer tokens between accounts.
- **Vesting**: Vest tokens for a specified period and claim vested tokens.
- **Account Management**: Open and close token accounts.

## Contract Actions 🚀

### Create a New Token 🛠️

```cpp
ACTION create(const eosio::name& issuer, const eosio::asset& maximum_supply);
```
Creates a new token with a specified maximum supply.

### Mint Tokens 💸

```cpp
ACTION mint(const eosio::name& to, const eosio::asset& quantity, const std::string& memo);
```
Mints tokens to a specified account.

### Burn Tokens 🔥

```cpp
ACTION burn(const eosio::name& burner, const eosio::asset& quantity, const std::string& memo);
```
Burns tokens from a specified account.

### Transfer Tokens 🔄

```cpp
ACTION transfer(const eosio::name& from, const eosio::name& to, const eosio::asset& quantity, const std::string& memo);
```
Transfers tokens between accounts.

### Vest Tokens ⏳

```cpp
ACTION vest(const eosio::name& to, const eosio::asset& quantity, uint64_t vest_seconds, const std::string& memo);
```
Vests tokens for a specified period.

### Claim Vested Tokens 💎

```cpp
ACTION claimvest(uint64_t id, const eosio::asset& quantity);
```
Claims vested tokens after the vesting period is over.

### Open Token Account 🔓

```cpp
ACTION open(const eosio::name& owner, const eosio::symbol& symbol, const eosio::name& ram_payer);
```
Opens a token account for a specified owner.

### Close Token Account 🔒

```cpp
ACTION close(const eosio::name& owner, const eosio::symbol& symbol);
```
Closes a token account for a specified owner.

## Static Methods 🧩

### Get Token Supply 🏷️

```cpp
static eosio::asset get_supply(const eosio::name& token_contract_account, const eosio::symbol_code& sym_code);
```
Returns the supply of a token.

### Get Account Balance 💰

```cpp
static eosio::asset get_balance(const eosio::name& token_contract_account, const eosio::name& owner, const eosio::symbol_code& sym_code);
```
Returns the balance of a specified account.

## Table Structures 📊

### Account Table 🗃️

```cpp
TABLE account {
    eosio::asset balance;
    uint64_t primary_key() const { return balance.symbol.code().raw(); }
};
```

### Currency Stats Table 📈

```cpp
TABLE currency_stats {
    eosio::asset supply;
    eosio::asset max_supply;
    eosio::name issuer;
    uint64_t primary_key() const { return supply.symbol.code().raw(); }
};
```

### Vest Record Table ⏳

```cpp
TABLE vest_record {
    uint64_t id;
    eosio::asset vested_balance;
    eosio::name receiver;
    uint64_t vested_until;
    uint64_t primary_key() const { return id; }
};
```

### Burner Record Table 🔥

```cpp
TABLE burner_record {
    eosio::name burner;
    eosio::asset total_burned;
    std::string last_memo;
    uint64_t primary_key() const { return burner.value; }
};
```

## Installation 🛠️

To deploy and interact with the GUDA token contract, follow these steps:

1. Clone the repository:
    ```sh
    git clone https://github.com/dougbutner/guda-token.git
    cd guda-token
    ```

2. Compile the contract:
    ```sh
    eosio-cpp -o guda.wasm guda.cpp --abigen
    ```

3. Deploy the contract to your EOSIO blockchain:
    ```sh
    cleos set contract <account> <path-to-contract-directory>
    ```

4. Interact with the contract using `cleos` or any other EOSIO-compatible tool.

## Contributing 🤝

We welcome contributions to the GUDA token contract! Please feel free to submit issues, fork the repository, and create pull requests.

## License 📄

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Stay Connected 🌐

Join our community and stay updated with the latest news and updates:

- [Discord](https://discord.com/invite/MrRXZYhHfp)
- [Linktree](https://linktr.ee/gudasol)
- [Twitter](https://twitter.com/GUDA_coin)
- [YouTube](https://youtube.com/@gudasol)

---

🜛 GUDA Token Contract 🜛