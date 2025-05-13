pub mod blockchain;
use blockchain::chain::BlockChain;

fn main() {
    let mut bc = BlockChain::new();
    
    bc.create_block(1);
    bc.create_block(2);
    bc.display_chain();

}
