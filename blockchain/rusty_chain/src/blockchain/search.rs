use super::block::Block;

pub enum BlockSearch {
    SearchByPreviousHash(Vec<u8>),
    SearchByBlockHash(Vec<u8>),
    SearchByNonce(i32),
    SearchByTimestamp(u128),
    SearchByTransaction(Vec<Vec<u8>>)
}

pub enum BlockSearchResult<'a> {
    Success(&'a Block),
    BlockNotFound,
    NoBlockInChain
}