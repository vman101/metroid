use std::net::{SocketAddr, UdpSocket};
use std::thread::sleep;
use std::time::Duration;
use std::sync::{Arc, Mutex};

#[repr(C)]
struct Rect {
    x: i32,
    y: i32,
    width: u32,
    height: u32,
}

fn to_bytes<T: Sized>(value: &T) -> Vec<u8> {
    let ptr = value as *const T as *const u8;
    let size = std::mem::size_of::<T>();
    unsafe { std::slice::from_raw_parts(ptr, size).to_vec() }
}

#[repr(C)]
struct ServerData {
	data_type: u32,
	id: u32,
	filler: u32,
	filler1: u32,
}

fn connected_to_user(buf: &mut [u8], n: usize, addr: SocketAddr, socket: &UdpSocket) {
	let rect = Rect {
        x: 50,
        y: 70,
        width: 70,
        height: 50,
    };

	let received_data = buf;
    println!(
        "Received from {}: {:?}",
        addr,
        String::from_utf8_lossy(received_data)
    );
    // Echo back the received data
    let bytes = to_bytes(&rect);
    let data = ServerData {
       	data_type: 1,
       	id: 1,
       	filler: 1,
       	filler1: 1,
       	// data: bytes.as_slice(),
    };
    let mut packet = to_bytes(&data);
    packet.extend(&bytes);
    match socket.send_to(&packet, addr) {
        Ok(_) => println!(
            "Sent to {}: {:?}",
            addr,
            bytes
        ),
        Err(e) => println!("Send to {} error: {:?}", addr, e),
    }
}

fn main() {
    let listen = "127.0.0.1:8080";
    let mut buf = [0u8; 1024];
    let mut id_counter: u32 = 1;

    // Bind the UDP socket to the specified address
    let socket = UdpSocket::bind(listen).expect("Failed to bind socket");
    println!("UDP echo Server is listening on {}", listen);

    // Spawn a new thread to handle the player
    let server_clone = Arc::clone(&server);
    let socket_clone = socket.try_clone().expect("Failed to clone socket");

    loop {
        match socket.recv_from(&mut buf) {
            Ok((n, addr)) => {
	           	socket.send_to(&id_counter.to_ne_bytes(), addr).expect("Failed To Send ACK");
            	connected_to_user(&mut buf, n, addr, &socket);
            }
            Err(e) => {
                println!("Receive error: {:?}", e);
                sleep(Duration::from_millis(100)); // Sleep for a bit before retrying
            }
        }
    }
}