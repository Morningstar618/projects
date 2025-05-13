use std::time::SystemTime;
use sha2::{Digest, Sha256};

#[derive(Debug)]
pub struct Block {
    nonce: i32,
    previous_hash: Vec<u8>,
    time_stamp: u128,
    transaction: Vec<Vec<u8>>
}

impl Block {
    
    pub fn new(nonce: i32, previous_hash: Vec<u8>) -> Self {
        let time_now = SystemTime::now()
            .duration_since(SystemTime::UNIX_EPOCH)
            .unwrap();

        Block { 
            nonce, 
            previous_hash, 
            time_stamp: time_now.as_nanos(), 
            transaction: Vec::<Vec<u8>>::new() 
        }
    }

    pub fn get_nonce(&self) -> i32 {
        self.nonce
    }
    
    pub fn get_previous_hash(&self) -> Vec<u8> {
        self.previous_hash.clone()
    }

    pub fn get_timestamp(&self) -> u128 {
        self.time_stamp
    }

    pub fn get_transaction(&self) -> Vec<Vec<u8>> {
        self.transaction.clone()
    }

    pub fn hash(&self) -> Vec<u8> {
        let mut bin = Vec::<u8>::new();
        bin.extend(self.nonce.to_be_bytes());
        bin.extend(self.previous_hash.clone());
        bin.extend(self.time_stamp.to_be_bytes());

        let transaction_bin = self.transaction
            .iter()
            .flatten()
            .map(|n| n.to_owned())
            .collect::<Vec<u8>>();

        bin.extend(transaction_bin);
        
        let mut hasher = Sha256::new();
        hasher.update(bin);

        hasher.finalize().to_vec()
    }

    pub fn display(&self) {
        println!("nonce: {}", self.nonce);
        println!("previous_hash: {:?}", self.previous_hash);
        println!("timestamp: {}", self.time_stamp);
        println!("transaction: {:?}", self.transaction);
        println!("{}", "-".repeat(45));
    }

}