#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <array>
#include <utility>
#include <string_view>
#include <map>

//thank you reddit for the improvements

constexpr char yes = 'y';
constexpr char no = 'n';
constexpr int blackjack = 21;

constexpr int stand = 17;
constexpr int dealer_threshold = 90;
constexpr int dealer_hit = 19;
constexpr int deck_size = 52;

const std::string clubs = "clubs";
const std::string diamonds = "diamonds";
const std::string hearts = "hearts";
const std::string spades = "spades";


const std::string_view blackjack_ascii = R"( 
 _     _            _    _            _    
| |   | |          | |  (_)          | |   
| |__ | | __ _  ___| | ___  __ _  ___| | __
| '_ \| |/ _` |/ __| |/ / |/ _` |/ __| |/ /
| |_) | | (_| | (__|   <| | (_| | (__|   < 
|_.__/|_|\__,_|\___|_|\_\ |\__,_|\___|_|\_\
                       _/ |                
                      |__/)";

class Card {
public:

	enum suits {
		CLUBS,
		DIAMONDS,
		SPADES,
		HEARTS
	} suit;

	enum values {
		TWO = 0,
		THREE,
		FOUR,
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		NINE,
		TEN,
		KING,
		QUEEN,
		JACK,
		ACE
	} value;


	Card() = default;
	Card(suits suit, values value) : suit{ suit }, value{value} {};


	int get_value() const { return this->value_mappings[std::to_underlying(value)]; } // to_underlying what a godsend

	friend std::ostream& operator << (std::ostream& os, Card& card) {
		os << card.enum_suit_to_string() << " " << card.enum_value_to_string(); // why can you even access private implementation
		return os;
	};

private:
	static constexpr std::array<int, 13> value_mappings = { 2,3,4,5,6,7,8,9,10,10,10,10,11};

	std::string enum_value_to_string() const {
		static std::array<std::string, 13> value_to_string = {  // gonna need someone to clarify why static is used in different scenerios local scopes?
		"Two", "Three", "Four", 
		"Five", "Six", "Seven", 
		"Eight", "Nine", "Ten", 
		"King", "Queen", "Jack", 
		"Ace"}; // not very elegant not sure if enum to string is fun in c++, perhaps map?
	
		return value_to_string[std::to_underlying(this->value)];
	}

	std::string enum_suit_to_string() const {
		static std::array<std::string, 4> suit_to_string = { "Clubs", "Diamonds", "Spades", "Hearts" };
		return suit_to_string[std::to_underlying(this->suit)];
	}

};

class Deck {
	using enum Card::suits;
	using enum Card::values;
public:
	static constexpr std::array<Card::suits, 4> SUITS{ CLUBS, DIAMONDS, SPADES, HEARTS };

	static constexpr std::array<Card::values, 13> VALUES{
	  TWO, THREE, FOUR, FIVE,
	  SIX, SEVEN, EIGHT, NINE, TEN,
	  KING, QUEEN, JACK, ACE
	};

	Deck() {


		int i = 0;
		for (auto& suit : SUITS)
		{
			for (auto& value : VALUES)
			{
				this->deck[i] = Card{ suit, value };
				++i;
			}
		}

		this->shuffle_deck();

	}
;

	void print_deck() {
		for (Card& card : deck) {
			
			std::cout << card << "\n";
		}
	}
	Card give_card() const {
		static int current_card = 0;

		if (current_card >= deck_size) { current_card = 0; };

		Card card = this->deck[current_card]; 
		++current_card; // stored on heap will continue to increment per call
		
		return card;
	}



private:

	void shuffle_deck() {
		if (this->deck.empty())	return;


		std::random_device rd;
		std::mt19937 g{ rd() };

		std::shuffle(this->deck.begin(), this->deck.end(), g);
	}

	std::array<Card, deck_size> deck;



};

class Entity {
public:
	
	Entity() = default;
	Entity(std::string name) : name{name} {};

	

	void push(const Card& card) { this->hand.push_back(card);} // gonna need justification for emplace back > push

	virtual void print_hand() {
		std::cout << "\n" << this->name << " Cards\n";

		for (Card& card : this->hand) {
			std::cout << card << "\n";
		}
		std::cout << this->name << " Score: " << this->hand_sum() << "\n";

	}

	int hand_sum() {
		int total = 0; // total will be set to 0 every call to ensure correct total when iterating through hand
		for (Card& card: this->hand){
			total += card.get_value();
		}
		this->score = total;
		
		this->ace_logic();
		
		return this->score;
	}

	void reset_hand() {
		this->hand.clear();
	}

protected:
	std::vector<Card> hand; // derrived classes should be able to access within scope
	int score = 0; 

private:
	void ace_logic() {

		int ace_count = 0;
		for (auto& card : this->hand) {
			if (card.get_value() == 11) {
				++ace_count;
			}
		}

		while (ace_count > 0 && this->score > blackjack) {
			this->score -= 10; // check the score
			--ace_count; // decrement the ace count exit loop
		}

		return;
	}

	std::string name = "Player 1";
};

class Player: public Entity {
public:


	Player() = default;
	Player(std::string name) : Entity(name) {}; // cba to implement

	bool is_hit() {

		if (this->score >= blackjack) {
			return false;
		}

		char prompt;
		std::cout << "\nWould you like to hit? y/n ";
		while (std::cin >> prompt) {
			switch (prompt) {
			case yes: 
				return true;
			case no: return false;
			default:
				std::cout << "\nWould you like to hit? y/n " << "\n";
				continue;
			}
		}
		return false;
	}
};
 
class Dealer: public Entity {
public:
	void hit(Deck& deck) {

		this->hand_sum();

		while(this->score != blackjack && this->score < dealer_hit && this->score != stand ) {
			this->add_card(deck);
			std::uniform_int_distribution<int> distribution(1, 100);
			std::random_device rd;

			std::mt19937 engine{ rd() }; // Mersenne twister MT19937

			int value = distribution(engine); 
			if (value < dealer_threshold && this->score > stand && this->score != blackjack) {
				this->add_card(deck);


			}
			this->hand_sum(); // feels repetitive

		}
		return;
	}

	void print_hand() override {
		std::cout << "\nDealer Cards\n";
		for (Card& card : hand) {
			std::cout << card << "\n";
		}

		std::cout << "\nDealer Score: " << this->hand_sum() << "\n";

	}

	void reveal_card() {
		std::cout << "\nDealer Card: " << this->hand.front() << "\n";
	}

	void deal_card(Deck& d, Entity& e) {
		e.push(d.give_card());
	}

private:

	void add_card(Deck& deck) {
		Card card = deck.give_card();
		this->push(card);
		//this->score += card.get_value();
	}
};

class BlackJack {
public:
	BlackJack() = default;

	void deal_cards() {
		for (int i = 0; i < 2; ++i) {
			this->dealer.push(deck.give_card());
			this->player.push(deck.give_card());
		}

		this->dealer.reveal_card(); // player can see at least one dealer card

	}

	void init_game() {

		this->deal_cards();
		this->player.print_hand();
		while (this->player.is_hit()) {
			this->dealer.deal_card(this->deck, this->player);
			this->player.print_hand();

		}
		this->dealer.hit(this->deck);
		this->dealer.print_hand();



	/*	this->player.hand.clear();
		this->dealer.hand.clear();*/

	}

private:
	/*void win_logic() const {

		std::cout << "\n";

		int dealer_score = this->dealer.score;
		int player_score = this->player.score;


		if (dealer_score > player_score && dealer_score < blackjack || player_score > blackjack) {
			std::cout << "Dealer Wins\n";
		}


		if (player_score > dealer_score && player_score < blackjack || dealer_score > blackjack) {
			std::cout << "You Win\n";

		}

		if (player_score == dealer_score || player_score > blackjack && dealer_score > blackjack) {
			std::cout << "Draw\n";
		}

	}*/

	Dealer dealer;
	Player player;
	Deck deck;
};

int main() {
	std::cout << "\n" << blackjack_ascii << "\n";

	char prompt;
	std::cout << "\nWould you like to play? y/n ";
	while (std::cin >> prompt) {
		BlackJack blackjack;
		switch (prompt) {
		case yes: 
			blackjack.init_game();
			std::cout << "\nWould you like to play? y/n ";

			break;
		case no:
			return -1;
		default:
			std::cout << "\nWould you like to play? y/n " << "\n";
			break;
		}
	}
	return 0;
}

