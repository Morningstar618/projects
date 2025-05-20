use super::{
    block::Block,
    search::{BlockSearch, BlockSearchResult},
    transaction::Transaction
};

#[derive(Debug)]
pub struct BlockChain {
    transaction_pool: Vec<Vec<u8>>,
    chain: Vec<Block>
}

impl BlockChain {
    pub fn new() -> Self {
        let mut bc = BlockChain {
            transaction_pool: Vec::<Vec<u8>>::new(),
            chain: Vec::<Block>::new()
        };

        bc.create_block(0);
        bc
    }

    pub fn create_block(&mut self, nonce: i32) {
        let previous_hash = if self.chain.len() > 0 
        {
            self.chain[self.chain.len() - 1].hash()
        } else {
            vec![0 as u8; 32]
        };

        let b = Block::new(nonce, previous_hash);
        self.chain.push(b);
    }

    pub fn display_chain(&self) {
        self.chain
            .iter()
            .for_each(|b| b.display());
    }

    pub fn last_block(&self) -> &Block {
        if self.chain.len() > 1 {
            return &self.chain[self.chain.len() - 1];
        }

        &self.chain[0]
    }

    pub fn search_block(&self, search: BlockSearch) -> BlockSearchResult {
        for block in self.chain.iter() {
            match search {
                BlockSearch::SearchByNonce(val) => {
                    if val as usize >= self.chain.len() {
                        return BlockSearchResult::BlockNotFound;
                    }

                    if val == block.get_nonce() {
                        return BlockSearchResult::Success(block);
                    }
                }

                BlockSearch::SearchByPreviousHash(ref hash) => {
                    if hash == block.get_previous_hash() {
                        return BlockSearchResult::Success(block);
                    }
                }

                BlockSearch::SearchByTimestamp(ts) => {
                    if ts == block.get_timestamp() {
                        return BlockSearchResult::Success(block);
                    }
                }

                BlockSearch::SearchByTransaction(ref transaction) => {
                    if transaction == block.get_transaction() {
                        return BlockSearchResult::Success(block);
                    }
                }

                BlockSearch::SearchByBlockHash(ref block_hash) => {
                    if block_hash == &block.hash() {
                        return BlockSearchResult::Success(block);
                    }
                }
            }
        }

        if self.chain.len() == 0 {
            return BlockSearchResult::NoBlockInChain;
        }

        BlockSearchResult::BlockNotFound
    }

}
