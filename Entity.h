#pragma once

class UserInput;

class Entity {
public:
	virtual void step(double dt, const UserInput& userInput) = 0;
};