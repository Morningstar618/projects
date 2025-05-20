pub mod blockchain;
use blockchain::{
    chain::BlockChain,
    block::Block,
    search::{BlockSearch, BlockSearchResult}
};

fn main() {
    let mut bc = BlockChain::new();
    
    bc.create_block(1);
    bc.create_block(2);
    bc.create_block(3);
    bc.display_chain();

    let result = match bc.search_block(BlockSearch::SearchByNonce(5)) {
        BlockSearchResult::Success(b) => Some(b),

        BlockSearchResult::BlockNotFound => {
            eprintln!("Block not found");
            return ();
        }

        BlockSearchResult::NoBlockInChain => {
            eprintln!("Blockchain is empty");
            return ();
        }
    };

    println!("{:?}", result.unwrap());
}
