from http.server import HTTPServer, BaseHTTPRequestHandler
import json
import logging

logging.basicConfig(level=logging.INFO)
PASSWORD = "1999"  # Change this to your desired 4-digit password

class PasswordHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        logging.info(f"Received POST request from {self.client_address}")
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        logging.info(f"Received data: {post_data}")
        
        try:
            data = json.loads(post_data.decode('utf-8'))
            
            if 'password' in data:
                guess = data['password']
                logging.info(f"Password guess: {guess}")
                if guess == PASSWORD:
                    response = {"message": "Correct password!"}
                    print("You are: "+"The One" +"found the secret protocals! heres the link: https://tinyurl.com/TheMatrixProtocol ")
                    self.send_response(200)
                else:
                    response = {"message": "Incorrect password."}
                    self.send_response(400)
            else:
                response = {"message": "No password provided."}
                self.send_response(400)
            
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            response_data = json.dumps(response).encode('utf-8')
            self.wfile.write(response_data) 
            logging.info(f"Sent response: {response}")
        except Exception as e:
            logging.error(f"Error processing request: {e}")
            self.send_error(500, "Internal Server Error")

def run_server(port=8000):
    server_address = ('', port)
    httpd = HTTPServer(server_address, PasswordHandler)
    logging.info(f"Server running on port {port}")
    print("what's the password? (use post-req, use: payload = {""password"": password} ),sever_ip = 127.0.0.1 )")
    httpd.serve_forever()

if __name__ == "__main__":
    run_server()