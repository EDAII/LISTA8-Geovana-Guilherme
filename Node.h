#pragma once

#include <SFML/Graphics.hpp>

class Node {

public:

	Node(int, int, float, float, int, int);
	~Node();
	
	friend bool operator <(const Node&, const Node&);

	void registerDirection(int direction, Node * n);

	void setVisited();
	void setStart();
	void setEnd();
	void setRecursionMade();

	void setCheckedPath();
	void setRealPath();

	void blockDirection(int dir);

	int getRandomUnusedDir();

	sf::Shape **shapes;
	int shape_count = 5;
	// 4 is the main shape
	// 0 to 3 are the borders (same identification as DIR)

	Node **directions;

	static constexpr int UP_DIR = 0;
	static constexpr int DOWN_DIR = 1;
	static constexpr int LEFT_DIR = 2;
	static constexpr int RIGHT_DIR = 3;

	const sf::Color selected_color = sf::Color::Blue;
	const sf::Color default_color = sf::Color::Black;
	const sf::Color recursion_made_color = sf::Color::White;

	const sf::Color path_color = sf::Color::Green;
	const sf::Color path_step_color = sf::Color::Yellow;
	
	const sf::Color start_color = sf::Color::Cyan;
	const sf::Color end_color = sf::Color::Magenta;
	

	static constexpr float BORDER_FACTOR = 15.75;

	bool visited = false;
	bool path_check = false;
	bool end_point = false;

	int position_x;
	int position_y;

	int max_lin;
	int max_col;

	float rectangle_W;
	float rectangle_H;


};

