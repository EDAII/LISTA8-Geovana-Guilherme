#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <cstdlib>
#include <iostream>
#include <ctime>


#include "Node.h"


Node **nodes;
sf::Mutex node_mutex;
bool skip_render = false;

bool generation_running = false;
bool generated = false;


int lin = 30;// 55;
int col = 30;// 75;
int max_elements = lin * col;

float rectangle_W = 20.f;
float rectangle_H = 20.f;

int ms_wait = 100;
const int ms_step = 10;


void generateNodes() {

	// accessing nodes in matrix order is Node[i * col + j]

	skip_render = true;

	node_mutex.lock(); //lock to initialize mutexes

	if (nodes != NULL) { 
	
		for (int i = 0; i < max_elements; i++) {
			delete nodes[i];
		}

		free(nodes);
	}

	nodes = (Node **)calloc(max_elements, sizeof(Node *));

	int k = 0;
	for (int i = 0; i < lin; i++) {
		for (int j = 0; j < col; j++) {

			Node *n = new Node(j, i, rectangle_W, rectangle_H, lin, col);

			nodes[k++] = n;
		}

	}

	node_mutex.unlock();


	skip_render = false;
}


void renderingThread(sf::RenderWindow* window) {

	unsigned seed = (unsigned)std::time(0);

	std::cout << "[RENDERING] Seed: " << seed << "\n";

	std::srand(seed);

	// activate the window's context
	window->setActive(true);
	
	generateNodes();

	// the rendering loop
	while (window->isOpen()) {

		window->clear();

		for (int i = 0; i < max_elements; i++) {
			
			if (skip_render) continue;

			for (int j = nodes[i]->shape_count - 1; j >= 0; j--) {

				if (skip_render) continue;
		
				window->draw(*nodes[i]->shapes[j]);
				
			}
		}
		
		// end the current frame -- this is a rendering function (it requires the context to be active)
		window->display();

	}

}

int depth_first(Node *n) {

	n->setVisited();
	
	sf::sleep(sf::milliseconds(ms_wait));

	int dir = n->getRandomUnusedDir();
	
	while (dir != -1) {

		int i = n->position_y;
		int j = n->position_x;

		switch (dir)
		{
		case Node::UP_DIR:
			i -= 1;
			break;
		
		case Node::DOWN_DIR:
			i += 1;
			break;

		case Node::RIGHT_DIR:
			j += 1;
			break;

		case Node::LEFT_DIR:
			j -= 1;
			break;

		default:
			break;
		}

		Node *target = nodes[i * col + j];

		if (target->visited) {
			n->blockDirection(dir);
			dir = n->getRandomUnusedDir();
			continue;
		}

		n->registerDirection(dir, target);

		depth_first(target);

		//n->setRecursionMade();


		dir = n->getRandomUnusedDir();
	}

	n->setRecursionMade(); // called again in case it's a dead end
	sf::sleep(sf::milliseconds(ms_wait));

	return 0;
}

int *getRandomPoint() {

	int *points = new int[2];

	points[0] = std::rand() % col;

	if (points[0] == 0 || points[0] == col - 1) {

		points[1] = std::rand() % lin;

	}
	else {
		points[1] = (std::rand() % 2 == 1 ? 0 : lin - 1);
	}

	return points;

}

void generationThread() {

	generation_running = true;

	unsigned seed = (unsigned)std::time(0);

	std::cout << "[GENERATION] Seed: " << seed << "\n";

	std::srand(seed);

	// get random starting point on border
	int *start_points = getRandomPoint();


	// get random end point on border
	int *end_points = NULL;

	do {

		if (end_points != NULL) delete end_points;
		end_points = getRandomPoint();
	
	} while (start_points[0] == end_points[0] || start_points[1] == end_points[1]);
	
	node_mutex.lock(); // lock the current nodes

	// generate maze with depth first

	Node *start = nodes[start_points[1] * col + start_points[0]];
	Node *end = nodes[end_points[1] * col + end_points[0]];

	std::cout << "Start x = " << start_points[0] << " y = " << start_points[1] << "\n";
	std::cout << "End x = " << end_points[0] << " y = " << end_points[1] << "\n";

	delete start_points;
	delete end_points;

	depth_first(start);

	start->setStart();
	end->setEnd();

	// find solution with breadth first

	// unlock
	node_mutex.unlock();

	

	generation_running = false;
	generated = true;
}

int main() {

	unsigned seed = (unsigned)std::time(0);
	
	std::cout << "[MAIN] Seed: " << seed << "\n";
	
	std::srand(seed);

	// create the window (remember: it's safer to create it in the main thread due to OS limitations)
	sf::RenderWindow window(sf::VideoMode(800, 600), "Grafos - Labirintos");

	// deactivate its OpenGL context
	window.setActive(false);

	// launch the rendering thread
	sf::Thread rend_thread(&renderingThread, &window);
	rend_thread.launch();

	sf::Thread gen_thread(&generationThread);

	// the event/logic/whatever loop
	while (window.isOpen()) {



		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {

				rend_thread.terminate();
				gen_thread.terminate();

				window.close();
			}

			if (event.type == sf::Event::KeyReleased) {
				
				if (event.key.code == sf::Keyboard::R) {
					// run generation alg

					if (generation_running == false) {
						// start thread

						if (generated) {

							generated = false;
							generateNodes();

							gen_thread.launch();

						}
						else {

							gen_thread.launch();
						}

					}
					else {
						continue;
					}
				}
				else if (event.key.code == sf::Keyboard::Q) {

					if (ms_wait - ms_step > 0) ms_wait -= ms_step; else ms_wait = 0;

					std::cout << "[MAIN] ms_wait = " << ms_wait << "\n";

				}
				else if (event.key.code == sf::Keyboard::W) {

					ms_wait += ms_step;

					std::cout << "[MAIN] ms_wait = " << ms_wait << "\n";
				}
			}

			
		}
	}

	free(nodes);

	return 0;
}