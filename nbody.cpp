/* Jamie, Hughes, 2108715, Assignment 3, 159.341
   This program simulates the motion of a number of bodies (planets, particles, etc.) with an attractive
   force between each pair of particles. The simulation uses parallelization to improve performance using multi-threading.
   - Added a loader as using the non multi threaded version took very long on 10,000 particles (approx 45 minutes), needed to make sure that progress was being made.


   Computer -> Macbook Pro M1 -> 10 cores

   Language and Multi-Threaded API:
   - Programming Language: C++
   - Multi-Threaded API: C++ Standard Library std::<thread> (C++11)
   - Compiler: g++ or clang++
   - Compilation Flags: -std=c++11 -O3 -pthread

   Small: (1,000)
   Original Single thread Implementation
   - Time Taken: 28.7027 seconds
   Multi Threaded Implmentation
   - Time Taken: 0.284372 seconds

   Large: (5,000)
	Original Single thread Implementation
   - Time Taken: 744.033 seconds
   Multi Threaded Implmentation
   - Time Taken: 5.64824 seconds

   Massive (10,000):
   Original Single thread Implementation
   - Time Taken: 2949.54 seconds
   Multi Threaded Implmentation
   - Time Taken: 22.4863 seconds

*/

// System Headers
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>

// Project Headers
#include "nbody.h"

// #define GRAPHICS
#ifdef GRAPHICS
#include <SFML/Graphics.hpp>
#endif

// Number of particles
// #define LARGE
// #define SMALL
#define MASSIVE

#if defined(SMALL)
const int N = 1000;
#elif defined(LARGE)
const int N = 5000;
#elif defined(MASSIVE)
const int N = 10000;
#endif

// Constants
const double min2 = 2.0;	 // Minimum distance squared to avoid singularity
const double G = 1 * 10e-10; // Gravitational constant
const double dt = 0.01;		 // Time step for the simulation
const int NO_STEPS = 500;	 // Number of steps to simulate

// Size of Window/Output image
const int width = 1920;
const int height = 1080;

// Bodies array
body bodies[N];

// Function to calculate forces for a subset of bodies
void calculate_forces(int start, int end, vec2 *acc)
{
	for (int i = start; i < end; ++i)
	{
		for (int j = i + 1; j < N; ++j)
		{
			vec2 dx = bodies[i].pos - bodies[j].pos; // Difference in position
			vec2 u = normalise(dx);					 // Normalised direction vector
			double d2 = length2(dx);				 // Distance squared
			if (d2 > min2)
			{
				double x = smoothstep(min2, 2 * min2, d2);			  // Smoothing factor
				double f = -G * bodies[i].mass * bodies[j].mass / d2; // Gravitational force
				acc[i] += (u * f / bodies[i].mass) * x;				  // Update acceleration for body i
				acc[j] -= (u * f / bodies[j].mass) * x;				  // Update acceleration for body j
			}
		}
	}
}

// Update Nbody Simulation
void update()
{
	int num_threads = std::thread::hardware_concurrency(); // Number of available threads
	std::vector<std::thread> threads;					   // Vector to hold threads
	std::vector<vec2> thread_acc[num_threads];			   // Array of acceleration vectors for each thread

	// Initialize thread_acc arrays
	for (int t = 0; t < num_threads; ++t)
	{
		thread_acc[t].resize(N);
		for (int i = 0; i < N; ++i)
		{
			thread_acc[t][i] = vec2(0, 0); // Initialize acceleration to zero
		}
	}

	int chunk_size = N / num_threads; // Determine chunk size for each thread

	// Launch threads to calculate forces
	for (int t = 0; t < num_threads; ++t)
	{
		int start = t * chunk_size;
		int end = (t == num_threads - 1) ? N : (t + 1) * chunk_size;
		threads.emplace_back([start, end, &thread_acc, t]()
							 {
            for (int i = start; i < end; ++i)
            {
                for (int j = i + 1; j < N; ++j)
                {
                    vec2 dx = bodies[i].pos - bodies[j].pos;
                    vec2 u = normalise(dx);
                    double d2 = length2(dx);
                    if (d2 > min2)
                    {
                        double x = smoothstep(min2, 2 * min2, d2);
                        double f = -G * bodies[i].mass * bodies[j].mass / d2;
                        thread_acc[t][i] += (u * f / bodies[i].mass) * x;
                        thread_acc[t][j] -= (u * f / bodies[j].mass) * x;
                    }
                }
            } });
	}

	// Wait for all threads to complete
	for (auto &t : threads)
	{
		t.join();
	}

	// Aggregate results from all threads
	vec2 acc[N];
	for (int i = 0; i < N; ++i)
	{
		acc[i] = vec2(0, 0);
		for (int t = 0; t < num_threads; ++t)
		{
			acc[i] += thread_acc[t][i];
		}
	}

	// Update position and velocity of each body
	for (int i = 0; i < N; ++i)
	{
		bodies[i].pos += bodies[i].vel * dt; // Update position
		bodies[i].vel += acc[i] * dt;		 // Update velocity
	}
}

