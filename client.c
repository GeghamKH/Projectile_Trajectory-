#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <SDL2/SDL.h>

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

const SDL_Renderer * const GlobalRen;

// Function to draw a bold point (square) of given size
void drawBoldPoint(SDL_Renderer *ren, int x, int y, int size) {
    for (int i = -size / 2; i <= size / 2; i++) {
        for (int j = -size / 2; j <= size / 2; j++) {
            SDL_RenderDrawPoint(ren, x + i, y + j);
        }
    }
}


int InitializeSDL(double *bufferX_Coordinates, double * bufferY_Coordinates)
{

  int BSize= bufferX_Coordinates[0];
  int ScaleX = WINDOW_WIDTH/bufferX_Coordinates[BSize-1];
  int ScaleY = WINDOW_HEIGHT / (bufferY_Coordinates[BSize/2] * 2);

  printf("ScaleX: %i \n", ScaleX);
  printf("ScaleY: %i \n", ScaleY);

 // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create SDL window
    SDL_Window *win = SDL_CreateWindow("Projectile Trajectory", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create SDL renderer
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL) {
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }



    /// Clear screen
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    SDL_RenderClear(ren);

    // Set draw color to black
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);


  //Graphics

   

  for(int i = 1; i < BSize; ++i)
  {
    printf("X: %f  Y: %f \n", bufferX_Coordinates[i], bufferY_Coordinates[i]);

    double screenX = bufferX_Coordinates[i] * ScaleX;
    double screenY = WINDOW_HEIGHT - bufferY_Coordinates[i] * ScaleY;

    // Draw the point if it's within the window bounds
        if (screenX >= 0 && screenX < WINDOW_WIDTH && screenY >= 0 && screenY < WINDOW_HEIGHT) {
            //SDL_RenderDrawPoint(ren, screenX, screenY);
            drawBoldPoint(ren,screenX,screenY,3);
            SDL_RenderPresent(ren);
        }
  // Wait for a short interval
        SDL_Delay(100);

  }


// Wait for a few seconds before quitting
    SDL_Delay(5000);

    // Cleanup and quit
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;

}



int main(){

  char *ip = "127.0.0.1";
  int port = 5566;

  int sock;
  struct sockaddr_in addr;
  socklen_t addr_size;
  double bufferX_Coordinates[100];
  double bufferY_Coordinates[100];
  int n;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0){
    perror("[-]Socket error");
    exit(1);
  }
  printf("[+]TCP server socket created.\n");

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr(ip);

  connect(sock, (struct sockaddr*)&addr, sizeof(addr));
  printf("Connected to the server.\n");

  bzero(bufferX_Coordinates, 100);
  

  //Inputs 
  double Velocity = -1, theta = -1, ConstG = -1;

    int hendler = -1;

    
   while (hendler)
    {    
        printf("1 new projectile trajectory\n");
        printf("2 get all data from the server\n");
        printf("3 exit\n");
        printf("Choose a method: ");
        scanf("%i", &hendler);

    switch (hendler)
    {
    case 1:
        // Input initial values
        printf("Enter initial velocity (v0)[ > 0]: ");
        scanf("%lf", &Velocity);
        printf("Enter angle (theta) in degrees [ > 0, <360]: ");
        scanf("%lf", &theta);
        printf("Enter gravitational constant (g)[ >= 1]: ");
        scanf("%lf", &ConstG);

        if( !(Velocity > 0 && (theta > 0  || theta < 360) && ConstG >= 1))
        {
            printf("Invalid input\n");
            continue;
        }
        double params[3] = {Velocity, theta, ConstG};
        printf("Velocity= %f \n", params[0]);
        printf("theta = %f \n", params[1]);
        printf("ConstG = %f \n", params[2]);

        // End of inputs part

        send(sock, params, sizeof(params), 0); // post request

        bzero(bufferX_Coordinates, 100);
        recv(sock, bufferX_Coordinates, sizeof(bufferX_Coordinates), 0);
        recv(sock,bufferY_Coordinates,sizeof(bufferY_Coordinates), 0);
  

        //Graphics 
  
        InitializeSDL(bufferX_Coordinates, bufferY_Coordinates);

    case 2:
        // get all data from the server 
        printf("not implemented yet");
        break;
    
    case 3:

        hendler = 0;
        break;
    default:
        //assert
        break;
    }
 
    }

  close(sock);
  printf("Disconnected from the server.\n");

  return 0;

}


  
