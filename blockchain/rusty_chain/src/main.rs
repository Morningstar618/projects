pub mod blockchain;
use blockchain::{
    chain::BlockChain,
    block::Block,
    search::{BlockSearch, BlockSearchResult},
    transaction::Transaction,
    utility_traits::Serialization
};

fn main() {
    let mut bc = BlockChain::new();
    
    let tx = Transaction::new(
        "sender".as_bytes().to_vec(),
        "recipient".as_bytes().to_vec(),
        100
    );

    bc.add_transaction(&tx);

    bc.create_block(1);

    bc.display_chain();
}