// Initialise NBody Simulation
void initialise()
{
	// Create a central heavy body (sun)
	bodies[0] = body(width / 2, height / 2, 0, 0, 1e15, 5);

	// Initialize other bodies with random positions and velocities
	for (int i = 1; i < N; ++i)
	{
		double r = (uniform() + 0.1) * height / 2;					// Random radius
		double theta = uniform() * 2 * M_PI;						// Random angle
		double v = sqrt(G * (bodies[0].mass + bodies[i].mass) / r); // Orbital velocity

		// Create orbiting body
		bodies[i] = body(width / 2 + r * cos(theta), height / 2 + r * sin(theta), -sin(theta) * v, cos(theta) * v, 1e9, 2);
	}
}

#ifdef GRAPHICS
// Main Function - Graphical Display
int main()
{
	// Create Window
	sf::ContextSettings settings;
	settings.antialiasingLevel = 1;
	sf::RenderWindow window(sf::VideoMode(width, height), "NBody Simulator", sf::Style::Default, settings);

	// Initialise NBody Simulation
	initialise();
	int i = 0;
	// Run the program as long as the window is open
	while (window.isOpen())
	{
		// Check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "Close requested" event: we close the window
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		if (i < NO_STEPS)
		{
			// Update NBody Simulation
			update();
			i++;
		}

		// Clear the window with black color
		window.clear(sf::Color::Black);

		// Render objects
		for (int i = 0; i < N; ++i)
		{
			// Create Circle
			sf::CircleShape shape(bodies[i].radius);
			shape.setFillColor(sf::Color(255, 0, 0));
			shape.setPosition(bodies[i].pos.x, bodies[i].pos.y);

			// Draw Object
			window.draw(shape);
		}

		// Display window
		window.display();
	}
}
#else
// Main Function - Benchmark
int main()
{
	// Print assignment information
	printf("--------------------------------------------------\n");
	printf("Assignment 3 Semester 1 2024\n");
	printf("Submitted by: Jamie Hughes, 2108715\n");
	printf("--------------------------------------------------\n");

	// Initialise NBody Simulation
	initialise();

	// Get start time
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	// Run Simulation with progress display
	for (int i = 0; i < NO_STEPS; i++)
	{
		// Update NBody Simulation
		update();

		// Display progress
		int progress = (i + 1) * 100 / NO_STEPS;
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		double elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
		double estimated_total_seconds = (elapsed_seconds / (i + 1)) * NO_STEPS;
		double remaining_seconds = estimated_total_seconds - elapsed_seconds;

		std::cout << "\rProgress: " << progress << "% [";
		int pos = 50 * progress / 100;
		for (int j = 0; j < 50; ++j)
		{
			if (j < pos)
				std::cout << "=";
			else if (j == pos)
				std::cout << ">";
			else
				std::cout << " ";
		}
		std::cout << "] Elapsed time: " << elapsed_seconds << "s, Remaining time: " << remaining_seconds << "s" << std::flush;
	}

	// Get end time
	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	std::cout << std::endl; // Move to the next line after progress display

	// Generate output image
	unsigned char *image = new unsigned char[width * height * 3];
	memset(image, 0, width * height * 3);

	// For each body
	for (int i = 0; i < N; ++i)
	{
		// Get Position
		vec2 p = bodies[i].pos;

		// Check particle is within bounds
		if (p.x >= 0 && p.x < width && p.y >= 0 && p.y < height)
		{
			// Add a red dot at body
			image[((((int)p.y * width) + (int)p.x) * 3)] = 255;
		}
	}

	// Write position data to file
	char data_file[200];
	snprintf(data_file, sizeof(data_file), "output%i.dat", N);
	write_data(data_file, bodies, N);

	// Write image to file
	char image_file[200];
	snprintf(image_file, sizeof(image_file), "output%i.png", N);
	write_image(image_file, bodies, N, width, height);

	// Check Results
	char reference_file[200];
	snprintf(reference_file, sizeof(reference_file), "reference%i.dat", N);
	calculate_maximum_difference(reference_file, bodies, N);

	// Time Taken
	std::cout << "Time Taken: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000000.0 << " seconds" << std::endl;

	delete[] image;
}
#endif
