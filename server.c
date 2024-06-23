#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>
#include <unistd.h>  // for usleep function


// Function prototypes
double calculateX(double v0, double theta, double t);
double calculateY(double v0, double theta, double t, double g);

int CalcualateProjectileTrajactory(double *BufferCoordinatesX, double *BufferCoordinatesY, double Velocity, double PTheta, double ConstG) 
{

int Counter = 1;

double v0 = Velocity, theta = PTheta, t = 0.0, g = ConstG, dt = 0.12;
double x, y;

/*
// Input initial values
printf("Enter initial velocity (v0): ");
scanf("%lf", &v0);
printf("Enter angle (theta) in degrees: ");
scanf("%lf", &theta);
printf("Enter gravitational constant (g): ");
scanf("%lf", &g);
printf("Enter time step (dt): ");
scanf("%lf", &dt);
*/


// Convert theta to radians
theta = theta * M_PI / 180.0;

// Loop to calculate the trajectory


while (1) {
  // Calculate x(t) and y(t)
  x = calculateX(v0, theta, t);
  y = calculateY(v0, theta, t, g);

  BufferCoordinatesX[Counter] = x;
  BufferCoordinatesY[Counter] = y;
  Counter += 1;



  // Break the loop if the projectile hits the ground
  if (y < 0) {
      break;
  }

  // Print the results
 //printf("Time: %.2lf s, x(t) = %.2lf m, y(t) = %.2lf m\n", t, x, y);

  // Wait for a short interval (simulating real-time)
  usleep(100000);  // 100000 microseconds = 0.1 seconds

  // Update time
  t += dt;
}

return Counter;
}

// Function to calculate x(t)
double calculateX(double v0, double theta, double t) {
    return v0 * cos(theta) * t;
}

// Function to calculate y(t)
double calculateY(double v0, double theta, double t, double g) {
    return v0 * sin(theta) * t - 0.5 * g * t * t;
}





int main(){

  char *ip = "127.0.0.1";
  int port = 5566;

  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size;

  //##
  double bufferX_Coordinates[100];
  double bufferY_Coordinates[100];
  int indexX_Counter = 0;
  int indexY_Counter = 0;
  //##
  
  int n;

  server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sock < 0){
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if (n < 0){
    perror("[-]Bind error");
    exit(1);
  }
  printf("[+]Bind to the port number: %d\n", port);

  listen(server_sock, 5);
  printf("Listening...\n");

  while(1){
    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
    printf("[+]Client connected.\n");

    bzero(bufferX_Coordinates, 100);
    bzero(bufferY_Coordinates, 100);


    // To save iput parameters such as velocity, theta, constanta g[can be non const to show another world], delta time[can be const]
    double params[3];

    recv(client_sock, params, sizeof(params), 0);
  
    printf("P1 = %f \n", params[0]);
    printf("P2 = %f \n", params[1]);
    printf("P2 = %f \n", params[2]);

   int BufferSize = CalcualateProjectileTrajactory(bufferX_Coordinates,bufferY_Coordinates, params[0], params[1], params[2]);

  bufferX_Coordinates[0] = BufferSize;
  bufferY_Coordinates[0] = BufferSize;

  for(int i = 0; i < BufferSize; ++i)
  {
    printf("X: %f  Y: %f \n", bufferX_Coordinates[i], bufferY_Coordinates[i]);
  }

  send(client_sock, bufferX_Coordinates, sizeof(bufferX_Coordinates), 0);
  send(client_sock, bufferY_Coordinates, sizeof(bufferY_Coordinates), 0);
    

    close(client_sock);
    printf("[+]Client disconnected.\n\n");

  }

  return 0;
}

