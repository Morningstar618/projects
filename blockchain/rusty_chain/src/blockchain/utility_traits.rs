pub trait Serialization<T> {
    fn serialization(&self) -> Vec<u8>;
    fn deserialization(bytes: Vec<u8>) -> T;
}
