use std::net::UdpSocket;
use std::thread::sleep;
use std::time::Duration;

fn main() {
    let listen = "127.0.0.1:8080";
    let mut buf = [0u8; 1024];

    // Bind the UDP socket to the specified address
    let socket = UdpSocket::bind(listen).expect("Failed to bind socket");
    println!("UDP echo Server is listening on {}", listen);

    loop {
        match socket.recv_from(&mut buf) {
            Ok((n, addr)) => {
                let received_data = &buf[..n];
                println!("Received from {}: {:?}", addr, String::from_utf8_lossy(received_data));
                // Echo back the received data
                match socket.send_to(received_data, addr) {
                    Ok(_) => println!("Sent to {}: {:?}", addr, String::from_utf8_lossy(received_data)),
                    Err(e) => println!("Send to {} error: {:?}", addr, e),
                }
            }
            Err(e) => {
                println!("Receive error: {:?}", e);
                sleep(Duration::from_millis(100)); // Sleep for a bit before retrying
            }
        }
    }
}
