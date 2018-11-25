#include "Node.h"

#include <iostream>
#include <algorithm>



Node::Node(int pos_x, int pos_y, float rect_W, float rect_H, int _lin, int _col) {

	this->position_x = pos_x;
	this->position_y = pos_y;
	this->rectangle_H = rect_H;
	this->rectangle_W = rect_W;
	this->max_lin = _lin;
	this->max_col = _col;

	this->directions = (Node **)calloc(4, sizeof(Node *));

	this->shapes = (sf::Shape **) calloc(this->shape_count, sizeof(sf::Shape *));

	if (this->shapes == NULL) {
		std::cout << "Error allocating shapes" << "\n";
		exit(-1);
	}
	
	this->shapes[4] = new sf::RectangleShape(sf::Vector2f(rectangle_W, rectangle_H));
	this->shapes[4]->setPosition(position_x * rectangle_W, position_y * rectangle_H);
	this->shapes[4]->setFillColor(Node::default_color);

	this->shapes[UP_DIR] = new sf::RectangleShape(sf::Vector2f(rectangle_W, rectangle_H / BORDER_FACTOR));
	this->shapes[UP_DIR]->setPosition(position_x * rectangle_W, position_y * rectangle_H);
	this->shapes[UP_DIR]->setFillColor(sf::Color::Black);

	this->shapes[DOWN_DIR] = new sf::RectangleShape(sf::Vector2f(rectangle_W, rectangle_H / BORDER_FACTOR));
	this->shapes[DOWN_DIR]->setPosition(
		position_x * rectangle_W, 
		position_y * rectangle_H + (BORDER_FACTOR - 1)*(rectangle_H / BORDER_FACTOR)
	);
	this->shapes[DOWN_DIR]->setFillColor(sf::Color::Black);

	this->shapes[LEFT_DIR] = new sf::RectangleShape(sf::Vector2f(rectangle_W / BORDER_FACTOR, rectangle_H));
	this->shapes[LEFT_DIR]->setPosition(position_x * rectangle_W, position_y * rectangle_H);
	this->shapes[LEFT_DIR]->setFillColor(sf::Color::Black);

	this->shapes[RIGHT_DIR] = new sf::RectangleShape(sf::Vector2f(rectangle_W / BORDER_FACTOR, rectangle_H));
	this->shapes[RIGHT_DIR]->setPosition(
		position_x * rectangle_W + (BORDER_FACTOR - 1)*(rectangle_W / BORDER_FACTOR),
		position_y * rectangle_H 
	);
	this->shapes[RIGHT_DIR]->setFillColor(sf::Color::Black);

	if (position_x == 0) {
		this->blockDirection(LEFT_DIR);
	}

	if (position_x == max_col - 1) {
		this->blockDirection(RIGHT_DIR);
	}

	if (position_y == 0) {
		this->blockDirection(UP_DIR);
	}
	
	if (position_y == max_lin - 1) {
		this->blockDirection(DOWN_DIR);
	}

}

Node::~Node() {

	for (int i = 0; i < this->shape_count; i++) {
		delete this->shapes[i];
	}

	delete this->directions;

}


void Node::registerDirection(int direction, Node *n) {

	this->directions[direction] = n;

	this->shapes[direction]->setFillColor(sf::Color::Transparent);

	// set bidirectional (reverse) conection on Node n
	int reverse_direction = 0;

	switch (direction)
	{
	case UP_DIR:
		reverse_direction = DOWN_DIR;
		break;
	
	case DOWN_DIR:
		reverse_direction = UP_DIR;
		break;
	
	case LEFT_DIR:
		reverse_direction = RIGHT_DIR;
		break;
	
	case RIGHT_DIR:
		reverse_direction = LEFT_DIR;
		break;

	default:
		break;
	}

	// check if reverse direction is defined
	if (n->directions[reverse_direction] == NULL) {
		
		n->registerDirection(reverse_direction, this);

	}

}

void Node::setVisited() {

	this->visited = true;	
	this->shapes[4]->setFillColor(Node::selected_color);

	//	sf::Color(rand() % 256, rand() % 256, rand() % 256);


}

void Node::setStart() {

	this->shapes[4]->setFillColor(Node::start_color);

}

void Node::setEnd() {

	this->shapes[4]->setFillColor(Node::end_color);
	this->end_point = true;

}

void Node::setRecursionMade() {

	this->shapes[4]->setFillColor(Node::recursion_made_color);

}

void Node::setCheckedPath() {

	if (this->shapes[4]->getFillColor() != this->start_color && this->shapes[4]->getFillColor() != this->end_color)
		this->shapes[4]->setFillColor(this->path_step_color);
	
	this->path_check = true;

}

void Node::setRealPath() {

	if (this->shapes[4]->getFillColor() != this->start_color && this->shapes[4]->getFillColor() != this->end_color)
		this->shapes[4]->setFillColor(this->path_color);

}

void Node::blockDirection(int dir) {

	this->shapes[dir]->setFillColor(sf::Color::Black);
	this->directions[dir] = this;

	//this->shapes[UP_DIR]->setFillColor(sf::Color::Green);
}

int Node::getRandomUnusedDir() {
	
	int random = std::rand() % 4;

	for (int i = 0; i < 4; i++) {
	
		if (this->directions[random] == NULL) {

			return random;
		}
		else {
			
			random++;

			if (random >= 4)
				random = 0;
		
		}

	}

	// no free border
	return -1;
}

bool operator<(const Node &a, const Node &b) {

	return a.position_x < b.position_x && a.position_y < b.position_y;
}
