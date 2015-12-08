/*
 * Player.cpp
 *
 *  Created on: 2012. 11. 20.
 *      Author: chaerim
 */

#include "Player.h"

Player::Player() {
}

Player::Player(string name, Automaton &st) {
	configure(name, st);
}

Player::~Player() {

}

void Player::configure(string name, Automaton &st) {
	setNameOfPlayer(name);
	setAutomaton(st);
}

void Player::setAutomaton(Automaton &st) {
	strategy = st;
}

Automaton Player::getAutomaton() {
	return strategy;
}

string Player::getNameOfPlayer() {
	return namePlayer;
}

void Player::setNameOfPlayer(string name) {
	namePlayer = name;
}

string Player::getNameOfStates(int index) {
	return strategy.getNameOfStates(index);
}

string Player::getNameOfActions(int index) {
	return strategy.getNameOfActions(index);
}

string Player::getNameOfSignals(int index) {
	return strategy.getNameOfSignals(index);
}

int Player::getNumberOfStates() {
	return strategy.getNumberOfStates();
}

int Player::getNumberOfActions() {
	return strategy.getNumberOfActions();
}

int Player::getNumberOfSignals() {
	return strategy.getNumberOfSignals();
}

int Player::getActionChoice(int state) {
	return strategy.getActionChoice(state);
}

int Player::getStateTransition(int state, int signal) {
	return strategy.getStateTransition(state, signal);
}

int Player::getIndexOfStates(string state) {
	return strategy.getIndexOfStates(state);
}

int Player::getIndexOfActions(string action) {
	return strategy.getIndexOfActions(action);
}

int Player::getIndexOfSignals(string signal) {
	return strategy.getIndexOfSignals(signal);
}


void Player::view() {
	displayTitle(namePlayer);
	strategy.view();
}
