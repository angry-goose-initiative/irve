//Command to compile so far: RUSTFLAGS="-C linker=riscv32-unknown-elf-gcc -C link-arg=-T../irve.ld" cargo build --target riscv32i-unknown-none-elf -Zbuild-std=core




#![no_std]


pub fn add(left: usize, right: usize) -> usize {
    left + right
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}

#[panic_handler]
pub fn panic_handler(_info: &core::panic::PanicInfo) -> ! {
    loop {}//TODO
}
