use super::utility_traits::Serialization;

pub struct Transaction {
    sender_address: Vec<u8>,
    recipient_address: Vec<u8>,
    value: u64
}

impl Transaction {
    pub fn new(sender: Vec<u8>, recipient: Vec<u8>, value: u64) -> Self {
        Transaction {
            sender_address: sender,
            recipient_address: recipient,
            value
        }
    }
}

impl Serialization<Self> for Transaction {
    fn serialization(&self) -> Vec<u8> {
        let mut bin = Vec::<u8>::new();
        
        let len_sender = self.sender_address.len() as u64;
        bin.extend(&len_sender.to_be_bytes());
        bin.extend(&self.sender_address);

        let len_recipient = self.recipient_address.len() as u64;
        bin.extend(&len_recipient.to_be_bytes());
        bin.extend(&self.recipient_address);

        bin.extend(&self.value.to_be_bytes());

        bin
    }

    fn deserialization(bytes: Vec<u8>) -> Self {
        let mut pos = 0;
        
        let len_sender = usize::from_be_bytes(bytes[pos..pos+8].try_into().unwrap());
        pos += 8;
        let sender_address = bytes[pos..pos+len_sender].to_vec();
        pos += len_sender;

        let len_recipient = usize::from_be_bytes(bytes[pos..pos+8].try_into().unwrap());
        pos += 8;
        let recipient_address = bytes[pos..pos+len_recipient].to_vec();
        pos += len_recipient;

        let value = u64::from_be_bytes(bytes[pos..pos+8].try_into().unwrap());

        Transaction { sender_address, recipient_address, value }
    }
}
