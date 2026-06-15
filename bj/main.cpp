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

		std::random_device rd;
		std::mt19937 g{ rd() };

		std::shuffle(this->deck.begin(), this->deck.end(), g);

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
	std::array<Card, deck_size> deck;



};

class Entity {
public:
	
	Entity() = default;
	Entity(std::string name) : name{name} {};

	int score = 0; // probably should not be public 
	
	std::vector<Card> hand;

	void push(const Card& card) { this->hand.push_back(card);} // gonna need justification for emplace back > push

	virtual void print_hand() {
		std::cout << "\nYour Cards\n";

		for (Card& card : hand) {
			std::cout << card << "\n";
		}
	}

private:
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



		while(this->score != blackjack && this->score < dealer_hit && this->score != stand ) {
			this->add_card(deck);
			std::uniform_int_distribution<int> distribution(1, 100);
			std::random_device rd;

			std::mt19937 engine{ rd() }; // Mersenne twister MT19937

			int value = distribution(engine); 
			if (value < dealer_threshold && this->score > stand && score != blackjack) { 
				this->add_card(deck);


			}
			this->deck_sum();
		}



		return;
	}

	void print_hand() override {
		std::cout << "\nDealer Cards\n";
		for (Card& card : hand) {
			std::cout << card << "\n";
		}
	}

	void reveal_card() {
		std::cout << "\nDealer Card: " << this->hand.front() << "\n";
	}

private:

	void add_card(Deck& deck) {
		Card card = deck.give_card();
		this->push(card);
		//this->score += card.get_value();
	}

	void deck_sum() {

		int total = 0;
		for (auto& card: this->hand) {
			total += card.get_value();
		}
		this->score = total;
	}

};

class BlackJack {
public:
	BlackJack() = default;

	Deck deck;

	void deal_cards() {
		for (int i = 0; i < 2; ++i) {
			this->dealer.push(deck.give_card());
			this->player.push(deck.give_card());
		}

		this->dealer.reveal_card(); // player can see at least one dealer card

	}

	void init_game() {

		this->deal_cards();
		this->player_logic(this->player);
		while (this->player.is_hit()) {
			this->player.push(this->deck.give_card());
			this->player_logic(this->player);

		}
		this->dealer_logic(this->dealer);
		


		this->win_logic();

		this->player.hand.clear();
		this->dealer.hand.clear();

	}

private:

	void player_logic(Player& p) {
		this->score(p);
		p.print_hand();
		this->get_entity_score(p);
	}

	void dealer_logic(Dealer& d) {
		d.hit(this->deck);
		this->score(d);
		this->dealer.print_hand();
		this->get_entity_score(d);

	}



	void get_entity_score(Entity& e) const  {
		std::cout << e.score;
	}

	void ace_logic(Entity& e) {

		int ace_count = 0;
		for (auto& card : e.hand) {
			if (card.get_value() == 11) {
				++ace_count;
			}
		}

		while (ace_count > 0 && e.score > blackjack) {
			e.score -= 10; // check the score
			--ace_count; // decrement the ace count exit loop
		}
		return;
	}

	void score(Entity& e) {

		int score = 0; 

		if (e.hand.empty()) return ;
		
		for (auto& card : e.hand) {
			score += card.get_value();
		}

		e.score = score;

		this->ace_logic(e);

		return;

	}

	void win_logic() const {

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

	}

	Dealer dealer;
	Player player;
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

