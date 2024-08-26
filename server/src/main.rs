use std::net::{SocketAddr, UdpSocket};
use std::thread::{self, JoinHandle, sleep};
use std::time::Duration;
use std::sync::{Arc, Mutex};
use std::collections::HashMap;

#[repr(C)]
struct Rect {
    x: i32,
    y: i32,
    width: u32,
    height: u32,
}

// fn to_bytes<T: Sized>(value: &T) -> Vec<u8> {
//     let ptr = value as *const T as *const u8;
//     let size = std::mem::size_of::<T>();
//     unsafe { std::slice::from_raw_parts(ptr, size).to_vec() }
// }

struct ServerData {
	server_data: HashMap<u32, Arc<Mutex<ClientData>>>,
}

struct ClientData {
    id: u32,
    connected: bool,
    socket:UdpSocket,
    address: SocketAddr,
    server_state: Arc<Mutex<ServerData>>,
}

fn connected_to_user(client: Arc<Mutex<ClientData>>) {
	println!("User Connected succesfully");
	{
		let mut user = client.lock().expect("failed to lock mutex user");
		user.connected = true;
	}
	loop {}
}

fn create_user (
    received_data: &[u8],
    id: u32,
    socket: UdpSocket,
    address: SocketAddr,
    server_state: Arc<Mutex<ServerData>>
    ) -> Result<ClientData, i32> {

    let message_chunk = u32::from_be_bytes(
        received_data
            .try_into()
            .expect("Slice with incorrect length")
    );

    println!("{message_chunk}");
    if message_chunk.to_ne_bytes() == 42691337_u32.to_ne_bytes() {
        Ok(ClientData{
            id,
            connected: false,
            socket,
            address,
            server_state
        })
    } else {
        Err(1)
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
    let server = Arc::new(Mutex::new(ServerData {
                server_data: HashMap::new(),
            }
        )
    );

    let mut threads: Vec<JoinHandle<()>> = Vec::new();
    loop {
        match socket.recv_from(&mut buf) {
            Ok((n, addr)) => {
	            let server_clone = Arc::clone(&server);

	            let new_user = match create_user(
	                &buf[..n],
	                id_counter,
	                socket.try_clone().expect("Failed to clone server socket"),
	                addr,
	                server_clone.clone(),
	            ) {
	                Ok(user) => user,
	                Err(e) => {
	                    println!("Failed to create user: {:?}", e);
	                    continue; // Skip to the next iteration if user creation fails
	                }
	            };
	            let new_user: Arc<Mutex<ClientData>> = Arc::new(Mutex::new(new_user));
                {
	                let mut server_state = server.lock().expect("Failed to lock server");
	                server_state.server_data.insert(id_counter, Arc::clone(&new_user));
                }

	           	socket.send_to(&id_counter.to_ne_bytes(), addr).expect("Failed To Send ACK");
				let user_clone = Arc::clone(&new_user);
                let handle = thread::spawn( || {
                    connected_to_user(user_clone)
                });
                id_counter += 1;
                threads.push(handle);
            }
            Err(e) => {
                println!("Receive error: {:?}", e);
                sleep(Duration::from_millis(100)); // Sleep for a bit before retrying
            }
        }
    }
}